#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool compare(char arr1[], char arr2[]){
	
	size_t len = strlen(arr1);
	if(len > 0 && arr1[len - 1] == '\n'){
		arr1[len-1] = '\0';
	}
	
	return strcmp(arr1, arr2) == 0;
}

int main(void){
	
	// Commands' array
	char arreglo[10][6] = {"touch","rm","mkdir","rmdir","ls","cd","pwd", "wrts", "help", "exit"};
	printf("%s", arreglo[2]);
	
	char *entrada = NULL;
		
	size_t n = 6;
	
	printf("%s", "\n");

	getline(&entrada, &n, stdin);

	bool check = false;

	printf("%s", entrada);

	check = compare(entrada, arreglo[2]);

	printf("%d", check);

	return 0;
}

