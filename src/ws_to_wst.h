#ifndef WS_TO_WST_H
#define WS_TO_WST_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#define SPACE 32
#define TAB 9
#define LF 10


using namespace std;

vector<string> parse_ws(string fname);

int int_value(int i);

void error(string error);

string get_bits(int location);

int get_bits_end(int location);

bool pos_or_neg(int location);

int get_value(string bits);

int stack_manipulation(int location);

int arithmetic(int location);

int heap_access(int location);

int flow_control(int location);

int io(int location);

void identify_commands();

void write_file();

#endif
