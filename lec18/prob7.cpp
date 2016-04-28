#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

int M = 3, N = 5;
int numA, numB;
sem_t Aempty, Afull, Bempty, Bfull, BminusA;
sem_t mutex;

void* proA (void* arg) {
    for (int i = 0; i < 100; ++i) {
        sem_wait(&Afull);
        sem_wait(&mutex);
        numA++;
        printf("put  A now A %d B %d\n", numA, numB);
        sem_post(&Aempty);
        sem_post(&BminusA);
        sem_post(&mutex);
        usleep(rand() % 100);
    }
    return NULL;
}

void* proB (void* arg) {
    for (int i = 0; i < 100; ++i) {
        sem_wait(&Bfull);
        sem_wait(&BminusA);
        sem_wait(&mutex);
        numB++;
        printf("put  B now A %d B %d\n", numA, numB);
        sem_post(&Bempty);
        sem_post(&mutex);
        usleep(rand() % 100);
    }
    return NULL;
}

void* proC (void* arg) {
    for (int i = 0; i < 100; ++i) {
        sem_wait(&Aempty);
        sem_wait(&Bempty);
        sem_wait(&mutex);
        numA--;
        numB--;
        printf("make C now A %d B %d\n", numA, numB);
        sem_post(&Afull);
        sem_post(&Bfull);
        sem_post(&mutex);
        usleep(rand() % 100);
    }
    return NULL;
}

int main() {

    numA = numB = 0;
    sem_init(&Afull, 0, N);
    sem_init(&Bfull, 0, N);
    sem_init(&Aempty, 0, 0);
    sem_init(&Bempty, 0, 0);
    sem_init(&BminusA, 0, M);
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