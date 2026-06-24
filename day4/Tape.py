class Tape:
    def __init__(self):
        self.tape = []
        self.head = 0
        self.size = 256

        for i in range(self.size):
            self.tape.append(0)
    
    def headInc(self):
        self.head += 1

        if(self.head > self.size):
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