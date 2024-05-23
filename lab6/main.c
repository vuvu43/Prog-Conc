#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    int *buffer;
    int size;
    int count;
    int in;
    int out;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} Buffer;

void buffer_init(Buffer *b, int size) {
    b->buffer = (int *)malloc(size * sizeof(int));
    b->size = size;
    b->count = 0;
    b->in = 0;
    b->out = 0;
    pthread_mutex_init(&b->mutex, NULL);
    pthread_cond_init(&b->not_empty, NULL);
    pthread_cond_init(&b->not_full, NULL);
}

void buffer_destroy(Buffer *b) {
    free(b->buffer);
    pthread_mutex_destroy(&b->mutex);
    pthread_cond_destroy(&b->not_empty);
    pthread_cond_destroy(&b->not_full);
}

void buffer_insert(Buffer *b, int item) {
    pthread_mutex_lock(&b->mutex);
    while (b->count == b->size) {
        pthread_cond_wait(&b->not_full, &b->mutex);
    }
    b->buffer[b->in] = item;
    b->in = (b->in + 1) % b->size;
    b->count++;
    pthread_cond_signal(&b->not_empty);
    pthread_mutex_unlock(&b->mutex);
}

int buffer_remove(Buffer *b, int *finished) {
    pthread_mutex_lock(&b->mutex);
    while (b->count == 0) {
        if (*finished) {
            pthread_mutex_unlock(&b->mutex);
            return -1; // indica que não há mais números a serem consumidos
        }
        pthread_cond_wait(&b->not_empty, &b->mutex);
    }
    int item = b->buffer[b->out];
    b->out = (b->out + 1) % b->size;
    b->count--;
    pthread_cond_signal(&b->not_full);
    pthread_mutex_unlock(&b->mutex);
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

// função para verificar se um número é primo
int is_prime(int num) {
    if (num <= 1) return 0;
    if (num == 2) return 1;
    if (num % 2 == 0) return 0;
    for (int i = 3; i * i <= num; i += 2) {
        if (num % i == 0) return 0;
    }
    return 1;
}

// função executada pela thread produtora
void *producer(void *arg) {
    ProducerArgs *args = (ProducerArgs *)arg;
    FILE *file = fopen(args->filename, "rb");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        *args->finished = 1;
        
    }

    int number;
    while (fread(&number, sizeof(int), 1, file) == 1) {
        buffer_insert(args->buffer, number);
    }

    fclose(file);
    *args->finished = 1; // indica que a produção foi concluída
    pthread_cond_broadcast(&args->buffer->not_empty); // notifica consumidores
    pthread_exit(NULL);
}

void *consumer(void *arg) {
    ConsumerArgs *args = (ConsumerArgs *)arg;
    while (1) {
        int item = buffer_remove(args->buffer, args->finished);
        printf("Thread %d, %d\n", args->id, item);
        if (item == -1) break;
        if (is_prime(item)) {
            args->prime_count++;
        }
    }
    pthread_exit(NULL);
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

    int finished = 0; // flag para quando não houver mais números a serem inseridos no buffer

    // lê a quantidade de números primos 
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
    pthread_create(&producer_thread, NULL, producer, &prod_args); // thread produtora começa a trabalhar

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
        printf("A contagem de numeros primos corresponde: %d\n", total_primes);
    } else {
        printf("A contagem de numeros primos não corresponde. Encontrados: %d, Esperados: %d\n", total_primes, prime_count_from_file);
    }

    printf("Thread %d encontrou mais primos: %d\n", max_primes_thread, max_primes);

    buffer_destroy(&buffer);
    return 0;
}
