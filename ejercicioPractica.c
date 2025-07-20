#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

sem_t mutex, estacion;
sem_t cliente_espera, cliente_nuevo;
sem_t mutex_clientes_esperando_rellenar, mutex_nuevos;
sem_t surtidores_ocupados;

int clientes_esperando_rellenar = 0;
int jefe_avisado = 0;
int autos_abastecidos = 0;
int clientes_cargando = 0;

int pagar() {
    sem_wait(&mutex);
    int hay_gasolina = (autos_abastecidos < 5);
    if (hay_gasolina) {
        autos_abastecidos++;
        int nivel_restante = 5 - autos_abastecidos;
        printf("Gasolina reservada. Nivel restante: %d\n", nivel_restante);
    }
    sem_post(&mutex);
    return hay_gasolina;
}

void cargar() {
    printf("Cargando gasolina al vehiculo...\n");
    sleep(2);

    sem_wait(&mutex);
    clientes_cargando--;
    int nivel_actual = 5 - autos_abastecidos;
    printf("Carga completada. Nivel actual de gasolina: %d\n", nivel_actual);
    sem_post(&mutex);
}

void rellenar() {
    printf("Jefe: Iniciando proceso de llenado de tanques...\n");
    sleep(3);

    sem_wait(&mutex);
    autos_abastecidos = 0;
    printf("Tanques llenos nuevamente - nivel de gasolina: 5\n");
    sem_post(&mutex);
}

void jefe() {
    while(1) {
        sem_wait(&cliente_nuevo);
        printf("Jefe: Recibido aviso para rellenar tanques\n");

        sem_wait(&mutex);
        while (clientes_cargando > 0) {
            printf("Jefe: Esperando a que %d clientes terminen de cargar\n", clientes_cargando);
            sem_post(&mutex);
            sleep(1);
            sem_wait(&mutex);
        }
        printf("Jefe: Todos los clientes terminaron. Empezando rellenado.\n");
        sem_post(&mutex);

        sem_wait(&estacion);

        rellenar();

        sem_wait(&mutex);
        jefe_avisado = 0;
        int total_esperando = clientes_esperando_rellenar;
        sem_post(&mutex);

        for (int i = 0; i < total_esperando; i++) {
            sem_post(&cliente_espera);
        }

        sem_post(&estacion);

        printf("Jefe: Trabajo terminado, regresando a descansar\n");
    }
}

void cliente() {
    static int contador_clientes = 0;
    sem_wait(&mutex_nuevos);
    int mi_id = ++contador_clientes;
    sem_post(&mutex_nuevos);

    sleep(mi_id * 1);

    while(1) {
        printf("Cliente %d: Llega a la gasolinera y toma un surtidor\n", mi_id);

        sem_wait(&surtidores_ocupados);

        if (pagar()) {
            sem_wait(&mutex);
            int nivel_disponible = 5 - autos_abastecidos;
            sem_post(&mutex);

            printf("Cliente %d: Puede pagar, nivel de gasolina disponible: %d\n",
                    mi_id, nivel_disponible);

            sem_wait(&mutex);
            clientes_cargando++;
            sem_post(&mutex);

            sem_wait(&estacion);
            cargar();
            sem_post(&estacion);

        } else {
            printf("Cliente %d: No hay gasolina disponible (nivel: 0). Clientes esperando: %d\n",
                    mi_id, clientes_esperando_rellenar);

            sem_wait(&mutex);
            if (!jefe_avisado) {
                jefe_avisado = 1;
                printf("Cliente %d: Fui a avisar al jefe\n", mi_id);
                sem_post(&cliente_nuevo);
            }

            clientes_esperando_rellenar++;
            printf("Cliente %d: Me quede esperando el rellenado\n", mi_id);
            sem_post(&mutex);

            sem_wait(&cliente_espera);

            sem_wait(&mutex);
            clientes_esperando_rellenar--;
            clientes_cargando++;
            sem_post(&mutex);

            if (pagar()) {
                printf("Cliente %d: Ahora si hay gasolina tras el rellenado\n", mi_id);
            }

            sem_wait(&estacion);
            cargar();
            sem_post(&estacion);
        }

        sem_post(&surtidores_ocupados);

        printf("Cliente %d: Termine y me voy de la gasolinera\n", mi_id);
        sleep(rand() % 3 + 2);
    }
}

int main() {
    srand(time(NULL));

    sem_init(&mutex, 0, 1);
    sem_init(&estacion, 0, 1);
    sem_init(&cliente_espera, 0, 0);
    sem_init(&cliente_nuevo, 0, 0);
    sem_init(&mutex_clientes_esperando_rellenar, 0, 1);
    sem_init(&mutex_nuevos, 0, 1);
    sem_init(&surtidores_ocupados, 0, 5);

    pthread_t jefe_thread, cliente1, cliente2, cliente3, cliente4, cliente5, cliente6;

    pthread_create(&jefe_thread, NULL, (void *)jefe, NULL);
    pthread_create(&cliente1, NULL, (void *)cliente, NULL);
    pthread_create(&cliente2, NULL, (void *)cliente, NULL);
    pthread_create(&cliente3, NULL, (void *)cliente, NULL);
    pthread_create(&cliente4, NULL, (void *)cliente, NULL);
    pthread_create(&cliente5, NULL, (void *)cliente, NULL);
    pthread_create(&cliente6, NULL, (void *)cliente, NULL);

    pthread_join(jefe_thread, NULL);
    pthread_join(cliente1, NULL);
    pthread_join(cliente2, NULL);
    pthread_join(cliente3, NULL);
    pthread_join(cliente4, NULL);
    pthread_join(cliente5, NULL);
    pthread_join(cliente6, NULL);

    return 0;
}