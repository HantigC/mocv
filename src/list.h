#ifndef LIST_H
#define LIST_H
typedef struct node {
    void *item;
    struct node *next;
    struct node *prev;
} node;
typedef struct list {
    int length;
    node *first;
    node *last;

} list;

list *list_make();
void list_insert(list *, void *);

typedef void (*fn)(void *);
void display_list(list *, fn);
list *map_list(list *, fn);

void *pop_first(list *);
void *pop_last(list *);

void *get_first(list *);
void *get_last(list *);

void free_list(list *);
#endif
