#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE_SIZE 1024
#define MIN_TAPE_SIZE 256

uint8_t *tape = NULL;
size_t tape_head = 0;
size_t tape_size = MIN_TAPE_SIZE;

int check_balanced_loops(char *src){
    size_t opens = 0;
    size_t closes = 0;

    for(char *temp = src; *temp != '\0'; temp++){
        if(*temp == '[')
            opens++;
        if(*temp == ']')
            closes++;

        if(closes > opens)
            return -1;
    }

    return opens - closes;
}

void interpret(char *src){
    int balance = check_balanced_loops(src);
    if(balance > 0){
        fprintf(stderr, "[ERROR 0x%04X]:\tUnmatched '['.\r\n", 0x0935);
        fflush(stderr);

        exit(EXIT_FAILURE);
    }
    else if(balance < 0){
        fprintf(stderr, "[ERROR 0x%04X]:\tUnmatched ']'.\r\n", 0x4899);
        fflush(stderr);

        exit(EXIT_FAILURE);
    }

    char *head = src;
    while(*head != '\0' && head >= src){
        switch(*head){
            case '>':{
                tape_head++;
                break;
            }
            case '<':{
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
                tape[tape_head] = fgetc(stdin);
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

                    if(*head == '\0'){
                        fprintf(stderr, "[ERROR 0x%04X]:\tUnterminated loop somewhere.\r\n", 0x6293);
                        fflush(stderr);

                        exit(EXIT_FAILURE);
                    }
                }
                break;
            }
            case ']':{
                if(tape[tape_head] != 0){
                    int depth = 0;
                    while(head >= src){
                        if(*head == '[')
                            depth++;
                        if(*head == ']')
                            depth--;

                        if(depth == 0)
                            break;

                        head--;
                    }

                    if(head < src){
                        fprintf(stderr, "[ERROR 0x%04X]:\tUnterminated loop somewhere.\r\n", 0x9752);
                        fflush(stderr);

                        exit(EXIT_FAILURE);
                    }
                }
                break;
            }
            default:{
                // just do nothing by default
                break;
            }
        }

        if(tape_head > tape_size){
            size_t old_size = tape_size;
            tape_size *= 2;

            uint8_t *temp = realloc(tape, tape_size);
            if(temp == NULL){
                fprintf(stderr, "[FATAL 0x%04X]:\tNot enough memory to expand tape from %zu -> %zu.\r\n", 0x4235, old_size, tape_size);
                fflush(stderr);

                exit(EXIT_FAILURE);
            }
            memset(&temp[old_size], 0, old_size);

            tape = temp;
        }

        head++;
    }
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
    tape = calloc(tape_size, sizeof(uint8_t));
    if(tape == NULL){
        fprintf(stderr, "[FATAL 0x%04X]:\tHow do you not have 256 bytes free?????\r\n", 0x0001);
        fflush(stderr);

        return EXIT_FAILURE;
    }
    
    if(argc > 1){
        const char *buff = read_file(argv[1]);
        
        interpret(buff);
        
        return EXIT_SUCCESS;
    }
    
    fprintf(stdout, "BrainFuck Interpreter\n\nType 'bye' to exit.\r\n");
    fflush(stdout);
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
