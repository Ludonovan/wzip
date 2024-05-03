#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define LENGTH        2
#define MAX_FILE_SIZE 4096
#define NUM_THREADS   3

typedef struct __arg {
    struct arg_val_t {
        char *src;         // start address of memory mapped file
        size_t fsize;      // size of the file
    } arg_val;             // information passed to the thread

    struct ret_val_t {
        int count;         // # of times a char is in a row
        char c[LENGTH];    // compressed chars
    } ret_val;             // information passed back to the main thread
} arg_t;


void *worker(void *args) {
    arg_t *arg = (arg_t *)args;

    char *src = arg->arg_val.src;
    size_t fsize = arg->arg_val.fsize;

    char prev_c[LENGTH] = "\0";
    int count = 0;

    for (size_t i = 0; i < fsize; i++) {
        if (memcmp(&src[i], prev_c, LENGTH) == 0) {
            count++;
        } else {
            if (prev_c[0] != '\0') {
                arg->ret_val.count = count;
                memcpy(arg->ret_val.c, prev_c, LENGTH);
                pthread_exit(NULL);
            }
            count = 1;
            memcpy(prev_c, &src[i], LENGTH);
        }
    }

    arg->ret_val.count = count;
    memcpy(arg->ret_val.c, prev_c, LENGTH);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stdout, "wzip: file1 [file2 ...]\n");
        exit(1);
    }

    for (int i = 1; i < argc; i++) {
        char *filename = argv[i];
        int fd;
        struct stat finfo;

        if ((fd = open(filename, O_RDONLY)) == -1) {
            fprintf(stdout, "wzip: cannot open file\n");
            exit(1);
        }

        if (fstat(fd, &finfo) == -1) {
            fprintf(stdout, "wzip: fstat error\n");
            exit(1);
        }
        off_t fsize = finfo.st_size;

        if (fsize > MAX_FILE_SIZE) {
            char *src = mmap(0, fsize, PROT_READ, MAP_PRIVATE, fd, 0);
            size_t part_size = fsize / NUM_THREADS;

            pthread_t threads[NUM_THREADS];
            arg_t args[NUM_THREADS];

            for (int i = 0; i < NUM_THREADS; i++) {
                arg->arg.src   = *src;
                arg->arg.fsize = fsize;
                pthread_create(&threads[i], NULL, worker, (void *)&args[i]);
            }

            for (int i = 0; i < NUM_THREADS; i++) {
                pthread_join(threads[i], NULL);
            }

            for (int i = 0; i < NUM_THREADS; i++) {
                fwrite(&args[i].ret_val.count, sizeof(int), 1, stdout);
                fwrite(args[i].ret_val.c, 1, LENGTH, stdout);
            }

            munmap(src, fsize);
        } else {
            FILE *file = fopen(filename, "r");
            char c[LENGTH] = "\0";
            char prev_c[LENGTH] = "\0";
            int count = 0;

            while (fread(&c, 1, 1, file)) {
                if (strcmp(c, prev_c) == 0) {
                    count++;
                } else {
                    if (prev_c[0] != '\0') {
                        fwrite(&count, sizeof(int), 1, stdout);
                        fwrite(prev_c, 1, LENGTH, stdout);
                    }
                    count = 1;
                    strcpy(prev_c, c);
                }
            }

            fclose(file);
        }

        close(fd);
    }

    return 0;
}

