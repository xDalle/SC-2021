#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <iterator>
#include <algorithm>

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

int main(int argc, char *argv[]){
    std::ifstream file("tests/teste.jpeg", std::ios::binary);

    // Prepare iterator pairs to iterate the file content
    std::istream_iterator<unsigned char> begin(file), end;

    // Reading the file content using the iterator
    std::vector<unsigned char> buffer(begin,end);

    int init_index = 0;
    
    auto it = std::find(buffer.begin(), buffer.end(), 0xFFU);
    while(it < buffer.end()) {
        int i = it - buffer.begin();

        if (buffer[i + 1] == 0xDAU) {
            init_index = i + 2;
            break;
        } else it = std::find(++it, buffer.end(), 0xFFU);
    }

    std::copy(buffer.begin() + init_index - 2, buffer.end() + 2, buffer.begin());
    return 0;
}