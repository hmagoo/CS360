//
// Created by whomagoo on 1/25/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct NODE{
    char name[64];
    char type;
    struct NODE *child, *sibling, *parent;
} NODE;

NODE *root, *cwd;
char line[128]; //User input line
char command[16], pathname[64]; //user inputs
char dirname[64], basename[64]; //string holders

int splitLast(){
    if(strcmp(pathname, "\0") != 0) {
        char *result, *previousResult;
        strcpy(dirname, pathname);
        result = strtok(pathname, "/");

        while (result != NULL) {
            previousResult = result;
            result = strtok(NULL, "/");
        }

        if (previousResult == pathname) {
            dirname[0] = '\0';
        } else {
            //trim trailing '/'
            dirname[(previousResult - pathname - 1) / sizeof(char)] = '\0';

            //re-add the '/' if that was the only directory specified
            if(strcmp(dirname, "\0") == 0){
                dirname[0] = '/';
                dirname[1] = '\0';
            }
        }

        strcpy(basename, previousResult);

        printf("dirname: %s, basename:%s\n", dirname, basename);
    } else {
        printf("No parameters supplied for your call\n");
    }
}

NODE* makeChild(NODE *parent, char *name, char type){

    NODE *thisChild = parent->child, *lastChild = parent->child;

    while(thisChild){
        lastChild = thisChild;
        thisChild = thisChild->sibling;
    }

    NODE* newNode = (NODE *) malloc(sizeof(NODE));
    strcpy(newNode->name, name);
    newNode->type = type;
    newNode->parent = parent;
    newNode->child = 0;
    newNode->sibling = 0;

    if(lastChild) {
        lastChild->sibling = newNode;
    } else {
        parent->child = newNode;
    }

    return newNode;

}

NODE* getSibling(NODE *child, char *name){
    if(child) {
        if (strcmp(child->name, name) == 0) {
            return child;
        }

        return getSibling(child->sibling, name);
    } else {
        return 0;
    }
}

NODE* getNode(char nodeLocation[64], NODE* startingLocation){
    if(nodeLocation[0] == '/') {
        startingLocation = root;
        nodeLocation++;
    }

    char *curDirectory = strtok(nodeLocation, "/");

    while(curDirectory) {
        if (strcmp(curDirectory, "..") == 0) {
            startingLocation = startingLocation->parent;
        } else {
            NODE* tmp = getSibling(startingLocation->child, curDirectory);
            if(tmp == 0){
                printf("Search for directory %s in %s failed\n", curDirectory, startingLocation->name);
                return 0;
            } else {
                startingLocation = tmp;
            }
        }

        curDirectory = strtok(NULL, "/");
    }

    return startingLocation;
}

int mkdir(){
    splitLast();
    NODE *workingNode = getNode(dirname, cwd);
    if(workingNode && workingNode->type == 'D'){
        makeChild(workingNode, basename,'D');
    }


}

int cd(){
    NODE* newNode = getNode(pathname, cwd);
    if(newNode){
        cwd = newNode;
    }
}

int main(){

    root = (NODE *) malloc(sizeof(NODE));
    strcpy(root->name, "/");
    root->type = 'D';
    root->parent = root;
    root->sibling = root;
    root->child = 0;

    cwd = root;

    strcpy(pathname, "\0");


    do{
        printf("Enter a command: ");
        fgets(line, 128, stdin);
        sscanf(line, "%s%s", command, pathname);

        if(strcmp(command, "mkdir") == 0){
            mkdir(pathname);
        } else if(strcmp(command, "cd") == 0){
            cd(pathname);
        }

        command[0] = '\0';
        pathname[0] = '\0';

    }while(strcmp(command, "exit"));


    return 0;
}