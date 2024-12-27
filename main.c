#include <stdio.h>
#include "shell.h"

int main(int argc, char *argv[]) {
    fprintf(stderr, "Starting shell\n");
    
    char input[MAX_INPUT];
    char *args[MAX_ARGS];

    while (1) {
        prompt();
        read_input(input);

        if (strlen(input) == 0) continue;

        parse_input(input, args);

        // Exit shell
        if (strcmp(args[0], "exit") == 0 || strcmp(args[0], "quit") == 0) {
            fprintf(stderr, "Exiting shell\n");
            break;
        }
        else if (strcmp(args[0], "cd") == 0) {
            change_directory(args[1]);
        }
        else if (strcmp(args[0], "set") == 0) {
            set_env_variable(args[1], args[2]);
        }
        else if (strcmp(args[0], "unset") == 0) {
            unset_env_variable(args[1]);
        }
        else {
            execute_command(args);
        }
    }
    
    return 0;
}