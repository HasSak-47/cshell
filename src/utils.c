#include <utils.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void __temporal_suicide(int line, char* file, char* msg, int code){
    if (msg != NULL)
        printf("TODO: handle err at %s in line %d %s\n", file, line, msg);
    else 
        printf("TODO: handle err at %s in line %d\n", file, line);
    exit(code);
}


void set_to_foreground(){
    const int FD = STDIN_FILENO;
    if(tcgetpgrp(FD) < 0)
        temporal_suicide_msg("could not get fd");
    if(setpgid(0, 0) == -1)
        temporal_suicide_msg("could not set program id");
    pid_t group_id = getpgrp();
    // NOTE: I don't know why this is needed
    // it just works like that
    // sure why not ignore this random signal
    signal(SIGTTOU, SIG_IGN);
    int ok = tcsetpgrp(FD, group_id);
    if(ok == -1)
        temporal_suicide_msg("could not set group id");
}
