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
    node *next;
    node *node = alist->first;

    while (node) {
        next = node->next;
        free(node);
        node = next;
    }
    free(alist);
}

node *next(node *node) { return node->next; }

list *list_filter(p_fn fn, list *a_list) {
    list *dest_list = list_make();
    node *node = a_list->first;
    while (node) {
        if (fn(node->item)) {
            list_insert(dest_list, node->item);
        }
        node = node->next;
    }
    return dest_list;
}

void **list_to_array(list *list) {
    void **a = calloc(list->length, sizeof(void *));
    node *node = list->first;
    int count = 0;
    while (node) {
        a[count++] = node->item;
        node = node->next;
    }
    return a;
}

list *copy_list(list *l) {
    list *lc = list_make();
    node *n = l->first;
    while (n) {
        list_insert(lc, n->item);
        n = n->next;
    }
    return lc;
}

void concat_2_lists_(list *l1, list *l2) {
    l1->last->next = l2->first;
    l1->last = l2->last;
    l1->length += l2->length;
}

void *item_at(list *a_list, int k) {
    if (k >= a_list->length) {
        fprintf(stderr, "Index error: `k` is out of bounds. %d/%d", k,
                a_list->length);
        exit(-1);
    }

    node *n = a_list->first;
    while (k > 0) {
        n = n->next;
        k--;
    }
    return n->item;
}

list *slice_at(list *a_list, int start, int end) {

    if (start >= a_list->length || start < 0) {
        fprintf(stderr, "Index error: `start` is out of bounds. %d/%d", start,
                a_list->length);
        exit(-1);
    }

    if (end >= a_list->length) {
        fprintf(stderr, "Index error: `end` is out of bounds. %d/%d", end,
                a_list->length);
        exit(-1);
    }
    if (start > end) {

        fprintf(stderr, "start is bigger than end: start=%d, end=%d", start,
                end);
        exit(-1);
    }
    if (end < 0) {
        end = a_list->length;
    }

    int amount = end - start;
    node *n = a_list->first;
    while (start > 0) {
        n = n->next;
        start--;
    }
    list *dest = list_make();

    while (amount > 0) {
        list_insert(dest, n->item);
        n = n->next;
        amount--;
    }
    return dest;
}
