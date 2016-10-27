#include <malloc.h>
#include <stdbool.h>
#include "list.h"

struct thread_t* add_thread(struct thread_t** head);
int remove_thread(struct thread_t** head, struct thread_t* thread);
struct thread_t* get_next_thread(struct thread_t** head, struct thread_t* current_thread);
struct thread_t* pop_front(struct thread_t** head);
struct thread_t* pop_back(struct thread_t** head);
int clear(struct thread_t** head);
int size(struct thread_t** head);

struct thread_t* add_thread(struct thread_t** head)
{
    struct thread_t* current = *head;
    struct thread_t* new_node;

    new_node = (struct thread_t*)malloc(sizeof(struct thread_t));
    if (new_node == NULL)
        return NULL;

    new_node->next = NULL;
    new_node->isReady = true;
    new_node->wakeup_time = 0;

    if (current == NULL)
    {
        *head = new_node;
    }
    else
    {
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = new_node;
    }

    return new_node;
}

int remove_thread(struct thread_t** head, struct thread_t* thread)
{
    struct thread_t* current = *head;

    if (*head == NULL)
        return -2;

    if (*head == thread)
    {
        pop_front(head);
        return 0;
    }
    else
    {
        while (current->next != NULL)
        {
            if (current->next == thread)
            {
                if ((current->next)->next == NULL)
                {
                    pop_back(head);
                }
                else
                {
                    current->next = (current->next)->next;
                    (current->next)->next = NULL;
                }
                return 0;
            }
            current = current->next;
        }
    }
    return -1;
}

struct thread_t* get_next_thread(struct thread_t** head, struct thread_t* current_thread)
{
    struct thread_t* current = *head;
    if (*head == NULL)
        return NULL;

    while (current->next != NULL)
    {
        if (current->isReady)
        {
            if (current != current_thread)
                return current;
        }

        current = current->next;
    }
    if (current->isReady)
    {
        if (current != current_thread)
            return current;
    }

    if (current_thread->isReady)
        return current_thread;
    else
        return NULL;
}

int clear(struct thread_t** head)
{
    struct thread_t* current = *head;
    if (head == NULL)
        return 1;

    struct thread_t* temp = NULL;

    while (current->next != NULL)
    {
        temp = current->next;
        free(current);
        current = temp;
    }
    free(current);
    *head = NULL;

    return 0;
}

struct thread_t* pop_front(struct thread_t** head)
{
    if (*head == NULL)
        return NULL;

    struct thread_t* current = *head;
    struct thread_t* result = (struct thread_t*)malloc(sizeof(struct thread_t));
    result = current;

    *head = (*head)->next;

    return result;
}

struct thread_t* pop_back(struct thread_t** head)
{
    if (*head == NULL)
        return NULL;

    struct thread_t* current = *head;
    struct thread_t* temp = *head;
    struct thread_t* result = (struct thread_t*)malloc(sizeof(struct thread_t));

    while (current->next != NULL)
    {
        temp = current;
        current = current->next;
    }
    temp->next = NULL;
    result = current;

    return result;
}

int size(struct thread_t** head)
{
    if (*head == NULL)
        return 0;

    struct thread_t* current = *head;

    int size = 0;

    while (current->next != NULL)
    {
        size++;
        current = current->next;
    }
    size++;
    return size;
}
