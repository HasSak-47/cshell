#include <stdbool.h>
#include <string.h>

// Luall.vars.config
char* config_path = NULL;
char* init_path = NULL;
char* hot_path = NULL;
bool running = true;

// Luall.vars
struct User{
    char* name;
    char* home;
};
char* cwd = NULL;
char* host = NULL;
int error = 0;
bool debug = true;

// TODO:
// Luall.vars.env
