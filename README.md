# Requirements

To succesfully compile Whitespace to WebAssembly wat2wasm needs to be installed. It can be found in the [Wabt](https://github.com/WebAssembly/wabt) toolkit.

# Compiling the Compiler

There is a Makefile. Use it. It wil create the program `wswac` (Whitespace WebAssembly Compiler) in the top level directory.

# Running the compiler

## Compiler behaviour

The compiler will take both Whitespace source files and Whitespace text source files, and produce files in their respective folders. 

Whitespace source files will go into `ws/`.

Whitespace text source files will go into `wst/`.

Webassembly text files, JavaScript and HTML files will go into `wasm/`.

## Compiling to WebAssembly

To create WebAssembly files, run the compiler with either a `ws` or `wst` file like this:

`wswac file.ws` or  `wswac file.wst`

Then to convert the wat file to wasm : 

`wat2wasm file.wat`

Finally, load the HTML file into a browser via HTTP client and enter the browser console.

## Translating between Whitespace and Whitespace text

To translate a Whitespace text format file to normal Whitespace, run the following command: 
`wswac -wst2ws file.wst`

Likewise to translate a Whitespace file to a Whitespace text file, run this command:
`wswac -ws2wst file.ws`

# Langauge Specification

`S = Space`

`T = Tab`

`¬ = Line Feed`

## Stack Manipulation

| Description        | Whitespace        | Whitespace Text |
|----------|:-------------:|:--------:|
| Push item onto the stack   | `SS` | `push <value>` |
| Make a copy of the top item | `S¬S`     |  `duplicate` |
| Retrieve an item from further inside the stack | `STS`    |   `copy <value>` |
| Swap the top two items on the stack | `S¬T`     |    `swap` |
| Remove the top most item on the stack | `S¬¬`     |   `discard` |
| Pop the top item of the stack. Remove the specifies number of items. Push the item back onto the stack  | `ST¬`      |    `slide <value>` |

## Arithmetic

| Description        | Whitespace        | Whitespace Text |
| ------------- |:-------------:|:-----:|
| Add top two items on the stack and push the value | `TSSS` | `add` |
| Subtract the top item of the stack from the second top item, and push the result     | `TSST`     |  `sub` |
| Multiply the top two items on the stack and push the result | `TSS¬`   |  `mul` |
| Divide the second top item on the stack by the top item on the stack and push the result | `TSTS`     |   `div` |
| Push the remainder of dividing second top item on stack by top item on stack | `TSTT`   |    `mod` |

## Heap Access

| Description        | Whitespace        | Whitespace Text |
| ------------- |:-------------:|:-----:|
| Top stack item is stored at address identified by second top item on stack    | `TTS` | `store` |
| Top of stack identifies address, from which to load item and push it onto stack   | `TTT`   |   `retrieve` |

## Flow Control

| Description        | Whitespace        | Whitespace Text |
| ------------- |:-------------:|:-----:|
| Create a label   | `¬SS`| `mark <label>` |
| Call a subroutine / Jump to a label than will jump back to the caller after executing a return   | `¬ST`   |  `call <label>` |
| Jump to a label | `¬S¬`  |  `jump <label>` |
| Jump to a label if top of stack is zero | `¬TS`   |  `jumpz <label>` |
| Jump to a label if top of stack is a negative | `¬TT`   |  `jumpn <label>` |
| Return to the caller of a subroutine | `¬T¬`   |  `return`  |
| End the program | `¬¬¬`     |  `end` |

## I/O

| Description        | Whitespace        | Whitespace Text |
| ------------- |:-------------:|:-----:|
| Output a character from the top of the stack  | `T¬SS` | `out_char` |
| Output a number from the top of the stack      | `T¬ST`    |   `out_num` |
| Read in a character and store it at the address on the top of the stack | `T¬TS` |   `read_char` |
| Read in a number and store it at the address on the top of the satck | `T¬TT`     | `read_num`   |

# Whitespace programming restrictions

There cannot be duplicate label names. This includes subroutines as they are defined by labels.

## Flow Control 
Becasue of how WebAssembly handles flow control, the Whitespace flow control has to be restricted. 

**Jumps can only be made to the next label and the previous label. NO JUMPING TO ANY OTHER LOCATIONS**. Does not apply to subroutine calling.

Example in Whitespace Text:


If A is a jump 
```
mark 1
...
mark 2
...
A
...
mark 3
...
mark 4
...
```
Then A can legally jump to labels 2 and 3 but *not* to labels 1 and 4.

## Subroutines
Because of the flow control restrictions, subroutines have to be in their own section after the `end` instruction, away from the main body of code.

```
...
push 5
...
call 2
...
...
end

mark 1
...
...
...
return

mark 2
...
...
return
```
