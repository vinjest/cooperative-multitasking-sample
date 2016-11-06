#include "queue.h"
#include <stdlib.h>

void push_back(struct waiting_thread_t** head, struct thread_t** thread);
struct thread_t* pop_front(struct waiting_thread_t** head);

void push_back(struct waiting_thread_t** head, struct thread_t** thread)
{
    struct waiting_thread_t* current = *head;
    struct waiting_thread_t* new_node = (struct waiting_thread_t*)malloc(sizeof(struct waiting_thread_t));

    new_node->waiting_thread = *thread;
    new_node->next = NULL;

    if (!*head)
    {
        *head = new_node;
    }
    else
    {
        while (current->next != NULL)
            current = current->next;

        current->next = new_node;
    }
}

struct thread_t* pop_front(struct waiting_thread_t** head)
{
    if (!*head)
        return NULL;

    struct waiting_thread_t* old_head = *head;
    struct thread_t* result = (*head)->waiting_thread;
    *head = (*head)->next;
    free(old_head);
    old_head = NULL;

    return result;
}
