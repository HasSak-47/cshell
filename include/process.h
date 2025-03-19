#ifndef __PROCESS_H_
#define __PROCESS_H_

#include <stdbool.h>
#include <unistd.h>

struct Pipe{
    int p[2];
};

enum BindType{
    NoneBind = 0,
    ReadBind  = 1,
    WriteBind = 2,
    ErrorBind = 4,
};

struct PipeBind{
    struct Pipe* pipe;
    enum BindType ty;
};

struct Command{
    char* cmd;
    char** args;
    size_t argc;
    size_t argcap;
    bool foreground;

    struct PipeBind pipe;
};

struct Pipe new_pipe();
void close_pipe(struct Pipe* p);
struct Command new_command(const char* path);

void command_reserve_size(struct Command* cmd, size_t size);
void bind_pipe(struct Command* cmd, struct Pipe* pipe, enum BindType ty);
void add_arg(struct Command* cmd, const char* arg);

pid_t run(struct Command* p);

#ifdef TEST
void test_process();
#endif

#endif
