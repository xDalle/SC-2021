#include <iostream>
#include <string>
#include <limits>
#include <algorithm>
#include <locale>
#include <map>
#include <vector>
#include <set>
#include <string.h>

#define LETTERS 26
#define NGRAM_SIZE 3

/* https://en.wikipedia.org/wiki/Index_of_coincidence */
#define ENGLISH_IC 1.73
#define PORTUGUESE_IC 1.94


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

    /*
    for(auto& map_iterator : F) {
        std::cout << map_iterator.first << " - ";

        for (int distance: map_iterator.second) {
            std::cout << distance << ", ";
        }

        std::cout << std::endl;
    }
    */
   
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

    int k = 0, choice = -1;
    for(auto& length : lengths)
        std::cout << ++k << ": Tamanho: " << length.first << ": frequência " << length.second << std::endl;

    do {
        std::cout << "Escolha um indice do tamanho de chave: " << std::endl;
        std::cin >> choice;
    } while (choice < -1 or choice > lengths.size());
    
    auto it = lengths.begin();
    std::advance(it, choice - 1);
    return it->first;
}


void decrypt_without_key(std::string ciphertext){
    int i, j;
    int key_size = get_key_length(ciphertext);
    std::string ciphertext_treated;
    char *ptr = &ciphertext[0];

    for(i = 0; (unsigned)i < ciphertext.size(); i++){
        if(isalpha(ptr[i])){
            ciphertext_treated.push_back(ptr[i]);
        }
    }

    std::cout << "Tamanho da chave: " << key_size << std::endl;

    std::map<char, int> letter_frequency;

    for(i = 0; (unsigned)i < ciphertext_treated.size(); i++){
        if (letter_frequency.find(ciphertext_treated[i]) == letter_frequency.end())
            letter_frequency.insert(std::make_pair(ciphertext_treated[i], 1));
        else letter_frequency[ciphertext_treated[i]]++;
    }

    float IC = 0;
    for(auto e: letter_frequency) {
        IC += e.second * (e.second - 1);
    }

    IC /= (ciphertext_treated.size() * (ciphertext_treated.size() - 1));
    std::cout << "\nIC: " << IC << std::endl;

    char separate_key[key_size][(ciphertext_treated.size()/key_size)+key_size];
    int count_char = 0;

    for(i = 0; (unsigned)i < ciphertext_treated.size(); i++){
        separate_key[i % key_size][count_char] = ciphertext_treated[i];
        separate_key[i % key_size][count_char+1] = '\0';
        if((i % key_size) == (key_size - 1)){
            count_char++;
        }
    }

    /* calcular a partir das chaves separadas o IC de cada um, e verificar se a média é mais próxima de INGLES ou PORTUGUES */
    for(i = 0; i < key_size; i++){
        std::cout << separate_key[i] << std::endl;
    }

    /* para cada string deve-se calcular o IC, e deslocar o caractere para o próximo */
    /* o que mais se aproximar ao IC da língua, é o certo */

    char possible_strings[LETTERS][(ciphertext_treated.size()/key_size)+key_size];

    int key_char = 0;
    std::string actual_possible;
    float possible_IC[LETTERS];
    
    while(key_char < key_size){
        strcpy(possible_strings[0], separate_key[key_char]);
        for(i = 1; i < LETTERS; i++){
            for(j = 0; j < strlen(separate_key[key_char]); j++){
                if((separate_key[key_char][j] + i) > 'Z'){
                    possible_strings[i][j] = (separate_key[key_char][j] + i) % 'Z' + 64;
                }else{
                    possible_strings[i][j] = separate_key[key_char][j] + i;
                }
                possible_strings[i][j+1] = '\0';
            }
        }

        //std::cout << possible_strings[1] << std::endl;

        /* calcular IC e decidir a melhor das possible_strings */
        for(i = 0; i < LETTERS; i++){
            for (char c: possible_strings[i]) {
                actual_possible.push_back(c);
            }
            /* calcula IC e guarda em possible_IC[i] */
            /* reseta map */
        }

        key_char++;
    }

    /* verifica qual IC das possible_IC aproxima mais de ENGLISH_IC OU PORTUGUESE_IC */
    /* cabou essa porra */

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
            std::cout << "\nDigite a mensagem a ser decifrada: ";
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
