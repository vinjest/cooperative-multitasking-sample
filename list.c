#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <ctype.h>
#include "list.h"

int push_back(struct Task** head, int* context);
int* pop_front(struct Task** head);
int clear(struct Element* head);
int size(struct Task** head);

struct Task {
	int* saved_context;
	struct Task* next;
};

int push_back(struct Task** head, int* context) {
	struct Task* current = *head;
	struct Task* newNode;

	newNode = (struct Task*)malloc(sizeof(struct Task));
	newNode->saved_context = context;
	newNode->next = NULL;

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


int clear(struct Element* head)
{
	struct Task* current = head;
	if (head == NULL)
		return 1;

	struct Task* temp = NULL;

	while (current->next != NULL)
	{
		temp = current->next;
		free(current->saved_context);
		free(current);
		current = temp;
	}
	free(current->saved_context);
	free(current);

	return 0;
}

int* pop_front(struct Task** head)
{
	if (*head == NULL)
		return -1;

	struct Task* current = *head;
	int* result = current->saved_context;

	*head = (*head)->next;
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