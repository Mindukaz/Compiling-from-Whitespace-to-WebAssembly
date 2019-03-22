#include "main.h"


int main(int argc, char* argv[])
{
    size_t slash;
    string path;
    if (argc == 2)
    {
        path = argv[1];
        slash = path.find_last_of("/");
        if (slash == string::npos) manage_file(path, path);
        else
        {
            string fname = path.substr(slash+1);
            manage_file(fname, path);
        }

    }
    else if (argc == 3)
    {
        path = argv[2];
        slash = path.find_last_of("/");
        string fname;

        if (slash == string::npos) fname = path;
        else fname = path.substr(slash+1);

        string flag = argv[1];
        if (flag == "-wst2ws")
        {
            if (check_file_name(fname, ".wst")) wst_to_ws(fname, path);
            else error("Incorrect file format for given flag");
        }
        else if (flag == "-ws2wst")
        {
            if (check_file_name(fname, ".ws")) ws_to_wst(fname, path);
            else error("Incorrect file format for given flag");
        }
        else error("Bad command");
    }
    else error("Bad command");
    return 0;
}

void manage_file(string fname, string path)
{
    if (check_file_name(fname, ".wst")) wst_to_wasm(fname, path);
    else if (check_file_name(fname, ".ws")) ws_to_wasm(fname, path);
    else error("Incorrect file format");
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
void wst_to_wasm(string fname, string path)
{
    vector<string> commands = read_file(path);
    valid(commands);
    generate_wasm(commands, fname);
}

// translate readable whitespace to whitespace
void wst_to_ws(string fname, string path)
{
    vector<string> commands = read_file(path);
    commands = process_commands(commands);
    write_to_file_ws(commands, fname.substr(0, fname.find(".")));
}

// compile whitespace to wasm
void ws_to_wasm(string fname, string path)
{
    vector<string> c = parse_ws(path);
    generate_wasm(c, fname);
}

// translate whitespace to readable whitespace
void ws_to_wst(string fname, string path)
{
    parse_ws(path);
    write_file(fname);
}
