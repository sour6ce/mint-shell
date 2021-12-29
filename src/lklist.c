#include "lklist.h"

void lkinit(lklist* a){
    a->first=NULL;
    a->last=NULL;
}

void lkappend(lklist *list, void * value){
    node* newlink=NEW(node,1);
    newlink->data=value;
    if (list->last) {
        newlink->previous=list->last;
    } else {
        newlink->previous=NULL;
    }
    newlink->next=NULL;
    if (list->last) {
        list->last->next=newlink;
    } else {
        list->first=newlink;
    }
    list->last=newlink;
}

void * lkquit(lklist *list){
    node* temp;
    if (list->first){
        temp=list->first;
    } else {
        return NULL;
    }
    void *data=temp->data;
    if (temp->next) {
        list->first=temp->next;
        list->first->previous=NULL;
    } else {
        list->first=NULL;
        list->last=NULL;
    }
    free(temp);
    return data;
}

size_t lklen(lklist *list) {
    node *actual=(*list).first;
    size_t size=0;
    while (actual!=NULL) {
        actual=actual->next;
        size++;
    }
    return size;
}

node *lkndat(lklist *list, size_t index) {
    node *actual=(*list).first;
    size_t size=0;
    do {
        if (size>=index)
        {
            return actual;
        }
        actual=actual->next;
        size++;
    } while (actual!=NULL);
    return actual;
}

void lkins(lklist *list, size_t index, void * value) {
    size_t len=lklen(list);
    if (lkisempty(*list) || lkhasone(*list) || index>=len) {
        lkappend(list,value);
    } else {
        node * old=lkndat(list,index);
        node *new=NEW(node,1);
        new->data=value;
        new->next=old;
        new->previous=(index<=0)? NULL : old->previous;
        if (index<=0) {
            list->first=new;
        } else {
            old->previous->next=new;
        }
        old->previous=new;
    }
}

void *lkrm(lklist *list, size_t index) {
    size_t len=lklen(list);
    if (lkisempty(*list)) {
        return NULL;
    }
    if(lkhasone(*list) || index<=0) {
        return lkquit(list);
    }
    node *old=(index>=len)? list->last : lkndat(list,index);
    if (old->next!=NULL){
        old->next->previous=old->previous;
        old->previous->next=old->next;
    } else {
        old->previous->next=NULL;
        list->last=old->previous;
    }
    void *data=old->data;
    free(old);
    return data;
}

void __freedata(size_t index, node *node) {
    free(node->data);
}

void lkfor(lklist *list, void fn(size_t*,node*)) {
    node *actual=(*list).first;
    size_t size=0;
    while (actual!=NULL) {
        node *next=actual->next;
        fn(&size,actual);
        actual=next;
        size++;
    }
}

void lkfree(lklist *list, BOOL data) {
    if (data) lkfor(list,__freedata);
    while (!lkisempty(list))
    {
        lkquit(list);
    }
}
