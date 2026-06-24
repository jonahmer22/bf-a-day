class Interpreter:
    def __init__(self, src, tape):
        self.src = src
        self.tape = tape

    def checkBraceBalance(self) -> int:
        head = 0
        opens = 0
        closes = 0
        while(head < len(self.src)):
            if(self.src[head] == '['):
                opens += 1
            elif(self.src[head] == ']'):
                closes += 1

            if((opens - closes) < 0):
                return -1

            head += 1

        return opens - closes

    def interpret(self):
        balance = self.checkBraceBalance()
        if(balance > 0):
            print("[ERROR]: Unbalanced '['.")
            exit(1)
        elif(balance < 0):
            print("[ERROR]: Unbalanced ']'.")
            exit(1)

        head = 0
        while(head >= 0 and head < len(self.src)):
            match self.src[head]:
                case '>':
                    self.tape.headInc()
                case '<':
                    self.tape.headDec()
                case '+':
                    self.tape.incAtHead()
                case '-':
                    self.tape.decAtHead()
                case '.':
                    print(chr(self.tape.getVal()), end="")
                case ',':
                    self.tape.setVal(ord(input()[0]))
                case '[':
                    if(self.tape.getVal() == 0):
                        depth = 0
                        while(head < len(self.src)):
                            if(self.src[head] == '['):
                                depth += 1
                            elif(self.src[head] == ']'):
                                depth -= 1
                            
                            if(depth == 0):
                                break

                            head += 1
                        
                        if(head >= len(self.src)):
                            print("[ERROR]: Unbounded loop.")
                            exit(1)
                case ']':
                    if(self.tape.getVal() != 0):
                        depth = 0
                        while(head >= 0):
                            if(self.src[head] == '['):
                                depth += 1
                            elif(self.src[head] == ']'):
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

class Tape:
    def __init__(self):
        self.tape = []
        self.head = 0
        self.size = 256

        for i in range(self.size):
            self.tape.append(0)
    
    def headInc(self):
        self.head += 1

        if(self.head >= self.size):
            oldSize = self.size
            self.size *= 2
            for i in range(oldSize):
                self.tape.append(0)
    
    def headDec(self):
        if(self.head == 0):
            return
        
        self.head -= 1

    def getVal(self):
        return self.tape[self.head]

    def setVal(self, value):
        self.tape[self.head] = value

    def incAtHead(self):
        self.tape[self.head] = (self.tape[self.head] + 1) % 256

    def decAtHead(self):
        self.tape[self.head] -= 1
        if(self.tape[self.head] < 0):
            self.tape[self.head] = 255