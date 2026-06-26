local data_tape = {
    head = 0,
    size = 256,
    tape = {}
}

function main()
    -- set up the initial section of the tape
    for i = 0, data_tape.size do
        data_tape.tape[i] = 0
    end

    if #arg > 0 then
        -- get a file and just run interpreter on it
        local file = assert(io.open(arg[1], "r"))
        local src = file:read("*all")
        file:close()

        interpreter(src)
    else
        -- otherwise fall through to repl
        repl()
    end
end

function check_brace_balance(src)
    local opens = 0
    local closes = 0

    local head = 0
    while head <= #src do
        if src:sub(head, head) == '[' then
            opens = opens + 1
        elseif src:sub(head, head) == ']' then
            closes = closes + 1
        end

        if closes > opens then
            print("[ERROR]: Unbalanced ']'.")
            os.exit(false, true)
        end

        head = head + 1
    end

    return opens - closes
end

function interpreter(src)
    -- make sure that [] are balanced, this has always technically been optional, but it's not hard or remotely complicated so I figure why not just keep doing it
    local balance = check_brace_balance(src)
    if balance > 0 then
        print("[ERROR]: Unbalanced '['.")
        os.exit(false, true)
    elseif balance < 0 then
        print("[ERROR]: Unbalanced ']'.")
        os.exit(false, true)
    end

    -- idk what the technical term for this would be but it's effectively a "goto" dispatch table
    local instructions = {
        ['>'] = function()
            data_tape.head = data_tape.head + 1
            if data_tape.head > data_tape.size then
                data_tape.size = data_tape.size * 2
                for i = data_tape.head, data_tape.size do
                    data_tape.tape[i] = 0
                end
            end
        end,
        ['<'] = function() 
            if data_tape.head > 0 then 
                data_tape.head = data_tape.head - 1 
            end 
        end,
        ['+'] = function()
            data_tape.tape[data_tape.head] = (data_tape.tape[data_tape.head] + 1) % 256
        end,
        ['-'] = function()
            if data_tape.tape[data_tape.head] > 0 then
                data_tape.tape[data_tape.head] = data_tape.tape[data_tape.head] - 1 
            else 
                data_tape.tape[data_tape.head] = 255 
            end 
        end,
        [','] = function() 
            data_tape.tape[data_tape.head] = string.byte(io.read("*line"))
        end,
        ['.'] = function()
            io.write(string.char(data_tape.tape[data_tape.head]))
        end,
        ['['] = function(head)
            if data_tape.tape[data_tape.head] == 0 then
                local depth = 0
                while head <= #src do
                    if src:sub(head, head) == '[' then
                        depth = depth + 1
                    elseif src:sub(head, head) == ']' then
                        depth = depth - 1
                    end

                    if depth == 0 then
                        break
                    end

                    head = head + 1
                end

                return head
            end
        end,
        [']'] = function(head)
            if data_tape.tape[data_tape.head] ~= 0 then
                local depth = 0
                while head >= 0 do
                    if src:sub(head, head) == '[' then
                        depth = depth + 1
                    elseif src:sub(head, head) == ']' then
                        depth = depth - 1
                    end

                    if depth == 0 then
                        break
                    end

                    head = head - 1
                end

                return head
            end
        end
    }

    local head = 0

    while head >= 0 and head <= #src do
        local handler = instructions[src:sub(head, head)]
        if handler then
            local temp = handler(head)
            if temp then
                head = temp
            end
        end

        head = head + 1
    end

end

function repl()
    print("BrainFuck Interpreter\n\nType 'bye' to exit.")
    while true do
        io.write("$ ")
        local line = io.read("*line")

        if line == "bye" then
            break
        end

        interpreter(line)
    end

    os.exit(true, true)
end

main()