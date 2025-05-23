#ifndef __UTILS__
#define __UTILS__


#define temporal_suicide() __temporal_suicide(__LINE__, __FILE__, NULL, -1) 
#define temporal_suicide_msg(msg) __temporal_suicide(__LINE__, __FILE__, msg, -1) 
#define temporal_suicide_code(msg, code) __temporal_suicide(__LINE__, __FILE__, mgs, code)
void __temporal_suicide(int line, char* file, char* mgs, int code);

void set_to_foreground();


#endif
