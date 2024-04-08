#include<stdio.h>
#include<stdlib.h>


void imprimeMat(float* mat, int lin, int col){
    /*
      Printa a mat no terminal em forma de matriz
      Parâmetros: matriz alocada como um vetor, quantidade de linhas e colunas
      Saída: void
    */


    for (int i=0; i<lin; i++){
        for (int j=0; j<col; j++){
            printf("%.2f ", mat[i * col + j]);
        }
        printf("\n");
    }
    printf("\n");
}


void escreveMatrizBinario(char* arq, float* vet, int linhas, int colunas){
   /* 
      Programa que cria arquivo com uma matriz de valores do tipo float
      Entrada: dimensoes da matriz (linhas e colunas) e nome do arquivo de saida
      Saida: nome de arquivo
   */


   long long int tam = linhas * colunas; //qtde de elementos na matriz
   FILE *descritorArquivo; //descritor do arquivo de saida
   size_t ret; //retorno da funcao de escrita no arquivo de saida

   //escreve a matriz no arquivo
   //abre o arquivo para escrita binaria
   descritorArquivo = fopen(arq, "wb");
   if(!descritorArquivo) {
      fprintf(stderr, "Erro de abertura do arquivo\n");
      exit(3);
   }
   //escreve numero de linhas e de colunas
   ret = fwrite(&linhas, sizeof(int), 1, descritorArquivo);
   ret = fwrite(&colunas, sizeof(int), 1, descritorArquivo);
   //escreve os elementos da matriz
   ret = fwrite(vet, sizeof(float), tam, descritorArquivo);
   if(ret < tam) {
      fprintf(stderr, "Erro de escrita no  arquivo\n");
      exit(4);
   }

   //finaliza o uso das variaveis
   fclose(descritorArquivo);
}


typedef struct spec{
   float* mat;
   int lin, col;
}spec;


struct spec* leMatriz(char* arquivo) {
   /* 
      Programa para ler uma matriz de um arquivo binario
      Entrada: nome do arquivo de entrada passado para main na linha de comando
      Saida: struct com especificaçõs sobre a matriz
   */


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

   struct spec *retorno = (struct spec*) malloc(sizeof(struct spec));

   retorno->mat = matriz;
   retorno->lin = linhas;
   retorno->col = colunas;

   return (struct spec*) retorno;
}