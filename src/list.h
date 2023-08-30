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

typedef struct array_list {
    void **items;
    int length;
    list *list;
    int initial_length;
    int expand_length;
} array_list;

list *list_make();
void list_insert(list *, void *);

typedef void (*fn)(void *);
typedef int (*p_fn)(void *);
void display_list(list *, fn);
list *map_list(list *, fn);

void *pop_first(list *);
void *pop_last(list *);

node *next_node(node *node);

void *get_first(list *);
void *get_last(list *);
void **list_to_array(list *);

void free_list(list *);
node *next(node *);
list *list_filter(p_fn, list *);

#endif
