/* Autores: Arthur Pinheiro, Pedro Arthur Santos Gama
* Antes de rodar o programa:
* Você (usuário) deve escolher a função a ser integrada
* Então definir o intervalo de integração
* Por último, programar essa função dentro do código, deixamos uma de exemplo (double f(x))
* Finalmente, compilar  e executar passando os parâmetros necessários
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include "monte-carlo.h"
#include "timer.h"

typedef struct{
    double lim_inf; // limite inferior do intervalo
    double lim_sup; // limite superior do intervalo
    long int iter; // quantidade de simulações que cada thread realizará
    double (*func)(double); // função a ser integrada
}tArgs;

// função de exemplo
double f(double x){
    return sqrt(1 - x*x);
}

void* monte_carlo_int_thread(void* targs){
    tArgs* args = (tArgs*) targs;

    double area =  monte_carlo_int(
                                    args->lim_inf, 
                                    args->lim_sup, 
                                    args->iter, 
                                    args->func
                                    );

    double* ret = malloc(sizeof(double));
    if(ret == NULL){
        fprintf(stderr, "Erro ao alocar retorno da thread");
        exit(-1);
    }

    *ret = area;
    pthread_exit((void*)ret);
}

int main(int argc, char* argv[]){
    if (argc != 5){
        printf("Modo de uso: %s <a> <b> <iter> <qnt_thread>\n", argv[0]); 
        return 1;
    } 

    srand(time(NULL));

    // inicializa parâmetros
    double a = atof(argv[1]);
    double b = atof(argv[2]);
    long int iter_max = atoi(argv[3]);
    int M = atoi(argv[4]);
    
    pthread_t* tid = malloc(sizeof(pthread_t) * M); // identificadores das threads no sistema
    tArgs* args = malloc(sizeof(tArgs) * M); // vetor de argumentos 
    double inicio, fim, delta;// vars para calcular tempo de execução
    if (tid == NULL){
        fprintf(stderr, "Erro ao alocar threads.");
        exit(-1);
    }

    if (args == NULL){
        fprintf(stderr, "Erro ao alocar argumentos.");
        exit(-1);
    }

    double tam_bloco = (b - a) / M;
    long int iter_thread = iter_max / M;

    for (int i=0; i<M; i++){
        // inicializa argumentos para threads
        (args+i)->lim_inf = a + i * tam_bloco;
        (args+i)->lim_sup = (args+i)->lim_inf + tam_bloco;
        (args+i)->iter = iter_thread;
        (args+i)->func = f;

        // passa a função para as threads
        if(pthread_create(tid+i, NULL, monte_carlo_int_thread, (void*)(args+i))){
            fprintf(stderr, "Erro ao passar funcao para thread");
            exit(-1);
        }
    }

    double integral_a_b = 0.0;
    for (int i=0; i<M; i++) {
        double* result;
        if (pthread_join(tid[i], (void**)&result) != 0) {
            fprintf(stderr, "Erro ao juntar thread.\n");
            exit(-1);
        }

        integral_a_b += *result;
        free(result);  // libera a memória alocada para o resultado
    }

    printf("Area entre [%.2f, %.2f]: %f\n", a, b, integral_a_b);

    free(tid);
    free(args);

    return 0;
}
