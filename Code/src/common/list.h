#ifndef LIST_H
#define LIST_H

typedef struct list list_t;
typedef void (*list_delete_func)(void *);

#define list(type) (list_new(sizeof(type)))
#define LIST_DATA(list, type) ((type)list_data(list))
#define LIST_LOOP(list, i) for(i = 0; i < list_size(list); i++)

list_t *list_new(int elem_size);
void  list_free(list_t *l);
void  list_delete(list_t *l, list_delete_func del);
int   list_push(list_t *l,void *elem);
int   list_pop(list_t *l,void *elem);
int   list_get(list_t *l,int index,void *elem);
int   list_set(list_t *l,int index,void *elem);
void  list_remove(list_t *l, int index, void *elem);
void  list_reset(list_t *l);
int   list_size(list_t *l);
int   list_elem_size(list_t *l);
void *list_data(list_t *l);
void  list_sort(list_t *l, int (*compar)(const void * a, const void *b));
void  list_insert(list_t *l, int i, void *elem);
void  list_swap(list_t *l, int i, int j);

#endif
