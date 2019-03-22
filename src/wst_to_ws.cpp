#include "wst_to_ws.h"

void valid(vector<string> tokens)
{
    vector<string> multi_tokens = {"push", "copy", "slide", "mark", "call", "jumpz", "jumpn", "jump"};
    vector<string> ws_tokens = {"push", "duplicate", "copy" ,"swap" ,"discard" ,"slide" ,"add" ,"sub"
                            ,"mul","div" ,"mod" ,"store", "retrieve", "mark" ,"call" ,"jump" ,"jumpz"
                            ,"jumpn","return", "end", "out_char", "out_num", "read_char", "read_num"};
    int line = 0;

    for (auto &t : tokens)
    {
        cout << t << endl;
        line++;
        vector<string> toks = split_token(t);

        if ( !contains(toks[0], ws_tokens) ) error("Line " + to_string(line) + ": Syntax error: Unknown command");

        if ( contains(toks[0], multi_tokens) )
        {
            if (toks.size() != 2) error("Line " + to_string(line) + ": instruction " + toks[0] + " takes an argument, none provided");
            try
            {
                stoi(toks[1]);
            }
            catch (const std::invalid_argument& ia)
            {
                error("Line " + to_string(line) + ": instruction " + toks[0] + " takes an integer argument, bad argument");
            }
        }
    }


}

vector<string> read_file(string file_name)
{
    ifstream file(file_name);
	if(!file) error("File not found");
    vector<string> commands;
    if(file.is_open())
	{
        string line;
        while(getline(file, line))
        {
            if (line == "") continue;
            if (line.substr(0,2) == "//") continue;
            commands.push_back(line);
        }
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

string get_value_bits(string number)
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

    for(auto &token : commands)
    {
        line_num++;
        string command;
        string value;
        string val_bits;
        vector<string> multi_token = {"push", "copy", "slide", "mark", "call", "jumpz", "jumpn", "jump"};
        vector<string> toks = split_token(token);
        command = toks[0];

        if ( contains(toks[0], multi_token) )
        {
            try
            {
                val_bits = get_value_bits(toks[1]);
            }
            catch (const std::invalid_argument& ia)
            {
                error("Line " + to_string(line_num) + ": instruction " + toks[0] + " takes an argument, none provided");
            }
        }

        // Stack commands
        if(command == "push") ws_commands.push_back("  " + val_bits);
        else if(command == "duplicate") ws_commands.push_back(" \n ");
        else if(command == "copy") ws_commands.push_back(" \t " + val_bits);
        else if(command == "swap") ws_commands.push_back(" \n\t");
        else if(command == "discard") ws_commands.push_back(" \n\n");
        else if(command == "slide") ws_commands.push_back(" \t\n" + val_bits);

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
        else if(command == "mark") ws_commands.push_back("\n  " + val_bits);
        else if(command == "call") ws_commands.push_back("\n \t" + val_bits);
        else if(command == "jumpz") ws_commands.push_back("\n\t " + val_bits);
        else if(command == "jumpn") ws_commands.push_back("\n\t\t" + val_bits);
        else if(command == "jump") ws_commands.push_back("\n \n" + val_bits);
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
    ofstream file("ws/" + fname + ".ws");
    for(auto &instruction : instructions) file << instruction;
    file.close();
}
