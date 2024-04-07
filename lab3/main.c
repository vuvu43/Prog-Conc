#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "leMatrizBinario.h"
#include "timer.h"
#include "imprimeMat.h"

//caso queira checar se leu as matrizes A e B
//#define TEXTO 

float *matA; //matriz A
float *matB; //matriz B
float *matC; //matriz C
int dim; //tamanho da matriz
int nthreads; //numero de threads

struct tArgs{
   int id; //identificador da thread
   int linha_ini; //linha para começar processamento
   int linha_final; //linha para terminar processamento
};

//funcao que as threads executarao
void *matMult(void *arg){
   struct tArgs* args = (struct tArgs*) arg;

   for(int i = args->linha_ini; i < args->linha_final; i++){
      for(int j = 0; j < dim; j++){
         matC[i * dim + j] = 0;

         for(int k = 0; k < dim; k++){
               matC[i * dim + j] += matA[i * dim + k] * matB[k * dim + j];
         }
      }
   }
}

//fluxo principal
int main(int argc, char* argv[]) {
   pthread_t *tid; //identificadores das threads no sistema
   struct tArgs *args; //vetor de argumentos 
   double inicio, fim, delta;// vars para calcular tempo de execução
   
   GET_TIME(inicio);
   //leitura e avaliacao dos parametros de entrada
   if(argc<5) {
      //exemplo: ./main matA matB 4 2
      printf("Digite: %s <arquivoA(binario)> <arquivoB(binario)> <dimensao da matriz> <numero de threads>\n", argv[0]);
      return 1;
   }

   matA = leMatriz(argv[1]);
   matB = leMatriz(argv[2]);
   dim = atoi(argv[3]);
   nthreads = atoi(argv[4]);
   if (nthreads > dim) nthreads=dim;

#ifdef TEXTO
   //print matA
   printf("Matriz A:\n");
   imprimeMat(matA, dim);

   //print matB
   printf("Matriz B:\n");
   imprimeMat(matB, dim);
#endif

   //alocacao de memoria para as estruturas de dados
   matC = (float *) malloc(sizeof(float) * dim * dim);
   if (matC == NULL) {printf("ERRO--malloc\n"); return 2;}

   GET_TIME(fim);
   delta = fim - inicio;
   printf("Tempo linha_inicializacao:%lf\n", delta);

   //multiplicacao de matA com matB
   GET_TIME(inicio);

   //alocacao das estruturas para threads
   tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
   if(tid==NULL) {puts("ERRO--malloc"); return 2;}

   args = (struct tArgs*) malloc(sizeof(struct tArgs)*nthreads);
   if(args==NULL) {puts("ERRO--malloc"); return 2;}

   //criacao das threads
   for(int i=0; i<nthreads; i++) {
      (args+i)->id = i;
      (args+i)->linha_ini = i * (dim/nthreads); //linha inicial 
      (args+i)->linha_final = (args+i)->linha_ini + (dim/nthreads); //linha final
      //se quiser ver quais linhas cada thread vai executar descomente abaixo
      //printf("Thread %d: [%d, %d]\n", (args+i)->id, (args+i)->linha_ini, (args+i)->linha_final);
      
      if(pthread_create(tid+i, NULL, matMult, (void*) (args+i))){
         puts("ERRO--pthread_create"); return 3;
      }
   } 

   //calcula o restante das linhas de forma sequencial
   if (dim%nthreads){
      struct tArgs *param = (struct tArgs*) malloc(sizeof(struct tArgs));
      if (param == NULL) {printf("ERRO -- malloc"); return 2;}

      param->id = nthreads+1;
      param->linha_ini = dim - (dim/nthreads);
      param->linha_final = dim;

      matMult((void *) param);
   }

   //espera pelo termino das threads
   for(int i=0; i<nthreads; i++) {
      pthread_join(*(tid+i), NULL);
   }

   GET_TIME(fim)   
   delta = fim - inicio;
   printf("\nTempo multiplicacao (dimensao %d) (nthreads %d): %lf\n", dim, nthreads, delta);

#ifdef TEXTO
   printf("\nMatriz C\n");
   imprimeMat(matC, dim);
#endif

   //liberacao da memoria
   GET_TIME(inicio);

   free(matA);
   free(matB);
   free(matC);
   free(args);
   free(tid);

   GET_TIME(fim)   
   delta = fim - inicio;
   printf("\nTempo linha_finalizacao:%lf\n", delta);

   return 0;
}
