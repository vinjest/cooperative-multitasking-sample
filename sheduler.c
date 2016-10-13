#include <stdio.h>
#include <setjmp.h>
#include "sheduler.h"
#include <malloc.h>
#include "list.h"
#include "enums.h"

#define default_stack_size 65536
#define main_thread_id 0

void(*func_ptr)() = NULL;
static int thread_counter = 1;

struct mutex_t* create_mutex();

struct thread_t* thread_list;
struct thread_t* current_task = NULL;
struct thread_t* recently_added_thread = NULL;

int current_task_id = 1;

void resume(struct thread_t* thread_ptr);
void pause(struct thread_t* thread_ptr);
struct mutex_t* create_mutex();
void acquire_mutex(struct mutex_t* mtx);
void release_mutex(struct mutex_t* mtx);
void set_priority(struct thread_t* thread_ptr, int priority);
void yield();
void next();
void join(struct thread_t* thread_ptr);
struct thread_t* next_task();
void task_epilogue();
struct thread_t* create_thread(void(*func_ptr), void* stack_ptr);
void* stack_allocate();
void init();
int* buf_boxing(int buf[16]);

int* current_func_ret = &task_epilogue;

static jmp_buf yield_func_ctx;
static jmp_buf next_func_ctx;
static jmp_buf init_func_ctx;
static jmp_buf create_func_ctx;
static jmp_buf join_func_ctx;

void init()
{
	if (!setjmp(init_func_ctx));
	while (size(&thread_list) != 0)
	{
		current_task = next_task();
		if (current_task == NULL)
			break;

		yield();
	}
}

void* stack_allocate()
{
	intptr_t allocated_mem_base_ptr = malloc(default_stack_size);
	allocated_mem_base_ptr += default_stack_size - 7;
	return (void*)allocated_mem_base_ptr;
}

struct thread_t* create_thread(void(*func_ptr), void* stack_ptr)
{
	if (func_ptr == NULL || stack_ptr == NULL)
		return NULL;

	if (!setjmp(create_func_ctx))
	{
		struct thread_t* main_thread_ptr = get_by_id(&thread_list, main_thread_id);
		if (main_thread_ptr == NULL)
		{
			push_back(&thread_list, NULL, main_thread_id, medium, NULL);
			main_thread_ptr = get_by_id(&thread_list, main_thread_id);
			main_thread_ptr->saved_context = buf_boxing(create_func_ctx);
		}
		else
			main_thread_ptr->saved_context = buf_boxing(create_func_ctx);

		push_back(&thread_list, func_ptr, thread_counter, medium, stack_ptr);
		recently_added_thread = get_by_id(&thread_list, thread_counter);
		thread_counter++;
		init();
	}
	else
		return recently_added_thread;
}

void pause(struct thread_t* thread_ptr)
{
	struct thread_t* thread = get_by_id(&thread_list, thread_ptr->id);
	if (thread)
	{
		thread->state = blocked;
		longjmp(init_func_ctx, 1);
	}
}

void resume(struct thread_t* thread_ptr)
{
	struct thread_t* thread = get_by_id(&thread_list, thread_ptr->id);
	if (thread)
	{
		thread->state = ready;
		longjmp(init_func_ctx, 1);
	}
}

struct mutex_t* create_mutex()
{
	struct mutex_t* mutex = (struct mutex_t*)malloc(sizeof(struct mutex_t));

	mutex->owner_id = -1;
	mutex->counter = 0;

	return mutex;
}

void acquire_mutex(struct mutex_t* mtx)
{
	if (mtx->counter == 0)
	{
		mtx->owner_id = current_task->id;
		mtx->counter += 1;
	}
	else
	{
		if (mtx->owner_id == current_task->id)
		{
			mtx->counter += 1;
		}
		else
		{
			longjmp(yield_func_ctx, 1);
		}
	}
}

void release_mutex(struct mutex_t* mtx)
{
	if (mtx->counter != 0)
	{
		if (mtx->owner_id == current_task->id)
			mtx->counter -= 1;
	}
}

void yield()
{
	if (current_task->stack_base != NULL)
	{
		if (current_task->saved_context == NULL)
		{
			func_ptr = current_task->func_ptr;

			if (!setjmp(yield_func_ctx))
			{
				current_task->state = running;
				void* allocated_stack_base = current_task->stack_base;

				__asm
				{
					mov esp, allocated_stack_base;
					call func_ptr;
				}
				task_epilogue();
				longjmp(yield_func_ctx, 1);
			}
		}
		else
		{
			int* buf_boxed = current_task->saved_context;
			int buf_unboxed[16];
			int i = 0;
			for (i = 0; i < 16; i++)
				buf_unboxed[i] = buf_boxed[i];

			int* current_task_stack_base = current_task->stack_base;
			__asm
			{
				mov esp, current_task_stack_base;
				push current_func_ret;
			}
			longjmp(buf_unboxed, 1);
		}
	}
	else
	{

		int* buf_boxed = current_task->saved_context;
		int buf_unboxed[16];
		int i = 0;
		for (i = 0; i < 16; i++)
			buf_unboxed[i] = buf_boxed[i];

		longjmp(buf_unboxed, 1);

	}
}

void set_priority(struct thread_t* thread_ptr, int priority)
{
	struct thread_t* thread = get_by_id(&thread_list, thread_ptr->id);
	if (thread != NULL)
		thread->priority = priority;
}

void join(struct thread_t* thread_ptr)
{
	if (!setjmp(join_func_ctx))
	{
		struct thread_t* thread = get_by_id(&thread_list, thread_ptr->id);
		if (thread != NULL)
		{
			thread->subscriber = current_task;
			current_task->state = blocked;
			current_task->saved_context = buf_boxing(join_func_ctx);
			longjmp(init_func_ctx, 1);
		}
	}
}

void task_epilogue()
{
	if (current_task->subscriber != NULL)
		current_task->subscriber->state = ready;

	remove_by_id(&thread_list, current_task->id);

	longjmp(init_func_ctx, 1);
}

void next()
{
	if (!setjmp(next_func_ctx))
	{
		current_task->saved_context = buf_boxing(next_func_ctx);
		current_task->state = ready;

		longjmp(yield_func_ctx, 1);
	}
}

struct thread_t* next_task()
{
	if (size(&thread_list) == 0)
		return;

	struct thread_t* task_with_max_proirity_level = NULL;
	struct thread_t* current = NULL;

	if (current_task == NULL)
	{
		task_with_max_proirity_level = get_thread_t_with_max_priority(&thread_list);
		return task_with_max_proirity_level;
	}

	for (int i = 0; i < size(&thread_list); i++)
	{
		current = value_at(&thread_list, i);

		if (task_with_max_proirity_level == NULL)
		{
			if ((current->state == ready) && (current_task->id != current->id))
			{
				task_with_max_proirity_level = current;
			}
		}
		else
		{
			if ((current->state == ready) && (current_task->id != current->id))
			{
				if (current->priority > task_with_max_proirity_level->priority)
					task_with_max_proirity_level = current;
			}
		}
	}

	if (task_with_max_proirity_level == NULL)
		task_with_max_proirity_level = get_thread_t_with_max_priority(&thread_list);

	return task_with_max_proirity_level;
}

int* buf_boxing(int buf[16])
{
	int* arr = (int*)malloc(sizeof(jmp_buf));
	int i = 0;
	for (i = 0; i < 16; i++)
	{
		arr[i] = buf[i];
	}
	return arr;
}
