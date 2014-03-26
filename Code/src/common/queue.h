#ifndef QUEUE_H
#define QUEUE_H

typedef struct queue queue_t;

queue_t *queue_init(int size, int elem_size);
void     queue_delete(queue_t *q);
int      queue_read(queue_t *q,  void *data);
int      queue_read_nb(queue_t *q, void *data);
void     queue_write(queue_t *q, void *data);
void     queue_wait_empty(queue_t *q);

#endif
