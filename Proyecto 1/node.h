#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#ifndef NODE_H
#define NODE_H

typedef struct nodeStruct nodeStruct;


typedef struct cmdStruct cmdStruct;
struct cmdStruct{

    char name[6];
    char description[100];

};

nodeStruct* createNode();

void initNode(nodeStruct *node, nodeStruct *dir, char type);

char * getDateTime();

void strip(char *str);

void cleanSysFile( nodeStruct *pointer );

void pwdFunctionForTxt( nodeStruct *pointer, FILE *file);

void pwdFunction( nodeStruct *currDir, int counter);

void printSysFile( nodeStruct *pointer, FILE *file, int longestname );

bool searchFile( nodeStruct **pointer, char * dirName, char fileType);

bool findFile( nodeStruct *pointer, char * dirName, char fileType);

bool searchFileForDel( nodeStruct **pointer, nodeStruct **prevpointer, char * dirName, char fileType);

void helpFunction(cmdStruct * commands);

void printFiles(nodeStruct *pointer);

void printFileswithDate(nodeStruct *pointer, int longestFileName);

void wrtsFunction(nodeStruct *head, int longestname);

void createFile(nodeStruct *dir, char * nameFile, char type);

void deleteFile(nodeStruct *dir, nodeStruct *pathpointer, char *file, char type);

int map_to_int(char * command, char * arguments);

bool splitPath(char *path, char **dirs, char **file);

void inputParser(char *input, char **command, char **arguments);

bool pathParser(nodeStruct **actualpointer, nodeStruct **head, char *arguments);

int getLongestFileName(nodeStruct *pointer);

void lsFunction(char *arguments, nodeStruct *currDir, nodeStruct *head);

void touchFunction(char *path, nodeStruct *currDir, nodeStruct *root);

void mkdirFunction(char *path, nodeStruct *currDir, nodeStruct *root);

void cdFunction(char *path, nodeStruct **currDir, nodeStruct *root);

void rmFunction(char *path, nodeStruct *currDir, nodeStruct *head);

void rmdirFunction(char *path, nodeStruct *currDir, nodeStruct *head);

bool generateSysFile(char *fileName, nodeStruct *head, nodeStruct **pathpointer);

#endif // NODE_H