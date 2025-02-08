#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void strip(char *str) {

    if(strcmp(str, "") != 0){
        int inicio = 0, fin = strlen(str) - 1;

        // Mover el inicio hasta el primer carácter no vacío
        while (str[inicio] == ' ') {
            inicio++;
        }

        // Mover el fin hacia atrás hasta encontrar un carácter no vacío
        while (fin > inicio && str[fin] == ' ') {
            fin--;
        }

        // Desplazar la cadena para eliminar los espacios al inicio
        int i, j = 0;
        for (i = inicio; i <= fin; i++) {
            str[j++] = str[i];
        }
        str[j] = '\0'; // Agregar el terminador de cadena

        //str = realloc(str, j * sizeof(char));     Pending TASK!!!
    }
    
}

/**
 * @brief This function is used to determine if two strings are equal.
 * 
 * @param arr1 First String.
 * @param arr2 Second String.
 * @return bool True if they are equal, False if they are not.
 */
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

typedef struct nodeStruct nodeStruct;
struct nodeStruct{

    nodeStruct *parent;
    nodeStruct *child;
    nodeStruct *sibling; 
    char name[100];
    char type;

};

void cleanSysFile( nodeStruct *pointer ){
    if(pointer != NULL){
        cleanSysFile(pointer->child);
        cleanSysFile(pointer->sibling);
        printf("%s\n", pointer->name);
        free(pointer);
    }
}

void pwdFunction( nodeStruct *pointer ){
    if(pointer != NULL){
        pwdFunction(pointer -> parent);

        strcmp(pointer -> name, "/") == 0 ? printf("/") :  printf("%s/", pointer -> name);
    }
}

bool searchFile( nodeStruct **pointer, char * dirName, char fileType){
    
    bool dirmatch = false;

    while( *pointer != NULL && !dirmatch ){
        if(strcmp(dirName, (*pointer) -> name) == 0 && (*pointer) -> type == fileType ){
            dirmatch = true;
        } else {
            *pointer = (*pointer) -> sibling;
        }
    }

    return dirmatch;
}

bool searchFileForDel( nodeStruct **pointer, nodeStruct **backpointer, char * dirName, char fileType){
    
    bool dirmatch = false;

    while( *pointer != NULL && !dirmatch ){
        if(strcmp(dirName, (*pointer) -> name) == 0 && (*pointer) -> type == fileType ){
            dirmatch = true;
        } else {
            *backpointer = *pointer;
            *pointer = (*pointer) -> sibling;
        }
    }

    return dirmatch;
}

typedef struct cmdStruct cmdStruct;
struct cmdStruct{

    char name[6];
    char description[100];

};

/* Cuestiones a tener en cuenta:

    if(dir = "." o dir =".."){
        
    }

    Manejar con variable variable que sea dir
    1) ./path//                                         | "//" esto será ""    if dir = ""
    2) ./<nombre>                                       | con nombre no perteneciente a ./, se busca el directorio
    3) ./&/home/Juegos                                  | "&" NO VALIDO
    4) ./home/Juegos de mi primo/                       | VALIDO
    5) ./home/../home/../home/../home/../home/../home   | VALIDO
    6) ../                                              | VALIDO 
    7) home                                             | Mientras se guardar en el dir, es decir, dir = home y se verifica en la carpeta
    8) home     crack  amigo                            | Tendría dir = "home     crack  amigo" y buscaría esa carpeta

    "./homx/Juegos/Steam"
*/ 

bool pathParser(char *arguments){
    // dir contendrá cada directorio
    int j = 0;
    int q = 0;
    int cont;
    // Este bloque pretende contar para que malloc sepa cuánto espacio de memoria preparar para el directorio
    while( arguments[q] != '\0' ){
        j = q;
        cont = 0;
        while( arguments[j] != '/' && arguments[j] != '\0' ){
            j++;
            cont++;
        }
        char *dir = (char *)malloc(cont + 1);
        if (dir == NULL) {
            printf("Error: No se pudo asignar memoria.\n");
            return false;
        }
        j = q;
        int i = 0;
        while( j < q + cont ){
            dir[i] = arguments[j];
            j++;
            i++;
        }
        dir[cont] = '\0';
        printf("Este es el valor de dir: '%s'\n", dir);
        // Aqui deberíamos chequear si el directorio existe
        
        free(dir);
        if( arguments[j] == '\0' ){
            q = j - 1;
        } else {
            q = j;
        }
        q++;
    }
    return true;
}

int main(void){

    // Case when an entry file is not given

    if(pathParser("  hola   amigo crack ")){
        printf("Fino\n");
    }

    struct nodeStruct head = {NULL, NULL, NULL, "/", 'D'};
    struct nodeStruct home = {&head, NULL, NULL, "home", 'D'};
    head.child = &home;
    
    nodeStruct *pathpointer = &home;
    /*
        Trying something
            head.child = &home;
            home.parent = &head;

            printf("%s", head.name);
            printf("%s", head.child->name);
    */

	// Commands' array
    cmdStruct touch = {"touch", "Creates a file without any content."};
    cmdStruct rm = {"rm", "Erases file with the target name."};
    cmdStruct mkdir = {"mkdir", "Creates a directory with the given name."};
    cmdStruct rmdir = {"rmdir", "Erases target directory."};
    cmdStruct ls = {"ls", "Prints all files and directories in target directory."};
    cmdStruct cd = {"cd", "Changes from current directory to target."};
    cmdStruct pwd = {"pwd", "Prints absolute path of current location."};
    cmdStruct wrts = {"wrts", "Creates a file with the description of the current file system."};
    cmdStruct help = {"help", "Displays commands available in the shell with their description."};
    cmdStruct exit = {"exit", "Finishes execution of the program."};

	cmdStruct commands[10] = {touch, rm, mkdir, rmdir, ls, cd, pwd, wrts, help, exit};

	while(true){
        char *entry = NULL;
        size_t n = 0;

        // Get new command
        printf("%s", "> ");
        getline(&entry, &n, stdin);

        size_t len = strlen(entry);
        if (len > 0 && entry[len - 1] == '\n') {
            entry[len - 1] = '\0';
        }

        // We need to find a way to separate the command and the argument.
        // We could find the first empty char in the string and whatever is before would be the command
        // whatever is after would be the argument(s)

        // Determinar el tamaño necesario
        int i = 0;
        while (entry[i] != ' ' && entry[i] != '\0') {
            i++;
        }

        // Command section

            // Asignar memoria dinámica (+1 para el '\0')
            char *command = (char *)malloc(i + 1);  
            if (command == NULL) {  
                printf("Memory Error\n");
                return 1;  
            }

            // Copiar los caracteres
            for (int j = 0; j < i; j++) {
                command[j] = entry[j];
            }
            command[i] = '\0';


        // ------------------ //

        // Arguments section

            char * arguments;

            if(entry[i] == '\0'){
                // Argumentos nulos
                arguments = "";

            } else {
                
                int j = i + 1;
                int q = 0;

                // Determinar el tamaño necesario
                while (entry[j] != '\0') {
                    j++;
                    q++;
                }

                // Asignar memoria dinámica (+1 para el '\0')
                arguments = (char *)malloc(q + 1);
                
                if (arguments == NULL) {  
                    printf("Memory Error\n");
                    return 1;  
                }

                // Copiar los caracteres
                for (int j = 0; j < q; j++) {
                    arguments[j] = entry[j + i + 1];
                }

                arguments[q] = '\0'; 
            }

        // ------------------

        // Print the command recognized
        printf("Command: '%s' \n", command);
        printf("Arguments: '%s' \n", arguments);

        // --------------------------------------------

        //bool check = isCommand(command, arreglo);

        // Used to remove empty spaces of the string
        strip(arguments);

        // Commands conditionals
        if( strcmp(command,"exit") == 0 && strcmp(arguments, "") == 0 ){
            
            cleanSysFile(&head);
            
            break;

        } else if( strcmp(command,"pwd") == 0 && strcmp(arguments, "") == 0 ){
            
            nodeStruct *pointer = pathpointer;
            
            pwdFunction(pointer);

            printf("\n");

        } else if( strcmp(command, "touch") == 0 ){

            if(pathpointer -> child == NULL){
                
                nodeStruct *newfile = (nodeStruct *)malloc(sizeof(nodeStruct));
                if (!newfile) {
                    printf("Memory allocation failed\n");
                    return 1;
                }

                newfile->parent = pathpointer;
                newfile->child = NULL;
                newfile->sibling = NULL;
                newfile->type = 'F';
                
                strncpy(newfile->name, arguments, sizeof(newfile->name) - 1);
                newfile->name[sizeof(newfile->name) - 1] = '\0'; 

                pathpointer->child = newfile;

            } else {

                nodeStruct *pointer = pathpointer->child;
                while(pointer->sibling != NULL){
                    pointer = pointer->sibling;
                }

                nodeStruct *newfile = (nodeStruct *)malloc(sizeof(nodeStruct));
                if (!newfile) {
                    printf("Memory allocation failed\n");
                    return 1;
                }

                newfile->parent = pathpointer;
                newfile->child = NULL;
                newfile->sibling = NULL;
                newfile->type = 'F';

                strncpy(newfile->name, arguments, sizeof(newfile->name) - 1);
                newfile->name[sizeof(newfile->name) - 1] = '\0'; 

                pointer->sibling = newfile;

            }
        } else if( strcmp(command, "rm") == 0 ){
            
            nodeStruct *pointer = pathpointer->child;
            nodeStruct *backpointer = NULL;

            if(searchFileForDel(&pointer, &backpointer, arguments, 'F')){

                if(pointer == pathpointer -> child){
                    pathpointer -> child = pointer -> sibling;
                } else {
                    backpointer -> sibling = pointer -> sibling;
                }
                free(pointer);

            } else {
                printf("El archivo dado no existe.\n");
            }

        } else if( strcmp(command,"ls") == 0 && strcmp(arguments, "") == 0 ){

            nodeStruct *pointer = pathpointer->child;
            
            while(pointer != NULL){
                printf("%s     ", pointer->name);
                pointer = pointer->sibling;
            }
            printf("\n");

        } else if( strcmp(command, "mkdir") == 0 ){
            
            if(pathpointer -> child == NULL){
                
                nodeStruct *newdir = (nodeStruct *)malloc(sizeof(nodeStruct));
                if (!newdir) {
                    printf("Memory allocation failed\n");
                    return 1;
                }

                newdir->parent = pathpointer;
                newdir->child = NULL;
                newdir->sibling = NULL;
                newdir->type = 'D';
                
                strncpy(newdir->name, arguments, sizeof(newdir->name) - 1);
                newdir->name[sizeof(newdir->name) - 1] = '\0'; 

                pathpointer->child = newdir;

            } else {

                nodeStruct *pointer = pathpointer->child;
                while(pointer->sibling != NULL){
                    pointer = pointer->sibling;
                }

                nodeStruct *newdir = (nodeStruct *)malloc(sizeof(nodeStruct));
                if (!newdir) {
                    printf("Memory allocation failed\n");
                    return 1;
                }

                newdir->parent = pathpointer;
                newdir->child = NULL;
                newdir->sibling = NULL;
                newdir->type = 'D';

                strncpy(newdir->name, arguments, sizeof(newdir->name) - 1);
                newdir->name[sizeof(newdir->name) - 1] = '\0'; 

                pointer->sibling = newdir;
            }

        } else if( strcmp(command, "rmdir") == 0 && strcmp(arguments, "") != 0 ){

            nodeStruct *pointer = pathpointer->child;
            nodeStruct *backpointer = NULL;

            if(searchFileForDel(&pointer, &backpointer, arguments, 'D')){

                //
                if(pointer->child != NULL){
                    cleanSysFile(pointer->child);
                }

                if(pointer == pathpointer -> child){
                    pathpointer -> child = pointer -> sibling;
                } else {
                    backpointer -> sibling = pointer -> sibling;
                }
                free(pointer);

            } else {
                printf("El directorio dado no existe.\n");
            }

        } else if( strcmp(command, "cd") == 0 && strcmp(arguments, "") != 0 ){

            nodeStruct *pointer = pathpointer->child;

            // Esto es temporal, para poder salir de las carpetas para probar cositas
            if(strcmp(arguments, "..") != 0){
                if(searchFile(&pointer, arguments, 'D')){
                    pathpointer = pointer;
                } else {
                    printf("El directorio dado no existe.\n");
                }
            } else {
                pathpointer = pathpointer -> parent;
            }

            

        } else if( strcmp(command, "help") == 0 && strcmp(arguments, "") == 0 ){

            printf("---------------------------------------------\nYou can use the following list of commands: \n");

            for(int i = 0; i < 10; i++){
                printf("%s: \n     %s\n", commands[i].name, commands[i].description);
            }

            printf("---------------------------------------------\n");

        } else {
            printf("%s\n", "El comando no pertenece a la lista de comandos, escribe 'help' para ver los comandos.\n");
        }

        free(arguments);
        free(command);

    }

	return 0;
}