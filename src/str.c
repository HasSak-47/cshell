#include "utils.h"
#include <stdlib.h>
#include <str.h>
#include <string.h>

static void read_escape(
    struct VectorChars* cs, const char* chrs, size_t i, const size_t len) {
    switch (chrs[i]) {
    // special cases
    case 'x': // hex
        return;
    case 'u': // unicode
        return;
    case '0': // octal
        return;
    default: {
        struct Character c = {.ty = ESCAPED_CHARACTER, .data = chrs[i]};
        vector_push((*cs), c);
    }
    }
}

struct VectorChars read_nstring(const char* chrs, const size_t len) {
    struct VectorChars cs = {};

    for (size_t i = 0; i < len; ++i) {
        if (chrs[i] == '\\') {
            ++i;
            read_escape(&cs, chrs, i, len);
            continue;
        }
        struct Character c = {.ty = NORMAL_CHARACTER, .data = chrs[i]};
        vector_push(cs, c);
    }

    return cs;
}

char to_char(const struct Character c) {
    switch (c.ty) {
    case NORMAL_CHARACTER:
        return c.data;
    case ESCAPED_CHARACTER:
        switch (c.data) {
        case 'e':
            return '\e';
        default:
            return c.data;
        }
    default:
        return c.data;
        break;
    }
}

char* to_cstring(const struct VectorChars chars) {
    char* s = malloc((chars.len + 1) * sizeof(char));
    for (size_t i = 0; i < chars.len; ++i) {
        s[i] = to_char(chars.data[i]);
    }
    s[chars.len] = 0;

    return s;
}

struct VectorChars substring(
    struct VectorChars v, const size_t beg, const size_t end) {
    struct VectorChars name = {};
    for (size_t i = beg; i < end; ++i) {
        vector_push(name, v.data[i]);
    }

    return name;
}

bool string_cmp(struct VectorChars c, const char* str) {
    size_t len = strlen(str);
    size_t i   = 0;
    while (i < len && i < c.len) {
        if (str[i] != to_char(c.data[i]))
            return false;
        ++i;
    }

    return len == c.len;
}
