#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <cstdint>
#include <string>

#define MAX_ROUNDS 15
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define RESET   "\x1b[0m"

auto flagImage = {0x32U, 0x35U, 0x35U};

void initialize_sbox(unsigned int (&sbox)[17][17], unsigned int (&sbox_inverse)[17][17]){
    int i, j;
    unsigned int rows_and_columns = sbox[0][0] = 0x0U;
    sbox_inverse[0][0] = 0x1U;

    for(i = 1; i < 17; i++){
        sbox[i][0] = sbox[0][i] = sbox_inverse[i][0] = sbox_inverse[0][i] = rows_and_columns++;
    }
    
    std::ifstream sbox_file("structures/sbox.txt", std::ios::binary);
    std::ifstream sbox_inverse_file("structures/sbox_inverse.txt", std::ios::binary);
    
    for (i = 1; i < 17; i++) {
        for (j = 1; j < 17; j++) {
            sbox_file >> std::skipws >> std::hex >> sbox[i][j];
            sbox_inverse_file >> std::skipws >> std::hex >> sbox_inverse[i][j];
        }
    }
    sbox_file.close();
    sbox_inverse_file.close();
}

void initialize_rcon(unsigned int (&rcon)[15][4]){
    int i, j, round = 1;
    for(i = 0; i < 15; i++){
        rcon[i][1] = rcon[i][2] = rcon[i][3] = 0x0U;
        if(round == 1)
            rcon[i][0] = 0x1U;
        else if(round > 1 && rcon[i-1][0] < 0x80U)
            rcon[i][0] = rcon[i-1][0] << 1;
        else if(round > 1 && rcon[i-1][0] >= 0x80U)
            rcon[i][0] = (rcon[i-1][0] << 1) ^ 0x11BU;
        round++;
    }
}

void print_sbox(unsigned int sbox[17][17]){
    int i, j;
    if(sbox[0][0] == 0x0U)
        std::cout << "\nS-BOX:" << std::endl;
    else
        std::cout << "\nINVERSE S-BOX:" << std::endl;

    for (i = 0; i < 17; i++) {
        for (j = 0; j < 17; j++) {
            std::cout << std::hex << sbox[i][j] << "\t";
        }
        std::cout << std::endl;
    }
}

void print_rcon(unsigned int rcon[15][4]){
    int i, j;
    std::cout << "\nRCON:" << std::endl;

    for (i = 0; i < 15; i++) {
        for (j = 0; j < 4; j++) {
            std::cout << std::hex << rcon[i][j] << "\t";
        }
        std::cout << std::endl;
    }
}

int set_cipher_key_matrix(unsigned int (&key_matrix)[MAX_ROUNDS+1][4][4], char *key_argv){
    std::string key = key_argv;
    bool invalid_char = false;
    int i, j, key_it = 0;
    char *ptr_key = &key[0];
    std::string tmp;
    unsigned int key_array[16];

    if(key.size() != 32){
        std::cout << RED << "Key error. Key size must be 32 [128-bit]." << RESET << std::endl;
        return -1;
    }

    for(i = 0; i < key.size(); i++){
        if(key[i] < 48){    //inferior a zero em ascii
            invalid_char = true;
            break;
        }else if(key[i] > 57 && key[i] < 65){   //inferior a A em ascii
            invalid_char = true;
            break;
        }else if(key[i] > 70 && key[i] < 97){   // maior que F, mas menor que a
            invalid_char = true;
            break;
        }else if(key[i] > 102){ // maior que f
            invalid_char = true;
            break;
        }
    }
    
    if(invalid_char){
        std::cout << RED << "Key error. Invalid character detected, it should be [0-9A-Fa-f]." << RESET << std::endl;
        return -1;
    }

    for(i = 0; i < key.size(); i+=2){
        tmp.push_back(ptr_key[i]);
        tmp.push_back(ptr_key[i+1]);
        key_array[key_it++] = std::stoul(tmp, NULL, 16);
        tmp.clear();
    }
    
    key_it = 0;

    for(i = 0; i < 4; i++)
        for(j = 0; j < 4; j++)
            key_matrix[0][j][i] = key_array[key_it++];

    return 0;
}

void key_expansion(unsigned int (&key_matrix)[MAX_ROUNDS+1][4][4], unsigned int sbox[17][17], unsigned int rcon[15][4], int ROUNDS){
    std::cout << "Expanding initial key to " << GREEN << ROUNDS << RESET << " other keys..." << std::endl;
    int i, j, k;
    int sbox_row, sbox_column;
    unsigned int hi, lo;
    unsigned int tmp[4];
    unsigned int actual_column[4];
    bool sbox_found = false;

    for(k = 0; k < ROUNDS; k++){
        for(i = 0; i < 4; i++){
            tmp[i] = key_matrix[k][i][3];   // save temp in order to rot word
        }

        for(i = 0; i < 4; i++){
            key_matrix[k+1][i][3] = tmp[(i+1)%4];   // rotate word
            lo = key_matrix[k+1][i][3] & 0x0FU;
            hi = key_matrix[k+1][i][3] >> 4;
            for(sbox_row = 1; sbox_row < 17 && sbox_found == false; sbox_row++){
                for(sbox_column = 1; sbox_column < 17 && sbox_found == false; sbox_column++){
                    if(sbox[sbox_row][0] == hi && sbox[0][sbox_column] == lo){
                        actual_column[i] = sbox[sbox_row][sbox_column];
                        sbox_found = true;
                    }
                }
            }
            // std::cout << "\nACTUAL_COLUMN: " << actual_column[i] << std::endl;
            sbox_found = false;
            key_matrix[k+1][i][0] = key_matrix[k][i][0] ^ actual_column[i] ^ rcon[k][i];
        }

        for(i = 0; i < 4; i++){
            key_matrix[k+1][i][1] = key_matrix[k][i][1] ^ key_matrix[k+1][i][0];
        }

        for(i = 0; i < 4; i++){
            key_matrix[k+1][i][2] = key_matrix[k][i][2] ^ key_matrix[k+1][i][1];
        }

        for(i = 0; i < 4; i++){
            key_matrix[k+1][i][3] = key_matrix[k][i][3] ^ key_matrix[k+1][i][2];
        }
    }
}

void print_key_matrix(unsigned int (&key_matrix)[MAX_ROUNDS+1][4][4], int round){
    if(round == 0){
        std::cout << "\nINITIAL KEY MATRIX [0]: " << std::endl;
            for(int i = 0; i < 4; i++){
                for(int j = 0; j < 4; j++){
                    std::cout << std::hex << key_matrix[0][i][j] << "\t";
                }
            std::cout << std::endl;
        }
    }else{
        std::cout << "\nEXPANDED KEY MATRIX [" << std::dec << round << "]: " << std::endl;
        for(int i = 0; i < 4; i++){
            for(int j = 0; j < 4; j++){
                std::cout << std::hex << key_matrix[round][i][j] << "\t";
            }
            std::cout << std::endl;
        }
    }
}

void print_state_matrix(unsigned int (&state_matrix)[4][4]){
    int i, j;
    for(i = 0; i < 4; i++){
        for(j = 0; j < 4; j++){
            std::cout << std::hex << state_matrix[i][j] << "\t";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void sub_bytes(unsigned int (&state_matrix)[4][4], unsigned int sbox[17][17]){
    int i, j;
    unsigned int hi, lo;
    int sbox_row, sbox_column;
    bool sbox_found = false;

    for(i = 0; i < 4; i++){
        for(j = 0; j < 4; j++){
            lo = state_matrix[i][j] & 0x0FU;
            hi = state_matrix[i][j] >> 4;
            for(sbox_row = 1; sbox_row < 17 && sbox_found == false; sbox_row++){
                for(sbox_column = 1; sbox_column < 17 && sbox_found == false; sbox_column++){
                    if(sbox[sbox_row][0] == hi && sbox[0][sbox_column] == lo){
                        state_matrix[i][j] = sbox[sbox_row][sbox_column];
                        sbox_found = true;
                    }
                }
            }
            sbox_found = false;
        }
    }
}

void shift_row(unsigned int (&state_row)[4], int n) {
    int i, j;
    unsigned int tmp;
    for(i = 0; i < n; i++){
        tmp = state_row[0];
        for (j = 0; j < 3; j++)
            state_row[j] = state_row[j+1];
        state_row[3] = tmp;
    }
}

void shift_rows(unsigned int (&state_matrix)[4][4]){
    int i;
    for(i = 1; i < 4; i++) {
        shift_row(state_matrix[i], i);
    }   
}

void mix_columns(unsigned int (&state_matrix)[4][4]){
    unsigned int diffusion_matrix[4][4] = {{0x2U, 0x3U, 0x1U, 0x1U},
                                           {0x1U, 0x2U, 0x3U, 0x1U},
                                           {0x1U, 0x1U, 0x2U, 0x3U},
                                           {0x3U, 0x1U, 0x1U, 0x2U}};
    unsigned int xor_array[4][4];
    unsigned int hi, tmp;
    int i, j, k;
    for(i = 0; i < 4; i++){
        for(k = 0; k < 4; k++){
            for(j = 0; j < 4; j++){
                hi = state_matrix[j][i] >> 4;
                if(diffusion_matrix[k][j] == 0x2U){
                    xor_array[k][j] = state_matrix[j][i] << 1;
                    if(hi > 0x7U)
                        xor_array[k][j] ^= 0x11BU;
                }else if(diffusion_matrix[k][j] == 0x3U){
                    tmp = state_matrix[j][i];
                    xor_array[k][j] = state_matrix[j][i] << 1;
                    xor_array[k][j] ^= tmp;
                    if(hi > 0x7U)
                        xor_array[k][j] ^= 0x11BU;
                }else{
                    xor_array[k][j] = state_matrix[j][i];
                }
            }
        }
        for(k = 0; k < 4; k++)
            state_matrix[k][i] = xor_array[k][0] ^ xor_array[k][1] ^ xor_array[k][2] ^ xor_array[k][3];
    }
}

void add_round_key(unsigned int (&state_matrix)[4][4], unsigned int (&key_matrix)[MAX_ROUNDS+1][4][4], int round){
    int i, j;
    for(i = 0; i < 4; i++){
        for(j = 0; j < 4; j++){
            state_matrix[j][i] ^= key_matrix[round][j][i];
        }
    }
}

void inv_shift_row(unsigned int (&state_row)[4], int n) {
    int i, j;
    unsigned int tmp;
    for(i = 0; i < n; i++){
        tmp = state_row[3];
        for(j = 3; j > 0; j--)
            state_row[j] = state_row[j-1];
        state_row[0] = tmp;
    }
}

void inv_shift_rows(unsigned int (&state_matrix)[4][4]){
    int i;
    for(i = 1; i < 4; i++) {
        inv_shift_row(state_matrix[i], i);
    }   
}

void inv_mix_columns(unsigned int (&state_matrix)[4][4]){
    unsigned int diffusion_matrix[4][4] = {{0xeU, 0xbU, 0xdU, 0x9U},
                                           {0x9U, 0xeU, 0xbU, 0xdU},
                                           {0xdU, 0x9U, 0xeU, 0xbU},
                                           {0xbU, 0xdU, 0x9U, 0xeU}};
    unsigned int xor_array[4][4];
    unsigned int tmp;
    int i, j, k;
    for(i = 0; i < 4; i++){
        for(k = 0; k < 4; k++){
            for(j = 0; j < 4; j++){
                if(diffusion_matrix[k][j] == 0x9U){
                    tmp = state_matrix[j][i];
                    xor_array[k][j] = state_matrix[j][i] << 1;
                    if(xor_array[k][j] > 0xFFU)
                        xor_array[k][j] ^= 0x11BU;
                    xor_array[k][j] <<= 1;
                    if(xor_array[k][j] > 0xFFU)
                        xor_array[k][j] ^= 0x11BU;
                    xor_array[k][j] <<= 1;
                    if(xor_array[k][j] > 0xFFU)
                        xor_array[k][j] ^= 0x11BU;
                    xor_array[k][j] ^= tmp;
                }else if(diffusion_matrix[k][j] == 0xbU){
                    tmp = state_matrix[j][i];
                    xor_array[k][j] = state_matrix[j][i] << 1;
                    if(xor_array[k][j] > 0xFFU)
                        xor_array[k][j] ^= 0x11BU;
                    xor_array[k][j] <<= 1;
                    if(xor_array[k][j] > 0xFFU)
                        xor_array[k][j] ^= 0x11BU;
                    xor_array[k][j] ^= tmp;
                    xor_array[k][j] <<= 1;
                    if(xor_array[k][j] > 0xFFU)
                        xor_array[k][j] ^= 0x11BU;
                    xor_array[k][j] ^= tmp;
                }else if(diffusion_matrix[k][j] == 0xdU){
                    tmp = state_matrix[j][i];
                    xor_array[k][j] = state_matrix[j][i] << 1;
                    if(xor_array[k][j] > 0xFFU)
                        xor_array[k][j] ^= 0x11BU;
                    xor_array[k][j] ^= tmp;
                    xor_array[k][j] <<= 1;
                    if(xor_array[k][j] > 0xFFU)
                        xor_array[k][j] ^= 0x11BU;
                    xor_array[k][j] <<= 1;
                    if(xor_array[k][j] > 0xFFU)
                        xor_array[k][j] ^= 0x11BU;
                    xor_array[k][j] ^= tmp;
                }else{
                    tmp = state_matrix[j][i];
                    xor_array[k][j] = state_matrix[j][i] << 1;
                    if(xor_array[k][j] > 0xFFU)
                        xor_array[k][j] ^= 0x11BU;
                    xor_array[k][j] ^= tmp;
                    xor_array[k][j] <<= 1;
                    if(xor_array[k][j] > 0xFFU)
                        xor_array[k][j] ^= 0x11BU;
                    xor_array[k][j] ^= tmp;
                    xor_array[k][j] <<= 1;
                    if(xor_array[k][j] > 0xFFU)
                        xor_array[k][j] ^= 0x11BU;
                }
            }
        }
        for(k = 0; k < 4; k++)
            state_matrix[k][i] = xor_array[k][0] ^ xor_array[k][1] ^ xor_array[k][2] ^ xor_array[k][3];
    }
}

void aes_encrypt(unsigned int (&state_matrix)[4][4], unsigned int (&key_matrix)[MAX_ROUNDS+1][4][4], unsigned int sbox[17][17], int ROUNDS){
    //print_state_matrix(state_matrix);
    add_round_key(state_matrix, key_matrix, 0);
    //print_state_matrix(state_matrix);

    int i, j;
    for(i = 1; i < ROUNDS; i++){
        sub_bytes(state_matrix, sbox);
        shift_rows(state_matrix);
        mix_columns(state_matrix);
        add_round_key(state_matrix, key_matrix, i);
        //print_state_matrix(state_matrix);
    }

    sub_bytes(state_matrix, sbox);
    shift_rows(state_matrix);
    add_round_key(state_matrix, key_matrix, ROUNDS);
    //print_state_matrix(state_matrix);
}

void aes_decrypt(unsigned int (&state_matrix)[4][4], unsigned int (&key_matrix)[MAX_ROUNDS+1][4][4], unsigned int sbox_inverse[17][17], int ROUNDS){
    //print_state_matrix(state_matrix);
    add_round_key(state_matrix, key_matrix, ROUNDS);
    inv_shift_rows(state_matrix);
    sub_bytes(state_matrix, sbox_inverse);  // reverte sub_bytes com sbox inverso
    //print_state_matrix(state_matrix);

    int i;
    for(i = ROUNDS-1; i > 0; i--){
        add_round_key(state_matrix, key_matrix, i);
        inv_mix_columns(state_matrix);
        inv_shift_rows(state_matrix);
        sub_bytes(state_matrix, sbox_inverse);  // reverte sub_bytes com sbox inverso
        //print_state_matrix(state_matrix);
    }

    add_round_key(state_matrix, key_matrix, 0);
    //print_state_matrix(state_matrix);
}

void ctr_encrypt_decrypt(unsigned int (&state_matrix)[4][4], unsigned int (&key_matrix)[MAX_ROUNDS+1][4][4], unsigned int sbox[17][17], int ROUNDS){
    int i, j;
    static unsigned int counter_matrix[4][4] = {{0x0U, 0x0U, 0x0U, 0x0U},
                                                {0x0U, 0x0U, 0x0U, 0x0U},
                                                {0x0U, 0x0U, 0x0U, 0x0U},
                                                {0x0U, 0x0U, 0x0U, 0x0U}};
    unsigned int counter_aux[4][4];

    for(i = 0; i < 4; i++){
        for(j = 0; j < 4; j++){
            counter_aux[j][i] = counter_matrix[j][i];
        }
    }

    //print_state_matrix(state_matrix);
    add_round_key(counter_aux, key_matrix, 0);
    //print_state_matrix(state_matrix);

    for(i = 1; i < ROUNDS; i++){
        sub_bytes(counter_aux, sbox);
        shift_rows(counter_aux);
        mix_columns(counter_aux);
        add_round_key(counter_aux, key_matrix, i);
        //print_state_matrix(state_matrix);
    }

    sub_bytes(counter_aux, sbox);
    shift_rows(counter_aux);
    add_round_key(counter_aux, key_matrix, ROUNDS);

    for(i = 0; i < 4; i++)
        for(j = 0; j < 4; j++)
            state_matrix[j][i] ^= counter_aux[j][i];

    bool incremented = false;
    for(i = 3; i >= 0 && incremented == false; i--){
        for(j = 3; j >= 0 && incremented == false; j--){
            if(counter_matrix[j][i] != 0xFFU){
                counter_matrix[j][i]+= 0x01U;
                incremented = true;
            }
        }
    }
    //print_state_matrix(state_matrix);
}

int find_sequence_of_bytes(std::vector<unsigned int> buffer, std::vector<unsigned int> bytes) {
    auto it = std::search(buffer.begin(), buffer.end(), bytes.begin(), bytes.end());
    return it - buffer.begin();
}

char* getCmdOption(char ** begin, char ** end, const std::string & option){
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end){
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option){
    return std::find(begin, end, option) != end;
}

int main(int argc, char *argv[]){
    char* key;
    char* num_rounds;
    char* filename;
    char* outfilename;
    bool encrypt_exists = false;
    bool decrypt_exists = false;
    bool filename_exists = false;
    bool outfilename_exists = false;
    bool ecb_exists = false;
    bool ctr_exists = false;
    bool enc_or_dec = true;    // enc = 1 , dec = 0
    int alt_mode = 0;          // raw AES = 0, ecb = 1, ctr = 2

    if(cmdOptionExists(argv, argv+argc, "-c")){
        key = getCmdOption(argv, argv + argc, "-c");
        encrypt_exists = true;
    }
    
    if(cmdOptionExists(argv, argv+argc, "-d")){
        key = getCmdOption(argv, argv + argc, "-d");
        decrypt_exists = true;
        enc_or_dec = false;
    }
    
    if(cmdOptionExists(argv, argv+argc, "-f")){
        filename = getCmdOption(argv, argv + argc, "-f");
        filename_exists = true;
    }
    
    if(cmdOptionExists(argv, argv+argc, "-o")){
        outfilename = getCmdOption(argv, argv + argc, "-o");
        outfilename_exists = true;
    }

    if(!(encrypt_exists || decrypt_exists) || (encrypt_exists && decrypt_exists)){
        std::cout << RED << "Invalid action. Please insert a valid [-c/-d] <KEY> as argument." << RESET << std::endl;
        return -1;
    }
    
    if(!filename_exists){
        std::cout << RED << "Invalid action. Please insert '-f' as argument." << RESET << std::endl;
        return -1;
    }

    if(!outfilename_exists){
        std::cout << RED << "Invalid action. Please insert '-o' as argument." << RESET << std::endl;
        return -1;
    }

    if(cmdOptionExists(argv, argv+argc, "-ecb")){
        num_rounds = getCmdOption(argv, argv + argc, "-ecb");
        ecb_exists = true;
        alt_mode = 1;
    }
    
    if(cmdOptionExists(argv, argv+argc, "-ctr")){
        num_rounds = getCmdOption(argv, argv + argc, "-ctr");
        ctr_exists = true;
        alt_mode = 2;
    }

    if(!(ecb_exists || ctr_exists) || (ecb_exists && ctr_exists)){
        std::cout << RED << "Invalid action. Please insert '-ecb' or '-ctr' as argument, but not both." << RESET << std::endl;
        return -1;
    }
    
    std::ifstream file(filename, std::ios::binary);
    
    if(!file.is_open()){
        std::cout << RED << "Invalid source file. Please insert a valid -f <PATH_TO_SOURCE>." << RESET << std::endl;
        return -1;
    }
    
    if(num_rounds == NULL){
        std::cout << RED << "Invalid number of rounds. Please insert number desired." << RESET << std::endl;
        return -1;
    }

    const int ROUNDS = std::stoi(num_rounds);
    if(ROUNDS < 1 || ROUNDS > MAX_ROUNDS){
        std::cout << RED << "Rounds out of scope. Please insert a valid number of rounds [1-15]." << RESET << std::endl;
        return -1;
    }

    std::ofstream file_out(outfilename, std::ios::trunc | std::ios::binary);

    /* Finalizada análise de erro nos argumentos... */

    if(enc_or_dec)
        std::cout << "Encrypting " << GREEN << filename << RESET << " with key " << GREEN << "0x" << key << RESET << std::endl;
    else
        std::cout << "Decrypting " << GREEN << filename << RESET << " with key " << GREEN << "0x" << key << RESET << std::endl;

    if(ecb_exists)
        std::cout << "Using " << GREEN << "ECB" << RESET << " mode with " << GREEN << num_rounds << RESET << " rounds.\n" << std::endl;
    else
        std::cout << "Using " << GREEN << "CTR" << RESET << " mode with " << GREEN << num_rounds << RESET << " rounds.\n" << std::endl;

    // Input image
    std::vector<unsigned int> image; 

    // Reading the file content using the iterator
    unsigned char byte_from_file;
    while(file >> std::noskipws >> byte_from_file)
        image.push_back((unsigned int)byte_from_file);
    
    int init_index = find_sequence_of_bytes(image, flagImage);  // Find integer 255 (max-color in ppm)

    if(init_index >= image.size())
        return 1;
    
    init_index += flagImage.size() + 1; // Get next byte (after the integer sequence 255 and line feed)

    std::vector<unsigned int> buffer(image.begin() + init_index, image.end());  // Ignoring header in buffer
    std::vector<unsigned int> encrypted_image(image.begin(), image.begin() + init_index);   // Passing the header to encrypted image

    unsigned int sbox[17][17];
    unsigned int sbox_inverse[17][17];
    initialize_sbox(sbox, sbox_inverse);    // Initialize both s-box and inverse s-box (encrypt/decrypt allowed)

    unsigned int rcon[15][4];
    initialize_rcon(rcon);  // Initialize round constants

    unsigned int key_matrix[MAX_ROUNDS+1][4][4];
    
    if(set_cipher_key_matrix(key_matrix, key) == -1){
        return 1;   // If error at key...
    }

    key_expansion(key_matrix, sbox, rcon, ROUNDS);  // Expanding keys...
    
    int total_buffer_size = buffer.size();
    float percent = 0.001;
    unsigned int counter_matrix[4][4] = {{0x0U, 0x0U, 0x0U, 0x0U},
                                         {0x0U, 0x0U, 0x0U, 0x0U},
                                         {0x0U, 0x0U, 0x0U, 0x0U},
                                         {0x0U, 0x0U, 0x0U, 0x0U}};

    while (buffer.size() > 0) {
        int it = 0, padding_bytes = 0;
        while (buffer.size() < 16) {
            padding_bytes++;
            buffer.push_back(0x00U);    // Padding bytes
        }
        unsigned int matrix[4][4];
        
        for(int i = 0; i < 4; i++){
            for(int j = 0; j < 4; j++){
                matrix[j][i] = buffer[it++];    // Get 16 bytes each iteration
            }
        }

        if(enc_or_dec){
            if(total_buffer_size - buffer.size() > percent * total_buffer_size){   // Don't want to spam your terminal...
                std::cout << "Encrypting " << GREEN << "[" << (int)(percent*100) << "%]" << RESET << " ..." << std::endl;
                percent+= 0.01;
            }
            if(alt_mode == 1)
                aes_encrypt(matrix, key_matrix, sbox, ROUNDS);  // Common ECB
            else if(alt_mode == 2)
                ctr_encrypt_decrypt(matrix, key_matrix, sbox, ROUNDS);  // CTR easy encrypt/decrypt
        }
        else{
            if(total_buffer_size - buffer.size() > percent * total_buffer_size){   // Don't want to spam your terminal...
                std::cout << "Decrypting " << GREEN << "[" << (int)(percent*100) << "%]" << RESET << " ..." << std::endl;
                percent+= 0.01;
            }
            if(alt_mode == 1)
                aes_decrypt(matrix, key_matrix, sbox_inverse, ROUNDS);  // Common ECB
            else if(alt_mode == 2)
                ctr_encrypt_decrypt(matrix, key_matrix, sbox, ROUNDS);  // CTR easy encrypt/decrypt
        }
        std::vector<unsigned int> aux;
        for(int i = 0; i < 4; i++){
            for(int j = 0; j < 4; j++){
                aux.push_back(matrix[j][i]);    // Save encrypted/decrypted bytes
            }
        }
        
        buffer.erase(buffer.begin(), buffer.begin() + 16);  // Remove 16 bytes from buffer
        aux.erase(aux.end() - padding_bytes, aux.end());  // Control padding bytes (remove if exists)
        encrypted_image.insert(encrypted_image.end(), aux.begin(), aux.end());
        //break;
    }

    if(enc_or_dec){
        std::cout << "Encrypting " << GREEN << "[" << (int)(percent*100) << "%]" << RESET << " ..." << std::endl;
        std::cout << "Encryption finished! Encrypted image stored at: " << GREEN << outfilename << RESET << std::endl;
    }else{
        std::cout << "Decrypting " << GREEN << "[" << (int)(percent*100) << "%]" << RESET << " ..." << std::endl;
        std::cout << "Decryption finished! Decrypted image stored at: " << GREEN << outfilename << RESET << std::endl;
    }

    for(unsigned int x: encrypted_image)
        file_out << (unsigned char)x;       // Save output

    file_out.close();

    return 0;
}