import fs from 'fs';

let tape_head = 0
let tape_size = 256
let tape = []

function fgets(maxlen = 1024){
    const buf = Buffer.alloc(1);
    let result = '';

    for(let i = 0; i < maxlen - 1; i++){
        let n;
        try{
            n = fs.readSync(0, buf, 0, 1);
        }
        catch{
            break;
        }
        if(n === 0)
            break;  // EOF
        result += buf.toString();
        if(buf[0] === 0x0a)
            break;  // '\n'
    }

    return result.length ? result : null;  // null on EOF, like C
}

function fread(path) {
    try { return fs.readFileSync(path, 'utf8'); }
    catch { return null; }
}

function print(str){
    process.stdout.write(str)
}

function check_brace_balance(src){
    let balance = 0;

    let head = 0;
    while(head < src.length){
        if(src[head] == '[')
            balance++
        else if(src[head] == ']')
            balance--

        if(balance < 0)
            break

        head += 1
    }

    return balance
}

function interpreter(src){
    let balance = check_brace_balance(src)
    if(balance < 0){
        print("[ERROR]: Unbalanced ']'.")
        process.exit(1)
    }
    else if(balance > 0){
        print("[ERROR]: Unbalanced '['.")
        process.exit(1)
    }

    let head = 0;
    while(head < src.length){
        switch(src[head]){
            case '>':{
                tape_head++;
                if(tape_head >= tape_size){
                    tape_size *= 2
                    for(let i = tape_size / 2; i < tape_size; i++)
                        tape.push(0)
                }
                break;
            }
            case '<':{
                if(tape_head == 0)
                    break
                tape_head--
                break;
            }
            case '+':{
                tape[tape_head] = (tape[tape_head] + 1) % 256
                break
            }
            case '-':{
                if(tape[tape_head] == 0){
                    tape[tape_head] = 255
                    break
                }

                tape[tape_head]--
                break
            }
            case '.':{
                print(String.fromCharCode(tape[tape_head]))
                break
            }
            case ',':{
                tape[tape_head] = fgets().charCodeAt(0)
                break
            }
            case '[':{
                if(tape[tape_head] == 0){
                    let depth = 0
                    while(head < src.length){
                        if(src[head] == '[')
                            depth++
                        else if(src[head] == ']')
                            depth--

                        if(depth == 0)
                            break

                        head++
                    }
                }
                break
            }
            case ']':{
                if(tape[tape_head] != 0){
                    let depth = 0
                    while(head >= 0){
                        if(src[head] == '[')
                            depth++
                        else if(src[head] == ']')
                            depth--

                        if(depth == 0)
                            break

                        head--
                    }
                }
                break
            }
        }
        
        head++;
    }
}

function main(){
    const args = process.argv
    if(args.length > 1){
        let src = fread(args[2])

        // print(src)

        interpreter(src)

        return
    }

    // init the tape
    for(let i = 0; i < tape_size; i++)
        tape.push(0)

    // repl
    print("BrainFuck Interpreter\n\nType 'bye' to exit.\n");
    while(true){
        print("$ ")
        let line = fgets();


        interpreter(line)
    }
}

main()