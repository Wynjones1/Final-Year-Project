#include "queue.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

struct queue
{
	int             capacity;
	int             elem_size;
	void           *data;
	int             head;
	int             tail;
	int             num_valid;
	pthread_mutex_t mutex;
	pthread_cond_t  cond;
};

queue_t *queue_init(int capacity, int elem_size)
{
	queue_t *out    = malloc(sizeof(queue_t));
	out->data       = malloc(capacity * elem_size);
	out->capacity   = capacity;
	out->elem_size  = elem_size;
	out->head       = 0;
	out->tail       = 0;
	out->num_valid  = 0;
	pthread_mutex_init(&out->mutex, NULL);
	pthread_cond_init(&out->cond,   NULL);
	return out;
}

void queue_delete(queue_t *q)
{
	free(q->data);
	free(q);
}

int queue_read(queue_t *q,  void *data)
{
	int retval;
	pthread_mutex_lock(&q->mutex);

	while(q->num_valid == 0)
	{
		pthread_cond_wait(&q->cond, &q->mutex);
	}

	void *elem = q->data + q->head * q->elem_size;
	memcpy(data, elem, q->elem_size);
	q->head = (q->head + 1) % q->capacity;
	retval = --q->num_valid;

	pthread_cond_broadcast(&q->cond);
	pthread_mutex_unlock(&q->mutex);
	return retval;
}

int queue_read_nb(queue_t *q, void *data)
{
	if(q->num_valid)
	{
		pthread_mutex_lock(&q->mutex);

		if(q->num_valid == 0)
		{
			pthread_mutex_unlock(&q->mutex);
			return 0;
		}

		void *elem = q->data + q->head * q->elem_size;
		memcpy(data, elem, q->elem_size);
		q->head = (q->head + 1) % q->capacity;
		q->num_valid--;

		pthread_cond_broadcast(&q->cond);
		pthread_mutex_unlock(&q->mutex);
		return 1;
	}
	return 0;
}

void queue_write(queue_t *q, void *data)
{
	pthread_mutex_lock(&q->mutex);

	while(q->num_valid == q->capacity)
	{
		pthread_cond_wait(&q->cond, &q->mutex);
	}

	void *elem = q->data + q->tail * q->elem_size;
	memcpy(elem, data, q->elem_size);
	q->tail = (q->tail + 1) % q->capacity;
	q->num_valid++;

	pthread_cond_broadcast(&q->cond);
	pthread_mutex_unlock(&q->mutex);
}

void queue_wait_empty(queue_t *q)
{
	pthread_mutex_lock(&q->mutex);

	while(q->num_valid)
	{
		pthread_cond_wait(&q->cond, &q->mutex);
	}

	pthread_cond_broadcast(&q->cond);
	pthread_mutex_unlock(&q->mutex);
}
