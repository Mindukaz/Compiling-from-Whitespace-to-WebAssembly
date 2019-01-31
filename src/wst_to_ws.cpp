#include "wst_to_ws.h"
#include "main.h"

vector<string> read_file(string file_name)
{
    ifstream file(file_name);
	if(!file) error("File not found");
    vector<string> commands;
    if(file.is_open())
	{
        string line;
        while(getline(file, line)) commands.push_back(line);
        file.close();
	}
	else error("Could not open file");
    return commands;
}

string convert_to_space_tab(string binary)
{
    string ws_binary = "";
    for(auto &bit : binary)
    {
        if(bit == '0') ws_binary += " ";
        else ws_binary += "\t";
    }
    return ws_binary;
}

string get_value(string number)
{
    string sign = " ";
    int number_val = stoi(number);
    if(number_val < 0)
    {
        sign = "\t";
        number_val = abs(number_val);
    }
    return sign + convert_to_space_tab(bitset<32>(number_val).to_string()) + "\n";
}

string get_label(string label)
{
    return convert_to_space_tab(bitset<32>(stoi(label)).to_string()) + "\n";
}


vector<string> process_commands(vector<string> commands)
{
    int line_num = 0;
    vector<string> ws_commands;

    for(auto &command : commands)
    {
        line_num++;
        string command4 = command.substr(0,4);
        string command5 = command.substr(0,5);

        // Non-important
        if(command == "") continue;
        else if(command.substr(0,2) == "//") continue;

        // Stack commands
        else if(command4 == "push") ws_commands.push_back("  " + get_value(command.substr(5)));
        else if(command == "duplicate") ws_commands.push_back(" \n ");
        else if(command4 == "copy") ws_commands.push_back(" \t " + get_value(command.substr(5)));
        else if(command == "swap") ws_commands.push_back(" \n\t");
        else if(command == "discard") ws_commands.push_back(" \n\n");
        else if(command5 == "slide") ws_commands.push_back(" \t\n" + get_value(command.substr(6)));

        // Arithmetic
        else if(command == "add") ws_commands.push_back("\t   ");
        else if(command == "sub") ws_commands.push_back("\t  \t");
        else if(command == "mul") ws_commands.push_back("\t  \n");
        else if(command == "div") ws_commands.push_back("\t \t ");
        else if(command == "mod") ws_commands.push_back("\t \t\t");

        // Heap Access
        else if(command == "store") ws_commands.push_back("\t\t ");
        else if(command == "retrieve") ws_commands.push_back("\t\t\t");

        // Control flow
        else if(command4 == "mark") ws_commands.push_back("\n  " + get_value(command.substr(5)));
        else if(command4 == "call") ws_commands.push_back("\n \t" + get_value(command.substr(5)));
        else if(command5 == "jumpz") ws_commands.push_back("\n\t " + get_value(command.substr(6)));
        else if(command5 == "jumpn") ws_commands.push_back("\n\t\t" + get_value(command.substr(6)));
        else if(command4 == "jump") ws_commands.push_back("\n \n" + get_value(command.substr(5)));
        else if(command == "return") ws_commands.push_back("\n\t\n");
        else if(command == "end") ws_commands.push_back("\n\n\n");

        // io
        else if(command == "out_char") ws_commands.push_back("\t\n  ");
        else if(command == "out_num") ws_commands.push_back("\t\n \t");
        else if(command == "read_char") ws_commands.push_back("\t\n\t ");
        else if(command == "read_num") ws_commands.push_back("\t\n\t\t");

        // Bad command, error out with line number
        else error("Syntax error on line " + to_string(line_num));
    }
    return ws_commands;
}

void write_to_file_ws(vector<string> instructions, string fname)
{
    ofstream file(fname + ".ws");
    for(auto &instruction : instructions) file << instruction;
    file.close();
}
