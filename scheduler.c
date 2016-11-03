#include <setjmp.h>
#include <stdbool.h>
#include <malloc.h>
#include <time.h>
#include "list.h"

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
