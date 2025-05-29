#include <stdio.h>
#include <unistd.h>

#include <pthread.h>
#include <assert.h>

#define Mutex_init(m) assert(pthread_mutex_init(m, NULL) == 0);
#define Mutex_lock(m) assert(pthread_mutex_lock(m) == 0);
#define Mutex_unlock(m) assert(pthread_mutex_unlock(m) == 0);

#define Cond_init(c) assert(pthread_cond_init(c, NULL) == 0);
#define Cond_wait(c, m) assert(pthread_cond_wait(c, m) == 0);
#define Cond_signal(c) assert(pthread_cond_signal(c) == 0);

typedef struct __Zem_t {
    /* Zemaphore : Semaphore를 직접 구현한 구조체*/

    int value;
    pthread_cond_t cond;
    pthread_mutex_t lock;
} Zem_t;

void Zem_init(Zem_t *s, int value) {
    s->value = value;
    Cond_init(&s->cond);
    Mutex_init(&s->lock);
}

void Zem_wait(Zem_t *s) {
    Mutex_lock(&s->lock);
    while (s->value <= 0) {
        Cond_wait(&s->cond, &s->lock);
    }
    s->value--;
    Mutex_unlock(&s->lock);
}

void Zem_post(Zem_t *s) {
    Mutex_lock(&s->lock);
    s->value++;
    Cond_signal(&s->cond);
    Mutex_unlock(&s->lock);
}

typedef Zem_t sem_t;

/* 기존의 semaphore 함수들을 대체하기 위한 매크로 정의 */

#define sem_init(s, pshared, value) Zem_init(s, value)
#define sem_wait(s) Zem_wait(s)
#define sem_post(s) Zem_post(s)

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
