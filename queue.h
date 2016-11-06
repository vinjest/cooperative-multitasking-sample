#ifndef QUEUE_H
#define QUEUE_H

struct waiting_thread_t
{
    struct thread_t* waiting_thread;
    struct waiting_thread_t* next;
};

void push_back(struct waiting_thread_t** head, struct thread_t** thread);
struct thread_t* pop_front(struct waiting_thread_t** head);

#endif
