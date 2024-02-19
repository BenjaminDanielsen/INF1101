#include "common.h"
#include "../include/set.h"
#include "../include/printing.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct setnode SetNode;
struct setnode
{
    SetNode *next;
    SetNode *prev;
    void *elem;
};

struct set
{
    SetNode *head;
    SetNode *tail;
    int size;
    cmpfunc_t cmp;
};

struct set_iter
{
    SetNode *node;
};

static SetNode *node_create(void *elem)
{
    SetNode *node = (SetNode *)malloc(sizeof(SetNode));
    if (node == NULL)
    {
        goto error;
    }

    node->next = NULL;
    node->prev = NULL;
    node->elem = elem;
    return node;

error:
    return NULL;

}

set_t *set_create(cmpfunc_t cmpfunc)
{
    set_t *set = malloc(sizeof(set_t));
    if (set == NULL)
    {
        goto error;
    }

    set->head = NULL;
    set->tail = NULL;
    set->size = 0;
    set->cmp = cmpfunc;
    return set;

error:
    return NULL;
}

void set_destroy(set_t *set)
{
    SetNode *node = set->head;
    while (node != NULL)
    {
        SetNode *tmp = node;
        node = node->next;
        free(tmp);
    }
    free(set);
}

int set_size(set_t *set)
{
    return set->size;
}

static int set_addfirst(set_t *set, void *elem)
{
    SetNode *node = node_create(elem);
    if (node == NULL)
    {
        return -1;
    }

    if (set->head == NULL)
    {
        set->head = set->tail = node;
    }
    else
    {
        set->head->prev = node;
        node->next = set->head;
        set->head = node;
    }
    set->size++;
    return 0;
}

static int set_addlast(set_t *set, void *elem)
{
    SetNode *node = node_create(elem);
    if (node == NULL)
    {
        return -1;
    }

    if (set->head == NULL)
    {
        set->head = set->tail = node;
    }
    else
    {
        set->tail->next = node;
        node->prev = set->tail;
        set->tail = node;
    }
    set->size++;
    return 0;
}

void set_add(set_t *set, void *elem)
{
    SetNode *iter = set->head;

    if (iter == NULL)
    {
        set_addfirst(set, elem);
    }
    else if (set->cmp(elem, set->head->elem) <= 0)
    {
        set_addfirst(set, elem);
    }
    else if (set->cmp(elem, set->tail->elem) > 0)
    {
        set_addlast(set, elem);
    }
    else
    {
        while (iter->next != NULL)
        {
            if (set->cmp(elem, iter->next->elem) <= 0)
            {
                break;
            }
            iter = iter->next;
        }
        SetNode *node = node_create(elem);
        node->next = iter->next;
        iter->next->prev = node;
        iter->next = node;
        node->prev = iter;
        set->size++;
    }

    return;
}

int set_contains(set_t *set, void *elem) {
    SetNode *node = set->head;
    while (node != NULL) {
        if (set->cmp(elem, node->elem) == 0) {
            return 1; // Element found
        }
        node = node->next;
    }
    return 0; // Element not found
}

set_t *set_union(set_t *a, set_t *b) {
    set_t *result = set_create(a->cmp);

    SetNode *node = a->head;
    while (node != NULL) {
        set_add(result, node->elem);
        node = node->next;
    }

    node = b->head;
    while (node != NULL) {
        if (!set_contains(result, node->elem)) {
            set_add(result, node->elem);
        }
        node = node->next;
    }

    return result;
}

set_t *set_intersection(set_t *a, set_t *b) {
    set_t *result = set_create(a->cmp);

    SetNode *node = a->head;
    while (node != NULL) {
        if (set_contains(b, node->elem)) {
            set_add(result, node->elem);
        }
        node = node->next;
    }

    return result;
}

set_t *set_difference(set_t *a, set_t *b) {
    set_t *result = set_create(a->cmp);

    SetNode *node = a->head;
    while (node != NULL) {
        if (!set_contains(b, node->elem)) {
            set_add(result, node->elem);
        }
        node = node->next;
    }

    return result;
}

set_t *set_copy(set_t *set) {
    set_t *result = set_create(set->cmp);

    SetNode *node = set->head;
    while (node != NULL) {
        set_add(result, node->elem);
        node = node->next;
    }

    return result;
}

set_iter_t *set_createiter(set_t *set)
{
    set_iter_t *iter = (set_iter_t *)malloc(sizeof(set_iter_t));
    if (iter == NULL)
    {
        goto error;
    }

    iter->node = set->head;
    return iter;

error:
    return NULL;

}


void set_destroyiter(set_iter_t *iter)
{
    free(iter);
}

int set_hasnext(set_iter_t *iter)
{
    if (iter->node == NULL)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void *set_next(set_iter_t *iter)
{
    if (iter->node == NULL)
    {
        return NULL;
    }
    else
    {
        void *elem = iter->node->elem;
        iter->node = iter->node->next;
        return elem;
    }
}
