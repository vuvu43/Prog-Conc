#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

double pi_seq(int iter) {
    double ret = 0.0;

    for (long long int i=0; i<iter; i++) {

        ret += 1.0 / pow(16, i) * (
            4.0 / (8 * i + 1) -
            2.0 / (8 * i + 4) -
            1.0 / (8 * i + 5) -
            1.0 / (8 * i + 6)
        );
    }
    return ret;
}

//global vars init
double pi = 0.0;
long long int iterGlobal = 0;
pthread_mutex_t mutex;

void *pi_conc(void* arg){
    while (iterGlobal < (long long int) arg){
        pi += 1.0 / pow(16, iterGlobal) * (
            4.0 / (8 * iterGlobal + 1) -
            2.0 / (8 * iterGlobal + 4) -
            1.0 / (8 * iterGlobal + 5) -
            1.0 / (8 * iterGlobal + 6)
        );

        pthread_mutex_lock(&mutex);
        iterGlobal++;
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}


int main (int argc, char* argv[]){
    /*
    Progama para comparar resultados do cálculo de pi de forma sequencial e concorrente
    É utilizado uma bolsa de tarefa para calcular pi de forma concorrente
    Utilização:
        - Windows: pi_conc <iter> <#threads>
        - Exemplo: pi_conc 1000 5

        - Linux: ./pi_conc <iter> <#threads>
        - Exemplo: ./pi_conc 200 4
    */


    if (argc != 3){
        printf("Utilizacao: %s <iter> <#threads> \n", argv[0]); exit(-1);
    }

    //init vars
    long long int iteracoes = atoll(argv[1]);
    int NTHREADS = atoi(argv[2]);
    pthread_t tid[NTHREADS];

    //calcula pi de forma sequencial e printa
    double seq = pi_seq(iteracoes);
    printf("\nPi sequencial: %.50f\n", seq);

    //inicia mutex
    pthread_mutex_init(&mutex, NULL);

    //cria as threds
    for (int i=0; i<NTHREADS; i++){
        if(pthread_create(&tid[i], NULL, pi_conc, (void *) iteracoes)){
            printf("--ERRO: pthread_create()\n"); exit(-1);
        }
    }

    //espera as threads terminarem
    for (int t=0; t<NTHREADS; t++) {
        if (pthread_join(tid[t], NULL)) {
            printf("--ERRO: pthread_join() \n"); exit(-1); 
        } 
    } 

    //printa pi calculado de forma concorrente e o erro
    printf("Pi concorrente: %.50f\n", pi);
    printf("Diferenca: %.50f", seq - pi);

    pthread_mutex_destroy(&mutex);
    return 0;
}
