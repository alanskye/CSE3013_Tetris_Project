#include "treap.h"

#include <stdlib.h>
#include <string.h>

Treap* Treap_new(const int key, const char *str) {
    Treap* ret = (Treap*)calloc(1, sizeof(Treap));
    if (str != NULL) {
        ret->str = (char *)calloc(strlen(str) + 1, sizeof(char));
        strcpy(ret->str, str);
    }
    ret->key = key;
    ret->priority = rand();
    ret->size = 1;
    return ret;
}

void Treap_freeThis(Treap *self) {
    if (self->str) free(self->str);
    free(self);
}

void Treap_freeAll(Treap *self) {
    if (self->left)     Treap_freeAll(self->left);
    if (self->right)    Treap_freeAll(self->right);
    Treap_freeThis(self);
}

void Treap_setLeft(Treap *self, Treap *newLeft) {
    self->left  = newLeft;
    Treap_updateSize(self);
}

void Treap_setRight(Treap *self, Treap *newRight) {
    self->right = newRight;
    Treap_updateSize(self);
}

void Treap_updateSize(Treap *self) {
    self->size = 1;
    if (self->left)     self->size += self->left->size;
    if (self->right)    self->size += self->right->size;
}

TreapPair Treap_split(Treap *self, const int key) {
    if (self == NULL) return (TreapPair){NULL, NULL}; 
    if (self->key > key) {
        TreapPair rs = Treap_split(self->right, key);
        Treap_setRight(self, rs.first);
        return (TreapPair){self, rs.second};
    }
    else {
        TreapPair ls = Treap_split(self->left, key);
        Treap_setLeft(self, ls.second);
        return (TreapPair){ls.first, self};
    }
}

Treap* Treap_insert(Treap *self, Treap *node) {
    if (self == NULL) return node;
    if (self->priority < node->priority) {
        TreapPair sp = Treap_split(self, node->key);
        Treap_setLeft(node, sp.first);
        Treap_setRight(node, sp.second);
        return node;
    }
    else if (node->key > self->key)
        Treap_setLeft(self, Treap_insert(self->left, node));
    else
        Treap_setRight(self, Treap_insert(self->right, node));
    return self;
}

Treap* Treap_merge(Treap *a, Treap *b) {
    // a = left subtree, b = right subtree
    if (a == NULL) return b;
    if (b == NULL) return a;
    if (a->priority < b->priority) {
        Treap_setLeft(b, Treap_merge(a, b->left));
        return b;
    }
    else {
        Treap_setRight(a, Treap_merge(a->right, b));
        return a;
    }
}

// self를 루트로 하는 Treap에서 key를 지우고 결과 Treap의 self를 반환
Treap* Treap_erase(Treap *self, const int key) {
    if (self == NULL) return self;
    if (self->key == key) {
        Treap *ret = Treap_merge(self->left, self->right);
        Treap_freeThis(self);
        return ret;
    }
    if (key > self->key)
        Treap_setLeft(self, Treap_erase(self->left, key));
    else
        Treap_setRight(self, Treap_erase(self->right, key));
    return self;
}

Treap* Treap_eraseKth(Treap *self, const int k) {
    int leftSize = 0;
    if (self->left != NULL) leftSize = self->left->size;
    if (k == leftSize + 1) { // kth element //
        Treap *ret = Treap_merge(self->left, self->right);
        Treap_freeThis(self);
        return ret;
    }
    if (k <= leftSize)
        Treap_setLeft(self, Treap_eraseKth(self->left, k));
    else
        Treap_setRight(self, Treap_eraseKth(self->right, k - (leftSize + 1)));
    return self;
}

