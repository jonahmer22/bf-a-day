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
    char *head = body;

    while(*head != '\0'){
        switch(*head){
            // instructions
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
                    // don't hard error, but just let the user know
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
                // depth match looping (that was my problem)
                if(data[data_ptr] == 0){
                    int depth = 1;
                    while(depth > 0 && *++head != '\0'){
                        if(*head == '[')
                            depth--;
                        else if(*head == ']')
                            depth++;
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
                        if(*head == '[')
                            depth--;
                        else if(*head == ']')
                            depth++;
                    }
                }

                if(head < body){
                    fprintf(stderr, "[FATAL 0x%04X]: Unmatched ']'.\r\n". 0x9343);
                    exit(EXIT_FAILURE);
                }

                break;
            }
            default:{
                break;
            }
        }

        head++;
    }
    #ifdef DEBUG
    fprintf(stderr, "data_ptr: %llu\ndata: ", (long long)data_ptr);
    for(size_t i = 0; i <= data_ptr; i++){
        fprintf(stderr, "%d ", data[i]);
    }
    puts("");
    #endif
}

int main(int argc, char **argv){
    // there is a file to open
    if(argc > 1){
        // open the file and run the interpreter over the contents
        return EXIT_SUCCESS;
    }

    data = realloc(data, data_size);

    // repl
    while(true){
        fprintf(stdout, "?> ");

        // get the input from repl
        char line[1024] = {0};
        fgets(line, 1024, stdin);

        if(memcmp(line, "bye", 3) == 0){
            break;
        }

        interpreter(line);
    }

    free(data);
    return EXIT_SUCCESS;
}
