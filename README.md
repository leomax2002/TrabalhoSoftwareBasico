# TrabalhoSoftwareBasico

Repositório para o Primeiro Trabalho da Disciplina de Software Básico, o qual consiste da Implementação em C ou C++ de um Montador de Passagem Único e de um Ligador. 

O arquivo do código fonte e os executáveis podem ser encontrados na pasta:
```
/codigo/implementacao
```

## Compilação

```bash
g++ montador.cpp -std=c++17 -o montador

g++ linker.cpp -std=c++17 -o linker
```

## Utilização

Basta utilizar o seguinte comando no terminal:

```bash
./montador nome_do_arquivo [(opcional) mais_arquivos]

./linker nome_do_arquivo1 nome_do_arquivo2 [(opcional) mais_arquivos]
```

## Implementação

Utilizou-se uma etapa de processamento do arquivo de entrada para um arquivo intermediário. Nesse processo, retirou-se todos os erros de formatação e também retirou algumas incoveniências, como os caracteres de ',' entre os argumentos do COPY e também inverteu-se o texto caso as seções vissem na ordem trocada. Desse modo, considerou-se como referencal o arquivo intermediário para indicar as linhas dos erros. Também, durante o preprocessamento, converteu-se os números em hexadecimal em decimal. 

O montador, ao receber um arquivo com pelo menos uma das seguintes diretivas: "EXTERN, PUBLIC, BEGIN, END", em vez de criar o arquivo objeto final em .exc, ele gera um arquivo objeto .obj que pode ser utilizado pelo ligador para ligar. Foi utilizado MUL para realizar a operação de Multiplicação.

## Autores

200022172 - Leonardo Maximo Silva

200028880 - Wallace Ben Teng Lin Wu

##Link
Link para o Projeto no GitHub: https://github.com/leomax2002/TrabalhoSoftwareBasico.git
