#include <testing.h>
#include <state.h>
#include <utils.h>
#include <process.h>
#include <debug.h>

#include <errno.h>
#include <string.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>

#include <stddef.h>
#include <sys/wait.h>

#include <fcntl.h>
#include <unistd.h>

/*
 * cmd | cmd
 * cmd > &fd/file
 * cmd >> &fd/file 
 *
 * cmd |& cmd === cmd 2>&1 | cmd
 */

struct Pipe new_pipe(){
    struct Pipe p = {};
    int r = pipe(p.p);
    if(r < 0)
        temporal_suicide_msg("failed to create new pipe");

    return p;
}

void close_pipe(struct Pipe* p){
    close(p->p[0]);
    close(p->p[1]);
}

/*
 * takes an string md and clones it
 */
struct Command new_command(const char* cmd){
    struct Command c = {
        .cmd = strdup(cmd),
        .args = { 0, 0, 0 },
        .foreground = false,

        .pipe = {NULL, NoneBind},
    };
    debug_printf("creating cmd for path: %p %s\n", cmd, cmd);
    // first arg is the name
    add_arg(&c, cmd);

    return c;
}

void bind_pipe(struct Command *cmd, struct Pipe *pipe, enum BindType type){
    struct PipeBind binding = { pipe, type };
    debug_printf("binding %p(%d %d) pipe for cmd %p with bind %d\n", pipe, pipe->p[0], pipe->p[1], cmd, (int)type);

    cmd->pipe = binding;
}


void command_reserve_size(struct Command *cmd, size_t argc){
    vector_reserve(cmd->args, argc);
}

/*
 * takes an string and and clones it
 */
void add_arg(struct Command *cmd, const char *arg){
    debug_printf("add_arg: %p %s\n", arg, arg);
    char* clone_arg = arg != NULL ? strdup(arg) : NULL;
    vector_push(cmd->args, clone_arg);
}

/*
 * Runs a given command
 *
 * It frees all the command info at exit
 * returns the pid of the child, it does not wait for it to stop
 */
pid_t run(struct Command* p){
    // all commands must end with a trailing NULL
    add_arg(p, NULL);
    if (debug) {
        debug_printf("[parent]: running command: %s", p->cmd);
        for (size_t i = 0; i < p->args.len; ++i) {
            debug_printf(" %s", p->args.data[i]);
        }
        debug_printf("\n");
    }

    pid_t pid = fork();
    if(pid == 0){ // child
        debug_printf("[child]: name: %s\n", p->cmd);
        if (p->foreground) {
            debug_printf("[child]: setting to foreground\n");
            unset_raw_mode();
            set_to_foreground();
            debug_printf("[child]: child is foreground\n");
        }
        debug_printf("[child]: setting pipes\n");
        if (p->pipe.pipe != NULL) {
            if (p->pipe.ty & ReadBind){
                int error = dup2(p->pipe.pipe->p[0], STDIN_FILENO);
                if (error == -1) {
                    printf("errno: %d\n", errno);
                    temporal_suicide_msg("[child]: could not bind in");
                }
            }
            else{
                close(p->pipe.pipe->p[0]);
            }

            if (p->pipe.ty & WriteBind){
                int error = dup2(p->pipe.pipe->p[1], STDOUT_FILENO);
                if (error == -1) {
                    temporal_suicide_msg("[child]: could not bind out");
                }
            }
            else{
                close(p->pipe.pipe->p[1]);
            }

            if (p->pipe.ty & ErrorBind){
                int error = dup2(p->pipe.pipe->p[1], STDERR_FILENO);
                if (error == -1) {
                    temporal_suicide_msg("[child]: could not bind error");
                }
            }
            else{
                close(p->pipe.pipe->p[1]);
            }
        }

        debug_printf("[child]: executing cmd %s...\n", p->cmd);
        int r = execv(p->cmd, p->args.data);
        if (r < 0)
            temporal_suicide_msg("[child] didn't exec :)");
    }
    else if(pid < 0){
        temporal_suicide_msg("[parent]: didn't fork?");
    }

    // parent
    debug_printf("[parent]: cleaning command data\n");
    for (char** arg = p->args.data; *arg != NULL; ++arg) {
        debug_printf("[parent]: removing arg: %p %s\n", *arg, *arg);
        free(*arg);
    }

    free(p->args.data);
    free(p->cmd);
    debug_printf("[parent]: returning pid\n");
    p->cmd = NULL;
    p->args.data = NULL;
    return pid;
}

#ifdef TEST

void test_process(){
    printf("testing process...\n");
    struct Command* cmds = malloc( sizeof(struct Command) * 2);

    cmds[0] = new_command("/bin/ls");
    add_arg(&cmds[0], "-lA");
    add_arg(&cmds[0], "--color");

    cmds[1] = new_command("/bin/wc");
    add_arg(&cmds[1], "-l");

    struct Pipe p = new_pipe();

    bind_pipe(&cmds[0], &p, WriteBind);
    bind_pipe(&cmds[1], &p, ReadBind);


    pid_t pid0 = run(&cmds[0]);
    pid_t pid1 = run(&cmds[1]);

    close(p.p[0]);
    close(p.p[1]);

    waitpid(pid0, &error, 0);
    waitpid(pid1, &error, 0);

    free(cmds);
}

#endif
