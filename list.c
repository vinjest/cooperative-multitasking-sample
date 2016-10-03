#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <ctype.h>
#include "list.h"
#include "enums.h"
#include <string.h>

int push_back(struct Task** head, int* current_cmd, enum priority current_priority, int current_id);
struct Task* pop_front(struct Task** head);
struct Task* pop_back(struct Task** head);
int clear(struct Task** head);
int size(struct Task** head);
struct Task* value_at(struct Task** head, int position);
int remove_by_id(struct Task** head, int current_id);
struct Task* get_task_with_max_priority(struct Task** head);

int push_back(struct Task** head, int* func_ptr, enum priority current_priority, int current_id)
{
	struct Task* current = *head;
	struct Task* newNode;

	newNode = (struct Task*)malloc(sizeof(struct Task));
	newNode->id = current_id;
	newNode->saved_eip = NULL;
	newNode->saved_esp = NULL;
	newNode->next = NULL;
	newNode->func_ptr = func_ptr;

	newNode->state = ready;
	newNode->priority = current_priority;

	if (current == NULL)
	{
		*head = newNode;
	}
	else
	{
		while (current->next != NULL)
		{
			current = current->next;
		}
		current->next = newNode;
	}

	return 0;
}

struct Task* value_at(struct Task** head, int position)
{
	struct Task* current = *head;
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

int remove_by_id(struct Task** head, int current_id)
{
	if (*head == NULL)
		return -1;

	struct Task* current = *head;
	struct Task* temp = NULL;

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

int clear(struct Task** head)
{
	struct Task* current = *head;
	if (head == NULL)
		return 1;

	struct Task* temp = NULL;

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

struct Task* get_task_with_max_priority(struct Task** head)
{
	if (*head == NULL)
		return -1;

	struct Task* current = *head;
	struct Task* task_with_max_priority = NULL;

	while (current->next != NULL)
	{
		if (task_with_max_priority == NULL)
		{
			if (current->state == ready)
				task_with_max_priority = current;
		}
		else
		{
			if (current->priority > task_with_max_priority->priority)
				task_with_max_priority = current;
		}
		current = current->next;
	}
	if (task_with_max_priority == NULL)
	{
		if (current->state == ready)
			task_with_max_priority = current;
	}
	else
	{
		if (current->priority > task_with_max_priority->priority)
			task_with_max_priority = current;
	}

	return task_with_max_priority;
}

struct Task* pop_front(struct Task** head)
{
	if (*head == NULL)
		return -1;

	struct Task* current = *head;
	struct Task* result = (struct Task*)malloc(sizeof(struct Task));
	result = current;

	*head = (*head)->next;
	free(current);

	return result;
}

struct Task* pop_back(struct Task** head)
{
	if (*head == NULL)
		return -1;

	struct Task* current = *head;
	struct Task* temp = *head;
	struct Task* result = (struct Task*)malloc(sizeof(struct Task));

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

int size(struct Task** head)
{
	if (*head == NULL)
		return 0;

	struct Task* current = *head;

	int size = 0;

	while (current->next != NULL)
	{
		size++;
		current = current->next;
	}
	size++;
	return size;
}