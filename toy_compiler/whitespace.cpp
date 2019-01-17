#include "instructions.cpp"
static void* return_stack[100];
int stack_index = 0;
int cmp;
int main()
{
stack_push(0);
stack_push(10);
stack_push(33);
stack_push(100);
stack_push(108);
stack_push(114);
stack_push(111);
stack_push(87);
stack_push(32);
stack_push(111);
stack_push(108);
stack_push(108);
stack_push(101);
stack_push(72);
label_000000000000000000000000000000001:
stack_duplicate();
cmp = stack.top(); stack.pop();
if(cmp  == 0) goto label_000000000000000000000000000000010;
io_out_char();
goto label_000000000000000000000000000000001;
label_000000000000000000000000000000010:
return 0;
}
