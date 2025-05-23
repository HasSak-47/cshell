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

struct Command new_command(char* cmd){
    struct Command c = {
        .cmd = strdup(cmd),
        .args = NULL,
        .argc = 0,

        .in = NULL,
        .out = NULL,
        .err = NULL,
    };
    // first arg is the name
    add_arg(&c, cmd);

    return c;
}

void bind_stdin(struct Command *cmd, struct Pipe *p){
    cmd->in = p;
}

void bind_stdout(struct Command *cmd, struct Pipe *p){
    cmd->out = p;
}

void bind_stderr(struct Command *cmd, struct Pipe *p){
    cmd->err = p;
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
        if (p->in != NULL) {
            int err = dup2(p->in->p[0], STDIN_FILENO);
            if (err != 0)
                temporal_suicide_msg("failed to set in");
        }

        if (p->out != NULL) {
            int err = dup2(p->out->p[1], STDOUT_FILENO);
            if (err != 0)
                temporal_suicide_msg("failed to set out");
        }
        // unset_raw_mode();
        // set_to_foreground();

        int r = execv(p->cmd, p->args);
        if (r < 0)
            temporal_suicide_msg("[child] didn't exec :)");
    }
    else if(pid < 0){
        temporal_suicide_msg("[???] didn't fork?");
    }
    free(p->cmd);
    for (size_t i = 0; i < p->argc; ++i) 
        free(p->args[i]);
    free(p->args);

    return pid;
}


#ifdef TEST

void test_process(){
    printf("testing process...\n");
    struct Command* cmds = malloc( sizeof(struct Command) * 2);
    cmds[0] = new_command("/bin/ls");
    cmds[1] = new_command("/bin/count");
    add_arg(&cmds[0], "-la");
    add_arg(&cmds[1], "3");

    struct Pipe p = new_pipe();

    bind_stdout(&cmds[0], &p);
    bind_stdin(&cmds[1], &p);

    // pid_t pid0 = run(&cmds[0]);
    pid_t pid1 = run(&cmds[1]);
    // waitpid(pid0, &error, 0);

    printf("writing to pipe\n");
    write(p.p[0], "1\n2\n3\n4", 8);
    waitpid(pid1, &error, 0);
    if(error != 0){
        printf("child error: %d\n", error);
    }
    free(cmds);
}

#endif
