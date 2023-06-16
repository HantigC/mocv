#include "list.h"
#include <stdio.h>
#include <stdlib.h>

list *list_make() {
    list *a_list = (list *)malloc(sizeof(list));
    a_list->last = 0;
    a_list->first = 0;
    a_list->length = 0;
    return a_list;
}

void list_insert(list *alist, void *item) {
    node *anode = (node *)malloc(sizeof(node));
    anode->item = item;
    anode->next = 0;
    if (alist->length) {
        alist->last->next = anode;
        anode->prev = alist->last;
    } else {
        alist->first = anode;
        anode->prev = 0;
    }
    alist->last = anode;
    ++alist->length;
}

void display_list(list *alist, fn display_fn) {
    printf("length: %d\n", alist->length);
    printf("[\n");
    node *node = alist->first;
    while (node) {
        display_fn(node->item);
        printf(",\n");
        node = node->next;
    }
    printf("]\n");
}

void *pop_last(list *alist) {
    void *item = 0;
    if (alist->length) {
        item = alist->last->item;
        node *pre_last = alist->last->prev;
        if (pre_last) {
            pre_last->next = 0;
        }
        free(alist->last);
        alist->last = pre_last;
        --alist->length;
    }
    return item;
}

void *pop_first(list *alist) {
    void *item = 0;
    if (alist->length) {
        item = alist->first->item;
        node *second = alist->first->next;
        if (second) {
            second->prev = 0;
        }
        free(alist->first);
        alist->first = second;
        --alist->length;
    }
    return item;
}
void *get_first(list *alist) { return alist->first->item; }
void *get_last(list *alist) { return alist->last->item; }

void free_list(list *alist) {
    node *node = alist->first;
    while (node) {
        free(node);
        node = node->next;
    }
    free(alist);
}