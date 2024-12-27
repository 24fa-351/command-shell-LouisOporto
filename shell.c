#include "shell.h"

void prompt() {
    printf("mysh> ");
}

void read_input(char *input) {
    fgets(input, MAX_INPUT, stdin);
    input[strcspn(input, "\n")] = 0; // Remove newline character
}

void parse_input(char *input, char **args) {
    for (int ix = 0; ix < MAX_ARGS; ix++) {
        args[ix] = strsep(&input, " "); // Split input by space
        if (args[ix] == NULL) {
            break; // No more arguments
        }
    }
}

void change_directory(char *path) {
    if (path == NULL) {
        fprintf(stderr, "cd: missing argument\n");
    } else if (chdir(path) != 0) {
        perror("Failed to cd to path!");
    }
}

void set_env_variable(char *var, char *value) {
    if (setenv(var, value, 1) != 0) {
        perror("Failed to set variable!");
    }
}

void unset_env_variable(char *var) {
    if (unsetenv(var) != 0) {
        perror("Failed to unset variable!");
    }
}

void handle_echo(char **args) {
    for (int ix = 1; args[ix] != NULL; ix++) {
        if (args[ix][0] == '$') {
            char *env_var = getenv(args[ix] + 1); // Skip the '$' character
            if (env_var != NULL) {
                printf("%s ", env_var);
            }
            else {
                printf(" ");
            }
        }
        else {
            printf("%s ", args[ix]);
        }
    }
    printf("\n");
}

void execute_command(char **args) {
    if (strncmp(args[0], "echo", 4) == 0) {
        handle_echo(args);
        return;
    }

    int background = 0;
    int input_redirect = -1;
    int output_redirect = -1;
    char *input_file = NULL;
    char *output_file = NULL;
    int pipe_fd[2];
    int piping_is_true = 0;
    char *cmd_1[MAX_ARGS];
    char *cmd_2[MAX_ARGS];

    for (int ix = 0; args[ix] != NULL; ix++) {
        if (strcmp(args[ix], "&") == 0) {
            background = 1;
            args[ix] = NULL;
        } 
        else if (strcmp(args[ix], ">") == 0) {
            args[ix] = NULL;
            output_file = args[ix + 1];
            output_redirect = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        } 
        else if (strcmp(args[ix], "<") == 0) {
            args[ix] = NULL;
            input_file = args[ix + 1];
            input_redirect = open(input_file, O_RDONLY);
        }
        else if (strcmp(args[ix], "|") == 0) {
            piping_is_true = 1;
            for (int first_iter = 0; first_iter < ix; first_iter++) {
                cmd_1[first_iter] = args[first_iter];
            }
            cmd_1[ix] = NULL;
            for (int second_iter = ix + 1; args[second_iter] != NULL; second_iter++) {
                cmd_2[second_iter - ix - 1] = args[second_iter];
            }
            cmd_2[ix] = NULL;
            break;
        }
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("Failed to fork");
        return;
    }

    if (pid == 0) { // Child process
        if (input_redirect != -1) {
            dup2(input_redirect, STDIN_FILENO);
            close(input_redirect);
        }    
        if (output_redirect != -1) {
            dup2(output_redirect, STDOUT_FILENO);
            close(output_redirect);
        }
        if (piping_is_true) {
            pipe(pipe_fd);
            pid_t pid2 = fork();
            if (pid2 == 0) {
                close(pipe_fd[0]);
                dup2(pipe_fd[1], STDOUT_FILENO);
                execvp(cmd_1[0], cmd_1);
                perror("Failed to pipe exec child");
                exit(1);
            } 
            else {
                close(pipe_fd[1]);
                dup2(pipe_fd[0], STDIN_FILENO);
                execvp(cmd_2[0], cmd_2);
                perror("Failed to pipe exec parent");
                exit(1);
            }
        }
        execvp(args[0], args);
        perror("Failed to call exec");
        exit(1);
    }
    else {
        if (!background) {
            wait(NULL);
        }
        else {
            printf("Running in background with PID: %d\n", pid);
        }
    }
}