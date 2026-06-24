#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE_SIZE 1024

void interpret(char *src){

}

const char *read_file(const char *path){
    FILE *file = fopen(path, "r");
    if(file == NULL){
        fprintf(stderr, "[FATAL 0x%04X]:\tUnable to open file at path: %s", 0x5238, path);
        fflush(stderr);

        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    size_t end = ftell(file);
    fseek(file, 0, SEEK_SET);
    size_t start = ftell(file);

    size_t len = end - start;
    const char *buff = malloc(sizeof(char) * len);
    if(buff == NULL){
        fprintf(stderr, "[FATAL 0x%04X]:\tNot enough memory to load file of %zu characters.\r\n", 0x8134, len);
        fflush(stderr);

        exit(EXIT_FAILURE);
    }

    fread((void *)buff, sizeof(char), len, file);

    fclose(file);

    return buff;
}

int main(int argc, char **argv){
    fprintf(stdout, "BrainFuck Interpreter\n\nType 'bye' to exit.\r\n");
    fflush(stdout);

    if(argc > 1){
        const char *buff = read_file(argv[1]);

        interpret(buff);

        return EXIT_SUCCESS;
    }

    while(true){
        fprintf(stdout, "$ ");
        fflush(stdout);

        char line[MAX_LINE_SIZE] = {0};

        fgets(line, MAX_LINE_SIZE, stdin);

        if(memcmp(line, "bye", 3) == 0){
            break;
        }

        interpret(line);
    }

    return EXIT_SUCCESS;
}
