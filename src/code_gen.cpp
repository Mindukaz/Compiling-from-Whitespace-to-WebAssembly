#include "code_gen.h"

void generate_wasm(vector<string> commands, string fname)
{
    vector<string> code = map_commands(commands);
    vector<string> main;
    vector<string> subroutines;
    tie(main, subroutines) = code_blocks(code);

    string name = fname.substr(0, fname.find(".")+1);
    string dir = "wasm/";

    string html_name = name + "html";
    string js_name = name + "js";
    string wat_name = name + "wat";
    string wasm_name = name + "wasm";

    string code_main;
    string code_subroutine;
    for (auto &instruction : main) code_main += instruction;
    for (auto &instruction : subroutines) code_subroutine += instruction;

    ofstream file;
    file.open(dir + html_name);
    file << gen_html(js_name);
    file.close();

    file.open(dir + js_name);
    file << gen_js(wasm_name);
    file.close();

    file.open(dir + wat_name);
    file << gen_wat(code_main, code_subroutine);
    file.close();
}

tuple<vector<string>, vector<string>> code_blocks(vector<string> code)
{
    vector<string> main_code;
    vector<string> subroutines;

    vector<string> label_names;
    vector<string> subroutine_names{"$push", "$duplicate", "$copy", "$swap", "$discard", "$slide",
                                    "$add", "$sub", "$mul", "$div", "$mod", "$store", "$load",
                                    "$write_char", "$write_num", "$read_char", "$read_num", "$pop"};


    bool end_prog = false;
    bool inside_subroutine = false;

    for (auto &instruction : code)
    {
        if (!end_prog)
        {
            // dealing with the main code of a whitespace program, before an "end" instruction
            vector<string> label = split_token(instruction);
            if (label[0] == "block")
            {
                if ( contains(label[1], label_names) ) error("Block " + label[1] + " is a duplicate name");
                label_names.push_back(label[1]);
                main_code.push_back(instruction);
                continue;
            }
            if (instruction == "endprog\n")
            {
                end_prog = true;
                main_code.push_back("end\n\n");
                continue;
            }
            if ((instruction == "return\n") && (!inside_subroutine)) error("return not in subroutine");
            main_code.push_back(instruction);
        }
        else
        {
            // dealing with subroutines, after the "end" of a whitespace program
            if (instruction == "end\n\n") continue;
            vector<string> inst = split_token(instruction);
            if (inst[0] == "block")
            {
                inside_subroutine = true;
                subroutines.push_back("(func " + inst[1] + "\n");
                if ( (contains(inst[1], subroutine_names)) || ((contains(inst[1], label_names))) ) error("Subroutine " + inst[1] + " is a duplicate name");
                subroutine_names.push_back(inst[1]);
                continue;
            }
            else if (instruction == "return\n")
            {
                if (!inside_subroutine) error("return not in subroutine");
                subroutines.push_back(")\n\n");
                inside_subroutine = false;
                continue;
            }
            if (!inside_subroutine) error("Non-subroutine code inside subroutine section");
            subroutines.push_back(instruction);
        }
    }

    if (!end_prog) error("\'end\' instruction not found. There must be an \'end\' at the end of a Whitespace program");

    // check that all the subroutine calls in main code exist
    for (auto &instruction : main_code) subroutine_exists(instruction, subroutine_names);

    // check that all subroutine calls inside subroutines exist
    for (auto &instruction : subroutines) subroutine_exists(instruction, subroutine_names);

    // check that all code jumps are valid
    unsigned int label_name_it = 0;
    unsigned int current_block_location = 0;
    string current_block;
    string next_block;

    bool first_loop = true;
    bool loop_structure = false;

    for (unsigned int i = 0; i < main_code.size(); i++)
    {
        string instruction = main_code[i];
        vector<string> inst = split_token(instruction);
        if (inst[0] == "block")
        {
            if (!first_loop) label_name_it++;
            first_loop = false;
            if ( label_name_it == label_names.size()-1 )
            {
                current_block = label_names[label_name_it];
                current_block_location = i;
                next_block = "";
            }
            else if (label_name_it < label_names.size()-1)
            {
                current_block = label_names[label_name_it];
                current_block_location = i;
                next_block = label_names[label_name_it+1];
            }
            continue;
        }
        else if (inst[0] == "br")
        {
            if (inst[1] == current_block)
            {
                // loop
                main_code[current_block_location] = "block " + current_block + "\nloop " + current_block + "_l\n";
                main_code[i] = "br " + current_block + "_l\n";
                loop_structure = true;
            }
            else if (inst[1] == next_block) main_code[i] =  "br " + current_block + "\n";
            else error("Block " + current_block + ": jumping outside of local block space");
        }
        else if (inst[0] == "br_if")
        {
            if (inst[1] == current_block)
            {
                // loop
                main_code[current_block_location] = "block " + current_block + "\nloop " + current_block + "_l\n";
                main_code[i] = "br_if " + current_block + "_l\n";
                loop_structure = true;
            }
            else if (inst[1] == next_block) main_code[i] =  "br_if " + current_block + "\n";
            else error("Block " + current_block + ": jumping outside of local block space");
        }
        else if (inst[0] == "end")
        {
            if (loop_structure)
            {
                main_code[i] = "end\nend\n\n";
                loop_structure = false;
            }
        }
    }
    return make_tuple(main_code, subroutines);
}

vector<string> map_commands(vector<string> tokens)
{
    bool first_token = true;
    vector<string> instructions;

    for (auto &token : tokens)
    {
        vector<string> toks = split_token(token);

        if (first_token)
        {
            first_token = false;
            if (toks[0] != "mark") instructions.push_back("block $mainBlock\n");
        }

        // Stack Manipulation
        if (toks[0] == "push") instructions.push_back("i32.const " + toks[1] + "\ncall $push\n");
        else if (toks[0] == "duplicate") instructions.push_back("call $duplicate\n");
        else if (toks[0] == "copy") instructions.push_back("i32.const " + toks[1] + "\ncall $copy\n");
        else if (toks[0] == "swap") instructions.push_back("call $swap\n");
        else if (toks[0] == "discard") instructions.push_back("call $discard\n");
        else if (toks[0] == "slide") instructions.push_back("i32.const " + toks[1] + "\ncall $slide\n");

        // Arithmetic
        else if (toks[0] == "add") instructions.push_back("call $add\n");
        else if (toks[0] == "sub") instructions.push_back("call $sub\n");
        else if (toks[0] == "mul") instructions.push_back("call $mul\n");
        else if (toks[0] == "div") instructions.push_back("call $div\n");
        else if (toks[0] == "mod") instructions.push_back("call $mod\n");

        // Heap Access
        else if (toks[0] == "store") instructions.push_back("call $store\n");
        else if (toks[0] == "retrieve") instructions.push_back("call $load\n");

        // Flow Control
        else if (toks[0] == "mark")
        {
            instructions.push_back("end\n\n");
            instructions.push_back("block $" + toks[1] + "\n");
        }
        else if (toks[0] == "call") instructions.push_back("call $" + toks[1] + "\n");
        else if (toks[0] == "jump") instructions.push_back("br $" + toks[1] + "\n");
        else if (toks[0] == "jumpz")
        {
            instructions.push_back("call $pop\ni32.eqz\n");
            instructions.push_back("br_if $" + toks[1] +"\n");
        }
        else if (toks[0] == "jumpn")
        {
            instructions.push_back("call $pop\ni32.const 0\ni32.lt_s\n");
            instructions.push_back("br_if $" + toks[1] + "\n");
        }
        else if (toks[0] == "return") instructions.push_back("return\n");
        else if (toks[0] == "end") instructions.push_back("endprog\n");

        // I/O
        else if (toks[0] == "out_char") instructions.push_back("call $write_char\n");
        else if (toks[0] == "out_num") instructions.push_back("call $write_num\n");
        else if (toks[0] == "read_char") instructions.push_back("call $read_char\n");
        else if (toks[0] == "read_num") instructions.push_back("call $read_num\n");

    }
    return instructions;
}

bool contains(string item, vector<string> list)
{
    if ( find(list.begin(), list.end(), item) != list.end() ) return true;
    return false;
}

void subroutine_exists(string instruction, vector<string> subroutine_names)
{
    vector<string> inst = split_token(instruction);
    if (inst[0] == "call")
    {
        if ( !contains(inst[1], subroutine_names) ) error("Calling an undefined subroutine: " + inst[1].substr(1));
    }
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
        <script src=")" + fname + R"("></script>
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

    loadWebAssembly(")" + fname + R"(", importObject)
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
    (import "console" "write_char" (func $write_char_js (param i32)))
    (import "console" "write_num" (func $write_num_js (param i32)))


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

        ;; expand memory if needed
        get_global $stack_pointer
        current_memory
        get_global $memory_page_size
        i32.mul
        i32.ge_s
        if
            i32.const 1
            grow_memory
            drop
        end
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

    (func $write_char
        call $pop
        call $write_char_js
    )

    (func $write_num
        call $pop
        call $write_num_js
    )

    )"  + subroutines + R"(

    ;; MAIN
    (func (export "main")
        )"  + program + R"(
    )
)
)";
}
