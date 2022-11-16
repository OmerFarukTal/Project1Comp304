#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define ANSI_COLOR_MAGENTA "\033[1;33m"
#define ANSI_COLOR_RED "\033[0;31m"
#define ANSI_COLOR_BLACK "\033[0;30m"
#define ANSI_COLOR_GREEN "\033[0;32m"
#define ANSI_COLOR_YELLOW "\033[0;33m"
#define ANSI_COLOR_BLUE "\033[0;34m"
#define ANSI_COLOR_PURPLE "\033[0;35m"
#define ANSI_COLOR_CYAN "\033[0;36m"
#define ANSI_COLOR_RESET   "\033[0m"

void makeTime (char (*dest)[100] ,char *time, int index  ) {

    
    char *str0[9] = {      "-------",
                           "-XXXXX-",
                           "-X---X-",
                           "-X---X-",
                           "-X---X-",
                           "-X---X-",
                           "-X---X-",
                           "-XXXXX-",
                           "-------"
                            };
    char *str1[9] = {      "-------",
                           "-----X-",
                           "-----X-",
                           "-----X-",
                           "-----X-",
                           "-----X-",
                           "-----X-",
                           "-----X-",
                           "-------"
                           };
    char *str2[9] = {      "-------",
                           "-XXXXX-",
                           "-----X-",
                           "-----X-",
                           "-XXXXX-",
                           "-X-----",
                           "-X-----",
                           "-XXXXX-",
                           "-------"
                           };
    char *str3[9] = {      "-------",
                           "-XXXXX-",
                           "-----X-",
                           "-----X-",
                           "-XXXXX-",
                           "-----X-",
                           "-----X-",
                           "-XXXXX-",
                           "-------"
                           };
    char *str4[9] = {      "-------",
                           "-X---X-",
                           "-X---X-",
                           "-X---X-",
                           "-XXXXX-",
                           "-----X-",
                           "-----X-",
                           "-----X-",
                           "-------"
                           };
    char *str5[9] = {      "-------",
                           "-XXXXX-",
                           "-X-----",
                           "-X-----",
                           "-XXXXX-",
                           "-----X-",
                           "-----X-",
                           "-XXXXX-",
                           "-------"
                           };
    char *str6[9] = {      "-------",
                           "-XXXXX-",
                           "-X-----",
                           "-X-----",
                           "-XXXXX-",
                           "-X---X-",
                           "-X---X-",
                           "-XXXXX-",
                           "-------"
                           };
    char *str7[9] = {      "-------",
                           "-XXXXX-",
                           "-----X-",
                           "-----X-",
                           "-----X-",
                           "-----X-",
                           "-----X-",
                           "-----X-",
                           "-------"
                           };
    char *str8[9] = {      "-------",
                           "-XXXXX-",
                           "-X---X-",
                           "-X---X-",
                           "-XXXXX-",
                           "-X---X-",
                           "-X---X-",
                           "-XXXXX-",
                           "-------"
                           };
    char *str9[9] = {      "-------",
                           "-XXXXX-",
                           "-X---X-",
                           "-X---X-",
                           "-XXXXX-",
                           "-----X-",
                           "-----X-",
                           "-XXXXX-",
                           "-------"
                           };
    
    if ( *(time + index) -48 == 0 ) { 
        for (int i = 0 ; i < 9 ; i++) {
            
            strcat(dest[i], str0[i]);
        }
    }
    else if ( *(time + index) -48 == 1 ) { 
        for (int i = 0 ; i < 9 ; i++) {
            
            strcat(dest[i], str1[i]);
        }
    } 
    else if ( *(time + index) -48 == 2 ) { 
        for (int i = 0 ; i < 9 ; i++) {
            
            strcat(dest[i], str2[i]);
        }
    } 
    else if ( *(time + index) -48 == 3 ) { 
        for (int i = 0 ; i < 9 ; i++) {
            
            strcat(dest[i], str3[i]);
        }
    } 
    else if ( *(time + index) -48 == 4 ) { 
        for (int i = 0 ; i < 9 ; i++) {
            
            strcat(dest[i], str4[i]);
        }
    } 
    else if ( *(time + index) -48 == 5 ) { 
        for (int i = 0 ; i < 9 ; i++) {
            
            strcat(dest[i], str5[i]);
        }
    } 
    else if ( *(time + index) -48 == 6 ) { 
        for (int i = 0 ; i < 9 ; i++) {
            
            strcat(dest[i], str6[i]);
        }
    } 
    else if ( *(time + index) -48 == 7 ) { 
        for (int i = 0 ; i < 9 ; i++) {
            
            strcat(dest[i], str7[i]);
        }
    } 
    else if ( *(time + index) -48 == 8 ) { 
        for (int i = 0 ; i < 9 ; i++) {
            
            strcat(dest[i], str8[i]);
        }
    } 
    else if ( *(time + index) -48 == 9 ) { 
        for (int i = 0 ; i < 9 ; i++) {
            
            strcat(dest[i], str9[i]);
        }
    }
    else  {
        for (int i = 0; i < 9; i++) {
            
            if ( i == 3 || i == 5) strcat(dest[i], "X");
            else strcat(dest[i], "-");
        }

    }

}


void main(int argc, char *argv[]) {
    
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    
    char *str = asctime(timeinfo);


    char strFinal[9][100] = {  "\0",
                           "\0",
                           "\0",
                           "\0",
                           "\0",
                           "\0",
                           "\0",
                           "\0",
                           "\0"
                            };


    char *strPad[9] = {    "----",
                           "----",
                           "----",
                           "----",
                           "----",
                           "----",
                           "----",
                           "----",
                           "----"
                           };

    for (int i = 0 ; i < 9 ; i++) {
        
        strcat(strFinal[i], strPad[i]);
    }
    


    makeTime(strFinal, str, 11);
    makeTime(strFinal, str, 12);
    makeTime(strFinal, str, 13);
    makeTime(strFinal, str, 14);
    makeTime(strFinal, str, 15);
    char *strPad2[9] = {    "-------",
                           "-------",
                           "-------",
                           "-------",
                           "-------",
                           "-------",
                           "-------",
                           "-------",
                           "-------"
                           };

    for (int i = 0 ; i < 9 ; i++) {
        
        strcat(strFinal[i], strPad2[i]);
    }
    
    if(strcmp(argv[1],"mag") == 0 ) {
        printf(ANSI_COLOR_MAGENTA);
    }
    else if (strcmp(argv[1], "bla") == 0) {
        printf(ANSI_COLOR_BLACK);
    }
    else if (strcmp(argv[1], "red") == 0) {
        printf(ANSI_COLOR_RED);
    }
    else if (strcmp(argv[1], "yel") == 0) {
        printf(ANSI_COLOR_YELLOW);
    }
    else if (strcmp(argv[1], "blu") == 0) {
        printf(ANSI_COLOR_BLUE);
    }
    else if (strcmp(argv[1], "pur") == 0) {
        printf(ANSI_COLOR_PURPLE);
    }
    else {
    }
    
    for (int i = 0;  i<9 ; i++) {
        printf("%s\n", strFinal[i] );
    }

    system("fortune");
}



