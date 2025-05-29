#ifndef __ZEMAPHORE_H__
#define __ZEMAPHORE_H__

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

#endif
