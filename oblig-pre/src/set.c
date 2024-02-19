#include "../include/set.h"
#include "../include/printing.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct setnode SetNode;
struct setnode {
    void *data;
    SetNode *left;
    SetNode *right;
    SetNode *parent;
};

struct set {
    SetNode *root;
    int size;
    cmpfunc_t cmp;
};

struct set_iter {
    SetNode *node;
    set_t *set;
};

static SetNode *setnode_create(void *data) {
    SetNode *node = malloc(sizeof(SetNode));
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    return node;
}

static void setnode_destroy(SetNode *node) {
    if (node) {
        setnode_destroy(node->left);
        setnode_destroy(node->right);
        //free(node->data);
        free(node);
    }
}

static void setnode_copy(set_t *set, SetNode *node) {
    if (node) {
        set_add(set, node->data);    
        setnode_copy(set, node->left);
        setnode_copy(set, node->right);  
    }
}

static void traverse_union(set_t *unionset, SetNode *node) {
    if (node) {
        set_add(unionset, node->data);
        traverse_union(unionset, node->left);
        traverse_union(unionset, node->right);  
    }
}

static void traverse_intersect(set_t *intersectionset, set_t *set, SetNode *node) {
    if (node) {
        if (set_contains(set, node->data)) {
            set_add(intersectionset, node->data);
        }   
        traverse_intersect(intersectionset, set, node->left);
        traverse_intersect(intersectionset, set, node->right);  
    }
}

static void traverse_difference(set_t *differenceset, set_t *set, SetNode *node) {
    if (node) {
        if (!set_contains(set, node->data)) {
            set_add(differenceset, node->data);
        }
        traverse_difference(differenceset, set, node->left);
        traverse_difference(differenceset, set, node->right); 
    }
}


// Create a new set
set_t *set_create(cmpfunc_t cmpfunc) {
    set_t *set = (set_t *)malloc(sizeof(set_t));
    if (!set) {
        return NULL;
    }
    set->root = NULL;
    set->size = 0;
    set->cmp = cmpfunc;
    return set;
}

// Destroy a set
void set_destroy(set_t *set) {
    if (set) {
        setnode_destroy(set->root);
        free(set);
    }
}

// Insert an element into the set sorted
void set_add(set_t *set, void *data) {
    if (set == NULL) {
        ERROR_PRINT("An error occured");
        return;}

    SetNode *tmp = setnode_create(data);
    if (tmp == NULL) {
        ERROR_PRINT("An error occured");
        return;}

    if (set->root == NULL) {
        set->root = tmp;
        set->size++;
        return;
    }

    SetNode *current = set->root;
    SetNode *parent = NULL;

    while (current != NULL) {
        parent = current;
        if (set->cmp(current->data, tmp->data) < 0 ) {
            current = current->right;
        }
        else if (set->cmp(current->data, tmp->data) > 0) {
            current = current->left;
        }
        else {
            break;
        }
    } 
    if (set->cmp(parent->data, tmp->data) < 0) {
        parent->right = tmp;
        tmp->parent = parent;
        set->size++;
    } 
    else if (set->cmp(parent->data, tmp->data) > 0) {
        parent->left = tmp;
        tmp->parent = parent;
        set->size++;
    }
}

int set_size(set_t *set) {
    return set->size;
}

/*
 * Returns 1 if the given element is contained in
 * the given set, 0 otherwise.
 */
int set_contains(set_t *set, void *elem) {
    SetNode *current = set->root;

    while (current != NULL) {
        if (set->cmp(current->data, elem) < 0 ) {
            current = current->right;
        }
        else if (set->cmp(current->data, elem) > 0) {
            current = current->left;
        }
        else {
            return 1;
        }
    }
    return 0;
}

/*
 * Returns the union of the two given sets; the returned
 * set contains all elements that are contained in either
 * a or b.
 */
set_t *set_union(set_t *a, set_t *b) {
    set_t *unionset = set_create(a->cmp);
    traverse_union(unionset, a->root);
    traverse_union(unionset, b->root);
    return unionset;
}

/*
 * Returns the intersection of the two given sets; the
 * returned set contains all elements that are contained
 * in both a and b.
 */
set_t *set_intersection(set_t *a, set_t *b) {
    set_t *intersectionset = set_create(a->cmp);
    SetNode *current = a->root;
    traverse_intersect(intersectionset, b, current);
    return intersectionset;
}

/*
 * Returns the set difference of the two given sets; the
 * returned set contains all elements that are contained
 * in a and not in b.
 */
set_t *set_difference(set_t *a, set_t *b) {
    set_t *differenceset = set_create(a->cmp);
    traverse_difference(differenceset, b, a->root);
    return differenceset;
}

/*
 * Returns a copy of the given set.
 */
set_t *set_copy(set_t *set) {
    set_t *newset = set_create(set->cmp);
    setnode_copy(newset, set->root);
    return newset;
}


/*
 * Creates a new set iterator for iterating over the given set.
 */
set_iter_t *set_createiter(set_t *set) {
    set_iter_t *it = (set_iter_t *)malloc(sizeof(set_iter_t));
    SetNode *current = set->root;
    while (current && current->left) {
        current = current->left;}
    SetNode *tmp = setnode_create(NULL);
    tmp->parent = current;
    it->node = tmp;
    it->set = set;
    return it;
}   

/*
 * Destroys the given set iterator.
 */
void set_destroyiter(set_iter_t *iter) {
    free(iter);
}

/*
 * Returns 0 if the given set iterator has reached the end of the
 * set, or 1 otherwise.
 */
int set_hasnext(set_iter_t *iter) {
    if (!iter) {
        return 0;
    }
    SetNode *temp = iter->set->root;
    while (temp && temp->right) {
        temp = temp->right;
    }
    if (iter->node == temp) {
        return 0;}
    return 1;
}

/*
 * Returns the next element in the sequence represented by the given
 * set iterator.
 */
void *set_next(set_iter_t *iter) {
    if (!iter->node) {
        return NULL;
    }
    if (!iter->set->size) {
        return NULL;
    }
    SetNode *current = iter->node;
    SetNode *next = NULL;

    if (current->right) {
        next = current->right;
        while (next->left) {
            next = next->left;
        }
    } 
    else {
        while (current->parent && current == current->parent->right) {
            current = current->parent;
        }
        next = current->parent;
    }
    iter->node = next; 
    return next->data;
}

/*
static int compare_words(void *a, void *b)
{
    return strcasecmp(a, b);
}

int main(){
    set_t *test_set = set_create(compare_words);
    char a[] = "a";
    char aa[] = "aa";
    char b[] = "b";
    char c[] = "c";
    char d[] = "d";
    char e[] = "e";
    char f[] = "f";
    char g[] = "g";
    set_add(test_set, b);
    set_add(test_set, a);
    //set_add(test_set, aa);
    //printf("%s\n", test_set->root->data);
    //printf("%d\n", compare_words(b, a));
    set_add(test_set, b);
    set_add(test_set, c);
    set_add(test_set, d);
    set_add(test_set, c);
    set_t *copy = set_copy(test_set);
    //printf("%s\n", test_set->root->data);
    //printf("%s\n", copy->root->data);
}

*/