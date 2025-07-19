#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_BUFFER 10
#define DATOS_A_PRODUCIR 30
#define NUM_PRODUCTORES 3
#define NUM_CONSUMIDORES 3

sem_t elementos;
sem_t huecos;
sem_t mutex;
int buffer[MAX_BUFFER];
int in = 0;
int out = 0;

void* Productor(void* arg) {
    int id = *(int*)arg;
    int dato;
    int i;

    for(i = 0; i < DATOS_A_PRODUCIR; i++) {
        dato = id * 1000 + i;

        sem_wait(&huecos);
        sem_wait(&mutex);

        buffer[in] = dato;
        printf("[PRODUCTOR %d] >> Almacenando valor %d en posicion [%d] del buffer\n",
                id, dato, in);
        in = (in + 1) % MAX_BUFFER;

        sem_post(&mutex);
        sem_post(&elementos);

        usleep(50000);
    }

    printf("*** PRODUCTOR %d ha terminado su trabajo - Total producido: %d elementos ***\n",
            id, DATOS_A_PRODUCIR);
    pthread_exit(0);
}

void* Consumidor(void* arg) {
    int id = *(int*)arg;
    int dato;
    int total_elementos = (NUM_PRODUCTORES * DATOS_A_PRODUCIR) / NUM_CONSUMIDORES;
    int i;

    for(i = 0; i < total_elementos; i++) {
        sem_wait(&elementos);
        sem_wait(&mutex);

        dato = buffer[out];
        printf("    [CONSUMIDOR %d] << Extrayendo valor %d desde posicion [%d] - Item numero %d\n",
                id, dato, out, i + 1);
        out = (out + 1) % MAX_BUFFER;

        sem_post(&mutex);
        sem_post(&huecos);

        usleep(70000);
    }

    printf("*** CONSUMIDOR %d ha finalizado - Total consumido: %d elementos ***\n",
            id, total_elementos);
    pthread_exit(0);
}

int main(void) {
    pthread_t productores[NUM_PRODUCTORES];
    pthread_t consumidores[NUM_CONSUMIDORES];
    int ids_productores[NUM_PRODUCTORES];
    int ids_consumidores[NUM_CONSUMIDORES];
    int i;

    printf("========= INICIANDO PROBLEMA PRODUCTOR-CONSUMIDOR =========\n");
    printf("Configuracion: %d Productores, %d Consumidores, Buffer de tamaño %d\n\n",
            NUM_PRODUCTORES, NUM_CONSUMIDORES, MAX_BUFFER);

    sem_init(&elementos, 0, 0);
    sem_init(&huecos, 0, MAX_BUFFER);
    sem_init(&mutex, 0, 1);

    printf("--- Creando hilos productores ---\n");
    for(i = 0; i < NUM_PRODUCTORES; i++) {
        ids_productores[i] = i + 1;
        pthread_create(&productores[i], NULL, Productor, &ids_productores[i]);
        printf("Hilo Productor %d creado exitosamente\n", i + 1);
    }

    printf("\n--- Creando hilos consumidores ---\n");
    for(i = 0; i < NUM_CONSUMIDORES; i++) {
        ids_consumidores[i] = i + 1;
        pthread_create(&consumidores[i], NULL, Consumidor, &ids_consumidores[i]);
        printf("Hilo Consumidor %d creado exitosamente\n", i + 1);
    }

    printf("\n============ COMENZANDO EJECUCION ============\n\n");

    for(i = 0; i < NUM_PRODUCTORES; i++) {
        pthread_join(productores[i], NULL);
    }

    for(i = 0; i < NUM_CONSUMIDORES; i++) {
        pthread_join(consumidores[i], NULL);
    }

    sem_destroy(&elementos);
    sem_destroy(&huecos);
    sem_destroy(&mutex);

    printf("\n============ EJECUCION COMPLETADA ============\n");
    return 0;
}