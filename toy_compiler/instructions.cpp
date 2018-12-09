#include <stack>
#include <vector>
#include <iostream>
std::stack<int> stack;
std::vector<int> heap;

void stack_push(int value)
{
	stack.push(value);
}

void stack_duplicate()
{
	stack.push(stack.top());
}

void stack_copy(int n)
{
	std::vector<int> temp;
	if(n > stack.size() || n < 1) stack.push(0);
	else
	{
		while(n != 0)
		{
			temp.push_back(stack.top());
			stack.pop();
			n--;
		}
		int val = temp.back();
		for(int i = temp.size() -1; i >= 0; i--) stack.push(temp[i]);
		stack.push(val);
	}
}

void stack_swap()
{
	int temp1 = stack.top();
	stack.pop();
	int temp2 = stack.top();
	stack.pop();
	stack.push(temp1);
	stack.push(temp2);
}

void stack_discard()
{
	stack.pop();
}

void stack_slide(int n)
{
	int temp = stack.top();
	stack.pop();
	for(int i = 0; i < n; i++) stack.pop();
	stack.push(temp);
}

void arithmetic_add()
{
	int temp1 = stack.top();
	stack.pop();
	int temp2 = stack.top();
	stack.pop();
	stack.push(temp2 + temp1);
}

void arithmetic_sub()
{
	int temp1 = stack.top();
	stack.pop();
	int temp2 = stack.top();
	stack.pop();
	stack.push(temp2 - temp1);
}

void arithmetic_mul()
{
	int temp1 = stack.top();
	stack.pop();
	int temp2 = stack.top();
	stack.pop();
	stack.push(temp2 * temp1);
}

void arithmetic_div()
{
	int temp1 = stack.top();
	stack.pop();
	int temp2 = stack.top();
	stack.pop();
	stack.push(temp2 / temp1);
}

void arithmetic_mod()
{
	int temp1 = stack.top();
	stack.pop();
	int temp2 = stack.top();
	stack.pop();
	stack.push(temp2 % temp1);
}

void heap_store()
{
	int value = stack.top();
	stack.pop();
	int address = stack.top();
	stack.pop();
	if(address > heap.capacity()) 
	{
		while((heap.capacity()) < address)
		{	
			heap.push_back(0);
		}
	}
	heap[address] = value;
}

void heap_retrieve()
{
	int address = stack.top();
	stack.pop();
	if(address > heap.capacity()) stack.push(0);
	else stack.push(heap[address]);
}

void io_out_char()
{
	int temp = stack.top();
	stack.pop();
	char temp1 = (char)temp;
	std::cout << temp1;
}

void io_out_num()
{
	int temp = stack.top();
	stack.pop();
	std::cout << temp;
}

void io_in_char()
{
	char temp;
	std::cout << "Input a character:";
	std::cin >> temp;
	int temp1 = (int)temp;
	stack.push(temp1);
	heap_store();
}

void io_in_num()
{
	int temp;
	std::cout << "Input an integer:";
	std::cin >> temp;
	stack.push(temp);
	heap_store();
}
