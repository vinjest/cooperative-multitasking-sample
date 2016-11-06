#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <setjmp.h>
#include <stdbool.h>

struct mutex_t
{
    struct thread_t* owner;
    struct waiting_thread_t* waiting_queue;
    int count;
};

void yield();
void acquire_mutex(struct mutex_t** mtx);
void release_mutex(struct mutex_t** mtx);
void mutex_init(struct mutex_t** mtx);
void mutex_remove(struct mutex_t** mtx);
struct thread_t* create_thread(void(*func_ptr)());
void sleep(long milliseconds);
void join(struct thread_t* thread);

#endif
