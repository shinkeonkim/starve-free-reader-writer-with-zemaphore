#include <stdio.h>
#include <unistd.h>

#include "common.h"
#include "zemaphore.h"

typedef struct _rwlock_t {
    sem_t lock;        // protects readers count
    sem_t writelock;   // exclusive writer access
    sem_t read_try; // block readers if writer is waiting

    int readers;
    int waiting_writers;
} rwlock_t;

rwlock_t rw;

void rwlock_init(rwlock_t *rw) {
    rw->readers = 0;
    rw->waiting_writers = 0;
    sem_init(&rw->lock, 0, 1);
    sem_init(&rw->writelock, 0, 1);
    sem_init(&rw->read_try, 0, 1);
}

void rwlock_acquire_readlock(rwlock_t *rw) {
    sem_wait(&rw->read_try);
    sem_wait(&rw->lock);
    rw->readers++;
    if (rw->readers == 1)
        sem_wait(&rw->writelock); // 첫 번째 reader가 writelock을 획득
    sem_post(&rw->lock);
    sem_post(&rw->read_try);
}

void rwlock_release_readlock(rwlock_t *rw) {
    sem_wait(&rw->lock);
    rw->readers--;
    if (rw->readers == 0)
        sem_post(&rw->writelock); // 마지막 reader가 writelock을 해제
    sem_post(&rw->lock);
}

void rwlock_acquire_writelock(rwlock_t *rw) {
    sem_wait(&rw->read_try);
    rw->waiting_writers++;
    sem_wait(&rw->writelock); // writer는 writelock을 기다림
    rw->waiting_writers--;
}

void rwlock_release_writelock(rwlock_t *rw) {
    sem_post(&rw->writelock);
    sem_post(&rw->read_try);
}

// Reader 및 Writer 쓰레드 함수
void* reader(void* arg) {
    int id = *(int*)arg;
    rwlock_acquire_readlock(&rw);
    printf("Reader %d: reading...\n", id);
    sleep(2);
    printf("Reader %d: done.\n", id);
    rwlock_release_readlock(&rw);
    return NULL;
}

void* writer(void* arg) {
    int id = *(int*)arg;
    rwlock_acquire_writelock(&rw);
    printf("Writer %d: writing...\n", id);
    sleep(2);
    printf("Writer %d: done.\n", id);
    rwlock_release_writelock(&rw);
    return NULL;
}

int main() {
    pthread_t r_th[10];
    pthread_t w_th;
    int id_w = 1;

    rwlock_init(&rw);

    for (int i = 0; i < 10; i++) {
        pthread_create(&r_th[i], NULL, reader, &i);
        sleep(1);
        if(i == 5) {
            pthread_create(&w_th, NULL, writer, &id_w);            
        }
    }
    
    for (int i = 0; i < 10; i++) {
        pthread_join(r_th[i], NULL);
    }
    pthread_join(w_th, NULL);

    return 0;
}
