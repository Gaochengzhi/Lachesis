# Lachesis

[![Linux CI Build Status](https://github.com/crystal-lang/crystal/workflows/Linux%20CI/badge.svg)](https://github.com/crystal-lang/crystal/actions?query=workflow%3A"Linux+CI"+event%3Apush+branch%3Amaster)[![macOS CI Build Status](https://github.com/crystal-lang/crystal/workflows/macOS%20CI/badge.svg)](https://github.com/crystal-lang/crystal/actions?query=workflow%3A"macOS+CI"+event%3Apush+branch%3Amaster)

------------

```
 █████                          █████                        ███            
░░███                          ░░███                        ░░░             
 ░███         ██████    ██████  ░███████    ██████   █████  ████   █████    
 ░███        ░░░░░███  ███░░███ ░███░░███  ███░░███ ███░░  ░░███  ███░░     
 ░███         ███████ ░███ ░░░  ░███ ░███ ░███████ ░░█████  ░███ ░░█████    
 ░███      █ ███░░███ ░███  ███ ░███ ░███ ░███░░░   ░░░░███ ░███  ░░░░███   
 ███████████░░████████░░██████  ███ █████░░██████  ██████  █████ ██████    
░░░░░░░░░░░  ░░░░░░░░  ░░░░░░  ░░░░ ░░░░░  ░░░░░░  ░░░░░░  ░░░░░ ░░░░░░     
                                                                          
```

Lachesis is an experimental functional programming language designed and developed for my senior thesis. It has a clean and modern syntax similar to lisp language family. 

with the following goals:

* Full lambda calculus capability supported.

* Have a syntax similar to conventional programming language like Lisp (but compatibility with it is not a goal)

  

## Installation

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

### clean

```
make clean
```

## run

The basic usage is similar to most interpreter language like python and nodejs.

```
lac // interactive mode
lac file1.lac file2.lac //file mode
```

You can pass some command-line argument use`--`, like lac `--std` .

We currently support three arguments

* -g :debug
* -h:help
* --std: load stdlib

## Basic syntax

| Syntax                                  | Decription               | **Example**                                        |
| --------------------------------------- | ------------------------ | -------------------------------------------------- |
| [+-*/==!=><≥≤]  <sexpr>*                | 常见数字运算符和比较符   | *  1 5 => 5                                        |
| head  <qexpr> =><qexpr>                 | 返回表达式第一个元素     | head  {1 2 3} => {1}                               |
| tail  <qexpr> =><qexpr>                 | 返回表达式除了第一个元素 | tail  {1 2 3} => {2 3}                             |
| list  <sexpr> =><qexpr>                 | 将S表达式转换成Q-表达式  | list  [+ 1 2] => {+ 1 2}                           |
| join  <qexpr> =><qexpr>                 | 合并多个Q-表达式         | join  {+} {1 2} => {+ 1 2}                         |
| if  <bool><qexpr><qexpr> =><sexpr>      | 分支判断                 | if  [== x y] {+ x y} {- x y}                       |
| print  <string>* =><string>             | 打印多个字符串           | print  "hello world" => hello world                |
| fprint  <file_name><string>* =><string> | 向文件打印多个字符串     | fprint  "new.txt" "hello world" => new:hello world |
| import  <string>*                       | 导入其他lachesis文件     | import  "stdlib"                                   |
| loop  <num><qexpr>                      | 循环执行num 次命令       | Loop 2 {print 1} => 1 1                            |



## Contributing to Lachesis

Contributions to Lachesis are welcomed and encouraged! 

You can either add compile time built-in functions in `lachesis_builtin.h/c` file，and register them in `lachesis_environment.c` file or write runtime standard library in `stdlib` file.

## Learning More

The detail of  lachesis language and its interpreter in UNIX platform are elaborated in `build_paper` folder. Please read it carefully before you start to develop this project.
