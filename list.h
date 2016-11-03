#ifndef LIST_H
#define LIST_H

#include <setjmp.h>
#include <time.h>
#include <stdbool.h>

struct thread_t
{
    jmp_buf saved_context;
    struct thread_t* next;
    struct thread_t* prev;
    struct thread_t* joined_thread;
    long wakeup_time;
    bool isReady;
};

struct thread_t* add_thread(struct thread_t** head);
void remove_thread(struct thread_t** head, struct thread_t** removable);
struct thread_t* next_thread(struct thread_t** head, struct thread_t** prev_thread);
void clear(struct thread_t** head);
int size(struct thread_t** head);

#endif
