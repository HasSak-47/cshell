#ifndef __PROCESS_H_
#define __PROCESS_H_

#include <stdbool.h>
#include <unistd.h>

struct Pipe{
    int p[2];
};

struct Command{
    char* cmd;
    char** args;
    size_t argc;

    int* in;
    int* out;
    int* err;
};

struct Pipe new_pipe();
struct Pipe close_pipe();
struct Command new_command(char* path);

void bind_stdin(struct Command* cmd, struct Pipe* p);
void bind_stdout(struct Command* cmd, struct Pipe* p);
void bind_stderr(struct Command* cmd, struct Pipe* p);
void add_arg(struct Command* cmd, char* arg);

pid_t run(struct Command* p);

#ifdef TEST
void test_process();
#endif

#endif
