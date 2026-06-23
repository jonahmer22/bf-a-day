#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

// #define DEBUG

#define BASE_DATA_SIZE 1024

// globals
uintptr_t data_ptr = 0;
uint8_t *data = NULL;
size_t data_size = BASE_DATA_SIZE;

void interpreter(const char *body){
    // pre validate bracket matching before executing anything
    int depth = 0;
    for(const char *p = body; *p != '\0'; p++){
        if(*p == '[') depth++;
        else if(*p == ']'){
            if(--depth < 0){
                fprintf(stderr, "[FATAL 0x%04X]: Unmatched ']'.\r\n", 0x9854);
                return;
            }
        }
    }
    if(depth != 0){
        fprintf(stderr, "[FATAL 0x%04X]: Unmatched '['.\r\n", 0x4089);
        return;
    }

    char *head = (char *)body;

    while(*head != '\0'){
        switch(*head){
            case '>':{
                data_ptr++;
                if(data_ptr >= data_size){
                    data_size *= 2;
                    data = realloc(data, data_size);
                }
                break;
            }
            case '<':{
                if(data_ptr == 0){
                    fprintf(stderr, "[ERROR 0x%04X]: Undefined behavior decrement past 0 of data pointer.\r\n", 0x4032);
                    break;
                }
                data_ptr--;
                break;
            }
            case '+':{
                data[data_ptr]++;
                break;
            }
            case '-':{
                data[data_ptr]--;
                break;
            }
            case '.':{
                fputc(data[data_ptr], stdout);
                break;
            }
            case ',':{
                data[data_ptr] = fgetc(stdin);
                break;
            }
            case '[':{
                if(data[data_ptr] == 0){
                    int depth = 1;
                    while(depth > 0 && *++head != '\0'){
                        if(*head == '[') depth++;
                        else if(*head == ']') depth--;
                    }
                    if(depth != 0){
                        fprintf(stderr, "[FATAL 0x%04X]: Unmatched '['.\r\n", 0x7043);
                        exit(EXIT_FAILURE);
                    }
                }
                break;
            }
            case ']':{
                if(data[data_ptr] != 0){
                    int depth = 1;
                    while(depth > 0 && --head >= body){
                        if(*head == ']') depth++;
                        else if(*head == '[') depth--;
                    }
                }
                if(head < body){
                    fprintf(stderr, "[FATAL 0x%04X]: Unmatched ']'.\r\n", 0x9343);
                    exit(EXIT_FAILURE);
                }
                break;
            }
            default:{
                break;
            }
        }

#ifdef DEBUG
        fprintf(stderr, "data_ptr: %llu\ndata: ", (unsigned long long)data_ptr);
        for(size_t i = 0; i <= data_ptr; i++){
            fprintf(stderr, "%d ", data[i]);
        }
        puts("");
#endif

        head++;
    }
}

static char *read_file(const char *path){
    FILE *f = fopen(path, "r");
    if(!f){
        fprintf(stderr, "[FATAL]: Cannot open file '%s'.\r\n", path);
        exit(EXIT_FAILURE);
    }

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    rewind(f);

    char *buf = malloc(len + 1);
    fread(buf, 1, len, f);
    buf[len] = '\0';

    fclose(f);

    return buf;
}

int main(int argc, char **argv){
    data = calloc(BASE_DATA_SIZE, 1);

    if(argc > 1){
        char *src = read_file(argv[1]);

        interpreter(src);

        free(src);
        free(data);

        return EXIT_SUCCESS;
    }

    // repl
    while(true){
        fprintf(stdout, "?> ");
        fflush(stdout);

        char line[1024] = {0};
        if(!fgets(line, 1024, stdin)) break;

        if(memcmp(line, "bye", 3) == 0) break;

        interpreter(line);
    }

    free(data);

    return EXIT_SUCCESS;
}
