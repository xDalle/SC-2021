#include <iostream>
#include <string>
#include <limits>
#include <algorithm>
#include <locale.h>
#include <map>

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

void decrypt_without_key(std::string ciphertext){
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

    std::map<std::string, int> F;
    std::string actual_ngram;

    for(i = 0; i < ngram_quantity; i++, ptr++){
        for(j = 0; j < NGRAM_SIZE; j++){
            ngrams[i][j] = ptr[j];
            ngrams[i][j+1] = '\0';
        }

        if(F.find(ngrams[i]) == F.end()){
            F.insert(std::make_pair(ngrams[i], 1));
        }else{
            F[ngrams[i]]++;
        }
    }

    for(auto& map_iterator : F) {
        std::cout << map_iterator.first << " - " << map_iterator.second << std::endl;
    }

}

int main()
{
    setlocale(LC_ALL, "Portuguese");
    int alt = 0, alt2 = 0;
    std::string key, plaintext, ciphertext;
    setupMatrix(matrix);
    //printMatrix(matrix);

    while(alt != 1 && alt != 2){
        std::cout << "\nO que deseja fazer?\n\n1 - Cifrar texto.\n2 - Decifrar texto.\n" << std::endl;
        std::cin >> alt;
    }

    if(alt == 2){
        while(alt2 != 1 && alt2 != 2){
            std::cout << "\nVocê sabe a key para decifragem?\n\n1 - Sim.\n2 - Não.\n" << std::endl;
            std::cin >> alt2;
        }
    }

    if(alt2 != 2){
        std::cout << "\nDigite a key: ";
        std::cin >> key;

        std::transform(key.begin(), key.end(), key.begin(), ::toupper);
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if(alt == 1){
        std::cout << "Digite o plaintext para cifrar texto: ";
        std::getline(std::cin, plaintext);

        std::transform(plaintext.begin(), plaintext.end(), plaintext.begin(), ::toupper);

        std::cout << "\n" << encrypt(plaintext, key) << std::endl;
    }else{
        std::cout << "Digite o ciphertext para decifrar texto: ";
        std::getline(std::cin, ciphertext);

        std::transform(ciphertext.begin(), ciphertext.end(), ciphertext.begin(), ::toupper);

        if(alt2 == 1){
            std::cout << "\n" << decrypt(ciphertext, key) << std::endl;
        }else{
            //std::cout << decrypt_without_key(ciphertext) << std::endl;
            decrypt_without_key(ciphertext);
        }
    }

    return 0;
}
