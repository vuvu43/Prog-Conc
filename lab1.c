#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#define MTHREADS 6

//vetor teste
int vet[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21};
int vet_teste[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21};
int N = sizeof(vet) / sizeof(int);

struct t_args{
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
    
    if (arg.fim > arg.N) arg.fim = arg.N; // corrigindo caso o .fim seja maior que o tamanho do vetor
    
    //debug printf("[%d, %d]", arg.ini, arg.fim-1);
    
    for (int i=arg.ini; i<arg.fim; i++){
        vet[i] *= vet[i];
    }

}

void func_teste(){
    
    for (int i=0; i<N; i++){
        if (sqrt(vet[i]) != vet_teste[i]){
            printf("\nInd> %d\nsqrt> %f\norig> %d\n", i, sqrt(vet[i]), vet_teste[i]);
            puts("--ERRO: vetores diferentes"); exit(-1);
        }
    }
    printf("\nTESTE OK :)");
}

int main(int argc, char** argv){
    puts("Vetor original: ");
    imprimeVet(vet, N);
    
    pthread_t tid_sistema[MTHREADS];//id das threads
    
    //cria as threads e atribui a tarefa de cada thread
    for(long int i=0; i<MTHREADS; i++) {
        
        //inicializa argumentos
        struct t_args* args = malloc(sizeof(struct t_args));
        args->N = N;
        args->ini = i * (N/MTHREADS);
        args->fim = (i + 1) * (N/MTHREADS);
        
        if( i == MTHREADS-1) args->fim = N; 
        
        //código da prof
        //cria as  threads
        if (pthread_create(&tid_sistema[i], NULL, quadVet, (void*) args)) {
            printf("--ERRO: pthread_create()\n"); exit(-1);
        }
    }
    
    //código da prof
    //espera as threads terminarem
    for (int i=0; i<MTHREADS; i++) {
        if (pthread_join(tid_sistema[i], NULL)) {
            printf("--ERRO: pthread_join() \n"); exit(-1); 
        } 
    }

    //checa resultado
    puts("Vetor alterado: ");
    imprimeVet(vet, N);
    func_teste();
    return 0;
}
