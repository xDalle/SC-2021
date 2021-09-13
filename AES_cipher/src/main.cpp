#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <cstdint>
#include <string>

#define ROUNDS 10

void initialize_sbox(unsigned int (&sbox)[17][17], unsigned int (&sbox_inverse)[17][17]){
    int i, j;
    unsigned int rows_and_columns = sbox[0][0] = 0x0;
    sbox_inverse[0][0] = 0x1;

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
        rcon[i][1] = rcon[i][2] = rcon[i][3] = 0x0;
        if(round == 1)
            rcon[i][0] = 0x1;
        else if(round > 1 && rcon[i-1][0] < 0x80)
            rcon[i][0] = rcon[i-1][0] << 1;
        else if(round > 1 && rcon[i-1][0] >= 0x80)
            rcon[i][0] = (rcon[i-1][0] << 1) ^ 0x11B;
        round++;
    }
}

void print_sbox(unsigned int sbox[17][17]){
    int i, j;
    if(sbox[0][0] == 0x0)
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

int set_cipher_key_matrix(unsigned int (&key_matrix)[ROUNDS+1][4][4], char *key_argv){
    std::string key = key_argv;
    bool invalid_char = false;
    int i, j, key_it = 0;
    char *ptr_key = &key[0];
    std::string tmp;
    unsigned int key_array[16];

    if(key.size() != 32){
        std::cout << "Key size must be 32 [128-bit]." << std::endl;
        return -1;
    }

    for(i = 0; i < key.size(); i++){
        if(key[i] < 48){    //inferior a zero em ascii
            std::cout << "Invalid character passed at key, It should be [0-9A-Fa-f]." << std::endl;
            return -1;
        }else if(key[i] > 57 && key[i] < 65){   //inferior a A em ascii
            std::cout << "Invalid character passed at key, It should be [0-9A-Fa-f]." << std::endl;
            return -1;
        }else if(key[i] > 70 && key[i] < 97){   // maior que F, mas menor que a
            std::cout << "Invalid character passed at key, It should be [0-9A-Fa-f]." << std::endl;
            return -1;
        }else if(key[i] > 102){ // maior que f
            std::cout << "Invalid character passed at key, It should be [0-9A-Fa-f]." << std::endl;
            return -1;
        }
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

void key_expansion(unsigned int (&key_matrix)[ROUNDS+1][4][4], unsigned int sbox[17][17], unsigned int rcon[15][4]){
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
            lo = key_matrix[k+1][i][3] & 0x0F;
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

void print_key_matrix(unsigned int (&key_matrix)[ROUNDS+1][4][4], int round){
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
            lo = state_matrix[i][j] & 0x0F;
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
    unsigned int diffusion_matrix[4][4] = {{0x2, 0x3, 0x1, 0x1},
                                           {0x1, 0x2, 0x3, 0x1},
                                           {0x1, 0x1, 0x2, 0x3},
                                           {0x3, 0x1, 0x1, 0x2}};
    unsigned int xor_array[4][4];
    unsigned int hi, tmp;
    int i, j, k;
    for(i = 0; i < 4; i++){
        for(k = 0; k < 4; k++){
            for(j = 0; j < 4; j++){
                hi = state_matrix[j][i] >> 4;
                if(diffusion_matrix[k][j] == 0x2){
                    xor_array[k][j] = state_matrix[j][i] << 1;
                    if(hi > 0x7)
                        xor_array[k][j] ^= 0x11B;
                }else if(diffusion_matrix[k][j] == 0x3){
                    tmp = state_matrix[j][i];
                    xor_array[k][j] = state_matrix[j][i] << 1;
                    xor_array[k][j] ^= tmp;
                    if(hi > 0x7)
                        xor_array[k][j] ^= 0x11B;
                }else{
                    xor_array[k][j] = state_matrix[j][i];
                }
            }
        }
        for(k = 0; k < 4; k++)
            state_matrix[k][i] = xor_array[k][0] ^ xor_array[k][1] ^ xor_array[k][2] ^ xor_array[k][3];
    }
}

void add_round_key(unsigned int (&state_matrix)[4][4], unsigned int (&key_matrix)[ROUNDS+1][4][4], int round){
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
    unsigned int diffusion_matrix[4][4] = {{0xe, 0xb, 0xd, 0x9},
                                           {0x9, 0xe, 0xb, 0xd},
                                           {0xd, 0x9, 0xe, 0xb},
                                           {0xb, 0xd, 0x9, 0xe}};
    unsigned int xor_array[4][4];
    unsigned int tmp;
    int i, j, k;
    for(i = 0; i < 4; i++){
        for(k = 0; k < 4; k++){
            for(j = 0; j < 4; j++){
                if(diffusion_matrix[k][j] == 0x9){
                    tmp = state_matrix[j][i];
                    xor_array[k][j] = state_matrix[j][i] << 1;
                    if(xor_array[k][j] > 0xFF)
                        xor_array[k][j] ^= 0x11B;
                    xor_array[k][j] <<= 1;
                    if(xor_array[k][j] > 0xFF)
                        xor_array[k][j] ^= 0x11B;
                    xor_array[k][j] <<= 1;
                    if(xor_array[k][j] > 0xFF)
                        xor_array[k][j] ^= 0x11B;
                    xor_array[k][j] ^= tmp;
                }else if(diffusion_matrix[k][j] == 0xb){
                    tmp = state_matrix[j][i];
                    xor_array[k][j] = state_matrix[j][i] << 1;
                    if(xor_array[k][j] > 0xFF)
                        xor_array[k][j] ^= 0x11B;
                    xor_array[k][j] <<= 1;
                    if(xor_array[k][j] > 0xFF)
                        xor_array[k][j] ^= 0x11B;
                    xor_array[k][j] ^= tmp;
                    xor_array[k][j] <<= 1;
                    if(xor_array[k][j] > 0xFF)
                        xor_array[k][j] ^= 0x11B;
                    xor_array[k][j] ^= tmp;
                }else if(diffusion_matrix[k][j] == 0xd){
                    tmp = state_matrix[j][i];
                    xor_array[k][j] = state_matrix[j][i] << 1;
                    if(xor_array[k][j] > 0xFF)
                        xor_array[k][j] ^= 0x11B;
                    xor_array[k][j] ^= tmp;
                    xor_array[k][j] <<= 1;
                    if(xor_array[k][j] > 0xFF)
                        xor_array[k][j] ^= 0x11B;
                    xor_array[k][j] <<= 1;
                    if(xor_array[k][j] > 0xFF)
                        xor_array[k][j] ^= 0x11B;
                    xor_array[k][j] ^= tmp;
                }else{
                    tmp = state_matrix[j][i];
                    xor_array[k][j] = state_matrix[j][i] << 1;
                    if(xor_array[k][j] > 0xFF)
                        xor_array[k][j] ^= 0x11B;
                    xor_array[k][j] ^= tmp;
                    xor_array[k][j] <<= 1;
                    if(xor_array[k][j] > 0xFF)
                        xor_array[k][j] ^= 0x11B;
                    xor_array[k][j] ^= tmp;
                    xor_array[k][j] <<= 1;
                    if(xor_array[k][j] > 0xFF)
                        xor_array[k][j] ^= 0x11B;
                }
            }
        }
        for(k = 0; k < 4; k++)
            state_matrix[k][i] = xor_array[k][0] ^ xor_array[k][1] ^ xor_array[k][2] ^ xor_array[k][3];
    }
}

void aes_encrypt(unsigned int (&state_matrix)[4][4], unsigned int (&key_matrix)[ROUNDS+1][4][4], unsigned int sbox[17][17]){
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
    for(i = 0; i < 4; i++){
        for(j = 0; j < 4; j++){
            if(state_matrix[j][i] == 0xD9 || state_matrix[j][i] == 0xDA){
                state_matrix[j][i] = 0x0;
            }
        }
    }
    //print_state_matrix(state_matrix);
}

void aes_decrypt(unsigned int (&state_matrix)[4][4], unsigned int (&key_matrix)[ROUNDS+1][4][4], unsigned int sbox_inverse[17][17]){
    print_state_matrix(state_matrix);
    add_round_key(state_matrix, key_matrix, ROUNDS);
    inv_shift_rows(state_matrix);
    sub_bytes(state_matrix, sbox_inverse);  // reverte sub_bytes com sbox inverso
    print_state_matrix(state_matrix);

    int i;
    for(i = ROUNDS-1; i > 0; i--){
        add_round_key(state_matrix, key_matrix, i);
        inv_mix_columns(state_matrix);
        inv_shift_rows(state_matrix);
        sub_bytes(state_matrix, sbox_inverse);  // reverte sub_bytes com sbox inverso
        print_state_matrix(state_matrix);
    }

    add_round_key(state_matrix, key_matrix, 0);
    print_state_matrix(state_matrix);
}

int main(int argc, char *argv[]){
    std::ifstream file("tests/br.jpeg", std::ios::binary);
    std::ofstream file_out("tests/out.jpeg", std::ios::trunc | std::ios::binary);

    // Prepare iterator pairs to iterate the file content
    //std::istream_iterator<unsigned char> begin(file), end;
    std::vector<unsigned int> image; 

    // Reading the file content using the iterator
    unsigned char byte_from_file;
    while(file >> std::noskipws >> byte_from_file)
        image.push_back((unsigned int)byte_from_file);
    
    int init_index = 0;
    auto it = std::find(image.begin(), image.end(), 0xFFU);
    while(it < image.end()) {
        int i = it - image.begin();

        if (image[i + 1] == 0xDAU) {
            init_index = i + 2;
            break;
        } else it = std::find(++it, image.end(), 0xFFU);
    }

    std::vector<unsigned int> buffer(image.begin() + init_index, image.end() - 2);
    std::vector<unsigned int> encrypted_image(image.begin(), image.begin() + init_index);

    unsigned int sbox[17][17];
    unsigned int sbox_inverse[17][17];
    initialize_sbox(sbox, sbox_inverse);

    unsigned int rcon[15][4];
    initialize_rcon(rcon);

    unsigned int key_matrix[ROUNDS+1][4][4];
    
    if(set_cipher_key_matrix(key_matrix, argv[1]) == -1){
        return 1;
    }

    key_expansion(key_matrix, sbox, rcon);
    
    while (buffer.size() > 0) {
        int it = 0, remover = 0;
        while (buffer.size() < 16) {
            remover++;
            buffer.push_back(0x00U);
        }
        unsigned int matrix[4][4];
        /*unsigned int matrix[4][4] = {buffer[0], buffer[4], buffer[8], buffer[12]
                                    , buffer[1], buffer[5], buffer[9], buffer[13]
                                    , buffer[2], buffer[6], buffer[10], buffer[14]
                                    , buffer[3], buffer[7], buffer[11], buffer[15]};*/
        
        for(int i = 0; i < 4; i++){
            for(int j = 0; j < 4; j++){
                matrix[j][i] = buffer[it++];
            }
        }
        it = 0;
        aes_encrypt(matrix, key_matrix, sbox);

        std::vector<unsigned int> aux;
        for(int i = 0; i < 4; i++){
            for(int j = 0; j < 4; j++){
                aux.push_back(matrix[j][i]);
            }
        }
        
        buffer.erase(buffer.begin(), buffer.begin() + 16);
        aux.erase(aux.end() - remover, aux.end());
        encrypted_image.insert(encrypted_image.end(), aux.begin(), aux.end());
        aux.clear();
        //break;
    }   

    encrypted_image.push_back(0xFFU);
    encrypted_image.push_back(0xD9U);
    //std::cout << std::hex << 0x0CU << std::endl;

    //std::cout << "image size: " << image.size() << ", encrypted size: " <<  encrypted_image.size() << std::endl;

    for (unsigned int x: encrypted_image) {
        std::cout << std::hex << x << " ";
        file_out << (unsigned char) x;
    }

    file_out.close();

    //std::cout << "buffer: " << std::dec << buffer.size() << std::endl;
    //std::cout << std::hex << buffer[450824] << std::endl;

    //unsigned char char_of_file;
    //std::fstream debug("tests/debug.txt");

    /* std::string image_data;
    std::string image_data_encrypted;
    while(file >> std::noskipws >> char_of_file){
        image_data.push_back(char_of_file);
        debug << std::hex << (unsigned int)char_of_file;
    }

    std::ofstream image_encrypted("tests/out.jpeg", std::ios::trunc | std::ios::binary);
    
    for(int i = 0; i < image_data.size(); i++){
        if (image_data[i] == (unsigned char)0xFFU) {
            std::cout << "index: " << i << std::endl;
        }
        //image_encrypted << (unsigned char)image_data[i]; 
    }

    image_encrypted.close(); */

    /* resto */

    // unsigned int sbox[17][17];
    // unsigned int sbox_inverse[17][17];
    // initialize_sbox(sbox, sbox_inverse);

    //print_sbox(sbox);
    //print_sbox(sbox_inverse);

    // unsigned int rcon[15][4];
    // initialize_rcon(rcon);

    //print_rcon(rcon);
    
    // unsigned int key_matrix[ROUNDS+1][4][4];
    
    // if(set_cipher_key_matrix(key_matrix, argv[1]) == -1){
    //     return 1;
    // }

    //print_key_matrix(key_matrix, 0);

    //key_expansion(key_matrix, sbox, rcon);

    //for(int teste = 1; teste != ROUNDS+1; teste++)
        //print_key_matrix(key_matrix, teste);

    //unsigned int state[4][4];    // actual state (vector 128-bit separation)
    // unsigned int kstate[4][4];
    //std::ifstream encrypt_test("src/encrypt_test.txt", std::ios::binary);
    // std::ifstream decrypt_test("src/decrypt_test.txt", std::ios::binary);

    // for(int i = 0; i < 4; i++){
    //      for(int j = 0; j < 4; j++){
    //          encrypt_test >> std::skipws >> std::hex >> state[j][i];
    //          //decrypt_test >> std::skipws >> std::hex >> kstate[j][i];
    //      }
    // }
    // encrypt_test.close();
    // decrypt_test.close();

    //std::cout << "AES ENCRYPTING...\n" << std::endl;
    //aes_encrypt(state, key_matrix, sbox);

    //std::cout << "AES DECRYPTING...\n" << std::endl;
    //aes_decrypt(kstate, key_matrix, sbox_inverse);

    return 0;
}