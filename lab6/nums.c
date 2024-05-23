#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Função para verificar se um número é primo
int is_prime(int num) {
    if (num <= 1) return 0;
    if (num == 2) return 1;
    if (num % 2 == 0) return 0;
    for (int i = 3; i * i <= num; i += 2) {
        if (num % i == 0) return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <N>\n", argv[0]);
        return 1;
    }

    long long int N = atoll(argv[1]);
    if (N <= 0) {
        fprintf(stderr, "N deve ser um número inteiro positivo.\n");
        return 1;
    }

    // Alocar memória para os números
    int *numbers = (int *)malloc(N * sizeof(int));
    if (numbers == NULL) {
        fprintf(stderr, "Erro ao alocar memória.\n");
        return 1;
    }

    // Inicializar o gerador de números aleatórios
    srand(time(NULL));

    // Gerar números e contar primos
    int prime_count = 0;
    for (long long int i = 0; i < N; i++) {
        numbers[i] = rand() % 1000000 + 1; // Números aleatórios entre 1 e 1000000
        if (is_prime(numbers[i])) {
            prime_count++;
        }
    }

    // Abrir o arquivo binário para escrita
    FILE *file = fopen("output.bin", "wb");
    if (file == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo.\n");
        free(numbers);
        return 1;
    }
    
    // Escrever a quantidade de primos no final do arquivo
    fwrite(&prime_count, sizeof(int), 1, file);

    // Escrever os números no arquivo
    fwrite(numbers, sizeof(int), N, file);

    // Fechar o arquivo e liberar a memória
    fclose(file);
    free(numbers);

    return 0;
}
