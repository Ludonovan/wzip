#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void out(int count, char c[2]) {
    fwrite(&count, 4, 1, stdout);
    fwrite(c, 1, 1, stdout);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stdout, "wzip: file1 [file2 ...]");
        exit(1);
    }

    char c[2]      = "\0"; // current character
    char prev_c[2] = "\0"; // previous character
    int count = 0;         // # of times character occurs
    
    FILE *fp;
    for (int i = 1; i < argc; i++) {
        char *filename = argv[i];
        if ((fp = fopen(filename, "r")) == NULL) {
            fprintf(stdout, "wzip: cannot open file");
            exit(1);
        }


        while (fread(&c, 1, 1, fp)) {
            if (strcmp(c, prev_c) == 0) {
                count++;
            } else {
                if (prev_c[0] != '\0') {
                    out(count, c);
                }
                count = 1; 
                strcpy(prev_c, c);
            }
        } 
        fclose(fp); 
    }

   out(count, c);
   return 0; 
}

