#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MTHREADS 3

//vetor teste
int vet[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
int N = 11;

struct t_args{
    int* vet;
    int N;
    int ini;
    int fim;
};

void imprimeVet(int* vet, int N){
    // imprime o vetor para checar resultado

    for (int i=0; i<N; i++){ 
        printf("%d ", vet[i]);
    }
    puts("");
}

void* quadVet(void* args){
    //eleva ao quadrado elementos no intervalo [ini, fim-1]
    struct t_args arg = *(struct t_args*) args;
    
    if (arg.fim > arg.N) arg.fim = arg.N; // corrigindo caso o .fim seja maior que o Nanho do vetor

    for (int i=arg.ini; i<arg.fim; i++){
        vet[i] *= vet[i];
    }

}

int main(int argc, char** argv){
    puts("Vetor original: ");
    imprimeVet(vet, N);
    
    pthread_t tid_sistema[MTHREADS];//id das threads
    
    //cria as threads e atribui a tarefa de cada thread
    for(long int i=0; i<MTHREADS; i++) {
        
        //inicializa argumentos
        struct t_args* args = malloc(sizeof(struct t_args));
        args->vet = vet;
        args->N = N;
        args->ini = i * N/MTHREADS;
        args->fim = (i + 1) * (N/MTHREADS);
        
        if( i == MTHREADS-1) args->fim = N; 
        
        if (pthread_create(&tid_sistema[i], NULL, quadVet, (void*) args)) {
            printf("--ERRO: pthread_create()\n"); exit(-1);
        }
    }
    
    //espera as threads terminarem
    for (int i=0; i<MTHREADS; i++) {
        if (pthread_join(tid_sistema[i], NULL)) {
            printf("--ERRO: pthread_join() \n"); exit(-1); 
        } 
    }

    //checa resultado
    puts("Vetor alterado: ");
    imprimeVet(vet, N);
    return 0;
}