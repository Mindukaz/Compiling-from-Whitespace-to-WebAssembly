#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

#define SPACE 32
#define TAB 9
#define LF 10

vector<int> text_chars;
int char_num = 0;
vector<string> instructions;
int line = 1;
char dynamic_label = 'A';

// need a stack (linked list of nodes)
// need a heap (just an array? size could be defined dynamically by program??


void error(string error)
{
	cerr << "Line:" << line << " " <<  error << endl;
	exit(1);
}

string get_bits(int location)
{
	string bits = "";
	while(text_chars[location] != LF)
	{
		if(text_chars[location] == SPACE) bits += '0';
		else bits += '1';
		location++;
	}
	return bits;
}

int get_bits_end(int location)
{
	while((text_chars[location] != LF) && (location < char_num)) location++;
	line++;
	return location + 1;
}

bool pos_or_neg(int location)
{
	if(text_chars[location] == SPACE) return true;
	else if(text_chars[location] == TAB) return false;
	else error("Syntax error");
}

int get_value(string bits)
{
	if(bits == "") return 0;
	else return stoi(bits, nullptr, 2);
}

int stack_manipulation(int location)
{
	//cout << "stack manipulation" << endl;
	
	int value;
	// Push value onto stack
	if(text_chars[location + 1] == SPACE)
	{	
		// get integer value from bits
		value = get_value(get_bits(location + 3));
		if(!pos_or_neg(location + 2)) value *=  -1;

		instructions.push_back("stack_push(" + to_string(value) + ");");
		return get_bits_end(location + 3);
	}
	// Value manipulation
	else if(text_chars[location + 1] == LF)
	{
		line++;
		switch(text_chars[location + 2])
		{
			case SPACE: instructions.push_back("stack_duplicate();");
				    break;
			case TAB: instructions.push_back("stack_swap();");
				  break;
			case LF: line++;
				 instructions.push_back("stack_discard();");
				 break;
			default: error("Syntax error");
	
		}
		return location + 3;
	}
	// Deep stack manipulation
	else if(text_chars[location + 1] == TAB)
	{
		value = get_value(get_bits(location + 4));
		if(!pos_or_neg(location + 3)) value *= -1;
		switch(text_chars[location + 2])
		{
			case SPACE: instructions.push_back("stack_copy(" + to_string(value) +");");
				    break;
			case LF: line++;
				 instructions.push_back("stack_slide(" + to_string(value) + ");");
				 break;
			default: error("Syntax error");
		}
		return get_bits_end(location + 4);
	}
}

int arithmetic(int location)
{
	//cout << "arithmetic" << endl;

	if(text_chars[location + 2] == SPACE)
	{
		switch(text_chars[location + 3])
		{
			case SPACE: instructions.push_back("arithmetic_add();");
				    break;
			case TAB: instructions.push_back("arithmetic_sub();");
				  break;
			case LF: line++;
				 instructions.push_back("arithmetic_mul();");
				 break;
			default: error("Syntax error");
		}
	}
	else if(text_chars[location + 2] == TAB)
	{
		switch(text_chars[location + 3])
		{
			case SPACE: instructions.push_back("arithmetic_div();");
				    break;
			case TAB: instructions.push_back("arithmetic_mod();");
				  break;
			default: error("Syntax error");
		}
	}
	else error("Syntax error");

	return location + 4;
}

int heap_access(int location)
{
	//cout << "heap" << endl;

	switch(text_chars[location + 2])
	{
		case SPACE: instructions.push_back("heap_store();");
			    break;
		case TAB: instructions.push_back("heap_retrieve();");
			  break;
		default: error("Syntax error");
	}

	return location + 3;
}

int flow_control(int location)
{
	//cout << "flow control" << endl;

	switch(text_chars[location + 1])
	{
		case SPACE: switch(text_chars[location + 2])
			    {
				    case SPACE: instructions.push_back("label_" + get_bits(location + 3) + ":");
						break;
				    case TAB: dynamic_label++;
					      instructions.push_back("last_call = \"label_\" + dynamic_label++; " + string("goto label_") + get_bits(location + 3) + ";" + "\nlabel_" + dynamic_label + ":");
					      break;
				    case LF: line++;
					     instructions.push_back("goto label_" + get_bits(location + 3) + ";");
					     break;
			    }
			    return get_bits_end(location + 3);
		case TAB: switch(text_chars[location + 2])
			  {
				  case SPACE: instructions.push_back("if(stack.top() == 0) goto label_" + get_bits(location + 3) + ";");
					      break;
				  case TAB: instructions.push_back("if(stack.top() < 0) goto label_" + get_bits(location + 3) + ";");
					    break;
				  case LF: line++;
					   instructions.push_back("goto  last_call;");
					   break;
			  }
			  return get_bits_end(location + 3);
		case LF: line++;
			 if(text_chars[location + 2] == LF) 
			 {
				 line++;
				 instructions.push_back("return 0;");
			 }
			 else error("Syntax error");
			 return location + 3;
	}
}

int io(int location)
{
	//cout << "io" << endl;

	if(text_chars[location + 2] == SPACE)
	{
		switch(text_chars[location + 3])
		{
			case SPACE: instructions.push_back("io_out_char();");
				    break;
			case TAB: instructions.push_back("io_out_num();");
				  break;
			default: error("Syntax error");
		}
	}
	else if(text_chars[location + 2] == TAB)
	{
		switch(text_chars[location + 3])
		{
			case SPACE: instructions.push_back("io_in_char();");
				    break;
			case TAB: instructions.push_back("io_in_num();");
				  break;
			default: error("Syntax error");
		}
	}
	else error("Syntax error");

	return location + 4;
}


void identify_commands()
{
	for(int i = 0; i < char_num - 1;)
	{
		switch(text_chars[i])
		{
			case SPACE: i = stack_manipulation(i);
				    break;
			
			case TAB: switch(text_chars[i+1])
				{
					case SPACE: i = arithmetic(i);
						    break;

					case LF: line++;
						 i = io(i);
						 break;
					
					case TAB: i = heap_access(i);
						  break;

					default: error("Synatx error");
				} 
				break;
				
			case LF: line++;
				 i = flow_control(i);
				 break;
		
			default: error("Syntax error");
		}
	}	
}

void write_file()
{
	ofstream file("whitesapce.cpp");

	//file << "#include <stack>" << endl;
	//file << "#include <vector>" << endl;
	//file << "#include <iostream>" << endl;
	file << "#include \"instructions.h\"" << endl;
	//file << "std::stack<int> stack;" << endl;
	//file << "std::vector<int> heap" << endl;
	file << "std::string last_call;" << endl;
	file << "char dynamic_label = 'A';" << endl;
	file << "int main()" << endl << "{" << endl;

	for(auto &i : instructions) file << i << endl;

	file << "}" << endl;
	file.close();
}


int main(int argc, char* argv[])
{	
	//read  in contents of provided file
	ifstream file(argv[1]);
	if(!file) error("File not found");

	if(file.is_open())
	{
		char temp;
		while(!file.eof())
		{
			file.get(temp);
			if((int(temp) == SPACE) || (int(temp) == TAB) || (int(temp) == LF))
			{
				text_chars.push_back(int(temp));
				char_num++;
			}
		}
	}
	else error("Could not open file");
	file.close();
	
	// iterate over array to identify commands
	identify_commands();

	//for(auto &i : instructions) cout << i << endl;

	// write cpp eqivalent commands to a new file
	write_file();

	
	// make script to compile new file to wasm????	
	return 0;
}
