#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "node.h"

int longestname = 0;

int main(int argc, char *argv[]){

    nodeStruct* head;
    nodeStruct* pathpointer;

    if(argc > 1){
        // Case when more than one argument is given
        head = createNode();
        if (!head) {
            printf("Error: Memory Error\n");
            return 1;
        }

        initNode(head, NULL, 'D');
        // Set pathpointer
        pathpointer = head;

        if(!generateSysFile(argv[1], head, &pathpointer)){
            return 1;
        }

    } else {
        // Case when only one argument is given
        // Set initial value for head
        head = createNode();
        if (!head) {
            printf("Error: Memory Error\n");
            return 1;
        }

        initNode(head, NULL, 'D');
        // Set pathpointer
        pathpointer = head;
    }
    
	// Commands' array
    cmdStruct commands[10] = {
        {"touch", "Creates a file without any content.\n"},
        {"rm", "Erases file with the target name.\n"},
        {"mkdir", "Creates a directory with the given name.\n"},
        {"rmdir", "Erases target directory.\n"},
        {"ls", "Prints all files and directories in target directory.\n"},
        {"cd", "Changes from current directory to target.\n"},
        {"pwd", "Prints absolute path of current location.\n"},
        {"wrts", "Creates a file with the description of the current file system.\n"},
        {"help", "Displays commands available in the shell with their description.\n"},
        {"exit", "Finishes execution of the program.\n"}
    };

    bool cmdON = true;

	while(cmdON){
        char *entry = NULL;
        size_t n = 0;

        // Get new entry
        pwdFunction(pathpointer, 0);
        printf("\x1b[35m%s\x1b[0m", "> ");
        printf("\x1b[37m");
        getline(&entry, &n, stdin);

        // Put the terminate char to the entry
        size_t len = strlen(entry);
        if (len > 0 && entry[len - 1] == '\n') {
            entry[len - 1] = '\0';
        }

        char *command;
        char *arguments;
        inputParser(entry, &command, &arguments);

        free(entry);
        
        // Used to remove empty spaces of the argument
        strip(arguments);
        
        // Map command to integer
        int cmd = map_to_int(command, arguments);

        // Determine what command the user is calling
        switch (cmd){

            //exit
            case 1: {
                cleanSysFile(head);
                cmdON = false;
                break;
            }

            //pwd
            case 2: {
                pwdFunction(pathpointer, 0);
                printf("\n");
                break;
            }

            //touch
            case 3: {
                touchFunction(arguments, pathpointer, head);
                break;
            }

            //rm
            case 4: {
                rmFunction(arguments, pathpointer, head);
                break;
            }

            //ls
            case 5: {
                lsFunction(arguments, pathpointer, head);
                break;
            }

            //mkdir
            case 6: {
                mkdirFunction(arguments, pathpointer, head);
                break;
            }

            //rmdir
            case 7: {
                rmdirFunction(arguments, pathpointer, head);
                break;
            }

            //cd
            case 8: {
                cdFunction(arguments, &pathpointer, head);
                break;
            }

            //help
            case 9: {
                helpFunction(commands);
                break;
            }

            //wrts
            case 10:{
                wrtsFunction(head, longestname);
                break;
            }

            //otherwise
            case 11:
                printf("\x1b[31mError:\x1b[37m Not valid command, write help to see the list of commands available!\n");
                break;
        }
        free(arguments);
        free(command);

    }

	return 0;
}