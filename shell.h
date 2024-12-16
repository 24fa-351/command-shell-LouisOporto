#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

#ifndef SHELL_H
#define SHELL_H

#define MAX_INPUT 1024
#define MAX_ARGS 100

void execute_command(char **args);

void change_directory(char *path);

void set_env_variable(char *var, char *value);

void unset_env_variable(char *var);

void parse_input(char *input, char **args);

void read_input(char *input);

void prompt();

void handle_echo(char **args);


#endif