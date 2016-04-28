#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>

int M = 3, N = 5;
int numA, numB, numC;
sem_t Acont, Bcont, BmAcont;
sem_t mutex;

void* proA (void* arg) {
    for (int i = 0; i < 100; ++i) {
        sem_wait(&Acont);
        sem_wait(&mutex);
        numA++;
        printf("put  A now A %d B %d\n", numA, numB);
        sem_post(&BmAcont);
        sem_post(&mutex);
    }
    return NULL;
}

void* proB (void* arg) {
    for (int i = 0; i < 100; ++i) {
        sem_wait(&Bcont);
        sem_wait(&mutex);
        numB++;
        printf("put  B now A %d B %d\n", numA, numB);
        sem_wait(&BmAcont);
        sem_post(&mutex);
    }
    return NULL;
}

void* proC (void* arg) {
    for (int i = 0; i < 100; ++i) {
        sem_wait(&mutex);
        if (numA > 0 && numB > 0) {
            sem_post(&Acont);
            sem_post(&Bcont);
            numA--;
            numB--;
            printf("make C now A %d B %d\n", numA, numB);
        }
        sem_post(&mutex);
    }
    return NULL;
}

int main() {

    numA = numB = 0;
    sem_init(&Acont, 0, N);
    sem_init(&Bcont, 0, N);
    sem_init(&BmAcont, 0, M);
    sem_init(&mutex, 0, 1);

    pthread_t threads[3];
    pthread_create(&threads[0], 0, proA, NULL);
    pthread_create(&threads[1], 0, proB, NULL);
    pthread_create(&threads[2], 0, proC, NULL);
    for (int i = 0; i < 3; ++i) {
        pthread_join(threads[i], 0);
    }

    return 0;
}