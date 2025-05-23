#include "str.h"
#include "vectors.h"
#include <stdio.h>
#include <utils.h>
#include <path.h>
#include <stdlib.h>
#include <string.h>

static struct PathSegment build_segment(const struct VectorChars name){
    // not optimal but meaningful!
    if (string_cmp(name, ".")) {
        return (struct PathSegment){
            .ty = CURR_PATH,
            .name = NULL
        };
    }
    if (string_cmp(name, "..")) {
        return (struct PathSegment){
            .ty = PREV_PATH,
            .name = NULL
        };
    }

    return (struct PathSegment){
        .ty = NAMED_PATH,
        .name = name,
    };
}

void push_name(struct Path* path, const char* name){
    size_t len = strlen(name);
    struct VectorChars nname = read_nstring(name, len);
    struct PathSegment segment = build_segment(nname);

    vector_push(path->_inner, segment);
}

void push_segment(struct Path* path, const struct PathSegment segment){
    vector_push(path->_inner, segment);
}

struct Path root_path(){
    struct PathSegment segment = {
        .ty = ROOT_PATH,
        .name = NULL
    };

    struct Path path = {};
    push_segment(&path, segment);

    return path;
}

static struct PathSegment make_segment(struct Path path, struct VectorChars cs, size_t beg, size_t end){
    struct VectorChars name = substring(cs, beg, end);

    if (string_cmp(name, ".")) {
        free(name.data);
        return (struct PathSegment){
            .name = {},
            .ty = CURR_PATH,
        };
    }
    else if (string_cmp(name, "..")) {
        free(name.data);
        return (struct PathSegment){
            .name = {},
            .ty = PREV_PATH,
        };
    }
    return (struct PathSegment){
        .name = name,
        .ty = NAMED_PATH,
    };
}

struct Path parse_path(const char *path){
    struct Path _path = {};
    size_t start = 0; 
    size_t i = 0;
    if (path[0] == '/') {
        _path = root_path();
        start = 1;
        i = 1;
    }

    struct VectorChars cs = read_nstring(path, strlen(path));

    for (; i < cs.len; ++i) {
        if(cs.data[i].ty == NORMAL_CHARACTER && cs.data[i].data == '/'){
            push_segment(&_path, make_segment(_path, cs, start, i));

            start = i + 1;
        }
    }

    push_segment(&_path, make_segment(_path, cs, start, cs.len));

    free(cs.data);
    return _path;
}

char* get_path_string(const struct Path path){
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
                    char* start = to_cstring( path._inner.data[i].name );
                    for (char* inn = start; *inn != 0; ++inn)
                        vector_push(str, *inn);
                    
                    free(start);
                }
                break;
        } 
        vector_push(str, '/');
    }
    vector_push(str, '\0');
    size_t len = str.len;
    char* d = realloc(str.data, len);

    return d;
}

void destruct_path(struct Path* path){
    for (size_t i = 0; i < path->_inner.len; ++i) {
        free(path->_inner.data[i].name.data);
    }
    free(path->_inner.data);
    path->_inner.data = NULL;
}
