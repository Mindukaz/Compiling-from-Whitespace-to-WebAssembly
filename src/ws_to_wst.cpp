#include "ws_to_wst.h"

vector<int> text_chars;
int char_num = 0;
vector<string> instructions;
int line = 1;

vector<string> parse_ws(string fname)
{
    //read  in contents of provided file
	ifstream file(fname);
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
	return instructions;
}

void error_ws(string error)
{
	cerr << "Line:" << line << " " <<  error << endl;
	exit(1);
}

int int_value(int i)
{
    return get_value(get_bits(i));
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
	else error_ws("Syntax error");
    return false; // this is never reached
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
		value = int_value(location + 3);
		if(!pos_or_neg(location + 2)) value *=  -1;

		instructions.push_back("push " + to_string(value));
		return get_bits_end(location + 3);
	}
	// Value manipulation
	else if(text_chars[location + 1] == LF)
	{
		line++;
		switch(text_chars[location + 2])
		{
			case SPACE: instructions.push_back("duplicate");
				    break;
			case TAB: instructions.push_back("swap");
				  break;
			case LF: line++;
				 instructions.push_back("discard");
				 break;
			default: error_ws("Syntax error");

		}
		return location + 3;
	}
	// Deep stack manipulation
	else if(text_chars[location + 1] == TAB)
	{
		value = int_value(location + 4);
		if(!pos_or_neg(location + 3)) value *= -1;
		switch(text_chars[location + 2])
		{
			case SPACE: instructions.push_back("copy " + to_string(value));
				    break;
			case LF: line++;
				 instructions.push_back("slide " + to_string(value));
				 break;
			default: error_ws("Syntax error");
		}
		return get_bits_end(location + 4);
	}

    return 0; // this never gets reached
}

int arithmetic(int location)
{
	//cout << "arithmetic" << endl;

	if(text_chars[location + 2] == SPACE)
	{
		switch(text_chars[location + 3])
		{
			case SPACE: instructions.push_back("add");
				    break;
			case TAB: instructions.push_back("sub");
				  break;
			case LF: line++;
				 instructions.push_back("mul");
				 break;
			default: error_ws("Syntax error");
		}
	}
	else if(text_chars[location + 2] == TAB)
	{
		switch(text_chars[location + 3])
		{
			case SPACE: instructions.push_back("div");
				    break;
			case TAB: instructions.push_back("mod");
				  break;
			default: error_ws("Syntax error");
		}
	}
	else error_ws("Syntax error");

	return location + 4;
}

int heap_access(int location)
{
	//cout << "heap" << endl;

	switch(text_chars[location + 2])
	{
		case SPACE: instructions.push_back("store");
			    break;
		case TAB: instructions.push_back("retrieve");
			  break;
		default: error_ws("Syntax error");
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
				    case SPACE: instructions.push_back("mark " + to_string(int_value(location + 3)));
						        break;
				    case TAB: instructions.push_back("call " + to_string(int_value(location + 3)));
					          break;
				    case LF: line++;
					         instructions.push_back("jump " + to_string(int_value(location + 3)));
					         break;
			    }
			    return get_bits_end(location + 3);
		case TAB: switch(text_chars[location + 2])
			  {
				  case SPACE: instructions.push_back("jumpz " + to_string(int_value(location + 3)));
					      break;
				  case TAB: instructions.push_back("jumpn " + to_string(int_value(location + 3)));
					    break;
				  case LF: line++;
					   instructions.push_back("return");
					   break;
			  }
			  return get_bits_end(location + 3);
		case LF: line++;
			 if(text_chars[location + 2] == LF)
			 {
				 line++;
				 instructions.push_back("end");
			 }
			 else error_ws("Syntax error");
			 return location + 3;
	}
    return 0; // this is never reached
}

int io(int location)
{
	//cout << "io" << endl;

	if(text_chars[location + 2] == SPACE)
	{
		switch(text_chars[location + 3])
		{
			case SPACE: instructions.push_back("out_char");
				    break;
			case TAB: instructions.push_back("out_num");
				  break;
			default: error_ws("Syntax error");
		}
	}
	else if(text_chars[location + 2] == TAB)
	{
		switch(text_chars[location + 3])
		{
			case SPACE: instructions.push_back("read_char");
				    break;
			case TAB: instructions.push_back("read_num");
				  break;
			default: error_ws("Syntax error");
		}
	}
	else error_ws("Syntax error");

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

					default: error_ws("Synatx error");
				}
				break;

			case LF: line++;
				 i = flow_control(i);
				 break;

			default: error_ws("Syntax error");
		}
	}
}

void write_file()
{
	ofstream file("whitespace.wst");
	for(auto &i : instructions) file << i << endl;
	file.close();
}
