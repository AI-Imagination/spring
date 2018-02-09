# spring: another programming language

This the design for the programming language, currently, it is just a demo.

# syntax

spring language will give a simple and familiar syntax for `C` and `Python` programmers, it will avoid obscure grammars.

## basics

```js
# import a library
import math
import math.real as realmath

# declare variables 
var a = 1
var b = a + 1
```

## conditional operations

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

## function

```python
def func(arg0, arg1) {
	...
	return res
}
```

## Lexier

Input a text stream, output a token stream, the tokens are basic element of codes, such as `if` or `12.3`.

The main interface

```c++
class TokenStream {
    Token NextToken();
    void IgnoreSpace();
};
```

by calling `NextToken()`, one will get a list of tokens.

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

## block

Any codes inside a `{}` is a block, a block has its own scope, and a pointer to parent block.

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
};
// function definition
struct Function {
    vector<Variable> const_vars;
    Block* block;
}
struct FunctionCall {
    vector<Variable*> args;
    uint32_t function;
};
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
struct Variable {
    VarType type;
    VarData data;
}
struct Scope {
    map<string, uint32_t> varnames;
    vector<Variable> variables;
}
struct Block {
    Block* parent;
    Scope* scope;
    vector<Instr> instrs;
    string content;
}
```

