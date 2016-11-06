#include "list.h"
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

struct thread_t* add_thread(struct thread_t** head);
void remove_thread(struct thread_t** head, struct thread_t** removable);
struct thread_t* next_thread(struct thread_t** head, struct thread_t** prev_thread);
void clear(struct thread_t** head);
int size(struct thread_t** head);

struct thread_t* add_thread(struct thread_t** head)
{
    struct thread_t* new_node = (struct thread_t*)malloc(sizeof(struct thread_t));
    new_node->joined_thread = NULL;
    new_node->isReady = true;
    new_node->wakeup_time = 0;
    new_node->isBlockedOnMutex = false;

    if (*head == NULL)
    {
        *head = new_node;
        new_node->next = new_node;
        new_node->prev = new_node;
    }
    else
    {
        struct thread_t* current = *head;

        while (current->next != *head)
            current = current->next;

        new_node->prev = current;
        current->next = new_node;

        new_node->next = *head;
        (*head)->prev = new_node;
    }
    return new_node;
}

struct thread_t* next_thread(struct thread_t** head, struct thread_t** prev_thread)
{
    if (!*head)
        return NULL;

    struct thread_t* current = *head;

    if (!((*prev_thread)->isReady))
        current = *head;
    else
    {
        while (current != *prev_thread)
            current = current->next;
        current = current->next;
    }

    for (;;)
    {
        if (current->isReady && current->isBlockedOnMutex == false)
        {
            if (((clock() / CLOCKS_PER_SEC) * 1000) >= current->wakeup_time)
            {
                if (current->joined_thread == NULL)
                    return current;
                else
                {
                    if ((current->joined_thread)->isReady == false)
                    {
                        return current;
                    }
                }
            }
        }
        current = current->next;
    }
}

void remove_thread(struct thread_t** head, struct thread_t** removable)
{
    if (!*head)
        return;

    if (*head == *removable)
    {
        if ((*head)->next == *head)
        {
            (*head)->next = NULL;
            (*head)->prev = NULL;
            *head = NULL;
        }
        else
        {
            ((*head)->next)->prev = (*head)->prev;
            ((*head)->prev)->next = (*head)->next;
            struct thread_t* old_head = *head;
            *head = (*head)->next;
            old_head->next = NULL;
            old_head->prev = NULL;
        }
    }
    else
    {
        if (*removable != (*head)->prev)
        {
            ((*removable)->prev)->next = (*removable)->next;
            ((*removable)->next)->prev = (*removable)->prev;
            (*removable)->next = NULL;
            (*removable)->prev = NULL;
        }
        else
        {
            ((*removable)->prev)->next = *head;
            (*head)->prev = (*removable)->prev;
            (*removable)->next = NULL;
            (*removable)->prev = NULL;
        }
    }
}

void clear(struct thread_t** head)
{
    if (!*head)
        return;

    struct thread_t* current = *head;
    struct thread_t* removable = NULL;

    while (current->next != *head)
    {
        removable = current;
        current = current->next;
        free(removable);
    }
    *head = NULL;
}

int size(struct thread_t** head)
{
    int count = 0;
    struct thread_t* current = *head;

    if (*head)
    {
        while (current->next != *head)
        {
            count++;
            current = current->next;
        }
        count++;
    }
    return count;
}
