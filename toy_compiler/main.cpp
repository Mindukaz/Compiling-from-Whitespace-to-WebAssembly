#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

#define SPACE 32
#define TAB 9
#define LF 10

char text_chars[100000];
int char_num = 0;
vector<string> instructions;

// need a stack (linked list of nodes)
// need a heap (just an array? size could be defined dynamically by program??


void error(string error)
{
	cerr << error << endl;
	exit(1);
}

string get_label(int location)
{
	string bits = "";
	while(int(text_chars[location]) != LF)
	{
		if(int(text_chars[location]) == SPACE) bits += '0';
		else bits += '1';
		location++;
	}
	return bits;
}

int get_label_end(int location)
{
	while((int(text_chars[location]) != LF) && (location < char_num)) location++;
	return location + 1;
}

int stack_manipulation(int location)
{
	cout << "stack manipulation" << endl;

	// Push value onto stack
	if(int(text_chars[location + 1]) == SPACE)
	{
		bool positive_value;
		string bits = "";
		
		// determine if value is positive or negative
		if(int(text_chars[location + 2]) == SPACE) positive_value = true;
		else if(int(text_chars[location + 2]) == TAB) positive_value = false;
		else if(int(text_chars[location + 2]) == LF)
		{
			instructions.push_back("stack push 0");
			return location + 3;
		}

		location += 3;
	
		// get the bits representing the value
		while(int(text_chars[location]) != LF)
		{
			if(int(text_chars[location]) == SPACE) bits += '0';
			else if(int(text_chars[location]) == TAB) bits += '1';
			else break;
			location++;
		}
		// get integer value from bits
		int value;
		cout << bits << endl;
		if(bits == "") value = 0; 
		else value = stoi(bits, nullptr, 2);
		if(!positive_value) value *=  -1;
		cout << value << endl;

		instructions.push_back("stack push " + to_string(value));
		return location + 1;
	}
	// Other stack operation
	else if(int(text_chars[location + 1]) == LF)
	{
		cout << "other stack opeartion" << endl;
		switch(int(text_chars[location + 2]))
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
	else {cout << int(text_chars[location + 1]) << endl; error("Syntax error");}
}

int arithmetic(int location)
{
	cout << "arithmetic" << endl;

	if(int(text_chars[location + 2]) == SPACE)
	{
		switch(int(text_chars[location + 3]))
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
	else if(int(text_chars[location + 2]) == TAB)
	{
		switch(int(text_chars[location + 3]))
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
	cout << "heap" << endl;

	switch(int(text_chars[location + 2]))
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
	cout << "flow control" << endl;

	switch(int(text_chars[location + 1]))
	{
		case SPACE: switch(int(text_chars[location + 2]))
			    {
				    case SPACE: instructions.push_back("flow mark " + get_label(location + 3));
						break;
				    case TAB: instructions.push_back("flow call subroutine " + get_label(location + 3));
					      break;
				    case LF: instructions.push_back("flow jump " + get_label(location + 3));
					     break;
			    }
			    break;
		case TAB: switch(int(text_chars[location + 2]))
			  {
				  case SPACE: instructions.push_back("flow jump 0 " + get_label(location + 3));
					      break;
				  case TAB: instructions.push_back("flow jump neg " + get_label(location + 3));
					    break;
				  case LF: instructions.push_back("flow end subroutine");
					   break;
			  }
			  break;
		case LF: if(int(text_chars[location + 2]) == LF) instructions.push_back("flow end program");
			 else error("Syntax error");
			 break;
	}
	return get_label_end(location + 3);
}

int io(int location)
{
	cout << "io" << endl;

	if(int(text_chars[location + 2]) == SPACE)
	{
		switch(int(text_chars[location + 3]))
		{
			case SPACE: instructions.push_back("io out char");
				    break;
			case TAB: instructions.push_back("io out num");
				  break;
			default: error("Syntax error");
		}
	}
	else if(int(text_chars[location + 2]) == TAB)
	{
		switch(int(text_chars[location + 3]))
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
		switch(int(text_chars[i]))
		{
			// Space
			case SPACE: i = stack_manipulation(i);
				    break;
			
			// Tab
			case TAB: switch(int(text_chars[i+1]))
				{
					// Space
					case SPACE: i = arithmetic(i);
						    break;

					// Line feed
					case LF: i = io(i);
						 break;
					
					// Tab
					case TAB: i = heap_access(i);
						  break;

					default: error("Synatx error");
				} 
				break;
			
			// Line feed	
			case LF: i = flow_control(i);
				 break;
		
			// Syntax error
			default: error("Syntax error");
		}
	}	
}


int main(int argc, char* argv[])
{	
	//read  in contents of provided file
	ifstream file(argv[1]);
	if(!file) error("File not found");

	if(file.is_open())
	{
		int i = 0;
		while(!file.eof())
		{
			file.get(text_chars[i++]);
			char_num++;
		}
	}
	else error("Could not open file");
	file.close();

	// clean out any non whitespace characters from array
	

	// iterate over array to identify commands
	identify_commands();

	for(auto &i : instructions) cout << i << endl;

	// write cpp eqivalent commands to a new file

	
	// make script to compile new file to wasm????	
	return 0;
}
