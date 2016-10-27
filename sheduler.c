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

void sleep(long milliseconds);

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
        {
            current_thread = caller;
            longjmp(caller->saved_context, 1);
        }
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

void sleep(long milliseconds)
{
    if (size(&thread_list) != 0)
    {
        if (!current_thread)
        {
            caller = (struct thread_t*)malloc(sizeof(struct thread_t));
            current_thread = caller;
        }
        current_thread->wakeup_time = milliseconds;
        yield();
        while(((clock() / CLOCKS_PER_SEC) * 1000) < current_thread->wakeup_time);
        current_thread->wakeup_time = 0;
    }
    else
        while(((clock() / CLOCKS_PER_SEC) * 1000) < milliseconds);
}
