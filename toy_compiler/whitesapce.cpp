#include "instructions.h"
std::string last_call;
char dynamic_label = 'A';
int main()
{
stack_push(0);
stack_push(127);
stack_push(15);
stack_copy(2);
stack_slide(2);
stack_push(255);
stack_push(-810);
stack_push(-1);
stack_duplicate();
stack_push(7);
stack_swap();
stack_discard();
stack_discard();
stack_discard();
stack_push(4);
stack_push(7);
arithmetic_add();
stack_push(8);
arithmetic_sub();
stack_push(8);
arithmetic_mul();
stack_push(8);
arithmetic_div();
stack_push(10);
stack_push(7);
arithmetic_mod();
heap_store();
stack_push(3);
heap_retrieve();
stack_push(1);
stack_push(127);
heap_store();
label_1:
stack_push(1);
arithmetic_sub();
stack_duplicate();
if(stack.top() == 0) goto label_11;
goto label_1;
label_11:
stack_duplicate();
if(stack.top() < 0) goto label_111;
stack_discard();
goto label_11;
label_111:
last_call = "label_" + dynamic_label++; goto label_1111;
label_B:
return 0;
label_1111:
stack_push(62);
io_out_char();
stack_push(1);
io_in_char();
stack_push(62);
io_out_char();
stack_push(2);
io_in_num();
io_out_num();
goto  last_call;
}
