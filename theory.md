> Why not to write a little about the theory, that i am using in my laboratory?

### Abstract Syntax tree.

First, we need to understand, that our language is more complex, than basic commands, like two, three or n static commands (this is called **syntax directed interpretators**). So we need to analyse this correctly and accordingly.
For this, we will build an *intermediate representation* (IR).
> Our parser will be responsible for building an IR and our intepreter will use it to interpret the input represented as the IR.\
> It turns out that a tree is a very suitable data structure for an IR.

Like a smarter guy said:

> So, what is an abstract syntax tree? An **abstract syntax tree** (AST) is a tree that represents the abstract syntactic structure of a language construct where *each interior node and the root node represents an operator*, and *the children of the node represent the operands of that operator*.

