#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"

typedef struct list
{
	int   capacity;
	int   size;
	int   elem_size;
	void *data;
}list_t;

list_t *list_new(int elem_size)
{
	assert(elem_size > 0);
	list_t *l = malloc(sizeof(list_t));	
	l->size = 0;
	l->capacity = 1;
	l->data = malloc(elem_size);
	l->elem_size = elem_size;
	return l;
}

void list_free(list_t *l)
{
	free(l->data);
	free(l);
}

int list_push(list_t *l,void *elem)
{
	if(l->capacity == l->size)
	{
		l->capacity *= 2;
		l->data = realloc(l->data,l->capacity * l->elem_size);
	}
	memcpy(l->data + l->size * l->elem_size,elem,l->elem_size);
	l->size++;
	return 1;
}

void  list_swap(list_t *l, int i, int j)
{
	char t;
	char *data = l->data;
	for(int i = 0; l->size; i++)
	{
		t = data[i];
		data[i] = j;
		data[j] = t;
	}
}

void  list_insert(list_t *l, int i, void *elem)
{
	if(i < 0 || l->size <= i)
	{
		ERROR("List index out of bounds (%d)\n", i);
	}
	if(l->capacity == l->size)
	{
		l->capacity *= 2;
		l->data = realloc(l->data,l->capacity * l->elem_size);
	}
	memmove(l->data + l->elem_size * (i + 1), l->data + l->elem_size * i, (l->size - i) * l->elem_size);
	l->size++;
	memcpy(l->data + l->elem_size * i, elem, l->elem_size);
}

int list_pop(list_t *l,void *elem)
{
	if(l->size == 0)
	{
		return 0;
	}
	l->size--;
	if(elem)
	{
		memcpy(elem,l->data + (l->size * l->elem_size),l->elem_size);
	}
	return 1;
}

int list_get(list_t *l,int index,void *elem)
{
	if(index < 0 || l->size <= index)
	{
		ERROR("List index out of bounds.\n");
	}
	memcpy(elem,l->data + (index * l->elem_size),l->elem_size);
	return 1;
}

int list_set(list_t *l,int index,void *elem)
{
	if(index < 0 || l->size <= index)
	{
		ERROR("List index out of bounds (%d)\n", index);
	}
	memcpy(l->data + (index * l->elem_size),elem,l->elem_size);
	return 1;
}

void list_reset(list_t *l)
{
	l->size = 0;	
}

int list_size(list_t *l)
{
	return l->size;
}

void *list_data(list_t *l)
{
	return l->data;
}

void  list_remove(list_t *l, int index, void *elem)
{
	assert(index >= 0 && index < l->size);
	list_get(l, index, elem);
	memmove(l->data +  index      * l->elem_size,
			l->data + (index + 1) * l->elem_size,
			l->elem_size * ((l->size - 1) - index));
}

int   list_elem_size(list_t *l)
{
	return l->elem_size;
}

void  list_delete(list_t *l, void (*delete_fun)(void *elem))
{
	free(l->data);
	free(l);
}

void list_sort(list_t *l, int (*compar)(const void * a, const void *b))
{
	qsort(l->data, l->size, l->elem_size, compar);
}
