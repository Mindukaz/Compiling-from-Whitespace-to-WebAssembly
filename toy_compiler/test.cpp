#include <stack>
#include <iostream>

int main()
{
	std::stack<int> stack;
	stack.push(0);
	stack.push(1);

	while(!stack.empty())
	{
		std::cout << stack.top() << std::endl;
		stack.pop();
	}
}
