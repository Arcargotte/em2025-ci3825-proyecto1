#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

typedef struct nodeStruct nodeStruct;
struct nodeStruct{

    nodeStruct *parent;
    nodeStruct *child;
    nodeStruct *sibling; 
    char name[100];
    char type;
    char time[30];

};

typedef struct cmdStruct cmdStruct;
struct cmdStruct{

    char name[6];
    char description[100];

};

char * getDateTime() {

    char *buffer = (char*)malloc(30 * sizeof(char));
    if (!buffer) return NULL;

    time_t t;  
    struct tm *tm_info;
    
    time(&t);  // Obtener tiempo actual
    tm_info = localtime(&t);  // Convertir a estructura de fecha y hora

    strftime(buffer, 30, "%H:%M-%Y/%m/%d", tm_info); // Formato personalizado

    return buffer;
}

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

void cleanSysFile( nodeStruct *pointer ){
    if(pointer != NULL){
        cleanSysFile(pointer->child);
        cleanSysFile(pointer->sibling);
        printf("%s\n", pointer->name);
        free(pointer);
    }
}

void pwdFunctionForTxt( nodeStruct *pointer, FILE *file){
    if(pointer != NULL){
        pwdFunctionForTxt(pointer -> parent, file);

        strcmp(pointer -> name, "/") == 0 ? fprintf(file, "/") :  fprintf(file, "%s/", pointer -> name);
    }
}

void pwdFunction( nodeStruct *pointer ){
    if(pointer != NULL){
        pwdFunction(pointer -> parent);

        strcmp(pointer -> name, "/") == 0 ? printf("/") :  printf("%s/", pointer -> name);
    }
}

void printSysFile( nodeStruct *pointer, FILE *file, int longestname ){
    if(pointer != NULL){
        fprintf(file, "%s", pointer->name);
        int pointerlen = strlen(pointer->name);
        while(pointerlen < longestname + 4){
            fprintf(file, " ");
            pointerlen++;
        }
        fprintf(file, "%s     %c    ", pointer->time, pointer->type);
        pwdFunctionForTxt(pointer, file);
        fprintf(file, "\n");
        printSysFile(pointer->child, file, longestname);
        printSysFile(pointer->sibling, file, longestname);
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

void helpFunction(cmdStruct * commands){
    printf("---------------------------------------------\nYou can use the following list of commands: \n");

    for(int i = 0; i < 10; i++){
        printf("%s: \n     %s\n", commands[i].name, commands[i].description);
    }

    printf("---------------------------------------------\n");
}

void lsFunction(nodeStruct *pointer){
            
    while(pointer != NULL){
        printf("%s     ", pointer->name);
        pointer = pointer->sibling;
    }

    printf("\n");
}

void wrtsFunction(nodeStruct *pointerroot, int longestname){
    FILE *file = fopen("sysfile.txt", "w");  // "w" crea/sobrescribe el archivo
    if (file == NULL) {
        printf("Error al crear el archivo.\n");
        
    } else {
        printSysFile(pointerroot, file, longestname);
        fclose(file);  // Siempre cerrar el archivo después de usarlo
        printf("Archivo creado y escrito con éxito.\n");
    }
}

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

bool pathParser(nodeStruct **currDir, nodeStruct **root, char *arguments){
    // dir contendrá cada directorio
    int j = 0;
    int q = 0;
    int cont;
    int dircont = 1;
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
            printf("Error: Assign memory failed.\n");
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

        // Aqui deberíamos chequear si el directorio existe

        // Decide si es relativa o absoluta
        if(dircont == 1 && strcmp(dir, "") == 0){
            *currDir = *root;
        }

        if( strcmp(dir, ".") == 0 ){
            printf("Dir found: %s\n", (*currDir) -> name);
        } else if(strcmp(dir, "") == 0 && dircont != 1){
            printf("Error: Invalid route.\n");
            return false;
        } else if(strcmp(dir, "..") == 0){
            if (currDir && *currDir && (*currDir)->parent) {
                *currDir = (*currDir)->parent;
            } else {
                printf("Error: Invalid route.\n");
                free(dir);
                return false;
            }
        } else if (strcmp(dir, "") != 0 && strcmp(dir, "..") != 0 && strcmp(dir, ".") != 0){
            nodeStruct *pointer = (*currDir)->child;
            if(searchFile(&pointer, dir, 'D')){
                *currDir = pointer;
            } else {
                printf("Error: %s not found in %s.\n", dir, (*currDir) -> name);
                free(dir);
                return false;
            }
        }

        free(dir);
        if( arguments[j] == '\0' ){
            q = j - 1;
        } else {
            q = j;
        }
        q++;
        dircont++;
    }
    return true;
}

bool createFile(char * pathname [], nodeStruct **root, char T){
    /**
     * Detecta el archivo objetivo dada una ruta absoluta verificando que
     * el último carácter sea el caracter nulo "\0" o la concatenacion de las cadenas
     * "/ + \0".
     * 
     * */

    int i = 0; //índice a un caracter de la ruta absoluta
    int j = 0; //índice al caracter del nombre de una carpeta en la ruta absoluta
    nodeStruct * currDir = root;
    nodeStruct * targetDir = root; 
    int nameSize = 0;
    int foundTargetFile = 1;

    while (foundTargetFile == 1){
        //Determina el nombre encontrado de un directorio
        while((pathname[j] != '/') || (pathname[j] != '\0')){
            i++;
            j++;
            nameSize++;
        }

        char * fileNameBuffer;
        fileNameBuffer = (char *)malloc(nameSize);

        //VERIFICAR QUE EL BUFFER SE HAYA CREADO CORRECTAMENTE

        //DETERMINA SI EL NOMBRE DEL DIRECTORIO DADO ES UNA DIRECCIÓN ABSOLUTA O RELATIVA
        if (strcmp(fileNameBuffer, '.') == 0){
            fileNameBuffer = currDir->name;
        } else if (strcmp(fileNameBuffer, '..') == 0){
            fileNameBuffer == currDir->parent;
        } else if (strcmp(fileNameBuffer, '/') == 0){
            //CASO ESPECIAL: SI LA RUTA ABSOLUTA SE DA INICIANDO CON EL CARACTER '/', PASA A LA SIGUIENTE ITERACIÓN
            i = i + 1;
            continue;
        }

        if (pathname[j] == '\0'){
            foundTargetFile = 0;
            //DETERMINA EN QUÉ PARTE DEL DIRETORIO ACTUAL DEBE CREARSE EL NUEVO ARCHIVO
            if (currDir->child != NULL){
                nodeStruct newFile = {currDir->parent, NULL, NULL, fileNameBuffer, T, getDateTime()};
            } else{
                currDir = currDir->child;
                while (currDir != NULL){
                    currDir = currDir->sibling;
                }
                nodeStruct newFile = {currDir->parent, NULL, NULL, fileNameBuffer, T, getDateTime()};
            }
        }

        //CURRDIR SE ACTUALIZA DESCENDIENDO DESDE LA RAÍZ DEL SISTEMA DE ARCHIVOS SI VERIFICA QUE EL DIRECTORIO EXISTE 
        if ((strcmp(targetDir->name, fileNameBuffer) == 0)){
            targetDir = targetDir->child;
        }

        while ((strcmp(targetDir->name, fileNameBuffer) != 0) && targetDir->type == 'D' && targetDir != NULL){
            targetDir = targetDir->sibling;
        }

        if (targetDir == NULL){
            return 1;
        } //Directorio no encontrado

        currDir = targetDir;
        j = 0;
        nameSize = 0;
        i++;
    }
    //EL ARCHIVO SE CREÓ EXITOSAMENTE
    return 0;
}

int main(void){
    // Case when an entry file is not given

    struct nodeStruct root = {NULL, NULL, NULL, "/", 'D', ""};

    // This is going to be the way to add a date to each node
    strcpy(root.time, getDateTime());

    nodeStruct *pathpointer = &root;
    
    int longestname = 0;

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
        // -------------------------------------------
        // This section is used to see what command it's recognizing and what argument too
        // Print the command recognized
        // printf("Command: '%s' \n", command);
        // printf("Arguments: '%s' \n", arguments);
        // --------------------------------------------
        
        // Used to remove empty spaces of the string
        strip(arguments);

        // Commands conditionals
        if( strcmp(command, "exit") == 0 && strcmp(arguments, "") == 0 ){

            cleanSysFile(&root);
            break;

        } else if( strcmp(command, "pwd") == 0 && strcmp(arguments , "") == 0 ){
            
            nodeStruct *pointer = pathpointer;
            pwdFunction(pointer);
            printf("\n");

        } else if( strcmp(command, "touch") == 0 && strcmp(arguments, "") != 0 ){

            nodeStruct *newfile = (nodeStruct *)malloc(sizeof(nodeStruct));
            if (!newfile) {
                printf("Memory allocation failed\n");
                return 1;
            }

            newfile->parent = pathpointer;
            newfile->child = NULL;
            newfile->sibling = NULL;
            newfile->type = 'F';
            strcpy(newfile->time, getDateTime());
            
            strncpy(newfile->name, arguments, sizeof(newfile->name) - 1);
            newfile->name[sizeof(newfile->name) - 1] = '\0'; 

            // We save the length of the longest file name
            if(strlen(arguments) > (unsigned)longestname){
                longestname = strlen(arguments);
            }

            if(pathpointer->child == NULL){

                pathpointer->child = newfile;

            } else {

                nodeStruct *pointer = pathpointer->child;

                while(pointer->sibling != NULL){
                    pointer = pointer->sibling;
                }

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

        } else if( strcmp(command,"ls") == 0 ){

            //Pruebas para cd
            nodeStruct *pointerpath = pathpointer;
            nodeStruct *pointerroot = &root;

            if(pathParser(&pointerpath, &pointerroot, arguments)){
                lsFunction(pointerpath->child);
            }

        } else if( strcmp(command, "mkdir") == 0 ){
            
            nodeStruct *newdir = (nodeStruct *)malloc(sizeof(nodeStruct));
            if (!newdir) {
                printf("Memory allocation failed\n");
                return 1;
            }

            newdir->parent = pathpointer;
            newdir->child = NULL;
            newdir->sibling = NULL;
            newdir->type = 'D';
            strcpy(newdir->time, getDateTime());
            
            strncpy(newdir->name, arguments, sizeof(newdir->name) - 1);
            newdir->name[sizeof(newdir->name) - 1] = '\0'; 

            // We save the length of the longest file name
            if(strlen(arguments) > (unsigned)longestname){
                longestname = strlen(arguments);
            }

            if(pathpointer -> child == NULL){

                pathpointer->child = newdir;

            } else {

                nodeStruct *pointer = pathpointer->child;
                while(pointer->sibling != NULL){
                    pointer = pointer->sibling;
                }
                pointer->sibling = newdir;
            }

        } else if( strcmp(command, "rmdir") == 0 && strcmp(arguments, "") != 0 ){

            nodeStruct *pointer = pathpointer->child;
            nodeStruct *backpointer = NULL;

            if(searchFileForDel(&pointer, &backpointer, arguments, 'D')){

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

            //Pruebas para cd
            nodeStruct *pointerpath = pathpointer;
            nodeStruct *pointerroot = &root;

            if(pathParser(&pointerpath, &pointerroot, arguments)){
                pathpointer = pointerpath;
            }

        } else if( strcmp(command, "help") == 0 && strcmp(arguments, "") == 0 ){

            helpFunction(commands);

        } else if( strcmp(command, "wrts") == 0 && strcmp(arguments, "") == 0 ){

            nodeStruct *pointerroot = &root;
            wrtsFunction(pointerroot, longestname);

        } else {
            printf("%s\n", "El comando no pertenece a la lista de comandos, escribe 'help' para ver los comandos.\n");
        }

        free(arguments);
        free(command);

    }

	return 0;
}