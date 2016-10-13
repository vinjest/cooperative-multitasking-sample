#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <ctype.h>
#include "list.h"
#include "enums.h"
#include <string.h>

int push_back(struct thread_t** head, int* current_cmd, int current_id, enum priority current_priority, void* stack_base_ptr);
struct thread_t* pop_front(struct thread_t** head);
struct thread_t* pop_back(struct thread_t** head);
int clear(struct thread_t** head);
int size(struct thread_t** head);
struct thread_t* value_at(struct thread_t** head, int position);
int remove_by_id(struct thread_t** head, int current_id);
struct thread_t* get_thread_t_with_max_priority(struct thread_t** head);
struct thread_t* get_by_id(struct thread_t** head, int current_id);

int push_back(struct thread_t** head, int* func_ptr, int current_id, enum priority current_priority, void* stack_base_ptr)
{
	struct thread_t* current = *head;
	struct thread_t* new_node;

	new_node = (struct thread_t*)malloc(sizeof(struct thread_t));
	new_node->id = current_id;
	new_node->saved_context = NULL;
	new_node->next = NULL;
	new_node->func_ptr = func_ptr;
	new_node->stack_base = stack_base_ptr;
	new_node->subscriber = NULL;

	new_node->state = ready;
	new_node->priority = current_priority;

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

	return 0;
}

struct thread_t* value_at(struct thread_t** head, int position)
{
	struct thread_t* current = *head;
	if (head == NULL)
		return -1;
	int current_position = 0;

	while (current->next != NULL)
	{
		if (current_position == position)
		{
			return current;
		}

		current_position++;
		current = current->next;
	}
	if (current_position == position)
	{
		return current;
	}
	return NULL;
}

int remove_by_id(struct thread_t** head, int current_id)
{
	if (*head == NULL)
		return -1;

	struct thread_t* current = *head;
	struct thread_t* temp = NULL;

	if (current->id == current_id)
	{
		temp = pop_front(head);
		temp = NULL;

		return 0;
	}
	else
	{
		while (current->next != NULL)
		{
			if (current->id == current_id)
			{
				temp->next = current->next;
				free(current);
				return 0;
			}
			temp = current;
			current = current->next;
		}
		if (current->id == current_id)
		{
			temp = pop_back(head);
			return 0;
		}
	}
	return -2;
}

struct thread_t* get_by_id(struct thread_t** head, int current_id)
{
	struct thread_t* current = *head;
	if (*head == NULL)
		return NULL;

	while (current->next != NULL)
	{
		if (current->id == current_id)
		{
			return current;
		}
		current = current->next;
	}
	if (current->id == current_id)
	{
		return current;
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

struct thread_t* get_thread_t_with_max_priority(struct thread_t** head)
{
	if (*head == NULL)
		return -1;

	struct thread_t* current = *head;
	struct thread_t* thread_t_with_max_priority = NULL;

	while (current->next != NULL)
	{
		if (thread_t_with_max_priority == NULL)
		{
			if (current->state == ready)
				thread_t_with_max_priority = current;
		}
		else
		{
			if (current->priority > thread_t_with_max_priority->priority)
				thread_t_with_max_priority = current;
		}
		current = current->next;
	}
	if (thread_t_with_max_priority == NULL)
	{
		if (current->state == ready)
			thread_t_with_max_priority = current;
	}
	else
	{
		if (current->priority > thread_t_with_max_priority->priority)
			thread_t_with_max_priority = current;
	}

	return thread_t_with_max_priority;
}

struct thread_t* pop_front(struct thread_t** head)
{
	if (*head == NULL)
		return -1;

	struct thread_t* current = *head;
	struct thread_t* result = (struct thread_t*)malloc(sizeof(struct thread_t));
	result = current;

	*head = (*head)->next;
	free(current);

	return result;
}

struct thread_t* pop_back(struct thread_t** head)
{
	if (*head == NULL)
		return -1;

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
	free(current);

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
