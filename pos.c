#include "pos.h"

void check_malloc(void* p) {
    if (p == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
}

void check_null_pointer(void* p) {
    if (p == NULL) {
        fprintf(stderr, "Null pointer cannot be handled\n");
        exit(1);
    }
}

pos make_pos(unsigned int r, unsigned int c) {
    pos p;
    p.r = r;
    p.c = c;
    return p;
}

posqueue* posqueue_new() {
    posqueue* q = (posqueue*)malloc(sizeof(posqueue));
    check_malloc(q);
    q->head = NULL;
    q->tail = NULL;
    q->len = 0;
    return q;
}

void pos_enqueue(posqueue* q, pos p) {
    check_null_pointer(q);
    pq_entry* added_pos = (pq_entry*)malloc(sizeof(pq_entry));
    check_malloc(added_pos);
    added_pos->p = p;
    added_pos->prev = q->tail;
    added_pos->next = NULL;
    if (q->tail) {
        q->tail->next = added_pos;
    }
    q->tail = added_pos;
    if (q->head == NULL) {
        q->head = added_pos;
    }
    q->len += 1;
}

pos pos_dequeue(posqueue* q) {
    check_null_pointer(q);
    check_null_pointer(q->head);
    pos res = q->head->p;
    pq_entry* tmp = q->head;
    q->head = q->head->next;
    free(tmp);
    if (q->head == NULL) {
        q->tail = NULL;
    } else {
        q->head->prev = NULL;
    }
    q->len -= 1;
    return res;
}

pos posqueue_remback(posqueue* q) {
    check_null_pointer(q);
    check_null_pointer(q->head);
    pos res = q->tail->p;
    pq_entry* tmp = q->tail;
    q->tail = q->tail->prev;
    free(tmp);
    if (q->tail == NULL) {
        q->head = NULL;
    } else {
        q->tail->next = NULL;
    }
    q->len -= 1;
    return res;
}

void posqueue_free(posqueue* q) {
    pq_entry* tmp;
    while(q->head) {
        tmp = q->head;
        q->head = q->head->next;
        free(tmp);
    }
    free(q);
}

