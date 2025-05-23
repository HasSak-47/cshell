#ifndef __PATH__
#define __PATH__

enum SegmentType{
    ROOT_PATH,
    PREV_PATH,
    CURR_PATH,
    NAMED_PATH,
};


struct PathSegment{
    char* name;
    enum SegmentType ty;
};

#include "utils.h"

DefineVector(Path, struct PathSegment);

struct Path{
    // NOTE: PRIVATE/PROTECTED!!
    struct VectorPath _inner;
};

char* get_path_string(struct Path path);
void push_segment(struct Path* path, const char* name);

#endif // !__PATH__
