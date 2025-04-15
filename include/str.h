#ifndef __STR_H__
#define __STR_H__

#include "./vectors.h"
#include <stdbool.h>

enum CharacterType{
    ESCAPED_CHARACTER,
    ESCAPED_UNI,
    ESCAPED_HEX,
    ESCAPED_OCT,
    NORMAL_CHARACTER,
};

struct Character{
    enum CharacterType ty;
    int data;
};

char to_char(const struct Character c);

DefineVector(Chars, struct Character);

struct VectorChars read_nstring(const char* chrs, const size_t len);
struct VectorChars read_nstring_deter(const char* chrs, char deter, const size_t len);
char* to_cstring(const struct VectorChars chars);
bool string_cmp(struct VectorChars c, const char* str);

struct VectorChars substring(struct VectorChars v, const size_t beg, const size_t end);


#endif // !__STR_H__
