#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#define TAPE_BASE 256
#define LINE_SIZE 1024

uint8_t *tape = NULL;
size_t tape_head = 0;
size_t tape_size = TAPE_BASE;

void interpret(char *body){
    char *head = body;

    for(char *temp = head; *temp != '\0'; temp++){
        // TODO: do loop brace evenness checking
    }

    while(*head != '\0' && head >= body){
        switch(*head){
            case '>':{
                tape_head++;
                break;
            }
            case '<':{
                // prevent underflow
                if(tape_head == 0)
                    break;

                tape_head--;
                break;
            }
            case '+':{
                tape[tape_head]++;
                break;
            }
            case '-':{
                tape[tape_head]--;
                break;
            }
            case '.':{
                fputc((int)tape[tape_head], stdout);
                fflush(stdout);
                break;
            }
            case ',':{
                tape[tape_head] = (uint8_t)fgetc(stdin);
                break;
            }
            case '[':{
                if(tape[tape_head] == 0){
                    int depth = 0;
                    while(*head != '\0'){
                        if(*head == '[')
                            depth++;
                        if(*head == ']')
                            depth--;

                        if(depth == 0)
                            break;
                        
                        head++;
                    }
                }
                break;
            }
            case ']':{
                if(tape[tape_head] != 0){
                    int depth = 0;
                    while(head >= body){
                        if(*head == '[')
                            depth++;
                        if(*head == ']')
                            depth--;

                        if(depth == 0)
                            break;
                        
                        head--;
                    }
                }
                break;
            }
            default:{
                // by default just do nothing for unknown stuff, allows for comments...
                break;
            }
        }

        if(tape_head > tape_size){
            size_t old_size = tape_size;
            tape_size *= 2;
            
            uint8_t *temp = realloc(tape, tape_size);
            if(temp == NULL){
                fprintf(stderr, "[FATAL 0x%04X]:\tNot enough memory for tape of size: %zu.\r\n", 0x4135, (tape_size / 2));
                fflush(stderr);
                exit(EXIT_FAILURE);
            }
            memset(&temp[old_size], 0, old_size);

            tape = temp;
        }

        head++;
    }
}

char *read_file(const char *path){
    FILE *file = fopen(path, "r");
    if(file == NULL){
        fprintf(stderr, "[FATAL 0x%04X]:\tCould not open file at: %s.\r\n", 0x7564, path);
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    size_t end = ftell(file);
    fseek(file, 0, SEEK_SET);
    size_t start = ftell(file);

    size_t len = end - start;
    char *buff = malloc(sizeof(char) * len);

    fread(buff, sizeof(char), len, file);

    fclose(file);

    return buff;
}

int main(int argc, char **argv){
    tape = calloc(tape_size, sizeof(uint8_t));
    if(tape == NULL){
        fprintf(stderr, "[FATAL 0x%04X]:\tHow do you not have 256 bytes of ram free???.\r\n", 0x1001);
        fflush(stderr);
        return EXIT_FAILURE;
    }

    if(argc > 1){
        char *buff = read_file(argv[1]);

        interpret(buff);

        return EXIT_SUCCESS;
    }

    // repl
    fprintf(stdout, "Brain Fuck Interpreter\n\nType 'bye' to exit.\n");
    while(true){
        // print the prompt
        fprintf(stdout, "?> ");
        fflush(stdout);

        // get input
        char line[LINE_SIZE] = {0};
        fgets(line, LINE_SIZE, stdin);

        // check for exit
        if(memcmp(line, "bye", 3) == 0){
            break;
        }

        interpret(line);
    }

    return EXIT_SUCCESS;
}
