//
// Created by whomagoo on 1/25/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

typedef struct NODE{
    char name[64];
    char type;
    struct NODE *child, *sibling, *parent;
} NODE;

NODE *root, *cwd;
char line[128]; //User input line
char command[16], pathname[64]; //user inputs
char dirname[64], basename[64]; //string holders

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

NODE* getNode(char *nodeLocation, NODE* startingLocation){
    if(nodeLocation[0] == '/') {
        startingLocation = root;
        nodeLocation++;
    }

    char curDirectory[64];
    strcpy(curDirectory, nodeLocation);

    strtok(curDirectory, "/");


    if (strcmp(nodeLocation, curDirectory) != 0) {
        NODE* nextDirectory = getSibling(startingLocation->child, curDirectory);
        if(nextDirectory != 0) {
            return getNode(nodeLocation + strlen(curDirectory) + 1, nextDirectory);
        } else {
            printf("Search for directory %s in %s failed\n", curDirectory, startingLocation);
            return 0;
        }
    } else {
        strcpy(pathname, nodeLocation);
        return startingLocation;
    }

}

int mkdir(){
    NODE *workingNode = getNode(pathname, cwd);
    if(workingNode && workingNode->type == 'D'){
        makeChild(workingNode, pathname,'D');
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

    do{
        printf("Enter a command: ");
        fgets(line, 128, stdin);
        sscanf(line, "%s%s", command, pathname);

        if(strcmp(command, "mkdir") == 0){
            mkdir(pathname);
        }

        //getNode(command);

        command[0] = '\0';
        pathname[0] = '\0';

    }while(strcmp(command, "exit"));


    return 0;
}