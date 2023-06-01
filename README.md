# TrabalhoSoftwareBasico

Repositório para o Primeiro Trabalho da Disciplina de Software Básico, o qual consiste da Implementação em C ou C++ de um Montador de Passagem Único e de um Ligador. 

## Utilização

Basta utilizar o seguinte comando no terminal:

```bash
./montador nome_do_arquivo [(opcional) mais_arquivos]
```

## Implementação

Utilizou-se uma etapa de processamento do arquivo de entrada para um arquivo intermediário. Nesse processo, retirou-se todos os erros de formatação e também retirou algumas incoveniências, como os caracteres de ',' entre os argumentos do COPY e também inverteu-se o texto caso as seções vissem na ordem trocada. Desse modo, considerou-se como referencal o arquivo intermediário para indicar as linhas dos erros.
