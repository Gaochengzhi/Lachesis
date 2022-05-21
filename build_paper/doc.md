# Documentation

Guides, tutorials, and reference material to help you learn more about Lachesis

------

## Installing Lac

### requirement

```
git
make
clang or gcc
libreadine-dev
```

### build

```
git clone https://github.com/Gaochengzhi/Lachesis
cd build_Lachesis
make install debug test
```

### set path

```
PATH="YOUR_PATH_TO_THIS_PROJECT/build_Lachesis/build:$PATH"
```

------

## Language Syntax

### S-expressions

```
(_input) >> + 1 1 1 1 1 1
(output) >> 6
```

Here + is no longer seen as a binary operator, but as a symbol for an addition function, which is the name of the function. Such a design disappears the advantage of Polish notation itself without parentheses when multiple operator expressions are involved. For example, the following expressions will be ambiguous without parentheses:

```
(_input) >> * + 1 1 3 2
(output) >> error!
(output) >> could be 10 ( [ * [+ 1 1 3] 2 ] ) or 12 ( [ * [ + 1 1 ] 3 2] )
```

So the evaluation statement of Lachesis is designed as a symbolic expression (English: SymbolicExpression, S-expression) using square brackets as delimiters.

```
(_input) >> [* + [1 1 3] 2]
(output) >> 10
```

## Q-expression

S-expressions are evaluated as soon as they are read, which leads to a number of limitations. For example, if the expression contains unknown variables, solving directly will throw an error:

```
(_input) >> + x 1
(output) >> error! x is undefined!
```

The unknown variable may be defined later. In this case, it is natural to want to delay the derivation of the expression and only keep its form (lazy evaluation). In this case, it is necessary to introduce a quoted expression (English: Quoted-Expression). , also translated as Q-expression) concept.

The only formal difference between Q-expressions and S-expressions is that they use a pair of curly braces as delimiters:

```
(_input) >> {+x1}
(output) >> {+x1}
```

The lazy evaluation feature of Q-expressions is widely used to pass information between programs, because it is not immediately deduced like S-expressions, resulting in the loss of information within the expression.

## Variable

Variable is a very misleading name because variables in functional languages are actually immutable and cannot be changed once initialized. Repeating the definition of the symbol again will reallocate the memory.
The concept of variables in Lachesis is more like a symbolic binding. Lachesis passes the name of the variable through a Q-expression:

```
(_input) >> def {_var_name} 1
(_input) >> _var_name
(output) >> 1
```

Lachesis' def function can initialize multiple variables at once like golang or Python:

```
(_input) >> def {x y z} 1 2 3
(_input) >> print x y z
(output) >> 1 2 3
```

In functional languages, S-expressions can also be variables, but S-expressions are passed in the form of Q-expressions, and the eval function is used to convert Q-expressions into S-expressions when deduction is required.

```
(_input) >> def {_add_one} {+ x 1}
(_input) >> def {x} 1
(_input) >> eval _add_one
(output) >> 5
```

## lambda functions

Lambda function is a common core concept of functional languages. It refers to a type of function that does not need to define an identifier (function name). In Lachesis, it is a function used to define functions.
Binding a variable name to an expression does not express the concept of a function very well. An obvious disadvantage is that the variable name is exposed to the outside world. The lambda function solves this problem.

```
(_input) >> def {add_two} [\{x y}{+x y}]
(_input) >> [ add_two 1 2 ]
(output) >> 3
```

A Lambda function is formally an S-expression with "\" as the function name, one Q-expression as the parameter list, and another Q-expression as the function body. Each lambda function is its own domain, and its internal variables are invisible to the outside world.
The number of parameters of the Lambda function is variable, with & as the identifier, followed by a symbol, any actual parameters will be attached to this symbol in the form of Q-expression.
E.g:

```
(_input) >> def {&exp}[\{_operator & _list} {eval [join {_operator } _list]}]
(_input) >> &exp + 1 2 3
(output) >> 6
```

The number of parameters of the Lambda function can also be less than the specified number, that is, the original function can be converted into a partial function. E.g:

```
(_input) >> def {partial_func} [\{x y z} {+ x y z}]
(_input) >> def {exp} [partial_func 1 2]

(_input)>>exp
(output) >> [\{z}{+x y z}]

(_input) >> exp 2
(output) >> 5
```

Definitions of Lachesis utility functions

The above are the core syntax concepts of Lachesis. Lachesis also defines some preset utility functions, which are listed as follows:

|Interface| Description |Example|
|----|----|----|
|[+-*/==!=><≥≤] <sexpr>* |common numeric operators and comparators| * 1 5 => 5|
|head <qexpr> =><qexpr> |returns the first element of the expression | head {1 2 3} => {1}|
|tail <qexpr> =><qexpr> |returns the expression except the first element | tail {1 2 3} => {2 3}|
|list <sexpr> =><qexpr> |converts S-expressions to Q-expressions|  list [+ 1 2] => {+ 1 2}|
|join <qexpr> =><qexpr> |joins multiple Q-expressions | join {+} {1 2} => {+ 1 2}|
|if <bool><qexpr><qexpr> =><sexpr> |branch judgment | if [== x y] {+ x y} {- x y}|
|print <string>* =><string> |print multiple strings | print "hello world" => hello world|
|fprint <file_name><string>* =><string> |print multiple strings to file | fprint "new.txt" "hello world" => new:hello world|
|import <string>* |import other lachesis files | import "stdlib"|
|loop <num><qexpr> |Execute the command num times | Loop 2 {print 1} => 1 1|
