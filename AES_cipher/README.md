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

Acerca das heurísticas utilizadas no ataque à cifra:

* Escolha da chave:
O usuário só poderá escolher o tamanho da chave desejado
caso, durante o processo automático de definição do chave,
o programa erre o criptograma (pode acontecer para mensagens
consideravelmente pequenas, visto que dificulta a distinção
estatística da frequência dos símbolos do alfabeto);

* Escolha do idioma:
O usuário não escolhe o possível idioma da mensagem cifrada, 
visto que foi implementado processo para considerar a maior 
probabilidade da mensagem decifrada ser originada do idioma 
inglês ou português, com base nos índices de coincidência;

* Redução de chave replicada:
Durante o processo automático de recuperação da mensagem, pode
acontecer de retornar a chave replicada, a título de exemplo, 
suponha a chave "arara". Os índices de coincidência podem
considerar a chave como sendo de tamanho múltiplo de 5,
e podem retornar a chave como sendo "araraarara" ou
"araraararaarara". Para uma melhor experiência estética durante
a apresentação da senha descoberta, decidimos que, caso o usuário
não esteja controlando o tamanho da chave, ou seja, o programa
não errou a mensagem recuperada, então a senha replicada é tratada,
e é retornado apenas a primeira ocorrência da palavra.
Note que para a senha replicada, não há erro durante a recuperação
da mensagem, é uma questão estética, apenas.
Ademais, quando o usuário tem controle sobre o tamanho da chave,
foi decidido que o tratamento da chave replicada não aconteceria,
visto que força a chave ter um determinado tamanho, escolhido pelo
usuário.
