/* Programa que le um arquivo binario com dois valores inteiros (indicando as dimensoes de uma matriz) 
 * e uma sequencia com os valores da matriz (em float)
 * Entrada: nome do arquivo de entrada
 * Saida: vetor de floats representando uma matriz 
 * */

#include<stdio.h>
#include<stdlib.h>

float* leMatriz(char* arquivo) {
   float *matriz; //matriz que será carregada do arquivo
   int linhas, colunas; //dimensoes da matriz
   long long int tam; //qtde de elementos na matriz
   FILE * descritorArquivo; //descritor do arquivo de entrada
   size_t ret; //retorno da funcao de leitura no arquivo de entrada
   
   //abre o arquivo para leitura binaria
   descritorArquivo = fopen(arquivo, "rb");
   if(!descritorArquivo) {
      fprintf(stderr, "Erro de abertura do arquivo\n");
      exit(2);
   }

   //le as dimensoes da matriz
   ret = fread(&linhas, sizeof(int), 1, descritorArquivo);
   if(!ret) {
      fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
      exit(3);
   }

   ret = fread(&colunas, sizeof(int), 1, descritorArquivo);
   if(!ret) {
      fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
      exit(3);
   }
   tam = linhas * colunas; //calcula a qtde de elementos da matriz

   //aloca memoria para a matriz
   matriz = (float*) malloc(sizeof(float) * tam);
   if(!matriz) {
      fprintf(stderr, "Erro de alocao da memoria da matriz\n");
      exit(3);
   }

   //carrega a matriz de elementos do tipo float do arquivo
   ret = fread(matriz, sizeof(float), tam, descritorArquivo);
   if(ret < tam) {
      fprintf(stderr, "Erro de leitura dos elementos da matriz\n");
      exit(4);
   }

   //finaliza o uso das variaveis
   fclose(descritorArquivo);
   
   return matriz;
}

