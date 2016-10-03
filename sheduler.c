#include <stdio.h>
#include <setjmp.h>
#include "sheduler.h"
#include "list.h"
#include "enums.h"

#define DEFAULT_STACK_FRAME_OFFSET 1000

void(*func_ptr)() = NULL;

int mutex = 0;

struct Task* task_list;
struct Task* current_task = NULL;
unsigned char task_count = 0;

int current_task_id = 1;
int task_counter = 0;

void exec(void(*p_func_first)(), ...);
void yield();
void next();
struct Task* next_task();
void task_epilogue();
void enter_critical_section();
void leave_critical_section();
void sleep();
void wakeup(int* func_ptr);
int* intptr_boxing(int* value);

int* current_func_ret = &task_epilogue;
int* next_func_ret = NULL;

static jmp_buf yield_func_ctx;
static jmp_buf next_func_ctx;

void exec(void(*p_func_first)(), ...)
{
	int* params_pointer = &p_func_first;
	task_count = 0;
	unsigned end_of_parameters = 0;

	while (*params_pointer != -1)
	{
		params_pointer++;
		task_count++;
	}
	params_pointer = &p_func_first;

	int current_id = 0;
	while (*params_pointer != -1)
	{
		if (end_of_parameters)
		{
			push_back(&task_list, *params_pointer, medium, current_id);
			current_id++;
		}
		else
		{
			int priority = *(params_pointer + task_count + 1);
			if (priority == -1)
			{
				end_of_parameters = 1;
				push_back(&task_list, *params_pointer, medium, current_id);
				current_id++;
			}
			else
			{
				push_back(&task_list, *params_pointer, priority, current_id);
				current_id++;
			}
		}

		params_pointer++;
	}

	while (size(&task_list) != 0)
	{
		current_task = next_task();
		if (current_task == NULL)
			break;

		yield();
	}
	clear(&task_list);
}

void yield()
{
	if (current_task->saved_eip == NULL)
	{
		for (task_counter = 0; task_counter < (current_task->id + 1); task_counter++)
		{
			char buf[1000];
			buf[999] = 1;
		}
		func_ptr = current_task->func_ptr;

		if (!setjmp(yield_func_ctx))
		{
			current_task->state = running;
			func_ptr();
		}
	}
	else
	{
		for (task_counter = 0; task_counter < (current_task->id + 1); task_counter++)
		{
			char buf[1000];
			buf[999] = 1;
		}

		_asm
		{
			push current_func_ret;
		}

		longjmp(next_func_ctx, 1);
	}
}

void task_epilogue()
{
	remove_by_id(&task_list, current_task->id);

	longjmp(yield_func_ctx, 1);
}

void enter_critical_section()
{
	if (mutex)
	{
		longjmp(yield_func_ctx, 1);
	}
	mutex = current_task->id + 1;
}

void leave_critical_section()
{
	mutex = 0;
}

void sleep()
{
	current_task->state = blocked;

	int* ret;
	int* esp_value;

	_asm
	{
		mov eax, 4[ebp];
		mov ret, eax;
		xor eax, eax;
		mov eax, 0[ebp];
		mov esp_value, eax;
		xor eax, eax;
	}
	current_task->saved_eip = intptr_boxing(ret);
	current_task->saved_esp = intptr_boxing(esp_value);
	longjmp(yield_func_ctx, 1);
}

void wakeup(int* func_ptr)
{
	for (int i = 0; i < size(&task_list); i++)
	{
		struct Task* current = value_at(&task_list, i);
		if (current->func_ptr == func_ptr)
		{
			current->state = ready;
			break;
		}
	}
}

void next()
{
	int* ret;
	int* esp_value;

	if (!setjmp(next_func_ctx))
	{
		current_task->state = ready;
		_asm
		{
			mov eax, 4[ebp];
			mov ret, eax;
			xor eax, eax;
			mov eax, 0[ebp];
			mov esp_value, eax;
			xor eax, eax;
		}

		int* ret_boxed = intptr_boxing(ret);
		int* esp_boxed = intptr_boxing(esp_value);

		current_task->saved_eip = ret_boxed;
		current_task->saved_esp = esp_boxed;

		longjmp(yield_func_ctx, 1);
	}
	else
	{
		int* current_task_continue_addr = current_task->saved_eip;
		int* current_task_esp = current_task->saved_esp;
		current_task->state = running;
		_asm
		{
			mov eax, current_task_continue_addr;
			mov 4[ebp], eax;
			xor eax, eax;
			mov eax, current_task_esp;
			mov 0[ebp], eax;
			xor eax, eax;
		}
	}
}

struct Task* next_task()
{
	if (size(&task_list) == 0)
		return;

	struct Task* task_with_max_proirity_level = NULL;
	struct Task* current = NULL;

	if (current_task == NULL)
	{
		task_with_max_proirity_level = get_task_with_max_priority(&task_list);
		return task_with_max_proirity_level;
	}

	for (int i = 0; i < size(&task_list); i++)
	{
		current = value_at(&task_list, i);

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
		task_with_max_proirity_level = get_task_with_max_priority(&task_list);

	return task_with_max_proirity_level;
}

int* intptr_boxing(int* value)
{
	int* intptr_boxed = (int*)malloc(sizeof(int*));
	intptr_boxed = value;

	return intptr_boxed;
}