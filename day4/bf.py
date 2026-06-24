import sys
import State

def main():
    tape = State.Tape()

    if(len(sys.argv) > 1):
        file = open(sys.argv[1], "r")
        src = file.read()
        file.close()

        interpreter = State.Interpreter(src, tape)
        interpreter.interpret()

        return

    print("BrainFuck Interpreter")
    print()
    print("Type 'bye' to exit.")
    while(True):
        line = input("$ ")

        if(line == "bye"):
            break

        interpreter = State.Interpreter(line, tape)
        interpreter.interpret()

    return

if __name__ == "__main__":
    main()