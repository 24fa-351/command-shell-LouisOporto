# Command Shell
Create a command-line shell program like bash or Powershell. Have a prompt example below that reads inputs, writes to output. until "quit" or "exit"
Implement "cd" and "pwd". (directly). cd must take a full or relative path.
Implement the setting, deleting (unsetting) and getting of xsh environemnt vars: (this means that you should scan the command for $<something> and replace it if found)

Read PATH env

# Shell Environemnt
<cmd> <param1> <param2> <param3> ... <"<", ">", "|", "&"> 


# CMDS to handle
* pwd (see what current directory youre in)
* cd / chdir (change diretory)
* < (take input)
* > (send output to file)
* | (connect input of proj to next)
* Run programs