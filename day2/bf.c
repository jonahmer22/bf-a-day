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

    for(;*head != '\0'; head++){
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
                fputc(tape[tape_head], stdout);
                fflush(stdout);
                break;
            }
            case ',':{
                tape[tape_head] = fgetc(stdin);
                break;
            }
            case '[':{
                break;
            }
            case ']':{
                break;
            }
            default:{
                // by default just do nothing for unknown stuff, allows for comments...
                break;
            }
        }

        if(tape_head > tape_size){
            tape_size *= 2;
            uint8_t *temp = realloc(tape, tape_size);
            if(temp == NULL){
                fprintf(stderr, "[FATAL 0x%04X]:\tNot enough memory for tape of size: %zu.\r\n", 0x4135, (tape_size / 2));
                fflush(stderr);
                exit(EXIT_FAILURE);
            }
            tape = temp;
        }

        head++;
    }
}

int main(int argc, char **argv){
    tape = malloc(sizeof(uint8_t) * tape_size);
    if(tape == NULL){
        fprintf(stderr, "[FATAL 0x%04X]:\tHow do you not have 256 bytes of ram free???.\r\n", 0x1001);
        fflush(stderr);
        return EXIT_FAILURE;
    }

    // repl
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
