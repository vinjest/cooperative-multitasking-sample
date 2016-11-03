#ifndef SHEDULER_H
#define SHEDULER_H

#include <setjmp.h>
#include <stdbool.h>

void yield();
struct thread_t* create_thread(void(*func_ptr)());
void sleep(long milliseconds);
void join(struct thread_t* thread);

struct thread_t
{
    jmp_buf saved_context;
    struct thread_t* next;
    struct thread_t* prev;
    struct thread_t* joined_thread;
    long wakeup_time;
    bool isReady;
};

#endif
