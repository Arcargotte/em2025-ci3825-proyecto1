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

/**
 * @brief This function is used to get the actual date and time.
 * 
 * @param void.
 * @return Returns the formatted string with date and time.
 */
char * getDateTime() {

    // Uses malloc so we can usea this variable out of the function
    char *buffer = (char*)malloc(30 * sizeof(char));
    if (!buffer) return NULL;

    time_t t;  
    struct tm *tm_info;
    
    time(&t);  // Get time
    tm_info = localtime(&t);  // Convert to localtime struct

    strftime(buffer, 30, "%H:%M-%Y/%m/%d", tm_info);

    return buffer;
}

/**
 * @brief This function removes empty spaces on the two sides of the string
 * 
 * @param str Variable you want to strip.
 * @return Returns the string without empty spaces.
 */
void strip(char *str) {

    if(strcmp(str, "") != 0){
        int inicio = 0, fin = strlen(str) - 1;

        // Moves until it gets the first empty space
        while (str[inicio] == ' ') {
            inicio++;
        }

        // Moves the final pointer until it finds the last empty space
        while (fin > inicio && str[fin] == ' ') {
            fin--;
        }

        // Move the char to finally eliminate empty spaces
        int i, j = 0;
        for (i = inicio; i <= fin; i++) {
            str[j++] = str[i];
        }
        str[j] = '\0'; // Add terminator
    }
    
}

/**
 * @brief This function is used to delete the whole tree or a subtree. 
 * 
 * @param pointer Pointer to the first node of the subtree to be deleted.
 * @return void
 */
void cleanSysFile( nodeStruct *pointer ){
    if(pointer != NULL){
        // First delete child
        cleanSysFile(pointer->child);
        // Then sibling
        cleanSysFile(pointer->sibling);

        // At the end, the actual pointer
        if(pointer->parent != NULL){
            free(pointer);
        }
    }
}

/**
 * @brief This function prints pwd in a file specified. 
 * 
 * @param pointer Pointer to the current directory.
 * @param file File pwd will be written.
 * @return void
 */
void pwdFunctionForTxt( nodeStruct *pointer, FILE *file){
    if(pointer != NULL){
        pwdFunctionForTxt(pointer -> parent, file);

        strcmp(pointer -> name, "/") == 0 ? fprintf(file, "/") :  fprintf(file, "%s/", pointer -> name);
    }
}

/**
 * @brief This function prints pwd in console log. 
 * 
 * @param pointer Pointer to the current directory.
 * @param counter Int used to know what was the first dir, always use it with 0.
 * @return void
 */
void pwdFunction( nodeStruct *currDir, int counter){

    nodeStruct *pointer = currDir;

    if(pointer != NULL){
        counter++;
        // Recursive call
        pwdFunction(pointer->parent, counter);

        if(strcmp(pointer->name, "/") == 0){
            // Case it's root directory
            printf("\x1b[35m/\x1b[0m");
        } else{
            printf("\x1b[37m%s\x1b[0m", pointer->name);
            if(counter != 1){
                printf("\x1b[35m/\x1b[0m");
            }
        }
    }
}

/**
 * @brief This function prints the tree for wrts. 
 * 
 * @param pointer Pointer to the first directory.
 * @param file File where the tree will be printed.
 * @param longestname Int representing the longest name of directory.
 * @return void
 */
void printSysFile( nodeStruct *pointer, FILE *file, int longestname ){
    if(pointer != NULL){
        // Prints dir name
        fprintf(file, "%s", pointer->name);

        // Keeps format
        int pointerlen = strlen(pointer->name);
        while(pointerlen < longestname + 4){
            fprintf(file, " ");
            pointerlen++;
        }

        // Prints date, time and file type
        fprintf(file, "%s\t%c\t", pointer->time, pointer->type);
        // Prints path
        pwdFunctionForTxt(pointer, file);
        fprintf(file, "\n");
        // Recursives calls
        printSysFile(pointer->child, file, longestname);
        printSysFile(pointer->sibling, file, longestname);
    }
    
}

/**
 * @brief This looks for a file inside a directory/file. 
 * It CHANGES the pointer given to target directory/file's position, 
 * so it looks for and go to the target directory/file. 
 * 
 * @param pointer Pointer to the current directory/file.
 * @param dirName Name of directory/file.
 * @param fileType If 'D', directory and if 'F', file.
 * @return True if it finds the directory/file and changes the position of pointer, False otherwise.
 */
bool searchFile( nodeStruct **pointer, char * dirName, char fileType){
    
    bool dirmatch = false;

    while( *pointer != NULL && !dirmatch ){
        if(strcmp(dirName, (*pointer) -> name) == 0 && (*pointer) -> type == fileType ){
            dirmatch = true;
        } else {
            // Changing pointer to sibling
            *pointer = (*pointer) -> sibling;
        }
    }

    return dirmatch;
}

/**
 * @brief This looks for a file inside a directory/file. 
 * It does NOT changes the pointer given to target directory/file's position, 
 * so it looks for and go to the target directory/file. 
 * 
 * @param pointer Pointer to the current directory/file.
 * @param dirName Name of directory/file.
 * @param fileType If 'D', directory and if 'F', file.
 * @return True if it finds the directory/file, False otherwise.
 */
bool findFile( nodeStruct *pointer, char * dirName, char fileType){
    
    bool dirmatch = false;

    while( pointer != NULL && !dirmatch ){
        if(strcmp(dirName, pointer -> name) == 0 && pointer -> type == fileType ){
            dirmatch = true;
        } else {
            // Not changing by reference pointer to sibling
            pointer = pointer -> sibling;
        }
    }

    return dirmatch;
}

/**
 * @brief This looks for a file inside a directory/file in order to delete it. 
 * 
 * @param pointer Pointer to the current directory/file.
 * @param prevpointer Initialized with NULL, but it will always contain the previous pointer of 
 *                    the target directory.
 * @param dirName Name of directory/file.
 * @param fileType If 'D', directory and if 'F', file.
 * @return True if it finds the directory/file and deletes it, false otherwise.
 */
bool searchFileForDel( nodeStruct **pointer, nodeStruct **prevpointer, char * dirName, char fileType){
    
    bool dirmatch = false;

    while( *pointer != NULL && !dirmatch ){
        if(strcmp(dirName, (*pointer) -> name) == 0 && (*pointer) -> type == fileType ){
            dirmatch = true;
        } else {
            *prevpointer = *pointer;
            *pointer = (*pointer) -> sibling;
        }
    }

    return dirmatch;
}

/**
 * @brief Prints all the commands that user can use. 
 * 
 * @param commands Array of 2-dimensions that contains command and specification of the command.
 * @return void.
 */
void helpFunction(cmdStruct * commands){
    printf("\nYou can use the following list of commands: \n\n");

    for(int i = 0; i < 10; i++){
        printf("\x1b[36m%s:\x1b[37m \n     %s\n", commands[i].name, commands[i].description);
    }
}

/**
 * @brief Prints all the files that a directory given has. 
 * 
 * @param pointer Pointer to the first child file of the directory.
 * @return void.
 */
void printFiles(nodeStruct *pointer){
    int i = 0; 
    while(pointer != NULL){
        if(i == 0){
            printf("%s\t", pointer->name);
        } else {
            printf("\n%s\t", pointer->name);
        }
        // Goes to the sibling, remember it starts from the first child
        pointer = pointer->sibling;
        i++;
    }
    printf("\n");
}

/**
 * @brief Prints all the files that a directory given has and includes the dates. 
 * 
 * @param pointer Pointer to the first child file of the directory.
 * @return void.
 */
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

/**
 * @brief Prints all the files of the system files created. 
 * 
 * @param head Pointer to the root directory.
 * @param longestname Length of the longest file name.
 * @return void.
 */
void wrtsFunction(nodeStruct *head, int longestname){

    nodeStruct *pointerhead = head;

    FILE *file = fopen("sysfile.txt", "w");
    if (file == NULL) {
        printf("\x1b[31mError:\x1b[37m creating file!\n");
        
    } else {
        printSysFile(pointerhead, file, longestname);
        fclose(file);
        printf("File created succesfully!\n");
    }
}

/**
 * @brief Creates a new file in the specified directory
 * 
 * @param dir Pointer to the current directory
 * @param nameFile File's name.
 * @param type If 'D', directory and if 'F', file.
 * @return void.
 */
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

    // free memory
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

/**
 * @brief Creates a new file in the specified directory
 * 
 * @param dir Copy to current directory
 * @param pathpointer Current directory
 * @param file Name of the file.
 * @param type If 'D', directory and if 'F', file.
 * @return void.
 */
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
                    printf("\x1b[31mError:\x1b[37m Directory not empty!\n");
                }
            } else{
                printf("\x1b[31mError:\x1b[37m You can't delete current directory!\n");
            }
        } else {
            printf("\x1b[31mError:\x1b[37m Target directory does not exist!\n");
        }
    }

}

/**
 * @brief This function maps a condition to an int.
 * 
 * @param command Command given by the user.
 * @param arguments Arguments guven by the user. 
 * @return int mapped to the condition.
 */
int map_to_int(char * command, char * arguments){
    
    // exit command
    if( strcmp(command, "exit") == 0 && strcmp(arguments, "") == 0 ){

        return 1;
    
    // pwd command
    } else if( strcmp(command, "pwd") == 0 && strcmp(arguments , "") == 0 ){

        return 2;
    
    // touch command
    } else if( strcmp(command, "touch") == 0 && strcmp(arguments, "") != 0 ){

        return 3;
    
    // rm command
    } else if( strcmp(command, "rm") == 0 && strcmp(arguments, "") != 0 ){

        return 4;
    
    // ls command
    } else if( strcmp(command,"ls") == 0 ){

        return 5;

    // mkdir command
    } else if( strcmp(command, "mkdir") == 0 ){
        
        return 6;
    
    // rmdir command
    } else if( strcmp(command, "rmdir") == 0 && strcmp(arguments, "") != 0 ){

        return 7;
    
    // cd command
    } else if( strcmp(command, "cd") == 0 && strcmp(arguments, "") != 0 ){

        return 8;
    
    // help command
    } else if( strcmp(command, "help") == 0 && strcmp(arguments, "") == 0 ){

        return 9;
    
    // wrts command
    } else if( strcmp(command, "wrts") == 0 && strcmp(arguments, "") == 0 ){

        return 10;
    
    // otherwise
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
        printf("\x1b[31mError:\x1b[37m Invalid Path!\n");
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
        printf("\x1b[31mMemory Error\x1b[37m\n");
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
    strip(*file);

    *dirs = (char *)malloc(size - fileNameSize + 2);  
    if (*dirs == NULL) {  
        printf("\x1b[31mMemory Error\x1b[37m\n");
        free(*file);
        return false;  
    }

    // Copy the rest to dirs
    i = 0;
    while(i != size - fileNameSize + 1){
        (*dirs)[i] = path[i];
        i++;
    }
    (*dirs)[i] = '\0';
    
    return true;
}

/**
 * @brief This function parse the user input to divide entry in two parts, command and argument
 * 
 * @param input User input.
 * @param command Variable to save command part.
 * @param arguments Variable to save arguments part.
 * @return void
 */
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
        printf("\x1b[31mMemory Error\x1b[37m\n");
        return;  
    }

    // Copy the command part
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
            printf("\x1b[31mMemory Error\x1b[37m\n");
            return;  
        }

        // Copy the arguments part
        for (int j = 0; j < q; j++) {
            (*arguments)[j] = input[j + i + 1];
        }

        (*arguments)[q] = '\0'; 
    }

}

/**
 * @brief This function verify that the path given is a valid path
 * 
 * @param actualpointer Pointer to current directory.
 * @param head Pointer to the root directory.
 * @param arguments String with path to parse.
 * @return True if path is valid, False otherwise.
 */
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
            printf("\x1b[31mMemory Error\x1b[37m\n");
            return false;
        }
        // Restart the variable j to copy the path
        j = q;
        int i = 0;
        while( j < q + cont ){
            dir[i] = arguments[j];
            j++;
            i++;
        }
        dir[cont] = '\0';

        // If the first char is "/", then
        if(dircont == 1 && strcmp(dir, "") == 0){
            *actualpointer = *head;
        }

        // Different cases to determine what to do with each path 
        if(strcmp(dir, "") == 0 && dircont != 1){
            printf("\x1b[31mError:\x1b[37m Invalid route.\n");
            free(dir);
            return false;
        } else if(strcmp(dir, "..") == 0){
            if (actualpointer && *actualpointer && (*actualpointer)->parent) {
                *actualpointer = (*actualpointer)->parent;
            } else {
                printf("\x1b[31mError:\x1b[37m Invalid route.\n");
                free(dir);
                return false;
            }
        } else if (strcmp(dir, "") != 0 && strcmp(dir, "..") != 0 && strcmp(dir, ".") != 0){
            nodeStruct *pointer = (*actualpointer)->child;
            if(searchFile(&pointer, dir, 'D')){
                *actualpointer = pointer;
            } else {
                printf("\x1b[31mError:\x1b[37m %s not found in %s!\n", dir, (*actualpointer) -> name);
                free(dir);
                return false;
            }
        }

        // free memory
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

/**
 * @brief This function gets the length of the longest file name from a given directory
 * 
 * @param pointer Pointer to first child of current directory.
 * @return Length of the longest file name from the directory.
 */
int getLongestFileName(nodeStruct *pointer){
    int longestFileName = 0;
    while(pointer != NULL){

        // If the length of this file name is longer than longest, then it's the longest 
        if((int)strlen(pointer->name) > longestFileName){
            longestFileName = strlen(pointer->name);
        }
        pointer = pointer->sibling;
        
    }
    return longestFileName;
}

/**
 * @brief This function prints the files from a directory given if the path is valid
 * 
 * @param arguments String with a path.
 * @param currDir Pointer to the current directory.
 * @param head Pointer to root directory.
 * @return void
 */
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

/**
 * @brief This function creates files in a path only if path is valid 
 * 
 * @param path String with a path.
 * @param currDir Pointer to the current directory.
 * @param root Pointer to root directory.
 * @return void
 */
void touchFunction(char *path, nodeStruct *currDir, nodeStruct *root){

    char *dirs; 
    char *file;
    // Divide the path in directory paths and file name
    if(splitPath(path, &dirs, &file)){
        
        if(strcmp(file, "") != 0){

            nodeStruct *pointerpath = currDir;
            nodeStruct *pointerhead = root;
            // Verifies that path is valid
            if(pathParser(&pointerpath, &pointerhead, dirs)){

                nodeStruct *cpy_pointerpath = pointerpath;
                // Verifies if there is no file called the same
                if(!findFile(cpy_pointerpath->child, file, 'F')){
                    createFile(pointerpath, file, 'F');
                } else {
                    printf("\x1b[31mError:\x1b[37m Already exists a file with that name!\n");
                }

            }
        } else {
            printf("\x1b[31mError:\x1b[37m No name of file provided!\n");
        }
        free(dirs);
        free(file);
    }

}

/**
 * @brief This function creates directory in a path only if path is valid 
 * 
 * @param path String with a path.
 * @param currDir Pointer to the current directory.
 * @param root Pointer to root directory.
 * @return void
 */
void mkdirFunction(char *path, nodeStruct *currDir, nodeStruct *root){

    char *dirs;
    char *file;
    // Divide the path in directory paths and file name
    if(splitPath(path, &dirs, &file)){

        if(strcmp(file, "") != 0){

            nodeStruct *pointerpath = currDir;
            nodeStruct *pointerhead = root;

            // Verifies that path is valid
            if(pathParser(&pointerpath, &pointerhead, dirs)){

                nodeStruct *cpy_pointerpath = pointerpath;
                // Verifies if there is no directory called the same
                if(!findFile(cpy_pointerpath->child, file, 'D')){
                    createFile(pointerpath, file, 'D');
                } else {
                    printf("\x1b[31mError:\x1b[37m Already exists a directory with that name!\n");
                }
            }

        } else {
            printf("\x1b[31mError:\x1b[37m No name of directory provided!\n");
        }

        free(dirs);
        free(file);
    }

}

/**
 * @brief This function sends the user to a path given.
 * 
 * @param path String with a path.
 * @param currDir Pointer to the current directory.
 * @param root Pointer to root directory.
 * @return void
 */
void cdFunction(char *path, nodeStruct **currDir, nodeStruct *root){

    nodeStruct *pointerpath = *currDir;
    nodeStruct *pointerhead = root;

    if(pathParser(&pointerpath, &pointerhead, path)){
        *currDir = pointerpath;
    }
}

/**
 * @brief This function deletes a file in a directory given only if path is valid.
 * 
 * @param path String with a path.
 * @param currDir Pointer to the current directory.
 * @param root Pointer to root directory.
 * @return void
 */
void rmFunction(char *path, nodeStruct *currDir, nodeStruct *head){
    
    char *dirs;
    char *file;
    // Divide the path in directory paths and file name
    if(splitPath(path, &dirs, &file)){
        nodeStruct *cpy_pathpointer = currDir;
        nodeStruct *pointerhead = head;
        // Verifies that path is valid
        if(pathParser(&cpy_pathpointer, &pointerhead, dirs)){
            deleteFile(cpy_pathpointer, currDir, file, 'F');
        }
        free(dirs);
        free(file);
    }

}

/**
 * @brief This function deletes a directory in a directory given only if path is valid.
 * 
 * @param path String with a path.
 * @param currDir Pointer to the current directory.
 * @param root Pointer to root directory.
 * @return void
 */
void rmdirFunction(char *path, nodeStruct *currDir, nodeStruct *head){

    char *dirs;
    char *file;
    // Divide the path in directory paths and file name
    if(splitPath(path, &dirs, &file)){

        nodeStruct *cpy_pathpointer = currDir;
        nodeStruct *pointerhead = head;
        // Verifies that path is valid
        if(pathParser(&cpy_pathpointer, &pointerhead, dirs)){
            // Verify if current directory is target directory
            deleteFile(cpy_pathpointer, currDir, file, 'D');
        }
        free(dirs);
        free(file);
    }
}

/**
 * @brief This function generates the system file tree from a file given.
 * 
 * @param fileName Name of the file.
 * @param head Pointer to the roott directory.
 * @param pathpointer Pointer to current directory.
 * @return void
 */
bool generateSysFile(char *fileName, nodeStruct *head, nodeStruct **pathpointer){

    FILE *txt_file;
    // Opens file with read function
    txt_file = fopen(fileName, "r");

    // In case the file couldn't open
    if (txt_file == NULL) {
        printf("\x1b[31mError:\x1b[37m Couldn't open text file!\n");
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
            printf("\x1b[31mError: in line \x1b[37m%d\x1b[31m: \x1b[37mNot valid file type!\n", line_counter);
            return 1;
        }  

        // Verify that first directory is correct
        if(strcmp(path, "/") != 0 && line_counter == 1){
            printf("\x1b[31mError: in line \x1b[37m1\x1b[31m: \x1b[37m Not valid first directory!\n");
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
                printf("\x1b[31mError:\x1b[37m Not valid command, write help to see the list of commands available!\n");
                break;
        }
        free(arguments);
        free(command);

    }

	return 0;
}
