#include "code_gen.h"

void generate_wasm(vector<string> commands, string fname)
{
    vector<string> code = map_commands(commands);
    for(auto &thing : code) cout << thing << endl;

    // code is a vector of all commands

    // need to loop through it, find all the subroutines (after endprog (temp identifier))
    // and add them to the subroutines string. (make sure they are valid i.e they have a return at the end)

    // then go through the main program, check jumping, insert loops if needed, and validate
    // i.e jumping back to a label is a loop, and jumping to the next one is a br exiting a block.

    // check for an end after an end prog (will get added because of how im parsing the code)

    // after this, get going with the better error handling, and test suite, and proper documentation for this.
}

vector<string> map_commands(vector<string> tokens)
{
    bool first_token = true;
    vector<string> block_commands;

    for (auto &token : tokens)
    {
        vector<string> toks = split_token(token);

        if (first_token)
        {
            first_token = false;
            if (toks[0] != "mark") block_commands.push_back("block $mainStartBlockNobodyUseThisNameEver\n");
        }

        // Stack Manipulation
        if (toks[0] == "push") block_commands.push_back("i32.const " + toks[1] + "\ncall $push\n");
        else if (toks[0] == "duplicate") block_commands.push_back("call $duplicate\n");
        else if (toks[0] == "copy") block_commands.push_back("i32.const " + toks[1] + "\ncall $copy\n");
        else if (toks[0] == "swap") block_commands.push_back("call $swap\n");
        else if (toks[0] == "discard") block_commands.push_back("call $discard\n");
        else if (toks[0] == "slide") block_commands.push_back("i32.const " + toks[1] + "\ncall $slide\n");

        // Arithmetic
        else if (toks[0] == "add") block_commands.push_back("call $add\n");
        else if (toks[0] == "sub") block_commands.push_back("call $sub\n");
        else if (toks[0] == "mul") block_commands.push_back("call $mul\n");
        else if (toks[0] == "div") block_commands.push_back("call $div\n");
        else if (toks[0] == "mod") block_commands.push_back("call $mod\n");

        // Heap Access
        else if (toks[0] == "store") block_commands.push_back("call $store\n");
        else if (toks[0] == "retrieve") block_commands.push_back("call $load\n");

        // Flow Control
        else if (toks[0] == "mark")
        {
            block_commands.push_back("end\n");
            block_commands.push_back("block $" + toks[1] + "\n");
        }
        else if (toks[0] == "call") block_commands.push_back("call " + toks[1] + "\n");
        else if (toks[0] == "jump") block_commands.push_back("br $" + toks[1] + "\n");
        else if (toks[0] == "jumpz")
        {
            block_commands.push_back("call $pop\ni32.const 0\ni32.eq_s\n");
            block_commands.push_back("br_if $" + toks[1] +"\n");
        }
        else if (toks[0] == "jumpn")
        {
            block_commands.push_back("call $pop\ni32.const 0\ni32.lt_s\n");
            block_commands.push_back("br_if $" + toks[1] + "\n");
        }
        else if (toks[0] == "return") block_commands.push_back("return\n");   /// todo
        else if (toks[0] == "end") block_commands.push_back("endprog\n");   /// todo

        // I/O
        else if (toks[0] == "out_char") block_commands.push_back("call $write_char\n");
        else if (toks[0] == "out_num") block_commands.push_back("call $write_num\n");
        else if (toks[0] == "read_char") block_commands.push_back("call $read_char\n");
        else if (toks[0] == "read_num") block_commands.push_back("call $read_num\n");

    }
    return block_commands;
}

vector<string> split_token(string token)
{
    vector<string> parts;
    string part;
    stringstream ssin(token);
    while(ssin.good())
    {
        ssin >> part;
        parts.push_back(part);
    }
    return parts;
}



string gen_html(string fname)
{
    return R"(
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <title></title>
    </head>
    <body>
        <script src=)" + fname + R"(></script>
    </body>
    </html>)";
}

string gen_js(string fname)
{
    return R"(
    var importObject = {
        console:
                {   read_char: read_char_f,
                    read_num: read_num_f,
                    write_char: write_char_f,
                    write_num: write_num_f,
                }
    };

    function read_char_f() {
        var input = prompt("Enter a character:");
        var input = readline();
        while (input.length != 1) {
            input = prompt("Enter a SINGLE CHARACTER:");
        }
        return input.charCodeAt(0);
    }

    function read_num_f() {
        var input = Number(prompt("Enter an integer value:"));
        while ( !Number.isInteger(input) ) {
            input = Number(prompt("Enter an INTEGER value:"));
        }
        return input;
    }

    function write_char_f(param) {
        console.log( String.fromCharCode(param) );
    }

    function write_num_f(param) {
        console.log(param);
    }


    // loading wasm
    var main;
    function loadWebAssembly(fileName, imports) {
      return fetch(fileName)
        .then(response => response.arrayBuffer())
        .then(bits => WebAssembly.compile(bits))
        .then(module => { return new WebAssembly.Instance(module, imports) });
    };

    loadWebAssembly()" + fname + R"(, importObject)
      .then(instance => {
        main = instance.exports.main;
        console.log('Finished compiling! Type main() to run the program');
    });
    )";
}

string gen_wat(string program, string subroutines)
{
    return R"(
(module

    ;; io js functions
    (import "console" "read_char" (func $read_char_js (result i32)))
    (import "console" "read_num" (func $read_num_js (result i32)))
    (import "console" "write_char" (func $write_char (param i32)))
    (import "console" "write_num" (func $write_num (param i32)))


    (memory 2)

    (global $memory_page_size i32 i32.const 65636)
    (global $stack_heap_wall i32 i32.const 65536)
    (global $stack_pointer (mut i32) i32.const 65536)


    (func $check_heap_bounds (param $address i32) (result i32)
        get_local $address
        get_global $stack_heap_wall
        i32.ge_s
        if unreachable end
        get_local $address

    )

    (func $check_stack_bounds (param $address i32) (result i32)
        get_local $address
        get_global $stack_heap_wall
        i32.lt_s
        if unreachable end
        get_local $address
    )


    ;; Stack Manipulation
    (func $push (param $value i32)
        get_global $stack_pointer
        get_local $value
        i32.store

        get_global $stack_pointer
        i32.const 4
        i32.add
        set_global $stack_pointer
    )

    (func $pop (result i32)
        get_global $stack_pointer
        i32.const 4
        i32.sub
        set_global $stack_pointer

        get_global $stack_pointer
        call $check_stack_bounds

        i32.load
    )

    (func $duplicate
        get_global $stack_pointer
        i32.const 4
        i32.sub
        call $check_stack_bounds
        i32.load
        call $push
    )

    (func $copy (param $offset i32)
        get_global $stack_pointer
        get_local $offset
        i32.const 4
        i32.mul
        i32.sub
        i32.const 4
        i32.sub
        call $check_stack_bounds
        i32.load
        call $push

    )

    (func $swap (local $holder i32)
        call $pop
        set_local $holder
        call $pop
        get_local $holder
        call $push
        call $push
    )

    (func $discard
        call $pop
        drop
    )

    (func $slide (param $offset i32)
        call $pop

        get_global $stack_pointer
        get_local $offset
        i32.const 4
        i32.mul
        i32.sub
        call $check_stack_bounds
        set_global $stack_pointer
        call $push
    )


    ;; Arithmetic
    (func $add
        call $pop
        call $pop
        i32.add
        call $push
    )

    (func $sub
        call $swap
        call $pop
        call $pop
        i32.sub
        call $push
    )

    (func $mul
        call $pop
        call $pop
        i32.mul
        call $push
    )

    (func $div
        call $swap
        call $pop
        call $pop
        i32.div_s
        call $push
    )

    (func $mod
        call $swap
        call $pop
        call $pop
        i32.rem_s
        call $push
    )

    ;; Heap Access
    (func $store (param $address i32) (param $data i32)
        get_local $address
        i32.const 4
        i32.mul
        call $check_heap_bounds
        get_local $data
        i32.store
    )

    (func $load (param $address i32) (result i32)
        get_local $address
        i32.const 4
        i32.mul
        call $check_heap_bounds
        i32.load
    )

    ;; I/O
    (func $read_char (param $address i32)
        get_local $address
        i32.const 4
        i32.mul
        call $check_heap_bounds
        call $read_char_js
        i32.store
    )

    (func $read_num (param $address i32)
        get_local $address
        i32.const 4
        i32.mul
        call $check_heap_bounds
        call $read_num_js
        i32.store
    )

    )"  + subroutines + R"(

    ;; MAIN
    (func (export "main")
        )"  + program + R"(
    )
)
)";
}
