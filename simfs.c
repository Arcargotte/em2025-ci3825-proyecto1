#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

int longestname = 0;

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

void cleanSysFile( nodeStruct *pointer ){
    if(pointer != NULL){
        cleanSysFile(pointer->child);
        cleanSysFile(pointer->sibling);
        if(pointer->parent != NULL){
            free(pointer);
        }
    }
}

void pwdFunctionForTxt( nodeStruct *pointer, FILE *file){
    if(pointer != NULL){
        pwdFunctionForTxt(pointer -> parent, file);

        strcmp(pointer -> name, "/") == 0 ? fprintf(file, "/") :  fprintf(file, "%s/", pointer -> name);
    }
}

void pwdFunction( nodeStruct *currDir, int counter){

    nodeStruct *pointer = currDir;

    if(pointer != NULL){
        counter++;
        pwdFunction(pointer->parent, counter);

        if(strcmp(pointer->name, "/") == 0){
            printf("\x1b[35m/\x1b[0m");
        } else{
            printf("\x1b[37m%s\x1b[0m", pointer->name);
            if(counter != 1){
                printf("\x1b[35m/\x1b[0m");
            }
        }
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

bool findFile( nodeStruct *pointer, char * dirName, char fileType){
    
    bool dirmatch = false;

    while( pointer != NULL && !dirmatch ){
        if(strcmp(dirName, pointer -> name) == 0 && pointer -> type == fileType ){
            dirmatch = true;
        } else {
            pointer = pointer -> sibling;
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
    printf("\nYou can use the following list of commands: \n\n");

    for(int i = 0; i < 10; i++){
        printf("\x1b[36m%s:\x1b[37m \n     %s\n", commands[i].name, commands[i].description);
    }
}

void printFiles(nodeStruct *pointer){
    int i = 0; 
    while(pointer != NULL){
        if(i == 0){
            printf("%s\t", pointer->name);
        } else {
            printf("\n%s\t", pointer->name);
        }
        pointer = pointer->sibling;
        i++;
    }
    printf("\n");
}

void printFileswithDate(nodeStruct *pointer, int longestFileName){
    int i = 0;
    while(pointer != NULL){
        if(i != 0){
            printf("\n");
        }
        printf("%s", pointer->name);
        int j = strlen(pointer->name);
        // Prints an space until it reaches the longest name size so it shows as a list
        while(j < longestFileName){
            printf(" ");
            j++;
        }
        printf("\t%s\t\x1b[36m%c\x1b[37m", pointer->time, pointer->type);
        pointer = pointer->sibling;
        i++;
    }
    printf("\n");
}


void wrtsFunction(nodeStruct *head, int longestname){

    nodeStruct *pointerhead = head;

    FILE *file = fopen("sysfile.txt", "w");
    if (file == NULL) {
        printf("Error creating file!\n");
        
    } else {
        printSysFile(pointerhead, file, longestname);
        fclose(file);
        printf("File created succesfully!\n");
    }
}

void createFile(nodeStruct *dir, char * nameFile, char type){

    nodeStruct *newfile = (nodeStruct *)malloc(sizeof(nodeStruct));
    if (!newfile) {
        printf("Memory allocation failed\n");
        return;
    }

    newfile->parent = dir;
    newfile->child = NULL;
    newfile->sibling = NULL;
    newfile->type = type;

    char *dateTime = getDateTime();

    strcpy(newfile->time, dateTime);

    //free memory
    free(dateTime);
    dateTime = NULL;
    
    strncpy(newfile->name, nameFile, sizeof(newfile->name) - 1);

    // We save the length of the longest file name
    if(strlen(nameFile) > (unsigned)longestname){
        longestname = strlen(nameFile);
    }

    if(dir->child == NULL){

        dir->child = newfile;

    } else {

        // pointerIndex is used to connect the new file in the tree
        nodeStruct *pointerIndex = dir->child;

        while(pointerIndex->sibling != NULL){
            pointerIndex = pointerIndex->sibling;
        }

        pointerIndex->sibling = newfile;
    }

}

void deleteFile(nodeStruct *dir, nodeStruct *pathpointer, char *file, char type){
    
    nodeStruct *pointer = dir->child;
    nodeStruct *prev = NULL;

    if(type == 'F'){

        if(searchFileForDel(&pointer, &prev, file, type)){

            if(pointer == dir->child){
                dir->child = pointer->sibling;
            } else {
                prev->sibling = pointer->sibling;
            }
            free(pointer);

        } else {
            printf("El archivo dado no existe.\n");
        }

    } else {

        if(searchFileForDel(&pointer, &prev, file, type)){

            if(pointer != pathpointer){
                // Delete the data inside the directory
                if(pointer->child == NULL){
                    // Delete the directory
                    if(pointer == dir->child){
                        dir->child = pointer->sibling;
                    } else {
                        dir->sibling = pointer->sibling;
                    }
                    free(pointer);

                } else{
                    printf("Error: Directory not empty!\n");
                }
            } else{

                printf("Error: You can't delete current directory!\n");

            }

        } else {
            printf("Error: Target directory does not exist!\n");
        }

    }

}

int map_to_int(char * command, char * arguments){
    
    if( strcmp(command, "exit") == 0 && strcmp(arguments, "") == 0 ){

        return 1;

    } else if( strcmp(command, "pwd") == 0 && strcmp(arguments , "") == 0 ){

        return 2;

    } else if( strcmp(command, "touch") == 0 && strcmp(arguments, "") != 0 ){

        return 3;

    } else if( strcmp(command, "rm") == 0 && strcmp(arguments, "") != 0 ){

        return 4;

    } else if( strcmp(command,"ls") == 0 ){

        return 5;

    } else if( strcmp(command, "mkdir") == 0 ){
        
        return 6;

    } else if( strcmp(command, "rmdir") == 0 && strcmp(arguments, "") != 0 ){

        return 7;

    } else if( strcmp(command, "cd") == 0 && strcmp(arguments, "") != 0 ){

        return 8;

    } else if( strcmp(command, "help") == 0 && strcmp(arguments, "") == 0 ){

        return 9;

    } else if( strcmp(command, "wrts") == 0 && strcmp(arguments, "") == 0 ){

        return 10;

    } else {
        
        return 11;

    }
}

/**
 * @brief This function splits a path in two different parts: First one is the path to the target directory 
 *        and second one the name of the target file.
 * 
 * @param path String with the pathname.
 * @param dirs String variable where the path with format "d0/d1/d2/.../dn" where di is the ith directory is going to be saved.
 * @param file String variable where the name of the file is going to be saved.
 * @return bool True if splitting was succesfull, False otherwise.
 */
bool splitPath(char *path, char **dirs, char **file){
    // Index to ith char of the path
    int size = strlen(path);  

    if(size == 0){
        printf("Error: Invalid Path!\n");
        return false;
    }

    int i = size - 1;

    while(i > -1){
        if(path[i] == '/'){
            break;
        }
        i--;
    }

    // Index to the first char of last file in path
    int fileNameSize = size - i;
    
    *file = (char *)malloc(fileNameSize + 1);  
    if (*file == NULL) {  
        printf("Memory Error\n");
        return false;  
    }

    // Saves in stack name of target file to create
    int j = i + 1;
    i = 0;
    while(j < size){
        (*file)[i] = path[j];
        i++;
        j++;
    }
    (*file)[i] = '\0';

    *dirs = (char *)malloc(size - fileNameSize + 2);  
    if (*dirs == NULL) {  
        printf("Memory Error\n");
        free(*file);
        return false;  
    }

    //
    i = 0;
    while(i != size - fileNameSize + 1){
        (*dirs)[i] = path[i];
        i++;
    }
    (*dirs)[i] = '\0';
    
    return true;
}

void inputParser(char *input, char **command, char **arguments){

    //Determine memory space for command
    int i = 0;
    while (input[i] != ' ' && input[i] != '\0') {
        i++;
    }

    // Command section
    // Assign dynamic memory
    *command = (char *)malloc(i + 1);  
    if (*command == NULL) {  
        printf("Memory Error\n");
        return;  
    }

    for (int j = 0; j < i; j++) {
        (*command)[j] = input[j];
    }
    (*command)[i] = '\0';
    // ------------------ //

    // Arguments section

    if(input[i] == '\0'){
        // Null argument
        *arguments = (char *)malloc(1);
        strcpy(*arguments, "");

    } else {
        
        int j = i + 1;
        int q = 0;

        // Determine memory space for arguments
        while (input[j] != '\0') {
            j++;
            q++;
        }

        // Assign dynamic memory
        *arguments = (char *)malloc(q + 1);
        
        if (*arguments == NULL) {  
            free(*command);
            printf("Memory Error\n");
            return;  
        }

        // Copy string
        for (int j = 0; j < q; j++) {
            (*arguments)[j] = input[j + i + 1];
        }

        (*arguments)[q] = '\0'; 
    }

}

bool pathParser(nodeStruct **actualpointer, nodeStruct **head, char *arguments){

    int j = 0;
    int q = 0;
    int cont;
    int dircont = 1;
    
    while( arguments[q] != '\0' ){
        j = q;
        cont = 0;
        while( arguments[j] != '/' && arguments[j] != '\0' ){
            j++;
            cont++;
        }
        char *dir = (char *)malloc(cont + 1);
        // If memory allocation fails
        if (dir == NULL) {
            printf("Error: Assign memory failed.\n");
            return false;
        }
        // Restart the variable j to copy the directory
        j = q;
        int i = 0;
        while( j < q + cont ){
            dir[i] = arguments[j];
            j++;
            i++;
        }
        dir[cont] = '\0';

        
        if(dircont == 1 && strcmp(dir, "") == 0){
            *actualpointer = *head;
        }

        if( strcmp(dir, ".") == 0 ){
            printf("Dir found: %s\n", (*actualpointer) -> name);
        } else if(strcmp(dir, "") == 0 && dircont != 1){
            printf("Error: Invalid route.\n");
            free(dir);
            return false;
        } else if(strcmp(dir, "..") == 0){
            if (actualpointer && *actualpointer && (*actualpointer)->parent) {
                *actualpointer = (*actualpointer)->parent;
            } else {
                printf("Error: Invalid route.\n");
                free(dir);
                return false;
            }
        } else if (strcmp(dir, "") != 0 && strcmp(dir, "..") != 0 && strcmp(dir, ".") != 0){
            nodeStruct *pointer = (*actualpointer)->child;
            if(searchFile(&pointer, dir, 'D')){
                *actualpointer = pointer;
            } else {
                printf("Error: %s not found in %s.\n", dir, (*actualpointer) -> name);
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

int getLongestFileName(nodeStruct *pointer){
    int longestFileName = 0;
    while(pointer != NULL){
        if((int)strlen(pointer->name) > longestFileName){
            longestFileName = strlen(pointer->name);
        }
        pointer = pointer->sibling;
        
    }
    return longestFileName;
}

void lsFunction(char *arguments, nodeStruct *currDir, nodeStruct *head){
    if(arguments[0] == '-' && arguments[1] == 'l'){
        char path[strlen(arguments) - 1];
        int i = 0;
        int j = 2;
        while(i < (int)strlen(arguments) - 2){
            path[i] = arguments[j];
            i++;
            j++;
        }
        path[i] = '\0';

        strip(path);

        nodeStruct *pointerpath = currDir;
        nodeStruct *pointerhead = head;
        
        if(pathParser(&pointerpath, &pointerhead, path)){
            int longestFileName= getLongestFileName(pointerpath->child);

            printFileswithDate(pointerpath->child, longestFileName);
        }

    } else{
        nodeStruct *pointerpath = currDir;
        nodeStruct *pointerhead = head;

        if(pathParser(&pointerpath, &pointerhead, arguments)){
            printFiles(pointerpath->child);
        }
    }
}

void touchFunction(char *path, nodeStruct *currDir, nodeStruct *root){

    char *dirs;
    char *file;
    if(splitPath(path, &dirs, &file)){
        
        nodeStruct *pointerpath = currDir;
        nodeStruct *pointerhead = root;

        if(pathParser(&pointerpath, &pointerhead, dirs)){

            nodeStruct *cpy_pointerpath = pointerpath;

            if(!findFile(cpy_pointerpath->child, file, 'F')){
                createFile(pointerpath, file, 'F');
            } else {
                printf("Error: Already exists a file with that name!\n");
            }

        }
        free(dirs);
        free(file);
    }

}

void mkdirFunction(char *path, nodeStruct *currDir, nodeStruct *root){

    char *dirs;
    char *file;
    if(splitPath(path, &dirs, &file)){

        nodeStruct *pointerpath = currDir;
        nodeStruct *pointerhead = root;

        if(pathParser(&pointerpath, &pointerhead, dirs)){

            nodeStruct *cpy_pointerpath = pointerpath;
            if(!findFile(cpy_pointerpath->child, file, 'D')){
                createFile(pointerpath, file, 'D');
            } else {
                printf("Error: Already exists a directory with that name!\n");
            }

        }
        free(dirs);
        free(file);
    }

}

void cdFunction(char *path, nodeStruct **currDir, nodeStruct *root){

    nodeStruct *pointerpath = *currDir;
    nodeStruct *pointerhead = root;

    if(pathParser(&pointerpath, &pointerhead, path)){
        *currDir = pointerpath;
    }
}

void rmFunction(char *path, nodeStruct *currDir, nodeStruct *head){
    
    char *dirs;
    char *file;
    if(splitPath(path, &dirs, &file)){
        nodeStruct *cpy_pathpointer = currDir;
        nodeStruct *pointerhead = head;

        if(pathParser(&cpy_pathpointer, &pointerhead, dirs)){
            deleteFile(cpy_pathpointer, currDir, file, 'F');
        }
        free(dirs);
        free(file);
    }

}

void rmdirFunction(char *path, nodeStruct *currDir, nodeStruct *head){

    char *dirs;
    char *file;
    if(splitPath(path, &dirs, &file)){

        nodeStruct *cpy_pathpointer = currDir;
        nodeStruct *pointerhead = head;

        if(pathParser(&cpy_pathpointer, &pointerhead, dirs)){
            // Verify if current directory is target directory
            deleteFile(cpy_pathpointer, currDir, file, 'D');
        }
        free(dirs);
        free(file);
    }
}

bool generateSysFile(char *fileName, nodeStruct *head, nodeStruct **pathpointer){

    FILE *txt_file;
    // Opens file with read function
    txt_file = fopen(fileName, "r");

    // In case the file couldn't open
    if (txt_file == NULL) {
        printf("Error: Couldn't open simfs.txt!\n");
        return false;
    }

    // Reads the content of the file line by line
    int line_counter = 1;
    char linea[999];
    while (fgets(linea, sizeof(linea), txt_file) != NULL) {
        
        // Get the memory space needed for path
        int q = 0;
        while(linea[q] != '\t'){
            q++;
        }

        // Allocating memory for path
        char *path = (char *)malloc(q + 1);

        // Saving first occurrence of tab '\t'
        int first_tab_occurr = q;

        // Iterating through a variable number of tabs until it finds the file type
        while(linea[first_tab_occurr] == '\t'){
            first_tab_occurr++;
        }
        char fileType = linea[first_tab_occurr];

        // 
        int j = 0;
        while(j < q){
            path[j] = linea[j];
            j++;
        }
        path[j] = '\0';

        if(fileType != 'D' && fileType != 'F'){
            printf("Error in line %d: Not valid file type!\n", line_counter);
            return 1;
        }  

        // Verify that first directory is correct
        if(strcmp(path, "/") != 0 && line_counter == 1){
            printf("Error in line 1: Not valid first directory!\n");
            return false;
        }

        // If first directory is correct, we initialize head and pathpointer
        if(strcmp(path, "/") == 0 && line_counter == 1){
            // Set initial value for head
            head->parent = NULL;
            head->child = NULL;
            head->sibling = NULL;
            strcpy(head->name, "/");
            head->type = 'D';
            
            // Gets DateTime
            char *dateTime = getDateTime();
            // Copy info to head
            strcpy(head->time, dateTime);
            // Free memory
            free(dateTime);
            dateTime = NULL;

            // Set pathpointer
            *pathpointer = head;
        }

        // Calls the appropiate function for each file type
        if(fileType == 'D' && line_counter != 1){
            mkdirFunction(path, *pathpointer, head);
        } else if(fileType == 'F' && line_counter != 1) {
            touchFunction(path, *pathpointer, head);
        }

        free(path);
        line_counter++;
    }   

    // Close file
    fclose(txt_file);

    return true;
}

int main(int argc, char *argv[]){

    struct nodeStruct head;
    nodeStruct *pathpointer;

    if(argc > 1){
        // Case when more than one argument is given
        if(!generateSysFile(argv[1], &head, &pathpointer)){
            return 1;
        }

    } else {
        // Case when only one argument is given
        // Set initial value for head
        head.parent = NULL;
        head.child = NULL;
        head.sibling = NULL;
        strcpy(head.name, "/");
        head.type = 'D';

        // Gets DateTime
        char *dateTime = getDateTime();
        // Copy info to head
        strcpy(head.time, dateTime);
        // Free memory
        free(dateTime);
        dateTime = NULL;

        // Set pathpointer
        pathpointer = &head;
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
                cleanSysFile(&head);
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
                touchFunction(arguments, pathpointer, &head);
                break;
            }

            //rm
            case 4: {
                rmFunction(arguments, pathpointer, &head);
                break;
            }

            //ls
            case 5: {
                lsFunction(arguments, pathpointer, &head);
                break;
            }

            //mkdir
            case 6: {
                mkdirFunction(arguments, pathpointer, &head);
                break;
            }

            //rmdir
            case 7: {
                rmdirFunction(arguments, pathpointer, &head);
                break;
            }

            //cd
            case 8: {
                cdFunction(arguments, &pathpointer, &head);
                break;
            }

            //help
            case 9: {
                helpFunction(commands);
                break;
            }

            //wrts
            case 10:{
                wrtsFunction(&head, longestname);
                break;
            }

            //otherwise
            case 11:
                printf("Not valid command, write help to see the list of commands availables\n");
                break;
        }
        free(arguments);
        free(command);

    }

	return 0;
}
