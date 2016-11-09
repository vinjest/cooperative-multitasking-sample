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

struct sem_t
{
    struct waiting_thread_t* waiting_queue;
    int waiting_count;
    int max_value;
    int value;
};

void* stack_allocate();
void yield();
struct thread_t* create_thread(void(*func_ptr)());
void thread_start(void(*func_ptr)(), struct thread_t* added_thread_ptr);
void terminate_thread(struct thread_t* thread);
void sleep(long milliseconds);
void join(struct thread_t* thread);

void mutex_init(struct mutex_t** mtx);
void mutex_remove(struct mutex_t** mtx);
void acquire_mutex(struct mutex_t** mtx);
void release_mutex(struct mutex_t** mtx);

void sem_init(struct sem_t** sem, int value);
void sem_remove(struct sem_t** sem);
void sem_up(struct sem_t** sem);
void sem_down(struct sem_t** sem);

#endif
