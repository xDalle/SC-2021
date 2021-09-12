#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <cstdint>
#define ROUNDS 10

/*
    1- AddRoundKey      1x


    1- SubBytes     }
    2- ShiftRows     }   LOOP(X)
    3- MixColumns    }
    4- AddRoundKey  }


    1- SubBytes     }
    2- ShiftRows     }  1x
    3- AddRoundKey  }
*/

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
            sbox_inverse_file >> std::hex >> sbox_inverse[i][j];
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

void set_cipher_key_matrix(unsigned int (&key_matrix)[ROUNDS+1][4][4]){
    int i, j;
    std::ifstream key_file("src/key.txt");

    for(i = 0; i < 4; i++)
        for(j = 0; j < 4; j++)
            key_file >> std::hex >> key_matrix[0][j][i];

    key_file.close();
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

void aes_encrypt(unsigned int (&state_matrix)[4][4], unsigned int (&key_matrix)[ROUNDS+1][4][4], unsigned int sbox[17][17], unsigned int rcon[15][4]){
    key_expansion(key_matrix, sbox, rcon);
    
    add_round_key(state_matrix, key_matrix, 0);
    print_state_matrix(state_matrix);
    std::cout << std::endl;

    int i;
    for(i = 1; i < ROUNDS; i++){
        sub_bytes(state_matrix, sbox);
        shift_rows(state_matrix);
        mix_columns(state_matrix);
        add_round_key(state_matrix, key_matrix, i);
        print_state_matrix(state_matrix);
        std::cout << std::endl;
    }

    sub_bytes(state_matrix, sbox);
    shift_rows(state_matrix);
    add_round_key(state_matrix, key_matrix, ROUNDS);
    print_state_matrix(state_matrix);
    std::cout << std::endl;
}

int main(int argc, char *argv[]){
    std::ifstream file("tests/teste.jpeg", std::ios::binary);

    // Prepare iterator pairs to iterate the file content
    std::istream_iterator<unsigned char> begin(file), end;

    // Reading the file content using the iterator
    std::vector<unsigned int> buffer(begin,end);

    int init_index = 0;
    
    auto it = std::find(buffer.begin(), buffer.end(), 0xFFU);
    while(it < buffer.end()) {
        int i = it - buffer.begin();

        if (buffer[i + 1] == 0xDAU) {
            init_index = i + 2;
            break;
        } else it = std::find(++it, buffer.end(), 0xFFU);
    }

    std::copy(buffer.begin() + init_index, buffer.end() - 2, buffer.begin());

    /* std::cout << "buffer: ";
    std::cout << std::hex << buffer[1]; */

    // for (unsigned int x: buffer)
    //     std::cout << std::hex << x << " ";

    file.close();

    /* resto */

    unsigned int sbox[17][17];
    unsigned int sbox_inverse[17][17];
    initialize_sbox(sbox, sbox_inverse);

    //print_sbox(sbox);
    //print_sbox(sbox_inverse);

    unsigned int rcon[15][4];
    initialize_rcon(rcon);

    //print_rcon(rcon);

    unsigned int key_matrix[ROUNDS+1][4][4];
    set_cipher_key_matrix(key_matrix);

    //print_key_matrix(key_matrix, 0);

    //key_expansion(key_matrix, sbox, rcon);

    //for(int teste = 1; teste != ROUNDS+1; teste++)
        //print_key_matrix(key_matrix, teste);

    unsigned int state[4][4];    // actual state (vector 128-bit separation)
    std::ifstream state_test("src/state_test2.txt");

    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            state_test >> std::hex >> state[j][i];

    state_test.close();

    // sub_bytes(state, sbox);
    // shift_rows(state);
    // mix_columns(state);
    // print_state_matrix(state);
    // add_round_key(state, key_matrix, 1);
    aes_encrypt(state, key_matrix, sbox, rcon);
    return 0;
}