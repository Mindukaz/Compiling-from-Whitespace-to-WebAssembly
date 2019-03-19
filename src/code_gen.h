#ifndef CODE_GEN_H
#define CODE_GEN_H

#include <vector>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

void generate_wasm(vector<string> commands, string fname);

vector<string> map_commands(vector<string> tokens);

vector<string> split_token(string token);

string gen_html(string fname);

string gen_js(string fname);

string gen_wat(string program, string subroutines);

#endif
