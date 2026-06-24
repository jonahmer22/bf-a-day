import sys
import Tape

def checkBraceBalance(src) -> int:
    head = 0
    opens = 0
    closes = 0
    while(head < len(src)):
        if(src[head] == '['):
            opens += 1
        elif(src[head] == ']'):
            closes += 1

        if((opens - closes) < 0):
            return -1

        head += 1

    return opens - closes

def interpret(src, tape):
    balance = checkBraceBalance(src)
    if(balance > 0):
        print("[ERROR]: Unbalanced '['.")
        exit(1)
    elif(balance < 0):
        print("[ERROR]: Unbalanced ']'.")
        exit(1)

    head = 0
    while(head >= 0 and head < len(src)):
        match src[head]:
            case '>':
                tape.headInc()
            case '<':
                tape.headDec()
            case '+':
                tape.incAtHead()
            case '-':
                tape.decAtHead()
            case '.':
                print(chr(tape.getVal()), end="")
            case ',':
                tape.setVal(ord(input()[0]))
            case '[':
                if(tape.getVal() == 0):
                    depth = 0
                    while(head < len(src)):
                        if(src[head] == '['):
                            depth += 1
                        elif(src[head] == ']'):
                            depth -= 1
                        
                        if(depth == 0):
                            break

                        head += 1
                    
                    if(head >= len(src)):
                        print("[ERROR]: Unbounded loop.")
                        exit(1)
            case ']':
                if(tape.getVal() != 0):
                    depth = 0
                    while(head >= 0):
                        if(src[head] == '['):
                            depth += 1
                        elif(src[head] == ']'):
                            depth -= 1

                        if(depth == 0):
                            break

                        head -= 1
                    
                    if(head < 0):
                        print("[ERROR]: Unbounded loop.")
                        exit(1)
            case _:
                pass
        
        head += 1
                

def main():
    tape = Tape.Tape()

    if(len(sys.argv) > 1):
        file = open(sys.argv[1], "r")
        src = file.read()
        file.close()

        interpret(src, tape)

        return

    print("BrainFuck Interpreter")
    print()
    print("Type 'bye' to exit.")
    while(True):
        line = input("$ ")

        if(line == "bye"):
            break

        interpret(line, tape)

    return

if __name__ == "__main__":
    main()