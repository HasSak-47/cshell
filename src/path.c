#include <path.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utils.h>
#include "debug.h"
#include "str.h"
#include "vectors.h"

static struct PathSegment build_segment(const struct VectorChars name) {
    // not optimal but meaningful!
    if (string_cmp(name, ".")) {
        return (struct PathSegment){.ty = CURR_PATH, .name = {}};
    }
    if (string_cmp(name, "..")) {
        return (struct PathSegment){.ty = PREV_PATH, .name = {}};
    }

    return (struct PathSegment){
        .ty   = NAMED_PATH,
        .name = name,
    };
}

void push_name(struct Path* path, const char* name) {
    size_t len                 = strlen(name);
    struct VectorChars nname   = read_nstring(name, len);
    struct PathSegment segment = build_segment(nname);

    vector_push(path->_inner, segment);
}

void push_segment(struct Path* path, const struct PathSegment segment) {
    vector_push(path->_inner, segment);
}

struct Path root_path() {
    struct PathSegment segment = {.ty = ROOT_PATH, .name = {}};

    struct Path path = {};
    push_segment(&path, segment);

    return path;
}

static struct PathSegment make_segment(
    struct Path path, struct VectorChars cs, size_t beg, size_t end) {
    struct VectorChars name = substring(cs, beg, end);

    return build_segment(name);
}

struct Path parse_path(const char* path) {
    struct Path _path = {};

    size_t start = 0;
    size_t i     = 0;

    if (path[0] == '/') {
        _path = root_path();
        start = 1;
        i     = 1;
    }

    struct VectorChars cs = read_nstring(path, strlen(path));

    for (; i < cs.len; ++i) {
        if (cs.data[i].ty == NORMAL_CHARACTER && cs.data[i].data == '/') {
            push_segment(&_path, make_segment(_path, cs, start, i));

            start = i + 1;
        }
    }

    if (start < cs.len) {
        push_segment(&_path, make_segment(_path, cs, start, cs.len));
    }

    free(cs.data);
    return _path;
}

void expand_path(struct Path* self, const struct Path* const cwd) {
    if (self->_inner.data[0].ty == CURR_PATH) {
        struct Path cpy = {};

        for (size_t i = 0; i < cwd->_inner.len; ++i) {
            struct PathSegment segment = {};
            segment.ty                 = cwd->_inner.data[i].ty;
            vector_clone(segment.name, cwd->_inner.data[i].name);

            push_segment(&cpy, segment);
        }

        for (size_t i = 1; i < self->_inner.len; ++i) {
            struct PathSegment segment = {};
            segment.ty                 = self->_inner.data[i].ty;
            vector_clone(segment.name, self->_inner.data[i].name);

            push_segment(&cpy, segment);
        }

        destruct_path(self);

        *self = cpy;
    }
    return;
}

char* get_path_string(const struct Path path) {
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
        case NAMED_PATH: {
            char* start = to_cstring(path._inner.data[i].name);
            for (char* inn = start; *inn != 0; ++inn) vector_push(str, *inn);

            free(start);
        } break;
        }
        vector_push(str, '/');
    }
    vector_pop(str);
    vector_push(str, '\0');
    size_t len = str.len;
    char* d    = realloc(str.data, len);

    return d;
}

void pop_segment(struct Path* path) {
    if (path->_inner.len < 1) {
        return;
    }

    struct PathSegment end = path->_inner.data[path->_inner.len - 1];
    free(end.name.data);

    vector_pop(path->_inner);
}

void destruct_path(struct Path* path) {
    for (size_t i = 0; i < path->_inner.len; ++i) {
        struct PathSegment seg = path->_inner.data[i];
        free(seg.name.data);
    }
    free(path->_inner.data);
    path->_inner.data = NULL;
    path->_inner.len  = 0;
}
