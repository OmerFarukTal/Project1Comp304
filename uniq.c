#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define BUFFERSIZE 128

void printUniqueStringArray(char **strArr, int lenArr);
void printUniqueCountStringArray(char **strArr, int lenArr);
//int isInStringArray(char** strArr, int lenArr, char*str);
void printStringArray(char** strArr, int lenArr);
void testFunction();
void freeStringArray(char **strArr, int len);


int main(int argc, char **argv){
    

    if (fopen(argv[1], "r") != NULL) {
        // Read from argv
        //printf("Arg0 = %s\n, Arg1 = %s\n", argv[0], argv[1]);
        
        FILE *fp = fopen(argv[1], "r");

        if (fp == NULL) {
            perror("Unable to open file\n");
            exit(1);
        }
        
        char buffer[BUFFERSIZE];
        
        int ARRAYSIZE = 128;
        char **allStrings = (char**) malloc(sizeof(char*)*ARRAYSIZE);
        int countLines = 0;

        while(fgets(buffer, BUFFERSIZE, fp) != NULL) {
            //printf("%s", buffer);
            if (countLines < ARRAYSIZE -3) {
                allStrings[countLines] = strdup(buffer);            
            }
            else {
                allStrings = realloc(allStrings, sizeof(char*)*ARRAYSIZE*2);
                allStrings[countLines] = strdup(buffer);
                ARRAYSIZE = 2*ARRAYSIZE;
            }
            countLines++;
        }

        fclose(fp);
        
        if ((argc == 3) && (strcmp(argv[2], "--count") == 0 || strcmp(argv[2], "-c") == 0)) {
            printf("Printing Uniq count:\n");
            printUniqueCountStringArray(allStrings, countLines);
        }
        else {
            printf("Printing Uniq:\n");
            printUniqueStringArray(allStrings, countLines);

        }
        
        freeStringArray(allStrings, countLines);
    }
    else {
        // Read from stdin
        //printf("HELLOOOOOOOOOO\n");
        char* buffer = NULL; // hold string
        size_t len = 0; // size of buffer, will be updated in the getline if necessary
        ssize_t lineSize = 0; // the size of returned line: i.e "naber" -> 5
        
        int ARRAYSIZE = 3;
        char **allStrings = (char**) malloc(sizeof(char*)*ARRAYSIZE);
        int countLines = 0;

        lineSize = getline(&buffer, &len, stdin);
        while (lineSize > 1) {
            //printf("You entered %s, which has %ld chars\n", buffer,  lineSize - 1 ); // lineSize - 1 excludes '\n'

            ///
            if (countLines < ARRAYSIZE - 3) {
                allStrings[countLines] = strdup(buffer);
            }
            else {
                allStrings = realloc(allStrings, sizeof(char*)*ARRAYSIZE*2);
                allStrings[countLines] = strdup(buffer);
                ARRAYSIZE = 2*ARRAYSIZE;
            }
            countLines++;
            /// 

            lineSize = getline(&buffer, &len, stdin);
        }

        free(buffer);
         
        if ((argc == 2) && (strcmp(argv[1], "--count") == 0 || strcmp(argv[1], "-c") == 0)) {
            printf("Printing Uniq count:\n");
            printUniqueCountStringArray(allStrings, countLines);
        }
        else {
            printf("Printing Uniq:\n");
            printUniqueStringArray(allStrings, countLines);

        }
        
        freeStringArray(allStrings, countLines);

    }
}

void printUniqueCountStringArray(char **strArr, int lenArr) {
    
    char **uniqueArr = malloc(sizeof(char*)*lenArr);
    int* uniqCountArr = malloc(sizeof(int)*lenArr);
    memset(uniqCountArr, 0, sizeof(int)*lenArr);
    int newArrLen = 0;

    for (int i = 0; i < lenArr; i++) {
        if (i == 0) {
            uniqueArr[0] = strArr[0];
            uniqCountArr[0] = 1;
            newArrLen++;
        }
        else {
            if (strcmp(uniqueArr[newArrLen-1], strArr[i]) != 0) {
                uniqueArr[newArrLen] = strArr[i];
                uniqCountArr[newArrLen]++;    
                newArrLen++;
            }
            
            else {
                uniqCountArr[newArrLen-1]++;    
            }
        }
    }
     
    for (int i = 0; i < lenArr; i++) {
        if (uniqueArr[i] != NULL) {
            printf("\t%d %s", uniqCountArr[i], uniqueArr[i]);
        }
        else {
            break;
        }
    }

    free(uniqueArr);
    free(uniqCountArr);
}



void printUniqueStringArray(char **strArr, int lenArr) {
    
    char **uniqueArr = malloc(sizeof(char*)*lenArr);
    int newArrLen = 0;

    for (int i = 0; i < lenArr; i++) {
        if (i == 0) {
            uniqueArr[0] = strArr[0];
            newArrLen++;
        }
        else {
            if (strcmp(uniqueArr[newArrLen-1], strArr[i]) != 0) {
                uniqueArr[newArrLen] = strArr[i];
                newArrLen++;
            }
        }
    }
    
    printStringArray(uniqueArr, newArrLen);
    free(uniqueArr);
}


void printStringArray(char** strArr, int lenArr) {
    
    for (int i = 0; i < lenArr; i++) {
        if (strArr[i] != NULL) {
            printf("%s", strArr[i]);
        }
        else {
            break;
        }
    }

}


void freeStringArray(char **strArr, int len) {
    
    for(int i = 0; i < len; i++) {
        free(strArr[i]);
    }
    free(strArr);

}



void testFunction() {
    
    char *strArr1[4] = {"ali", "ali", "ali", "ali"};
    char *strArr2[4] = {"ali", "bli", "bli", "cli"};
    char *strArr3[4] = {"ali", "bli", "cli", "dli"};
    

    printUniqueCountStringArray(strArr1, 4);
    printUniqueCountStringArray(strArr2, 4);
    printUniqueCountStringArray(strArr3, 4);
    
}




