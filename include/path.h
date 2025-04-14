#ifndef __PATH__
#define __PATH__

#include <string.h>

struct NamedPathSegment {
    const char* name;
};

enum SegmentType{
    PREV,
    CURR,
    NAMED,
};


struct PathSegment{
    struct NamedPathSegment segment;
    enum SegmentType ty;
};

#include "utils.h"

DefineVector(Path, struct PathSegment);

struct Path{
    // NOTE: PRIVATE/PROTECTED!!
    struct VectorPath _inner;
};

char* get_path_string(struct Path path){
    for (size_t i = 0; i < path._inner.len; ++i) {
    
    }
    return NULL;
}

#endif // !__PATH__
