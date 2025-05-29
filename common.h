#include <pthread.h>
#include <assert.h>

#define Mutex_init(m) assert(pthread_mutex_init(m, NULL) == 0);
#define Mutex_lock(m) assert(pthread_mutex_lock(m) == 0);
#define Mutex_unlock(m) assert(pthread_mutex_unlock(m) == 0);

#define Cond_init(c) assert(pthread_cond_init(c, NULL) == 0);
#define Cond_wait(c, m) assert(pthread_cond_wait(c, m) == 0);
#define Cond_signal(c) assert(pthread_cond_signal(c) == 0);
