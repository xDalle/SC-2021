#include <iostream>
#include <string>
#include <limits>
#include <algorithm>
#include <locale>
#include <map>
#include <vector>
#include <set>

#define LETTERS 26
#define NGRAM_SIZE 3

char matrix[LETTERS+1][LETTERS+1];

void setupMatrix(char matrix[LETTERS+1][LETTERS+1]){
    int i, j, flagCounter = 1;

    matrix[0][0] = 32; // ascii [space]

    for(i = 1; i < LETTERS+1; i++){
        matrix[i][0] = matrix[0][i] = i + 64; // ascii A-Z
    }

    for(i = 1; i < LETTERS+1; i++){
        for(j = 1; j < LETTERS+1; j++){
            matrix[i][j] = j + 64 + (i-1) * flagCounter - (27-i) * !flagCounter;
            if(matrix[i][j] == 'Z'){
                flagCounter = 0;
            }
        }
        flagCounter = 1;
    }
}

void printMatrix(char matrix[LETTERS+1][LETTERS+1]){
    int i, j;
    for(i = 0; i < LETTERS+1; i++){
        for(j = 0; j < LETTERS+1; j++){
            std::cout << matrix[i][j] << " ";
        }
        std::cout << "\n";
    }
}

std::string encrypt(std::string plaintext, std::string key){
    int i = 0;
    std::string ciphertext;

    for(char c : plaintext){
        if(c < 'A' || c > 'Z')
            ciphertext.push_back(c);
        else
            ciphertext.push_back(matrix[(int)(c - 'A') + 1][(int)(key[i++ % key.size()] - 'A') + 1]);
    }

    return ciphertext;
}

std::string decrypt(std::string ciphertext, std::string key){
    int i = 0;
    std::string plaintext;

    for(char c : ciphertext){
        if(c < 'A' || c > 'Z')
            plaintext.push_back(c);
        else
            plaintext.push_back((((int)c - key[i++ % key.size()] + 26) % 26) + 'A');
    }

    return plaintext;
}

/*
    1. Determine key length
    2. Break up ciphertext (based on key length)
    3. Use frequency analysis on each part
*/
std::vector<int> get_divisors_frequency(int n) {
    std::vector<int> result = std::vector<int>();

    for (int i = 2; i <= 20; i++) {
        if (i > n) break;
        if (n % i == 0)
            result.push_back(i);
    }

    return result;
}

int get_key_length(std::string ciphertext) {
    int i, j;
    std::string ciphertext_treated;
    char *ptr = &ciphertext[0];

    for(i = 0; (unsigned)i < ciphertext.size(); i++){
        if(isalpha(ptr[i])){
            ciphertext_treated.push_back(ptr[i]);
        }
    }

    int ngram_quantity = (ciphertext_treated.size()) - NGRAM_SIZE + 1;
    char ngrams[ngram_quantity][NGRAM_SIZE];

    ptr = &ciphertext_treated[0];

    std::map<std::string, std::set<int>> F;

    for(i = 0; i < ngram_quantity; i++, ptr++){
        for(j = 0; j < NGRAM_SIZE; j++){
            ngrams[i][j] = ptr[j];
            ngrams[i][j+1] = '\0';
        }

        if(F.find(ngrams[i]) == F.end()){
            size_t pos = ciphertext_treated.find(ngrams[i], 0);
            std::vector<int> positions;
            while(pos != std::string::npos) {
                positions.push_back(pos);
                pos = ciphertext_treated.find(ngrams[i], pos + 1);
            }

            std::set<int> distances;
            for (int pos_1: positions) {
                for(int pos_2: positions) {
                    if (pos_1 - pos_2 > 0) distances.insert(std::abs(pos_1 - pos_2));
                }
            }

            if (distances.size() > 0)
                F.insert(std::make_pair(ngrams[i], distances));
        }

    }

    for(auto& map_iterator : F) {
        std::cout << map_iterator.first << " - ";

        for (int distance: map_iterator.second) {
            std::cout << distance << ", ";
        }

        std::cout << std::endl;
    }

    std::map<int, int> lengths;
    for(auto& map_iterator : F) {
        for (int distance: map_iterator.second) {
            std::vector<int> lens = get_divisors_frequency(distance);

            for (int len: lens) {
                if (lengths.find(len) == lengths.end())
                    lengths.insert(std::make_pair(len, 1));
                else lengths[len]++;
            }
        }
    }

    for(auto& length : lengths)
        std::cout << length.first << ": " << length.second << std::endl;

}


void decrypt_without_key(std::string ciphertext){
    get_key_length(ciphertext);
}

int main()
{
    setlocale(LC_ALL, "");
    int alt_menu = 0, alt_decrypt_key = 0;
    std::string key, plaintext, ciphertext;
    setupMatrix(matrix);
    //printMatrix(matrix);

    while(alt_menu != 3){
        while(alt_menu < 1 || alt_menu > 3){
            std::cout << "\nO que deseja fazer?\n\n1 - Cifrar mensagem.\n2 - Decifrar mensagem.\n3 - Sair.\n" << std::endl;
            std::cin >> alt_menu;
        }

        if(alt_menu == 3){
            return 1;
        }

        if(alt_menu == 2){
            while(alt_decrypt_key != 1 && alt_decrypt_key != 2){
                std::cout << "\nVoce sabe a chave para decifragem?\n\n1 - Sim.\n2 - Nao.\n" << std::endl;
                std::cin >> alt_decrypt_key;
            }
        }

        if(alt_decrypt_key != 2){
            std::cout << "\nDigite a chave: ";
            std::cin >> key;

            std::transform(key.begin(), key.end(), key.begin(), ::toupper);
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if(alt_menu == 1){
            std::cout << "Digite a mensagem a ser cifrada: ";
            std::getline(std::cin, plaintext);

            std::transform(plaintext.begin(), plaintext.end(), plaintext.begin(), ::toupper);

            std::cout << "\nMensagem cifrada: " << encrypt(plaintext, key) << std::endl;
        }else{
            std::cout << "Digite a mensagem a ser decifrada: ";
            std::getline(std::cin, ciphertext);

            std::transform(ciphertext.begin(), ciphertext.end(), ciphertext.begin(), ::toupper);

            if(alt_decrypt_key == 1){
                std::cout << "\nMensagem decifrada: " << decrypt(ciphertext, key) << std::endl;
            }else{
                //std::cout << decrypt_without_key(ciphertext) << std::endl;
                decrypt_without_key(ciphertext);
            }
        }

        alt_menu = alt_decrypt_key = 0; //Reseta op��es do menu
    }
    return 0;
}
