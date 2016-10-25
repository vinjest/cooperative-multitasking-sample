#include <setjmp.h>
#include <stdbool.h>
#include <malloc.h>
#include <time.h>
#include "list.h"

#define default_stack_size 65536

static struct thread_t* caller = NULL;
static struct thread_t* current_thread = NULL;
static struct thread_t* thread_list = NULL;

static jmp_buf create_thread_return_ctx;

void* stack_allocate();
void yield();
void create_thread(void(*func_ptr)());
void thread_start(void(*func_ptr)(), struct thread_t* added_thread_ptr);
void terminate_thread(struct thread_t* thread);

void sleep(int milliseconds);

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
    {
        caller = (struct thread_t*)malloc(sizeof(struct thread_t));
        current_thread = caller;
    }

    if (!setjmp(current_thread->saved_context))
    {
        struct thread_t* next_thread = get_next_thread(&thread_list, current_thread);
        if (next_thread)
        {
            current_thread = next_thread;
            longjmp(current_thread->saved_context, 1);
        }
        else
            longjmp(caller->saved_context, 1);
    }
}

void terminate_thread(struct thread_t* thread)
{
    remove_thread(&thread_list, thread);
    current_thread->isReady = false;
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

void create_thread(void(*func_ptr)())
{
    if (!setjmp(create_thread_return_ctx))
    {
        struct thread_t* added_thread = add_thread(&thread_list);

        void* allocated_mem_ptr = stack_allocate();

        asm volatile("mov %0, %%esp"::"r"(allocated_mem_ptr));
        asm volatile("push %0"::"r"(added_thread));
        asm volatile("push %0"::"r"(func_ptr));
        asm volatile ("call %0"::"r"(&thread_start));
    }
}

void sleep(int milliseconds)
{
    if (!current_thread->isSleeping)
    {
        int gone_ms = 0;
        current_thread->fall_asleep_time = clock();
        current_thread->wakeup_time = milliseconds;
        current_thread->isSleeping = true;

        do
        {
            yield();
            clock_t difference = clock() - current_thread->fall_asleep_time;
            gone_ms = difference * 1000 / CLOCKS_PER_SEC;
        }
        while (gone_ms < current_thread->wakeup_time);
    }
    
    current_thread->isSleeping = false;
    current_thread->fall_asleep_time = NULL;
    current_thread->wakeup_time = 0;
}

