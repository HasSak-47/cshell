#ifndef __UTILS__
#define __UTILS__

#define unrecoverable_error(msg)\
    __error_msg(__LINE__, __FILE__, "[ERROR @ %s : %d]: %s\n", msg, -1)

#define temporal_suicide_crash(msg, code)\
    __error_msg(__LINE__, __FILE__, "TODO: handle err at %s in line %d %s\n", msg, code)

#define temporal_suicide_msg(msg)\
    temporal_suicide_crash(msg, -1) 

#define temporal_suicide()\
    temporal_suicide_crash("[?]", -1) 


void __error_msg(int line, char* file, char* fmt, char* msg, int code);

void set_to_foreground();


#endif
