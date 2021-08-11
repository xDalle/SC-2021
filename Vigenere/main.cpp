#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <algorithm>
#include <locale>
#include <map>
#include <vector>
#include <set>
#include <cmath>
#include <string.h>
#include <stdlib.h>
#include <limits>

#define LETTERS 26
#define NGRAM_SIZE 3
#define MIN_KEY_LENGTH 2
#define MAX_KEY_LENGTH 20
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define RESET   "\x1b[0m"

char matrix[LETTERS+1][LETTERS+1];

/* Inicializando tabula recta */
void setupMatrix(char matrix[LETTERS+1][LETTERS+1]){
    int i, j, flag_counter = 1;

    matrix[0][0] = 32; // ascii [space]

    for(i = 1; i < LETTERS+1; i++){
        matrix[i][0] = matrix[0][i] = i + 'A' - 1; // ascii A-Z
    }

    for(i = 1; i < LETTERS+1; i++){
        for(j = 1; j < LETTERS+1; j++){
            matrix[i][j] = j + 'A' - 1 + (i-1) * flag_counter - (LETTERS+1-i) * !flag_counter;
            if(matrix[i][j] == 'Z'){
                flag_counter = 0;
            }
        }
        flag_counter = 1;
    }
}

/* Processo de cifragem de um texto limpo */
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

/* Processo de decifragem de um criptograma */
std::string decrypt(std::string ciphertext, std::string key){
    int i = 0;
    std::string plaintext;
    
    /* Evita memory leak ao realizar posteriores cálculos com key = 0 */
    if(key.size() == 0){
        std::cout << RED << "\nMensagem muito pequena para decifragem sem chave!" << RESET << std::endl;
        return " ";
    }

    for(char c : ciphertext){
        if(c < 'A' || c > 'Z')
            plaintext.push_back(c);
        else
            plaintext.push_back((((int)c - key[i++ % key.size()] + LETTERS) % LETTERS) + 'A');
    }

    return plaintext;
}

/*
    1. Determinar o tamanho da chave
    2. Utilizar análise de frequência para subsets do criptograma
    3. Agora tenho a chave :)
*/

/* Retorna divisores de n */
std::vector<int> get_divisors(int n) {
    int i;
    std::vector<int> result = std::vector<int>();

    for (i = MIN_KEY_LENGTH; i <= MAX_KEY_LENGTH; i++) {
        if (i > n) break;
        if (n % i == 0)
            result.push_back(i);
    }

    return result;
}

/* Usuário pode interferir na função, escolhendo diretamente o tamanho da chave, caso processo automático se engane :( */
int get_key_length(std::string ciphertext, bool user_choose_keysize) {
    int i, j, k;
    std::string ciphertext_treated;
    char *ptr = &ciphertext[0];

    /* Deixa só símbolos do alfabeto */
    for(i = 0; (unsigned)i < ciphertext.size(); i++){
        if(isalpha(ptr[i])){
            ciphertext_treated.push_back(ptr[i]);
        }
    }

    /* Calcula N-Gramas de repetições */
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

    std::map<int, int> lengths;
    for(auto& map_iterator : F) {
        for (int distance: map_iterator.second) {
            std::vector<int> lens = get_divisors(distance);

            for (int len: lens) {
                if (lengths.find(len) == lengths.end())
                    lengths.insert(std::make_pair(len, 1));
                else lengths[len]++;
            }
        }
    }

    /* Caso usuário escolha o tamanho da chave, é importante mostrar a frequência NGRAMs */
    if(user_choose_keysize){
        std::cout << GREEN << "\n~~ Frequência dos n-gramas [" << NGRAM_SIZE << "] " << "e prováveis tamanhos de chave ~~\n" << RESET << std::endl;
        for(auto& length : lengths)
            std::cout << GREEN << "[" << length.first << "] -> "<< RESET << "Frequência: " << length.second << std::endl;
    }

    float ic_by_key_length[MAX_KEY_LENGTH] = {0.0};
    float ic_all_key[MAX_KEY_LENGTH] = {0.0};
    float biggest_ic = -1.0;
    int probable_key = -1;
    std::string separate_key[MAX_KEY_LENGTH];
    std::string actual_possible;
    std::map<char, int> letter_frequency;
    int key_it = 0;

    /* Cálculo do índice de coincidência */
    for(j = MIN_KEY_LENGTH; j <= MAX_KEY_LENGTH; j++){
        for(i = 0; (unsigned)i < ciphertext_treated.size(); i++){
            separate_key[i % j].push_back(ciphertext_treated[i]);
        }

        for(i = 0; i < j; i++){
            actual_possible = separate_key[i];
            for(k = 0; (unsigned)k < actual_possible.size(); k++){
                if (letter_frequency.find(actual_possible[k]) == letter_frequency.end())
                    letter_frequency.insert(std::make_pair(actual_possible[k], 1));
                else letter_frequency[actual_possible[k]]++;
            }

            for(auto e: letter_frequency) {
                ic_by_key_length[i] += e.second * (e.second - 1);
            }

            ic_by_key_length[i] /= (actual_possible.size() * (actual_possible.size() - 1));
            ic_all_key[key_it] += ic_by_key_length[i];
            letter_frequency.clear();
        }

        ic_all_key[key_it] /= j;        // Média
        key_it++;

        for(i = 0; i < MAX_KEY_LENGTH-1; i++){
            separate_key[i].clear();
        }
    }

    /* Caso usuário escolha o tamanho da chave, é importante mostrar o índice de coincidência */
    if(user_choose_keysize){
        std::cout << GREEN << "\n~~ Índice de coincidência das chaves de cada tamanho ~~" << RESET << std::endl;
        for(i = 0; i < MAX_KEY_LENGTH-1; i++){
            if(!std::isnan(ic_all_key[i]))
                std::cout << GREEN << "\n[" << i+2 << "] -> " << RESET << ic_all_key[i];
            else
                std::cout << GREEN << "\n[" << i+2 << "] -> " << RESET << "inviável!";
        }
        std::cout << RED << "\n\nEscolha o tamanho da chave: " << RESET;
        std::cin >> probable_key;
    }else{
        for(i = 0; i < MAX_KEY_LENGTH-1; i++){
            if(ic_all_key[i] > biggest_ic){
                biggest_ic = ic_all_key[i];
                probable_key = i+2;
            }
        }
    }

    return probable_key;
}

/* Faz mágica */
std::string decrypt_without_key(std::string ciphertext, bool user_choose_keysize){
    int i, j, k;
    int key_size = get_key_length(ciphertext, user_choose_keysize);
    std::string ciphertext_treated;
    std::string password;
    char *ptr = &ciphertext[0];

    float english_letter_frequency[LETTERS] = {0.08167, 0.01492, 0.02782, 0.04253, 0.12702, 0.02228, 0.02015,
                                                0.06094, 0.06966, 0.00153, 0.00772, 0.04025, 0.02406, 0.06749,
                                                0.07507, 0.01929, 0.00095, 0.05987, 0.06327, 0.09056,
                                                0.02758, 0.00978, 0.02360, 0.0015, 0.01974, 0.00074};

    float portuguese_letter_frequency[LETTERS] = {0.1463, 0.0104, 0.0388, 0.0499, 0.1257, 0.0102, 0.013,
                                                    0.0128, 0.0618, 0.004, 0.0002, 0.0278, 0.0474, 0.0505,
                                                    0.1073, 0.0252, 0.012, 0.0653, 0.0781, 0.0434,
                                                    0.0463, 0.0167, 0.0001, 0.0021, 0.0001, 0.0047};

    for(i = 0; (unsigned)i < ciphertext.size(); i++){
        if(isalpha(ptr[i])){
            ciphertext_treated.push_back(ptr[i]);
        }
    }

    std::string separate_key[key_size];
    std::string actual_string;

    for(i = 0; (unsigned)i < ciphertext_treated.size(); i++){
        separate_key[i % key_size].push_back(ciphertext_treated[i]);
    }

    std::map<char, float> letter_frequency;
    float coset_english[LETTERS] = {0.0};
    float chi_square_english[key_size];
    int index_of_key_english[key_size];
    float sum_english = 0.0;
    float coset_portuguese[LETTERS] = {0.0};
    float chi_square_portuguese[key_size];
    int index_of_key_portuguese[key_size];
    float sum_portuguese = 0.0;

    int coset_iterator = 0;

    /* calcular a partir das chaves separadas o IC de cada um, e verificar se a média é mais próxima de INGLES ou PORTUGUES */
    for(j = 0; j < key_size; j++){
        chi_square_english[j] = chi_square_portuguese[j] = std::numeric_limits<int>::max();
        index_of_key_english[j] = index_of_key_portuguese[j] = -1;
        actual_string = separate_key[j];
        for(k = 0; k < LETTERS; k++){
            for(char c = 'A'; c <= 'Z'; c++){
                letter_frequency.insert(std::make_pair(c, 0));
            }
            coset_english[k] = coset_portuguese[k] = 0.0;
            //std::cout << "VALIDATE STRING [" << k << "] = " << actual_string << std::endl;
            for(i = 0; (unsigned)i < actual_string.size(); i++){
                letter_frequency[actual_string[i]]+= 1.0;
            }

            for(auto e: letter_frequency) {
                e.second /= (float)actual_string.size();
                coset_english[k] += (pow((e.second - english_letter_frequency[coset_iterator]), 2))/(english_letter_frequency[coset_iterator]);
                coset_portuguese[k] += (pow((e.second - portuguese_letter_frequency[coset_iterator]), 2))/(portuguese_letter_frequency[coset_iterator]);
                coset_iterator++;
            }

            coset_iterator = 0;
            letter_frequency.clear();

            for(i = 0; (unsigned)i < actual_string.size(); i++){
                if(actual_string[i] - 1 < 'A'){
                    actual_string[i] = 'Z';
                }else{
                    actual_string[i]--;
                }
            }

            if(coset_english[k] < chi_square_english[j]){
                chi_square_english[j] = coset_english[k];
                index_of_key_english[j] = k;
            }

            if(coset_portuguese[k] < chi_square_portuguese[j]){
                chi_square_portuguese[j] = coset_portuguese[k];
                index_of_key_portuguese[j] = k;
            }
        }
    }

    for(i = 0; i < key_size; i++){
        sum_english+= chi_square_english[i];
        sum_portuguese+= chi_square_portuguese[i];
    }

    sum_english/= key_size;
    sum_portuguese/= key_size;

    if(sum_english < sum_portuguese){
        for(i = 0; i < key_size; i++){
            password.push_back((char)index_of_key_english[i]+'A');
        }
    }else{
        for(i = 0; i < key_size; i++){
            password.push_back((char)index_of_key_portuguese[i]+'A');
        }
    }

    /* Agora, identificamos senhas com palavras replicadas, tal como a senha inicial escolhida pelo usuário
       seja Arara, de tamanho 5, e, sem a identificação da replicação, o programa retorna a senha como sendo 
       AraraAraraArara, de tamanho 15. O processo de decifração ocorre com sucesso, do mesmo jeito, mas
       decidimos tratar por estética, para apresentar a senha escolhida ao encerramento do programa.
                                                                                                            */

    std::vector<int> password_divisors = get_divisors(password.size());

    password_divisors.pop_back();

    std::string replicated_analyzer;
    bool replicated = true;
    int save_replicated_size = 0;

    for (auto i = password_divisors.begin(); i != password_divisors.end(); i++){
        replicated = true;
        replicated_analyzer.append(password, 0, *i);
        for(j = replicated_analyzer.size(); j < password.size(); j++){
            if(replicated_analyzer[j % replicated_analyzer.size()] != password[j]){
                replicated = false;
                break;
            }
        }
        if(replicated){
            save_replicated_size = replicated_analyzer.size();
            break;
        }
        replicated_analyzer.clear();
    }

    std::string original_password;

    if(replicated){
        original_password.append(password, 0, save_replicated_size);
        std::cout << GREEN << "\nTamanho da chave: " << RESET << original_password.size() << std::endl;
        return original_password;
    }

    std::cout << GREEN << "\nTamanho da chave: " << RESET << password.size() << std::endl;
    return password;
}

int main(int argc, char *argv[]){
    setlocale(LC_ALL, "");
    int alternative_decrypt_key = 0;
    int decrypt_ok = 2;
    bool user_choose_keysize = false;
    std::string key, plaintext, ciphertext;
    std::string alternative = (std::string)argv[1];
    char char_of_file;
    std::ifstream file;

    setupMatrix(matrix);

    if(alternative != "-c" && alternative != "-d"){
        std::cout << "\n\tExecute da seguinte forma: ./vigenere.exe {-c, -d} <PATH_TO_FILE>\n" << std::endl;
        return -1;
    }

    file.open(argv[2]);
    if(!file.is_open()){
        std::cout << "\n\tErro ao abrir arquivo, utilize <PATH_TO_FILE> válido.\n" << std::endl;
        return -2;
    }

    if(alternative == "-d"){
        while(alternative_decrypt_key != 1 && alternative_decrypt_key != 2){
            std::cout << "\n\tVocê possui a chave para decifragem?\n\n\t[1] Sim.\n\t[2] Não.\n\n\t";
            std::cin >> alternative_decrypt_key;
        }
    }

    if(alternative_decrypt_key != 2){
        std::cout << "\n\tDigite a chave de tamanho [2..20]: \n\n\t";
        std::cin >> key;

        std::transform(key.begin(), key.end(), key.begin(), ::toupper);
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if(alternative == "-c"){
        while(file >> std::noskipws >> char_of_file){
            plaintext += char_of_file;
        }

        std::cout << GREEN << "\nTexto a ser cifrado: " << RESET << plaintext << std::endl;
        std::transform(plaintext.begin(), plaintext.end(), plaintext.begin(), ::toupper);
        std::cout << GREEN << "\nMensagem cifrada, utilizando a chave " << key << ": " << RESET << encrypt(plaintext, key) << std::endl;
    }else{
        while(file >> std::noskipws >> char_of_file){
            ciphertext += char_of_file;
        }

        std::cout << GREEN << "\nTexto a ser decifrado: " << RESET << ciphertext << std::endl;
        std::transform(ciphertext.begin(), ciphertext.end(), ciphertext.begin(), ::toupper);
        if(alternative_decrypt_key == 1){
            std::cout << GREEN << "\nMensagem decifrada: " << RESET << decrypt(ciphertext, key) << std::endl;
        }else{
            while(decrypt_ok != 1){
                key = decrypt_without_key(ciphertext, user_choose_keysize);
                std::cout << GREEN << "\nKey: " << RESET <<  key << std::endl;
                std::cout << GREEN << "\nMensagem decifrada: " << RESET << decrypt(ciphertext, key) << std::endl;
                std::cout << "\n\tAcertei? :)\n\n\t[1] Sim.\n\t[2] Não.\n\n\t" << std::endl;
                std::cin >> decrypt_ok;
                if(decrypt_ok != 1){
                    std::cout << RED << "\nEscolha um possível tamanho da chave, com base nos N-GRAMAS e IC-VALUES." << RESET << std::endl;
                    /* Já que errei, escolha o tamanho da chave aí usuário... */
                    user_choose_keysize = true;
                }
            }
        }
    }

    file.close();
    return 0;
}
