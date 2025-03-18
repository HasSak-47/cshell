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

struct Command new_command(char* cmd){
    struct Command c = {
        .cmd = strdup(cmd),
        .args = NULL,
        .argc = 0,

        .pipe = {NULL, NoneBind},
    };
    // first arg is the name
    add_arg(&c, cmd);

    return c;
}

void bind_pipe(struct Command *cmd, struct Pipe *pipe, enum BindType type){
    struct PipeBind binding = { pipe, type };

    cmd->pipe = binding;
}


void add_arg(struct Command *cmd, char *arg){
    char** aux = realloc(cmd->args, (cmd->argc + 1) * sizeof(char*));
    if (aux == NULL) 
        temporal_suicide();
    cmd->args = aux;
    if(arg != NULL){
        cmd->args[cmd->argc++] = strdup(arg);
    }
    else{
        cmd->args[cmd->argc++] = NULL;
    }
    
}

pid_t run(struct Command* p){
    // all commands must end with a trailing NULL
    add_arg(p, NULL);

    pid_t pid = fork();
    if(pid == 0){ // child
        printf("[child]: %s\n", p->cmd);
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

        int r = execv(p->cmd, p->args);
        if (r < 0)
            temporal_suicide_msg("[child] didn't exec :)");
    }
    else if(pid < 0){
        temporal_suicide_msg("[parent]: didn't fork?");
    }

    for (char** arg = p->args; *arg != NULL; ++arg) {
        free(*arg);
    }

    free(p->args);
    free(p->cmd);
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
