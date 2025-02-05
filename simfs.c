#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

bool areEqual(char arr1[], char arr2[]){
	
	size_t len = strlen(arr1);
	if(len > 0 && arr1[len - 1] == '\n'){
		arr1[len-1] = '\0';
	}
	
	return strcmp(arr1, arr2) == 0;
}

bool isCommand(char entry[], char commands[][6]){

    // Check if the command matches
    int i = 0;
    bool check = false;
    while(i < 10){
        check = areEqual(entry, commands[i]);
        if(check == true){
            i = 10;
        }
        i ++;
    }

    return check;
}

int main(void){
	
	// Commands' array
	char arreglo[10][6] = {"touch","rm","mkdir","rmdir","ls","cd","pwd", "wrts", "help", "exit"};

	char *entry = NULL;
	size_t n = 0;

	getline(&entry, &n, stdin);
    
    bool check = isCommand(entry, arreglo);

    if(check == true){
        printf("%s", "El comando pertenece a la lista de comandos.");
    } else{
        printf("%s", "El comando no pertenece a la lista de comandos, escribe 'help' para ver los comandos.");
    }

    free(entry);
    
	//printf("%d", check);

	return 0;
}

