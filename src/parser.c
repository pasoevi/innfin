#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "actor.h"

#define MAX_LINE_LEN 80

/* read a line into s. return length */
int readline(char s[], int len)
{
    int c, i;
    for (i = 0; i < len-1 && (c = getchar()) != EOF && c != '\n'; ++i)
        s[i] = c;
    
    if (c == '\n') {
        s[i] = c;
        i++;
    }

    s[i] = '\0';
    return i;
}

int parse_jar(char *filename, void*(*listener)(void))
{
    FILE *file;
    file = fopen(filename, "r");
    int len = 0;
    char line[MAX_LINE_LEN];
    int n_read = readline(line, len);

    while (len = fgets(line, MAX_LINE_LEN, file)) {
        char key[MAX_LINE_LEN];
        char val[MAX_LINE_LEN];
        char colon;

        /* % separates separate items, monsters, spells, etc. */
        if (starts_with_c(line, '%'))
            printf("An object created\n");

        if (!starts_with_c(line, '#')) {
            sscanf(line, "%[^:] %c %s", key, &colon, val);
            printf("%s = %s\n", key, val);
        }
    }
    
    return 0;
}


/*
 * Find a blank space in s starting from start up to and including n
 * characters. The n parameter can be negative to indicate search
 * backwards.
 */
int find_blnk(char s[], int start, int n)
{
    int pos = -1;

    /* if searching backwards, move cursor to the last character before
       '\0' */
    if (n < 0){
        if (s[start] == '\0')
            --start;
        if (s[start] == '\n') 
            --start;
    } else {
        /* 
         * correct improper start, but leave it as it is if searching
         * backwards
         **/
        if (start < 0)
            start = 0;
    }

    while (s[start] != '\0' &&  s[start] != '\n' && s[start] != EOF) {
        if (start < 0)
            printf("searching in vain: %d\n", start);

        if (s[start] == ' ')
            pos = start;

        if (n > 0)
            ++start;
        else
            --start;
    }

    return pos;
}

/*
 * find the last non-blank character in s. return -1 if no
 * non-whitespace characters found
 */
int find_last_nonblnk(char s[]){
    int pos = -1; /* position of the last non-blank character */
    // int len = strlen(s); /* length of s */
    int len = 10;

    int i = len;
    while (s[i] != ' ') {
        pos = i;
        i--;
    }

    if (pos > 0){
        while (s[pos] == ' ')
            pos--;
    }

    return pos;
}

int starts_with_c(char s[], char ch)
{
    if (strlen(s) < 0)
        return 0;
    
    return s[0] == ch;
}

int read_key_val(char *str, char *key, char *val)
{
    int i;
    
}
