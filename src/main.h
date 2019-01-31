#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <bitset>

using namespace std;

int main(int argc, char* argv[]);

void error(string error);

void wst_to_wasm(string fname);

void wst_to_ws(string fname);

void ws_to_wasm(string fname);

void ws_to_wst(string fname);

bool check_file_name(string fname, string extension);


#endif
