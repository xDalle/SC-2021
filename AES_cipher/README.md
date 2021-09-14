Autores:  
Lucas Dalle Rocha - 17/0016641  
Leonardo Rodrigues de Souza - 170060543  

Organiza��o dos arquivos:  
* docs -> Descritivo.pdf (relat�rio acerca da implementa��o)  
* src -> main.cpp
* structures -> esquematiza��o das S-BOX
* tests -> arquivos texto utilizados para cifra��o ou decifra��o  

O programa pode ser compilado pela utiliza��o do Makefile, 
com o seguinte comando em seu terminal:  
$ make compile
	
Caso seja de seu interesse compil�-lo manualmente, execute a instru��o:  
$ g++ -o aes src/main.cpp

Por fim, para execu��o do programa nos arquivos teste:  
$ ./aes [-c, -d] <key> -f <source_file> -o <destination_file> [-ecb, -ctr] <num_rounds>

Ex: ./aes -c a5b90c1a2b5e3a014253698745214523 -f tests/<image>.ppm -o tests/<image_encrypted>.ppm -ecb 1

* O tamanho da string chave DEVE ser 32, isto �, 
deve ser passada pela jun��o de 16 bytes.

Acerca das heur�sticas utilizadas no ataque � cifra:

* Escolha da chave:
O usu�rio s� poder� escolher o tamanho da chave desejado
caso, durante o processo autom�tico de defini��o do chave,
o programa erre o criptograma (pode acontecer para mensagens
consideravelmente pequenas, visto que dificulta a distin��o
estat�stica da frequ�ncia dos s�mbolos do alfabeto);

* Escolha do idioma:
O usu�rio n�o escolhe o poss�vel idioma da mensagem cifrada, 
visto que foi implementado processo para considerar a maior 
probabilidade da mensagem decifrada ser originada do idioma 
ingl�s ou portugu�s, com base nos �ndices de coincid�ncia;

* Redu��o de chave replicada:
Durante o processo autom�tico de recupera��o da mensagem, pode
acontecer de retornar a chave replicada, a t�tulo de exemplo, 
suponha a chave "arara". Os �ndices de coincid�ncia podem
considerar a chave como sendo de tamanho m�ltiplo de 5,
e podem retornar a chave como sendo "araraarara" ou
"araraararaarara". Para uma melhor experi�ncia est�tica durante
a apresenta��o da senha descoberta, decidimos que, caso o usu�rio
n�o esteja controlando o tamanho da chave, ou seja, o programa
n�o errou a mensagem recuperada, ent�o a senha replicada � tratada,
e � retornado apenas a primeira ocorr�ncia da palavra.
Note que para a senha replicada, n�o h� erro durante a recupera��o
da mensagem, � uma quest�o est�tica, apenas.
Ademais, quando o usu�rio tem controle sobre o tamanho da chave,
foi decidido que o tratamento da chave replicada n�o aconteceria,
visto que for�a a chave ter um determinado tamanho, escolhido pelo
usu�rio.
