/*
 * Written by Lucas Donovan for CMPE 320 at Shippensburg University
 * Following wzip project https://github.com/remzi-arpacidusseau/ostep-projects/tree/master/initial-utilities/wzip
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LENGTH 2

// write to stdout
void out(int count, char c[LENGTH]) {
    // write count, 4 bytes, 1 item of data, to stdout
    fwrite(&count, 4, 1, stdout);
    // write c (current char), 1 byte, 1 item of data, to stdout
    fwrite(c, 1, 1, stdout);
}

int main(int argc, char *argv[]) {
    if (argc < 2) { // no file input
        fprintf(stdout, "wzip: file1 [file2 ...]\n");
        exit(1);
    }

    char c[LENGTH]      = "\0"; // current character
    char prev_c[LENGTH] = "\0"; // previous character
    int count = 0;         // # of times character occurs
    
    FILE *fp; 
    // loop through input files
    for (int i = 1; i < argc; i++) {
        char *filename = argv[i];
        // cannot open file
        if ((fp = fopen(filename, "r")) == NULL) {
            fprintf(stdout, "wzip: cannot open file\n");
            exit(1);
        }

        // read 1 char at a time from file, store 1 byte in c
        while (fread(&c, 1, 1, fp)) {
            if (strcmp(c, prev_c) == 0) { // same char
                count++;
            } else { // different char
                if (prev_c[0] != '\0') { 
                    out(count, c);
                }
                count = 1; 
                // replace previous char with current char
                strcpy(prev_c, c);
            }
        } 
        fclose(fp); 
    }

   out(count, c);
   return 0; 
}

