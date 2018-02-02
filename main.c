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
        if(strcmp(thisChild->name, name) == 0){
            printf("Could not make %s in %s. %s already exists in directory\n", name, parent->name, name);
            return 0;
        }

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

void mkdir(){
    splitLast();
    NODE *workingNode = getNode(dirname, cwd);
    if(workingNode) {
        if (workingNode->type == 'D') {
            makeChild(workingNode, basename, 'D');
        } else {
            printf("Could not insert folder to %s, it is a file\n", workingNode->name);
        }
    }
}

int isEmpty(NODE *node){
    return node->child == 0;
}

void rmdir(){
    NODE *toRemove = getNode(pathname, cwd);
    if(toRemove){
        if(toRemove->type == 'D'){
            rmdir(toRemove);
        } else {
            printf("Could not remove %s, it is NOT a directory\n", toRemove->name);
        }
    }
}

void creat(){
    splitLast();
    NODE *workingNode = getNode(dirname, cwd);
    if(workingNode) {
        if (workingNode->type == 'D') {
            makeChild(workingNode, basename, 'F');
        } else {
            printf("Could not insert file to %s, it is a file\n", workingNode->name);
        }
    }
}

NODE* getOlderSibling(NODE *youngerNode){
    NODE *curNode = youngerNode->parent->child;
    NODE *lastNode = 0;

    while(curNode && curNode != youngerNode){
        lastNode = curNode;
        curNode = curNode->sibling;

    }

    return lastNode;
}

void removeNode(NODE *toRemove){
    if(strcmp(toRemove->name, "/") != 0) {
        if(toRemove != cwd) {
            NODE *olderSibling = getOlderSibling(toRemove);
            if (olderSibling) {
                olderSibling->sibling = toRemove->sibling;
                free(toRemove);
            } else {
                toRemove->parent->child = toRemove->sibling;
            }
        } else {
            printf("Cannot remove %s, it is the current working directory", toRemove);
        }
    } else {
        printf("Unable to remove root");
    }
}

void rm(){
    NODE *toRemove = getNode(pathname, cwd);
    if(toRemove){
        if(toRemove->type == 'F'){
            removeNode(toRemove);
        } else {
            printf("Could not remove %s, it is a Directory", toRemove->name);
        }
    }
}

void cd(){
    if(pathname[0] == '\0'){
        cwd = root;
    } else {
        NODE *newNode = getNode(pathname, cwd);
        if (newNode) {
            cwd = newNode;
        }
    }
}

void printNode(NODE *node){
    printf("%c %s\n", node->type, node->name);
}

void ls(){
    if(pathname[0] == '\0'){
        NODE *cur = cwd->child;
        while(cur){
            printNode(cur);
            cur = cur->sibling;
        }
    } else {
        NODE *cur = getNode(pathname, cwd);

        if(cur) {
            cur = cur->child;
            while(cur){
                printNode(cur);
                cur = cur->sibling;
            }
        }

    }
}

void pwd(){

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
            mkdir();
        } else if(strcmp(command, "cd") == 0){
            cd();
        } else if(strcmp(command, "ls") == 0){
            ls();
        } else if(strcmp(command, "creat") == 0){
            creat();
        } else if(strcmp(command, "rm") == 0){
            rm();
        }

        command[0] = '\0';
        pathname[0] = '\0';

    }while(strcmp(command, "exit"));


    return 0;
}