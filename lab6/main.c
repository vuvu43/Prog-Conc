#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

typedef struct {
    int *buffer;
    int size;
    int count;
    int in;
    int out;
    sem_t mutex;
    sem_t not_empty;
    sem_t not_full;
} Buffer;

void buffer_init(Buffer *b, int size) {
    b->buffer = (int *)malloc(size * sizeof(int));
    b->size = size;
    b->count = 0;
    b->in = 0;
    b->out = 0;
    sem_init(&b->mutex, 0, 1);
    sem_init(&b->not_empty, 0, 0);
    sem_init(&b->not_full, 0, size);
}

void buffer_destroy(Buffer *b) {
    free(b->buffer);
    sem_destroy(&b->mutex);
    sem_destroy(&b->not_empty);
    sem_destroy(&b->not_full);
}

void buffer_insert(Buffer *b, int item) {
    sem_wait(&b->not_full);
    sem_wait(&b->mutex);
    b->buffer[b->in] = item;
    b->in = (b->in + 1) % b->size;
    b->count++;
    sem_post(&b->mutex);
    sem_post(&b->not_empty);
}

int buffer_remove(Buffer *b, int *finished) {
    sem_wait(&b->not_empty);
    sem_wait(&b->mutex);
    if (*finished && b->count == 0) {
        sem_post(&b->mutex);
        sem_post(&b->not_empty);
        return -1; // Indica que não há mais números a serem consumidos
    }
    int item = b->buffer[b->out];
    b->out = (b->out + 1) % b->size;
    b->count--;
    sem_post(&b->mutex);
    sem_post(&b->not_full);
    return item;
}

typedef struct {
    Buffer *buffer;
    const char *filename;
    int *finished;
} ProducerArgs;

typedef struct {
    Buffer *buffer;
    int id;
    int prime_count;
    int *finished;
} ConsumerArgs;

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

void *producer(void *arg) {
    ProducerArgs *args = (ProducerArgs *)arg;
    FILE *file = fopen(args->filename, "rb");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        *args->finished = 1;
        sem_post(&args->buffer->not_empty); // Notifica consumidores
        return NULL;
    }

    int number;
    while (fread(&number, sizeof(int), 1, file) == 1) {
        buffer_insert(args->buffer, number);
    }

    fclose(file);
    *args->finished = 1; // Indica que a produção foi concluída
    sem_post(&args->buffer->not_empty); // Notifica consumidores
    return NULL;
}

void *consumer(void *arg) {
    ConsumerArgs *args = (ConsumerArgs *)arg;
    while (1) {
        int item = buffer_remove(args->buffer, args->finished);
        if (item == -1) break;
        printf("Consumidor %d: consumiu %d\n", args->id, item);
        if (is_prime(item)) {
            args->prime_count++;
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <num_consumidores> <tamanho_buffer> <arquivo_entrada>\n", argv[0]);
        return 1;
    }

    int num_consumers = atoi(argv[1]);
    int buffer_size = atoi(argv[2]);
    const char *filename = argv[3];

    Buffer buffer;
    buffer_init(&buffer, buffer_size);

    int finished = 0;

    // Lê a quantidade de números primos do final do arquivo
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }
    int prime_count_from_file;
    fread(&prime_count_from_file, sizeof(int), 1, file);
    fclose(file);

    pthread_t producer_thread;
    ProducerArgs prod_args = { &buffer, filename, &finished };
    pthread_create(&producer_thread, NULL, producer, &prod_args);

    pthread_t consumer_threads[num_consumers];
    ConsumerArgs cons_args[num_consumers];
    for (int i = 0; i < num_consumers; i++) {
        cons_args[i].buffer = &buffer;
        cons_args[i].id = i + 1;
        cons_args[i].prime_count = 0;
        cons_args[i].finished = &finished;
        pthread_create(&consumer_threads[i], NULL, consumer, &cons_args[i]);
    }

    pthread_join(producer_thread, NULL);
    for (int i = 0; i < num_consumers; i++) {
        pthread_join(consumer_threads[i], NULL);
    }

    // Verifica se a contagem de primos corresponde
    int total_primes = 0;
    int max_primes = 0;
    int max_primes_thread = 0;
    for (int i = 0; i < num_consumers; i++) {
        total_primes += cons_args[i].prime_count;
        if (cons_args[i].prime_count > max_primes) {
            max_primes = cons_args[i].prime_count;
            max_primes_thread = i + 1;
        }
    }

    if (total_primes == prime_count_from_file) {
        printf("A contagem de números primos corresponde: %d\n", total_primes);
    } else {
        printf("A contagem de números primos não corresponde. Encontrados: %d, Esperados: %d\n", total_primes, prime_count_from_file);
    }

    printf("Thread %d encontrou mais primos: %d\n", max_primes_thread, max_primes);

    buffer_destroy(&buffer);
    return 0;
}
