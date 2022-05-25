// Para compilar
//gcc -Wall sd.c -o sd.out -lpthread -lrt
// Para testar
//./sd.out 1 1 100 0

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>

#define MAX 10000


int *buffer;
int verbose, N;
int consumed_and_evaluated = 0, produced_and_shipped = 0;
pthread_mutex_t mutex;
sem_t empty, full, threadsProducer, threadsConsumer;


int randomNumber(){
    return rand() % 10000000 + 1;
}


int isPrime(int n) {
    // Check if number is prime
    if (n == 2) return 1;

    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) {
            return 0;
        }
    }
    return 1;
}


int *emptyVector(int N) {
    // Initialize the vector with zeros
    int *vector = (int *)malloc(N * sizeof(int));
    
    for (int i = 0; i <= N; i++) vector[i] = 0;

    return vector;
}

int findWritablePosition(){
    // Search through the buffer for a position that is not written to
    for (int i = 0; i < N; i++) {
        if (buffer[i] == 0){
          if (verbose >= 4) printf("Found writable position %d\n", i);  
          return i;
        } 
    }

    if(verbose) printf("Could not find Writable Position!\n");
    return -1;
}

int findProduct(){
    // Search through the buffer for a position with a number
    for (int i = 0; i < N; i++){
        if (buffer[i] != 0) {
            if (verbose >= 4) printf("At %d, found product: %d\n", i, buffer[i]);
            return i;
        }
    }
    if(verbose) printf("Could not find Product!?\n");
    return -1;
}


void *producer(){
    int random;
    while(true){
        // Já gera um número aleatório para não fazer na região crítica	
        random = randomNumber();

        // Apenas produz se o buffer tiver espaço
        sem_wait(&empty);
        // Protege o buffer
        pthread_mutex_lock(&mutex);
        
        // Escreve no buffer, numa posição disponível, um número aleatório
        buffer[findWritablePosition()] = random;
        // Incrementa o contador de produzidos
        produced_and_shipped++;

        // Libera o buffer
        pthread_mutex_unlock(&mutex);
        // Avisa que mais um item foi produzido
        sem_post(&full);

        // Se todo o necessário for produzido, para a produção	
        if (produced_and_shipped == MAX) break;
    }

    // Avisa que a produção terminou
    sem_post(&threadsProducer);

}

void *consumer(){
    while(true){
        // Espera que haja algum item no buffer
        sem_wait(&full);
        // Protege o buffer
        pthread_mutex_lock(&mutex);

        // Posição em que há um produto
        int productPosition     = findProduct();
        // Pega o produto do buffer
        int product             = buffer[productPosition];
        // Coloca 0 no lugar
        buffer[productPosition] = 0;

        // Incrementa o contador de consumidos
        consumed_and_evaluated++;
		
        // Libera o buffer
        pthread_mutex_unlock(&mutex);
        // Avisa que mais um item foi consumido
        sem_post(&empty);


        // Se o produto for primo, avise
        if (isPrime(product)){
			if (verbose >= 2){
				printf("%d é primo!\n", product);	
			}
		}
		
		if (verbose >= 3){
			printf("%d não é primo.\n", product);	
		}

        // Se todo o necessário for consumido, para o consumo
        if (consumed_and_evaluated == MAX) break;
    }

    // Avisa que a consumo terminou
    sem_post(&threadsConsumer);

}


int main (int argc, char *argv[]) {

	srand(time(NULL));


    //read args
    int Np  = atoi(argv[1]);
    int Nc  = atoi(argv[2]);
    N       = atoi(argv[3]);
    verbose = atoi(argv[4]);


    pthread_t producers[Np];
    pthread_t consumers[Nc];

    buffer = emptyVector(N);


    // initialize semaphores
    sem_init(&empty, 0, N);
    sem_init(&full, 0, 0);


    clock_t start, finish;
    start = clock();

    // Start the engines!
    for (int i = 0; i < Np; i++) {
        pthread_create(&producers[i], NULL, producer, (void *)(unsigned long long)i);
    }

    // Open the stores
    for (int i = 0; i < Nc; i++) {
        pthread_create(&consumers[i], NULL, consumer, (void *)(unsigned long long)i);
    }

	// Close the factores
    sem_wait(&threadsProducer);
    for (int i = 0; i < Np; i++){
        pthread_cancel(producers[i]);
    }

	// Close the stores
    sem_wait(&threadsConsumer); // aguardando as threads consumidoras receberem 10⁵ números.
    for (int i = 0; i < Nc; i++){
        pthread_cancel(consumers[i]);
    }

	// Erase helpers
	pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&threadsProducer);
    sem_destroy(&threadsConsumer);
	free(buffer);

    finish = clock();
    double time_taken = ((double)(finish - start)) / CLOCKS_PER_SEC;;  // in seconds
    if (verbose)printf("Com N = %d, Np = %d, Nc = %d tempo foi de %fs\n", N, Np, Nc, time_taken);
    else  printf("%d, %d, %d, %f\n", N, Np, Nc, time_taken);
    return 0;
}