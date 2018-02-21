# spring: another programming language

This the design for the programming language, currently, it is just a demo.

## syntax

spring language will give a simple and familiar syntax for `C` and `Python` programmers, it will avoid obscure grammars.

### basics

```js
# import a library
import math
import math.real as realmath

# declare variables
var a = 1
var b = a + 1
```

### conditional operations

```js
# if else
if (a > 10) {
    ...
} elif ( a > 0) {
    ...
} else {
    ...
}

# while
while (a > 0) {
    ...
}
```

### function

```python
def func(arg0, arg1) {
	...
	return res
}
```

### Lexier

Input a text stream, output a token stream, the tokens are basic element of codes, such as `if` or `12.3`.

The main interface

```c++
class TokenStream {
    Token NextToken();
    void IgnoreSpace();
};
```

by calling `NextToken()`, one will get a list of tokens.



## several concepts

### Variable

A variable can store any type of data, the `int`,`float`,`string` and `bool` are basic data types, to represent a complex data, `object` will be used.

```c++
enum class VarType {
    kInt = 0,
    kFloat,
    kString,
    kBool,
    kObject
}
union VarData {
    int32_t intv;
    float floatv;
    bool boolv;
    string* stringv;
    ObjectV* object;
}
struct Variable {
    VarType type;
    VarData data;
}
```

An Object represent a complex data, such as an instance of a class, the `Object` is the class definition, the `ObjectV` is instance of a `Object`.

```c++
struct Object {
    string name;
    int id;
	vector<Variable> static_members;
    vector<uint32_t> funcs;
}
struct ObjectV {
    uint32_t obj;
    vector<Variable> members;
}
```

### Block

Any codes inside a `{}` is a block, there are 3 kinds of blocks

- a block which has a local symbol-table.
- unnecessary blocks whose instructions can be merged into parent block.
- a function definition, that has a block as shared code snippet.

a block has its own symbol-table, and a pointer to parent block.

```c++
struct Block {
    Block* parent;
    SymbolTable* symbol_table;
    vector<Instr> instrs;
    string content;
}
```

### SymbolTable

A SymbolTable represent the scope where the variables are valid, each block has a scope of its self, and a block can visit parent's scope if local scope doesn't has some variables.

```c++
struct SymbolTable {
    map<string, uint32_t> varnames;
    vector<Variable> variables;
}
```

### Function

A function is an reusable block, with a list of arguments (might be represented as a `Scope`).

The function's definition is

```c++
// function definition
struct Function {
    vector<Variable> const_vars;
    Block* block;
}
```

A function call is

```c++
struct FunctionCall {
    vector<Variable*> args;
    uint32_t function;
};
```

### Instruction

Instruction is the underlying executation unit of an interprinter, the Parser will translate ASTs to instructions.

```c++
enum class Instr {
    LOAD_CONST = 0,
    LOAD,
    EQ,
    ADD,
    MUL,
    MOD,
    RETURN
}
```

## Parser

Parser takes a list of `token` as input and output a sequence of instructions.

an AST is a tree composed by nodes, each expression can be transformed to a unique AST,

for example, the expression `a = b+c*3` is just a tree as follows

```
           =
         /   \
       a      +
            /   \
           b     *
                /  \
               c    3
```

The expressions such as function call might need recursive parsing, `func1(a, a+b*c)` will be parsed in the following phases:

- parse `func1(...)`and get arguments

  - parse `a`, get a AST with a single node `a`

  - parse `a+b*c`, get an AST like

    ```
          +
        /   \
       a     *
            /  \
           b    c
    ```

an AST can be translated into a sequence of instructions, the above AST does the following operations:

```
load a
load b
load c
mul    # consume two datas from data stack, and push the result of b*c to data stack
add    # consume two datas, a, b*c, and push the result to data stack
# finally get the result
```

This can be done by a post-order traversal in the AST.

### parse algorithm

#### basic expression

1. initialize with nodes that each token is a node
2. merge the nodes inside pairs of parentheses, and build sub-AST, insert back to the node list.
3. keep parsing the most prior operators and consume the arguments besides them.
4. after all, there remains a list of nodes that share the same priority, parse from the left to right.

#### function

1. parse the definition, such as `function add(a, b)`
2. consume block which is inside a `{..}`
3. function definitions will be registered to a global dict.

#### block

A block is the code snippet inside a pair of `{…}`, operations like `if-else`, function can have one or more blocks.

When parsing a block

1. consume the left brace `{`
2. Keep consuming the expressions.
3. get the right brace `}`, end

### AST to instructions

The instructions are a list of operators with arguments, such as

```
push 2
push 3
add
push 2
mul # get 6 = (2+3)*2
```

The Parser will get ASTs and need to generate instructions based on them.

The expression `(2+3)*2` will generate an AST

```
          		*
          	  /   \
          	 +     2
          	/ \
           2   3
```

With the **post-order retrival**, it will get the visit sequence:

- 2  -> push 2
- 3  -> push 3
- `+` -> add
- 2 -> push 2
- `*` -> mul


## Executor

Input the instructions, and execute them.

The executor has following data structures

```c++
class Executor {
 public:
    Run();
 private:
    Frame main_frame;
    deque<Frame> frame_stack;
}
```

It has one `main_frame` to execute the main function, and `frame_stack` to help execute function calls, for example

```c++
a = func0(b, c)
```

will trigger a frame call

```
LOAD b
LOAD C
FUNC func0		# consume two variables
				# push func0 frame to frame_stack
				# run this frame
				# when frame ends, push the remaining variable to
				# main_frame's data_stack (return result)
```



### Frame

Frame helps to execute a sequence of instructions, it looks like

```c++
struct Frame {
    void Run();
    void PushData(Variable&& var);
    void RunInst();

    Frame* parent;				// push result to parent frame
   	Scope* parent_scope;
    Scope local_scope;
    deque<Variable> data_stack;
    Block* block;				// share one block
}
```



### block with conditional operations

Conditional operations relays on block to selectively execute some block, for example

```c++
if (a > 2) {
	# block 1
    ...
} else {
    # block 2
}
```

There should be some instructions to help select the block to run, for example, the above code can generate the following instructions

```
LOAD a
PUSH_CONST 2
COMP_GT 			# get a bool
IF_ELSE_GOTO x1 x2
...					# block1, start offset x1
GOTO x3
...					# block2, start offset x2
GOTO x3
...					# instruction after if-else, offset x3
```

```
while (a > 2) {
    # block 3
    ...
}
```

corresponds to

```
LOAD a 					# instruction offset: x0
PUSH_CONST 2
COMP_GT
IF_FALSE_GOTO	(x1+1)	# jump out the while loop
...						# instructions of block3
WHILE_TRUE_GOTO X0		# instruction offset: x1
```
