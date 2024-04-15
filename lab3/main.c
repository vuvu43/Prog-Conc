#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"
#include "matUtil.h"

//caso queira checar as matrizes A B e C
//#define TEXTO
//caso queria testar o codigo (imprime a matriz de TESTE)
//#define TESTE

struct spec *matA; //matriz A
struct spec *matB; //matriz B
float *matC; //matriz C
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
      for(int j = 0; j < matB->col; j++){
         matC[i * matB->col + j] = 0.0;

         for(int k = 0; k < matA->col; k++){
               matC[i * matB->col + j] += matA->mat[i * matA->col + k] * matB->mat[k * matB->col + j];
         }
      }
   }
}

//função de teste para checar se a multiplicação está certa
void matMultSeq(float* teste){
   for (int i = 0; i < matA->lin; i++) {
      for (int j = 0; j < matB->col; j++) {
         teste[i * matB->col + j] = 0.0;

         for (int k=0; k < matA->col; k++) {
            teste[i * matB->col + j] += matA->mat[i * matA->col + k] * matB->mat[k * matB->col + j];
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
      //exemplo: ./main matA matB 4 matc
      printf("Digite: %s <arquivoA(binario)> <arquivoB(binario)> <numero de threads> <nome arquivo saida>\n", argv[0]);
      return 1;
   }

   matA = leMatriz(argv[1]);
   matB = leMatriz(argv[2]);
   nthreads = atoi(argv[3]);
   if (nthreads > matA->lin) nthreads=matA->lin;

#ifdef TEXTO
   //print matA
   printf("Matriz A: %dx%d\n", matA->lin, matA->col);
   imprimeMat(matA->mat, matA->lin, matA->col);

   //print matB
   printf("Matriz B: %dx%d\n", matB->lin, matB->col);
   imprimeMat(matB->mat, matB->lin, matB->col);
#endif

   //alocacao de memoria para as estruturas de dados
   matC = (float *) malloc(sizeof(float) * matA->lin * matB->col);
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
      (args+i)->linha_ini = i * (matA->lin/nthreads); //linha inicial 
      (args+i)->linha_final = (args+i)->linha_ini + (matA->col/nthreads); //linha final
      
      if(pthread_create(tid+i, NULL, matMult, (void*) (args+i))){
         puts("ERRO--pthread_create"); return 3;
      }
   } 

   //calcula o restante das linhas de forma sequencial
   if (matA->lin%nthreads){
      struct tArgs *param = (struct tArgs*) malloc(sizeof(struct tArgs));
      if (param == NULL) {printf("ERRO -- malloc"); return 2;}

      param->id = nthreads+1;
      param->linha_ini = matA->lin - (matA->lin/nthreads);
      param->linha_final = matA->lin;

      matMult((void *) param);
   }

   //espera pelo termino das threads
   for(int i=0; i<nthreads; i++) {
      pthread_join(*(tid+i), NULL);
   }

   GET_TIME(fim)   
   delta = fim - inicio;
   printf("\nTempo multiplicacao (dimensao %dx%d) (nthreads %d): %lf\n", matA->lin, matB->col, nthreads, delta);
   escreveMatrizBinario(argv[4], matC, matA->lin, matB->col);

#ifdef TEXTO
   printf("\nMatriz C: %dx%d\n", matA->lin, matB->col);
   imprimeMat(matC, matA->lin, matB->col);
#endif

#ifdef TESTE
   float *teste = malloc(sizeof(float) * matA->lin * matB->col);
   matMultSeq(teste);
   printf("Matriz TESTE: %dx%d\n", matA->lin, matB->col);
   imprimeMat(teste, matA->lin, matB->col);
   free(teste);
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
