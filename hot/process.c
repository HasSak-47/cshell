// #include <errno.h>
#include <string.h>
#include <termios.h>
#include <testing.h>
#include <state.h>
#include <utils.h>
#include <process.h>

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
        temporal_suicide_msg("failed to create pipes");

    return p;
}

void close_pipe(struct Pipe* p){
    close(p->p[0]);
    close(p->p[1]);
}

struct Command new_command(const char* cmd){
    struct Command c = {
        .cmd = strdup(cmd),
        .args = NULL,
        .argc = 0,
        .argcap = 0,
        .foreground = false,

        .pipe = {NULL, NoneBind},
    };
    if (debug) {
        printf("creating cmd for path: %s\n", cmd);
    }
    // first arg is the name
    add_arg(&c, cmd);

    return c;
}

void command_reserve_size(struct Command *cmd, size_t argc){
    if (debug) {
        printf("reserving size for cmd of: %lu\n", argc);
    }
    cmd->args = malloc((argc + 1) * sizeof(char*));
    cmd->argc = argc;
}

void add_arg(struct Command *cmd, const char *arg){
    if (debug) {
        printf("adding arg: %s\n", arg);
    }
    // resize if necessary
    if (cmd->argc + 1 >= cmd->argcap) {
        size_t new_argcap = 2 * (cmd->argc + 1) * sizeof(char*);
        char** aux = realloc(cmd->args, new_argcap);
        if (aux == NULL) 
            temporal_suicide();
        cmd->args = aux;
        cmd->argcap = new_argcap;
    }
    if(arg != NULL){
        cmd->args[cmd->argc++] = strdup(arg);
    }
    else{
        cmd->args[cmd->argc++] = NULL;
    }
}

void bind_pipe(struct Command *cmd, struct Pipe *pipe, enum BindType type){
    struct PipeBind binding = { pipe, type };

    cmd->pipe = binding;
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
        printf("[parent]: running command: %s", p->cmd);
        for (size_t i = 0; i < p->argc; ++i) {
            printf(" %s", p->args[i]);
        }
        printf("\n");
    }

    pid_t pid = fork();
    if(pid == 0){ // child
        if (debug) {
            printf("[child]: name: %s\n", p->cmd);
        }
        if (p->foreground) {
            if (debug) {
                printf("[child]: setting to foreground\n");
            }
            unset_raw_mode();
            set_to_foreground();
            if (debug) {
                printf("[child]: child is foreground\n");
            }
        }
        if (debug) {
            printf("[child]: setting pipes\n");
        }
        if (p->pipe.pipe != NULL) {
            if (p->pipe.ty & ReadBind)
                dup2(p->pipe.pipe->p[0], STDIN_FILENO);
            else
                close(p->pipe.pipe->p[0]);

            if (p->pipe.ty & WriteBind)
                dup2(p->pipe.pipe->p[1], STDOUT_FILENO);
            else
                close(p->pipe.pipe->p[1]);
        }

        if (debug) {
            printf("[child]: executing cmd...\n");
        }
        int r = execv(p->cmd, p->args);
        if (r < 0)
            temporal_suicide_msg("[child] didn't exec :)");
    }
    else if(pid < 0){
        temporal_suicide_msg("[parent]: didn't fork?");
    }

    // parent
    if (debug) {
        printf("[parent]: cleaning command data\n");
    }
    for (char** arg = p->args; *arg != NULL; ++arg) {
        free(*arg);
    }

    free(p->args);
    free(p->cmd);
    if (debug) {
        printf("[parent]: returning pid\n");
    }
    p->cmd = NULL;
    p->args = NULL;
    return pid;
}

#ifdef TEST

void test_process(){
    printf("testing process...\n");
    struct Command* cmds = malloc( sizeof(struct Command) * 2);

    cmds[0] = new_command("/bin/ls");
    add_arg(&cmds[0], "-lA");
    add_arg(&cmds[0], "--color");

    cmds[1] = new_command("/bin/less");

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
