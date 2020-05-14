#ifndef _TREAP_
#define _TREAP_

typedef struct Treap {
    int key; // = score
    int priority;
    struct Treap *left, *right;
    int size;
    // additional entites
    char *str;
} Treap;

typedef struct TreapPair {
    Treap *first, *second;
} TreapPair;

Treap*      Treap_new(const int key, const char *str);
void        Treap_freeAll(Treap *self);
void        Treap_freeThis(Treap *self);
void        Treap_setLeft(Treap *self, Treap *newLeft);
void        Treap_setRight(Treap *self, Treap *newRight);
void        Treap_updateSize(Treap *self);
TreapPair   Treap_split(Treap *self, const int key);
Treap*      Treap_insert(Treap *self, Treap *node);
Treap*      Treap_merge(Treap *a, Treap *b);
Treap*      Treap_erase(Treap *self, const int key);
Treap*      Treap_eraseKth(Treap *self, const int k);
#endif
