#include <stdio.h>
#include <stdlib.h>

// Função para gerar um número aleatório entre min e max
double random_double(double min, double max) {
    return min + (rand() / (RAND_MAX / (max - min)));
}

// Função que recebe a, b, c e retorna um ponteiro para um array de dois doubles
double* random_point(double a, double b, double c) {
    double* ponto = (double*)malloc(2 * sizeof(double));
    if (ponto == NULL) {
        fprintf(stderr, "Erro ao alocar ponto aleatório1\n");
        exit(-1);
    }

    ponto[0] = random_double(a, b);
    ponto[1] = random_double(0, c);

    return ponto;
}

// Função para encontrar o maior valor que uma função assume no intervalo [a, b]
double find_max_value(double a, double b, double (*func)(double), int num_samples) {
    /*
    Função para encontrar o maior valor de uma outra função dentro do intervalo [a, b]
    é útil para fazer a integração de monte-carlo

    Entrada:
        - double a: limite inferior da intervalo
        - double b: limite superior do intervalo
        - double (*func)(double): função qualquer tal que f: R --> R
        - int num_samples: quantidade de pontos que serão testados

    Saída:
        - double max_value: maior valor que a função assume no intervalo [a, b]
    */
    double max_value = func(a);
    double passo = (b - a) / num_samples; //tamanho do passo

    for (int i = 1; i <= num_samples; i++) {
        double x = a + i * passo;
        double val = func(x);

        if (val > max_value) {
            max_value = val;
        }
    }

    return max_value;
}

double monte_carlo_int(double a, double b, long int iter, double (*func)(double)){
    /*
    Utiliza o método de Monte-Carlo para aproximar a integral em um intervalo [a, b]

    Entrada:
        - double a: limite inferior integrado
        - double b: limite superior integrado
        - double c: maior valor de func dentro do intervalo
        - long int iter: quantidade de simulações
        - double (*func)(double): função a ser integrada tal que f: R --> R
    
    Saída:
        - area: area calculada dentro do intervalo
    */

    if (b < a){
        fprintf(stderr, "O limite superior b deve ser maior que o inferior a");
        exit(-1);
    }
    
    if (b == a) return 0.0; //area 0

    double* ponto = malloc(sizeof(double) * 2);
    if (ponto == NULL){
        fprintf(stderr, "Erro ao alocar ponto2");
        exit(-1);
    }

    int dentro = 0;
    double c = find_max_value(a, b, func, iter); // encontra maior valor no intervalo
    double area_retangulo = (b - a) * c;

    //simulações
    for(long int i=0; i<iter; i++){
        ponto = random_point(a, b, c);
        if (ponto[1] <= func(ponto[0])) dentro++; 
    }

    free(ponto);
    double area =  area_retangulo * dentro / iter;

    return area;
}

