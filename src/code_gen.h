#ifndef CODE_GEN_H
#define CODE_GEN_H

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <tuple>
#include <fstream>

#include "main.h"

using namespace std;

void generate_wasm(vector<string> commands, string fname);

tuple<vector<string>, vector<string>> code_blocks(vector<string> code);

vector<string> map_commands(vector<string> tokens);

bool contains(string item, vector<string> list);

void subroutine_exists(string instruction, vector<string> subroutine_names);

vector<string> split_token(string token);

string gen_html(string fname);

string gen_js(string fname);

string gen_wat(string program, string subroutines);

#endif
