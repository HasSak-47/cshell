#include <utils.h>
#include <state.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void __error_msg(int line, char* file, char* fmt, char* msg, int code){
    printf(fmt, file, line, msg);
    exit(code);
}

void set_to_foreground(){
    if(debug) {
        printf("setting to foreground\n");
    }
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
