#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <bitset>
#include <string>

#include "wst_to_ws.h"
#include "ws_to_wst.h"
#include "code_gen.h"

using namespace std;

int main(int argc, char* argv[]);

void manage_file(string fname, string path);

void error(string error);

void wst_to_wasm(string fname, string path);

void wst_to_ws(string fname, string path);

void ws_to_wasm(string fname, string path);

void ws_to_wst(string fname, string path);

bool check_file_name(string fname, string extension);


#endif
