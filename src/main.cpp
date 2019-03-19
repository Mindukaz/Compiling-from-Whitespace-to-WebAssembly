#include "main.h"
#include "wst_to_ws.h"
#include "ws_to_wst.h"
#include "code_gen.h"

/*
 ./main.out file.ws
 ./main.out file.wst
 ./main.out -wst2ws file.wst
 ./main.out -ws2wst file.ws
*/


int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        if (check_file_name(argv[1], ".wst")) wst_to_wasm(argv[1]);
        else if (check_file_name(argv[1], ".ws")) ws_to_wasm(argv[1]);
        else error("Incorrect file format");
    }
    else if (argc == 3)
    {
        string flag = argv[1];
        if (flag == "-wst2ws")
        {
            if (check_file_name(argv[2], ".wst")) wst_to_ws(argv[2]);
            else error("Incorrect file format for given flag");
        }
        else if (flag == "-ws2wst")
        {
            if (check_file_name(argv[2], ".ws")) ws_to_wst(argv[2]);
            else error("Incorrect file format for given flag");
        }
        else error("Bad command");
    }
    else error("Bad command");
    return 0;
}

void error(string error)
{
    cout << error << endl;
    exit(1);
}

bool check_file_name(string fname, string extension)
{
    size_t delimiter = fname.find_last_of(".");
    if(delimiter == string::npos) error("Incorrect file format");
    if(fname.substr(delimiter) == extension) return true;
    return false;
}

// compile readable whitespace to wasm
void wst_to_wasm(string fname)
{
    vector<string> commands = read_file(fname);
    generate_wasm(commands, fname);
}

// translate readable whitespace to whitespace
void wst_to_ws(string fname)
{
    vector<string> commands = read_file(fname);
    commands = process_commands(commands);
    write_to_file_ws(commands, fname.substr(0, fname.find(".")));
}

// compile whitespace to wasm
void ws_to_wasm(string fname)
{
    vector<string> c = parse_ws(fname);
    generate_wasm(c, fname);
}

// translate whitespace to readable whitespace
void ws_to_wst(string fname)
{
    parse_ws(fname);
    write_file();
    // global variable "instructions" contains the tokens / commands
}
