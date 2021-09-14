Autores:  
Lucas Dalle Rocha - 17/0016641  
Leonardo Rodrigues de Souza - 170060543  

Organização dos arquivos:  
* docs -> Descritivo.pdf (relatório acerca da implementação)  
* src -> main.cpp
* structures -> esquematização das S-BOX
* tests -> arquivos texto utilizados para cifração ou decifração  

O programa pode ser compilado pela utilização do Makefile, 
com o seguinte comando em seu terminal:  
$ make compile
	
Caso seja de seu interesse compilá-lo manualmente, execute a instrução:  
$ g++ -o aes src/main.cpp

Por fim, para execução do programa nos arquivos teste:  
$ ./aes [-c, -d] <key> -f <source_file> -o <destination_file> [-ecb, -ctr] <num_rounds>

Ex: ./aes -c a5b90c1a2b5e3a014253698745214523 -f tests/<image>.ppm -o tests/<image_encrypted>.ppm -ecb 1

* O tamanho da string chave DEVE ser 32, isto é, 
deve ser passada pela junção de 16 bytes.
