#include "shell.h"

void prompt() {
    printf("mysh> ");
}

void read_input(char *input) {
    fgets(input, MAX_INPUT, stdin);
    input[strcspn(input, "\n")] = 0; // Remove newline character
}

void parse_input(char *input, char **args) {
    for (int i = 0; i < MAX_ARGS; i++) {
        args[i] = strsep(&input, " "); // Split input by space
        if (args[i] == NULL) {
            break; // No more arguments
        }
    }
}

void change_directory(char *path) {
    if (path == NULL) {
        fprintf(stderr, "cd: missing argument\n");
    } else if (chdir(path) != 0) {
        perror("cd");
    }
}

void set_env_variable(char *var, char *value) {
    if (setenv(var, value, 1) != 0) {
        perror("set");
    }
}

void unset_env_variable(char *var) {
    if (unsetenv(var) != 0) {
        perror("unset");
    }
}

void handle_echo(char **args) {
    for (int i = 1; args[i] != NULL; i++) {
        if (args[i][0] == '$') {
            char *env_var = getenv(args[i] + 1); // Skip the '$' character
            if (env_var != NULL) {
                printf("%s ", env_var);
            }
            else {
                printf(" ");
            }
        }
        else {
            printf("%s ", args[i]);
        }
    }
    printf("\n");
}

void execute_command(char **args) {
    if (strcmp(args[0], "echo") == 0) {
        handle_echo(args);
        return;
    }


    int background = 0;
    int input_redirect = -1;
    int output_redirect = -1;
    char *input_file = NULL;
    char *output_file = NULL;
    int pipe_fd[2];
    int piping = 0;
    char *cmd1[MAX_ARGS];
    char *cmd2[MAX_ARGS];

    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "&") == 0) {
            background = 1;
            args[i] = NULL;
        } 
        else if (strcmp(args[i], ">") == 0) {
            args[i] = NULL;
            output_file = args[i + 1];
            output_redirect = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        } 
        else if (strcmp(args[i], "<") == 0) {
            args[i] = NULL;
            input_file = args[i + 1];
            input_redirect = open(input_file, O_RDONLY);
        }
        else if (strcmp(args[i], "|") == 0) {
            piping = 1;
            args[i] = NULL;
            for (int j = 0; j < i; j++) {
                cmd1[j] = args[j];
            }
            cmd1[i] = NULL;
            for (int j = i + 1; args[j] != NULL; j++) {
                cmd2[j - i - 1] = args[j];
            }
            cmd2[i] = NULL;
            break;
        }
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
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
        if (piping) {
            pipe(pipe_fd);
            pid_t pid2 = fork();
            if (pipe_fd == 0) {
                close(pipe_fd[0]);
                dup2(pipe_fd[1], STDOUT_FILENO);
                execvp(cmd1[0], cmd1);
                perror("pipe exec");
                exit(EXIT_FAILURE);
            } 
            else {
                close(pipe_fd[1]);
                dup2(pipe_fd[0], STDIN_FILENO);
                execvp(cmd2[0], cmd2);
                perror("pipe exec");
                exit(EXIT_FAILURE);
            }
        }
        execvp(args[0], args);
        perror("exec");
        exit(EXIT_FAILURE);
    }
    else {
        if (!background) {
            int status;
            wait(NULL);
        }
        else {
            printf("Running in background with PID: %d\n", pid);
        }
    }
}