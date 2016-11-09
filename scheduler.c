#include <setjmp.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "list.h"
#include "queue.h"
#include "scheduler.h"

#define default_stack_size 65536

static struct thread_t* current_thread = NULL;
static struct thread_t* thread_list = NULL;

static jmp_buf create_thread_return_ctx;

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

void* stack_allocate()
{
    intptr_t allocated_mem_base_ptr = (intptr_t)malloc(default_stack_size);

    if (!allocated_mem_base_ptr)
        return NULL;

    allocated_mem_base_ptr += default_stack_size - 1;

    return (void*)allocated_mem_base_ptr;
}

void yield()
{
    if (!current_thread)
        current_thread = add_thread(&thread_list);

    if (!setjmp(current_thread->saved_context))
    {
        current_thread = next_thread(&thread_list, &current_thread);
        longjmp(current_thread->saved_context, 1);
    }
}

void terminate_thread(struct thread_t* thread)
{
    remove_thread(&thread_list, &thread);
    thread->isReady = false;
    yield();
}

void thread_start(void(*func_ptr)(), struct thread_t* added_thread_ptr)
{
    if (!setjmp(added_thread_ptr->saved_context))
    {
        longjmp(create_thread_return_ctx, 1);
    }
    else
    {
        func_ptr();
        terminate_thread(added_thread_ptr);
    }
}

struct thread_t* create_thread(void(*func_ptr)())
{
    struct thread_t* added_thread = NULL;

    if (!setjmp(create_thread_return_ctx))
    {
        added_thread = add_thread(&thread_list);

        void* allocated_mem_ptr = stack_allocate();

        asm volatile("mov %0, %%esp"::"r"(allocated_mem_ptr));
        asm volatile("push %0"::"r"(added_thread));
        asm volatile("push %0"::"r"(func_ptr));
        asm volatile ("call %0"::"r"(&thread_start));
    }
    return added_thread;
}

void sleep(long milliseconds)
{
    if (!current_thread)
        current_thread = add_thread(&thread_list);

        current_thread->wakeup_time = ((clock() / CLOCKS_PER_SEC) * 1000) + milliseconds;
        yield();
}

void join(struct thread_t* thread)
{
    if (!thread)
        return;

    current_thread->joined_thread = thread;
    yield();
}

void sem_init(struct sem_t** sem, int value)
{
    if (*sem)
        return;
    else
    {
        struct sem_t* semaphore = (struct sem_t*)malloc(sizeof(struct sem_t));
        semaphore->max_value = value;
        semaphore->value = value;
        semaphore->waiting_count = 0;
        semaphore->waiting_queue = NULL;
        *sem = semaphore;
    }
}

void sem_remove(struct sem_t** sem)
{
    if (!*sem)
        return;
    else
    {
        if ((*sem)->waiting_queue == NULL && (*sem)->value == (*sem)->max_value)
        {
            free(*sem);
            *sem = NULL;
        }
    }
}

void sem_down(struct sem_t** sem)
{
    if (!*sem)
        return;

    if ((*sem)->value == 0)
    {
        (*sem)->waiting_count++;
        current_thread->isBlocked = true;
        push_back(&((*sem)->waiting_queue), &current_thread);
        yield();
    }

    if ((*sem)->value > 0)
        (*sem)->value--;
}

void sem_up(struct sem_t** sem)
{
    if (!*sem)
        return;

    if ((*sem)->waiting_count != 0)
    {
        (*sem)->waiting_count--;
        struct thread_t* thread = pop_front(&((*sem)->waiting_queue));
        thread->isBlocked = false;
    }
    if ((*sem)->value < (*sem)->max_value)
        (*sem)->value++;
}

void mutex_init(struct mutex_t** mtx)
{
    if (*mtx)
        return;
    else
    {
        struct mutex_t* mutex = (struct mutex_t*)malloc(sizeof(struct mutex_t));
        mutex->count = 0;
        mutex->owner = NULL;
        mutex->waiting_queue = NULL;
        *mtx = mutex;
    }
}

void mutex_remove(struct mutex_t** mtx)
{
    if (!*mtx)
        return;
    else
    {
        if ((*mtx)->count == 0)
        {
            if ((*mtx)->waiting_queue == NULL)
            {
                free(*mtx);
                *mtx = NULL;
            }
        }
    }
}

void acquire_mutex(struct mutex_t** mtx)
{
    if (!*mtx)
        return;

    if ((*mtx)->count != 0)
    {
        if ((*mtx)->owner == current_thread)
            (*mtx)->count++;
        else
        {
            push_back(&((*mtx)->waiting_queue), &current_thread);
            current_thread->isBlocked = true;
            yield();
        }
    }
    if ((*mtx)->count == 0)
    {
        (*mtx)->owner = current_thread;
        (*mtx)->count++;
    }
}

void release_mutex(struct mutex_t** mtx)
{
    if (!*mtx)
        return;

    if ((*mtx)->count != 0)
    {
        if ((*mtx)->owner == current_thread)
        {
            (*mtx)->count--;

            if ((*mtx)->count == 0)
            {
                (*mtx)->owner = NULL;

                 if ((*mtx)->waiting_queue)
                 {
                      struct thread_t* thread = pop_front(&((*mtx)->waiting_queue));
                      thread->isBlocked = false;
                 }
            }
        }
    }
}
