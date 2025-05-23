#ifndef __PATH__
#define __PATH__

#include "str.h"

enum SegmentType{
    ROOT_PATH,
    PREV_PATH,
    CURR_PATH,
    NAMED_PATH,
};

struct PathSegment{
    struct VectorChars name;
    enum SegmentType ty;
};

#include "utils.h"

DefineVector(Path, struct PathSegment);

struct Path{
    // NOTE: PRIVATE/PROTECTED!!
    struct VectorPath _inner;
};

char* get_path_string(const struct Path path);
void push_name(struct Path* path, const char* name);
void push_segment(struct Path* path, const struct PathSegment segment);
struct Path root_path();
struct Path parse_path(const char* path);

void destruct_path(struct Path* path);

#endif // !__PATH__
