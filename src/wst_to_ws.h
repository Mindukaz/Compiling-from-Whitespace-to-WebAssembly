#ifndef WST_TO_WS_H
#define WST_TO_WS_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <bitset>

using namespace std;

vector<string> read_file(string file_name);

string convert_to_space_tab(string binary);

string get_value_bits(string number);

string get_label(string label);

vector<string> process_commands(vector<string> commands);

void write_to_file_ws(vector<string> instructions, string fname);

#endif
