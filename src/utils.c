#include <utils.h>
#include <state.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void __error_msg(int line, char* file, char* fmt, char* msg, int code){
    printf(fmt, file, line, msg);
    exit(code);
}

void set_to_foreground(){
    if(debug) {
        printf("setting to foreground\n");
    }
    const int FD = STDIN_FILENO;
    if(tcgetpgrp(FD) < 0)
        temporal_suicide_msg("could not get fd");
    if(setpgid(0, 0) == -1)
        temporal_suicide_msg("could not set program id");
    pid_t group_id = getpgrp();
    // NOTE: I don't know why this is needed
    // it just works like that
    // sure why not ignore this random signal
    signal(SIGTTOU, SIG_IGN);
    int ok = tcsetpgrp(FD, group_id);
    if(ok == -1)
        temporal_suicide_msg("could not set group id");
}

void __vector_push(struct __Vector* v, const void* data, const size_t size){
    if (v->len >= v->cap) {
        size_t t_cap = (v->cap + 1) * 2;
        void* aux = realloc(v->data, size * t_cap);
        if (aux == NULL) 
            temporal_suicide_msg("could not resize vector");
        v->data = aux;
        v->cap = t_cap;
    }
    size_t start = v->len * size;
    for (size_t i = 0; i < size; ++i)
        ((uint8_t*)v->data)[i + start] = ((uint8_t*)data)[i];
    v->len++;
}

void __vector_pop (struct __Vector* v, const size_t size){
    // probably should clear data but meh
    v->len--;
}

void __vector_reserve(struct __Vector* v, const size_t cap, const size_t size){
    void* aux = realloc(v->data, size * cap);
    if (aux == NULL) 
        temporal_suicide_crash("could not resize vector", 0);
    v->data = aux;
    v->cap = cap;
}

void __vector_clone(struct __Vector *dst, const struct __Vector *const src, const size_t size){
    dst->data = malloc(size * src->len);
    dst->len = src->len;
    dst->cap = src->len;

    for (size_t i = 0; i < size * src->len; ++i)
        ((char*)dst->data)[i] = ((char*)src->data)[i];
}

