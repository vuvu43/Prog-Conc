#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define TESTE
#define MAX 1000

float* vetor;

//argumentos passados para a thread
struct t_args{
    int idThread;
    int tamBloco;
};

//retorno da execução da thread
typedef struct t_ret{
    float soma;
};

void* tarefa( void* argumentos){
    struct t_args* args = (struct t_args*) argumentos;

    //calculando índices
    long int ini = args->idThread * args->tamBloco;
    long int fim = ini + args->tamBloco;

    //retorno da thread (soma de tamBloco elementos do vetor)
    struct t_ret* ret = (struct t_ret*) malloc(sizeof(struct t_ret));
    if (ret == NULL){ //checa se alocou o espaço com sucesso
        pthread_exit(NULL);
    }

    ret->soma = 0;
    for (long int i=ini; i<fim; i++){
        ret->soma += vetor[i];
    }


    pthread_exit((void*) ret);
}

int main(int argc, char* argv[]){
    long int dim; //dimensao do vetor de entrada
    long int tamBloco; //tamanho do bloco de cada thread 
    int nthreads; //numero de threads que serao criadas
    float somaGeral = 0;//soma dos elementos do vetor
    
    struct t_ret *retorno; //valor de retorno das threads

    if(argc < 2) {
       fprintf(stderr, "Digite: %s <numero threads>\n", argv[0]);
       return 1; 
    }
   
    nthreads = atoi(argv[1]);
    printf("nthreads=%d\n", nthreads); 

    //carrega o vetor de entrada
    scanf("%ld", &dim); //primeiro pergunta a dimensao do vetor
    printf("dim=%ld\n", dim); 

    //aloca o vetor de entrada
    vetor = (float*) malloc(sizeof(float)*dim);
    if(vetor == NULL) {
        fprintf(stderr, "ERRO--malloc\n");
        return 2;
    }

    //preenche vetor
    for(int i=0; i<dim; i++){
        scanf("%f", vetor+i);
    }
   
    //cria as threads
    pthread_t* tid = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
    if(tid==NULL) {
        fprintf(stderr, "ERRO--malloc\n");
        return 3;
    }
   
    tamBloco = dim/nthreads;  //tamanho do bloco de cada thread 
   
    for(int i=0; i<nthreads; i++) {
        //aloca e preenche argumentos para thread
        struct t_args *args = (struct t_args*) malloc(sizeof(struct t_args));
        if (args == NULL) {
            printf("--ERRO: malloc()\n"); 
            pthread_exit(NULL);
        }
       
        args->idThread = i; 
        args->tamBloco = tamBloco; 
       
        if(pthread_create(&tid[i], NULL, tarefa, (void*) args)){
            fprintf(stderr, "ERRO--pthread_create\n");
            return 5;
        }
       
    }
   
    //processa a parte final do vetor
    if(dim%nthreads) {
        puts("\nMain com tarefa");
        
        for(long int i=dim-(dim%nthreads); i<dim; i++) {
            somaGeral += vetor[i];
        }
    } 

    //aguarda o termino das threads
    for(int i=0; i<nthreads; i++) {
        if(pthread_join(tid[i], (void**) &retorno)){
            fprintf(stderr, "ERRO--pthread_create\n");
            return 6;
        }
        
        //somar valor retornado pelas threads
        somaGeral += retorno->soma;
    }
    
    //resultado
    printf("Soma dos elementos do vetor foi: %f\n", somaGeral);

#ifdef TESTE
    float somaTeste = 0;
    
    for (long int i=0; i<dim; i++){
        somaTeste += vetor[i];
    }
    
    printf("A soma do teste foi de: %f", somaTeste);
#endif

    free(vetor);
    free(tid);
    
    return 0;
}
