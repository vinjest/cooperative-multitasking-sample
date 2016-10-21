#ifndef LIST_H
#define LIST_H

#include <setjmp.h>

struct thread_t
{
    jmp_buf saved_context;
    struct thread_t* next;
    bool isReady;
};

struct thread_t* add_thread(struct thread_t** head);
int remove_thread(struct thread_t** head, struct thread_t* thread);
struct thread_t* get_next_thread(struct thread_t** head, struct thread_t* current_thread);
int clear(struct thread_t** head);
int size(struct thread_t** head);

#endif
