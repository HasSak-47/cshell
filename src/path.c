#include "vectors.h"
#include <utils.h>
#include <path.h>
#include <stdlib.h>
#include <string.h>

static struct PathSegment build_segment(const char* name){
    // not optimal but meaningful!
    if (strncmp(name, ".", 1) == 0) {
        return (struct PathSegment){
            .ty = CURR_PATH,
            .name = NULL
        };
    }
    if (strncmp(name, "..", 1) == 0) {
        return (struct PathSegment){
            .ty = PREV_PATH,
            .name = NULL
        };
    }

    size_t len = strlen(name);
    char* copy = malloc(len + 1);
    strcpy(copy, name);
    return (struct PathSegment){
        .ty = NAMED_PATH,
        .name = copy
    };
}

void push_segment(struct Path* path, const char* name){
    struct PathSegment segment = build_segment(name);
    vector_push(path->_inner, segment);
}

char* get_path_string(struct Path path){
    struct VectorString str = {};
    vector_reserve(str, path._inner.len * 5);
    for (size_t i = 0; i < path._inner.len; ++i) {
        switch (path._inner.data[i].ty) {
            case ROOT_PATH:
                break;
            case CURR_PATH:
                vector_push(str, '.');
                break;
            case PREV_PATH:
                vector_push(str, '.');
                vector_push(str, '.');
                break;
            case NAMED_PATH:{
                    char* inn = path._inner.data[i].name;
                    for (; inn != NULL; inn++)
                        vector_push(str, *inn);
                }
                break;
        } 
        vector_push(str, '/');
    }
    vector_push(str, '\0');

    return str.data;
}
