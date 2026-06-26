class Tape
    def initialize
        @head = 0
        @size = 256
        @tape = Array.new(@size, 0)
    end

    def get_val
        return @tape[@head]
    end

    def head_inc
        @head += 1

        if @head >= @tape.length
            old_size = @size
            for i in 0..@size
                @tape.push(0)
            end
            @size *= 2
        end
    end

    def head_dec
        if @head < 0
            return
        end

        @head -= 1
    end

    def inc_at_head
        @tape[@head] = (@tape[@head] + 1) % 256
    end

    def dec_at_head
        temp = @tape[@head] - 1
        if temp < 0
            @tape[@head] = 255
        else
            @tape[@head] -= 1
        end
    end

    def emit
        print(@tape[@head].chr)
    end

    def input
        @tape[@head] = gets.chars[0].ord
    end
end

class Interpreter
    def initialize
        @tape = Tape.new
    end

    def repl
        # repl
        puts "Brainfuck Interpreter\n\nType 'bye' to exit."
        while true
            print("$ ")
            STDOUT.flush

            line = STDIN.gets.chomp
            if line == "bye"
                exit(0)
            end

            interpret(line)
        end
    end

    def _check_balance(src)
        opens = 0
        closes = 0

        src_list = src.chars
        src_list.each do |head|
            case head
            when '['
                opens += 1
            when ']'
                closes += 1
            end

            if closes > opens
                break
            end
        end

        balance = opens - closes
        if balance > 0
            puts "[ERROR]: Unbalanced '['."
            exit(1)
        elsif balance < 0
            puts "[ERROR]: Unbalanced ']'."
            exit(1)
        end
    end

    def interpret(src)
        _check_balance(src)

        src_list = src.chars
        head = 0
        while head < src_list.length
            case src_list[head]
            when '>'
                @tape.head_inc
            when '<'
                @tape.head_dec
            when '+'
                @tape.inc_at_head
            when '-'
                @tape.dec_at_head
            when '.'
                @tape.emit
            when ','
                @tape.input
            when '['
                if @tape.get_val == 0
                    depth = 0
                    while head < src_list.length
                        if src_list[head] == '['
                            depth += 1
                        elsif src_list[head] == ']'
                            depth -= 1
                        end

                        if depth == 0
                            break
                        end

                        head += 1
                    end
                end
            when ']'
                if @tape.get_val != 0
                    depth = 0
                    while head > 0
                        if src_list[head] == '['
                            depth += 1
                        elsif src_list[head] == ']'
                            depth -= 1
                        end

                        if depth == 0
                            break
                        end

                        head -= 1
                    end
                end
            end

            head += 1
        end
    end
end