#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include "list.h"

struct thread_t* add_thread(struct thread_t** head);
int remove_thread(struct thread_t** head, struct thread_t* thread);
struct thread_t* get_next_thread(struct thread_t** head, struct thread_t** current_thread);
struct thread_t* pop_front(struct thread_t** head);
struct thread_t* pop_back(struct thread_t** head);
int clear(struct thread_t** head);
int size(struct thread_t** head);

struct thread_t* add_thread(struct thread_t** head)
{
    struct thread_t* current = *head;
    struct thread_t* new_node = NULL;

    new_node = (struct thread_t*)malloc(sizeof(struct thread_t));
    if (!new_node)
        return NULL;

    new_node->next = NULL;
    new_node->isReady = true;
    new_node->wakeup_time = 0;
    new_node->joined_thread = NULL;

    if (*head == NULL)
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

struct thread_t* get_next_thread(struct thread_t** head, struct thread_t** previous_thread)
{
    struct thread_t* current = *head;
    struct thread_t* previous = *previous_thread;

    struct thread_t* first_among_the_sleepless = NULL;
    struct thread_t* first_among_the_sleeping = NULL;

    if (*head == NULL)
        return NULL;

    while (current->next != NULL)
    {
        if (current->isReady)
        {
            if (((clock() / CLOCKS_PER_SEC) * 1000) >= current->wakeup_time)
            {
                if (current != previous)
                {
                    if (!first_among_the_sleepless)
                        first_among_the_sleepless = current;
                }
            }
            else
            {
                    if (!first_among_the_sleeping)
                        first_among_the_sleeping = current;
                    else
                    {
                        if (current->wakeup_time < first_among_the_sleeping->wakeup_time)
                            first_among_the_sleeping = current;
                    }

            }
        }
        current = current->next;
    }

    if (current->isReady)
    {
        if (((clock() / CLOCKS_PER_SEC) * 1000) >= current->wakeup_time)
        {
            if (current != previous)
            {
                if (!first_among_the_sleepless)
                    first_among_the_sleepless = current;
            }
        }
        else
        {
                if (!first_among_the_sleeping)
                    first_among_the_sleeping = current;
                else
                {
                    if (current->wakeup_time < first_among_the_sleeping->wakeup_time)
                        first_among_the_sleeping = current;
                }
        }
    }

    if (first_among_the_sleepless)
        return first_among_the_sleepless;
    else
        if (previous->isReady && (((clock() / CLOCKS_PER_SEC) * 1000) >= previous->wakeup_time))
            return previous;
    else
        if (first_among_the_sleeping)
        {
            while(((clock() / CLOCKS_PER_SEC) * 1000) < first_among_the_sleeping->wakeup_time);
            first_among_the_sleeping->wakeup_time = 0;
            return first_among_the_sleeping;
        }

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
