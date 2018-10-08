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

// need a stack (linked list of nodes)
// need a heap (just an array? size could be defined dynamically by program??


void error(string error)
{
	cerr << error << endl;
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

		instructions.push_back("stack push " + to_string(value));
		return get_bits_end(location + 3);
	}
	// Value manipulation
	else if(text_chars[location + 1] == LF)
	{
		switch(text_chars[location + 2])
		{
			case SPACE: instructions.push_back("stack duplicate");
				    break;
			case TAB: instructions.push_back("stack swap");
				  break;
			case LF: instructions.push_back("stack discard");
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
			case SPACE: instructions.push_back("stack copy " + to_string(value));
				    break;
			case LF: instructions.push_back("stack slide " + to_string(value));
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
			case SPACE: instructions.push_back("arithmetic add");
				    break;
			case TAB: instructions.push_back("arithmetic sub");
				  break;
			case LF: instructions.push_back("arithmetic mul");
				 break;
			default: error("Syntax error");
		}
	}
	else if(text_chars[location + 2] == TAB)
	{
		switch(text_chars[location + 3])
		{
			case SPACE: instructions.push_back("arithmetic div");
				    break;
			case TAB: instructions.push_back("arithmetic mod");
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
		case SPACE: instructions.push_back("heap store");
			    break;
		case TAB: instructions.push_back("heap retrieve");
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
				    case SPACE: instructions.push_back("flow mark " + get_bits(location + 3));
						break;
				    case TAB: instructions.push_back("flow call subroutine " + get_bits(location + 3));
					      break;
				    case LF: instructions.push_back("flow jump " + get_bits(location + 3));
					     break;
			    }
			    return get_bits_end(location + 3);
		case TAB: switch(text_chars[location + 2])
			  {
				  case SPACE: instructions.push_back("flow jump 0 " + get_bits(location + 3));
					      break;
				  case TAB: instructions.push_back("flow jump neg " + get_bits(location + 3));
					    break;
				  case LF: instructions.push_back("flow end subroutine");
					   break;
			  }
			  return get_bits_end(location + 3);
		case LF: if(text_chars[location + 2] == LF) instructions.push_back("flow end program");
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
			case SPACE: instructions.push_back("io out char");
				    break;
			case TAB: instructions.push_back("io out num");
				  break;
			default: error("Syntax error");
		}
	}
	else if(text_chars[location + 2] == TAB)
	{
		switch(text_chars[location + 3])
		{
			case SPACE: instructions.push_back("io in char");
				    break;
			case TAB: instructions.push_back("io in num");
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

					case LF: i = io(i);
						 break;
					
					case TAB: i = heap_access(i);
						  break;

					default: error("Synatx error");
				} 
				break;
				
			case LF: i = flow_control(i);
				 break;
		
			default: error("Syntax error");
		}
	}	
}

void write_file()
{
	ofstream file("whitesapce.cpp");

	file << "#include <stack>" << endl << "#include <vector>" << endl;
	file << "int main()" << endl << "{" << endl;
	file << "\tstd::stack<int> stack;" << endl;
	file << "\tstd::vector<int> heap;" << endl;

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

	for(auto &i : instructions) cout << i << endl;

	// write cpp eqivalent commands to a new file
	write_file();

	
	// make script to compile new file to wasm????	
	return 0;
}
