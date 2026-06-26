require "./State.rb"

def main()
    interpreter = Interpreter.new

    args = ARGV
    if(args.length > 0)
        src = File.read(args[0])

        interpreter.interpret(src)
        return
    end

    interpreter.repl()
end

if __FILE__ == $0
    main()
end