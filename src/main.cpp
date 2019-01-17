#include "main.h"
#include "translator.h"

using namespace std;

int main(int argc, char* argv[])
{
    if(argc != 2) error("Bad command");

    string fname = argv[1];
    int delimiter = fname.find(".");
    if(!(fname.substr(delimiter) == ".wst")) error("Incorrect file format");
    
	vector<string> commands = read_file(argv[1]);
    commands = process_commands(commands);
    write_to_file_ws(commands, fname.substr(0, delimiter));
    return 0;
}

void error(string error)
{
    cout << error << endl;
    exit(1);
}
