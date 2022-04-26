# 目录

1. [目录](#目录)
2. [引言](#引言)
3. [Lachesis 解释器运用的计算理论](#lachesis-解释器运用的计算理论)
   1. [什么是计算？函数作为一种解释](#什么是计算函数作为一种解释)
   2. [lambda 演算：表示函数的最小逻辑系统](#lambda-演算表示函数的最小逻辑系统)
   3. [函数式编程：lambda 演算的图灵表述](#函数式编程lambda-演算的图灵表述)
4. [Lachesis 解释器的系统概述](#lachesis-解释器的系统概述)
   1. [Lachesis 语言设计](#lachesis-语言设计)
      1. [S-表达式](#s-表达式)
      2. [Q-表达式](#q-表达式)
      3. [变量](#变量)
      4. [lambda 函数](#lambda-函数)
      5. [实用方法](#实用方法)
   2. [解释器的设计](#解释器的设计)
      1. [工作模式](#工作模式)
   3. [](#)
5. [Lachesis 解释器的具体实现](#lachesis-解释器的具体实现)
   1. [容器](#容器)
   2. [语法分析器](#语法分析器)
   3. [函数抽象语法树的推导](#函数抽象语法树的推导)
   4. [函子、环境与变量](#函子环境与变量)
   5. [lambda](#lambda)
   6. [内建函数设计](#内建函数设计)
   7. [动态调试器模式和错误处理](#动态调试器模式和错误处理)
   8. [标准库设计](#标准库设计)
      1. [从匿名函数到实名函数](#从匿名函数到实名函数)
6. [系统结果分析](#系统结果分析)
   1. [测试用例](#测试用例)
7. [总结与展望](#总结与展望)
   1. [不足与缺陷](#不足与缺陷)
8. [致谢](#致谢)
9. [参考文献](#参考文献)
10. [附录](#附录)


<div STYLE="page-break-after: always;"></div>

<center>
<div style='height:2mm;'></div>
<div style="font-family:华文楷体-Regular-SC;font-size:14pt;">基于lambda 演算的函数式语言解释器的设计与实现</div>
</center>
<center>
<div style="font-family:华文楷体-Regular-SC;font-size:9pt;line-height:9mm">作者：高成志 &nbsp 指导老师：章林忠</div>
<span style="font-family:华文楷体-Regular-SC;font-size:9pt;line-height:5mm"> （安徽农业大学 信息与计算科学专业 &nbsp 学号：18112722）</span>
</center>






<div style="width:52px;float:left; font-family:方正公文黑体;"> <b>摘　要：</b></div> 
<div style="overflow:hidden; font-family:华文楷体-Regular-SC;"> 一个基于lambda 演算的函数式语言Lachesis 以及配套的解释器系统被设计出来。在解释器开发过程中，Lachesis 经历了几个阶段的变化，最终以表示一类符号表达式的lambda函数方案为基础，能够完成基本的求值、变量绑定、匿名函数等功能，并具有相当程度的可拓展性。<br/>在本文中，我们首先介绍了Lachesis 设计背后的计算理论和函数式语言的一般特点，然后描述了Lachesis 语言的关键特性和解释器设计的总体框架，最后展示了解释器实现中的一些技术细节。<br/>通过Lachesis 的开发，我们加深了对函数式编程的理解和……</div> 

<div style="width:52px;float:left; font-family:方正公文黑体;"><b>关键词：</b></div> 
<div style="overflow:hidden; font-family:华文楷体-Regular-SC;">计算理论；计算机语言；解释器；UNIX 编程；</div>





# 引言

> 如果你想从头做苹果派，你必须先创造宇宙——卡尔萨根（Carl Edward Sagan，1934年11月9日－1996年12月20日）【1】





~~定义一种语言的一种方法是编写标准规范文档，例如ECMAScrip，解释语言中允许的各种表达式以及它们是如何被推导的。这种技术的优点是，读者可以快速地吸收语言的一般概念，但通常很难从标准的描述中提取出语言的细节。~~

~~另一种方法是用某种元语言为它实现一个解释器。这种技术的优点是能够清楚完整地指定语言的细节，否则它就不可能被造出来。假设元语言有一个形式化的规范，那么解释器在该语言中就有一个形式化的含义，可以对其进行分析。~~

~~用于定义另一种语言的元语言不需要高效地执行，因为它的主要目的是向开发者解释另一种语言。元语言的基本数据构造也不需要用位和字节来定义。事实上，我们可以直接使用元语言的逻辑和集合理论来处理整个程序文本。~~

~~本文要实现一个函数式语言解释器，叫做Lachesis，取名自古希腊神话的命运三女神之一。~~

~~Lachesis 实现了一些函数式语言的最小功能集合，包括表达式求值，变量绑定，函数等。同时又有着极强的可拓展性，方便后来的开发者和用户添加新的功能和函数。~~



# 计算理论介绍

## 什么是计算？函数作为一种解释

首先，计算科学的抽象理论至少应该回答以下问题——如何处理“计算”的概念。对这个概念的解释有很多数学模型，基于有限状态机理论的图灵机可能是其中最重要的一个。从某种角度来看，这个模型是相当具体的，因为它建立了一个特定的计算机制，所以我们甚至可以观察到它在物理上是如何施行的。但是，图灵机模型限制了我们对计算的想象力，导致几乎所有的现代计算机都必须以相同的方式进行计算。

要克服这个限制，最好的方法是绕过计算的物理机制定义，仅仅根据输入和输出信息之间的关系来描述计算过程的概念。

关于计算应该满足的等价性的直觉告诉我们：如果一个问题可以通过一台计算机来解决，那么也就可以通过另一台计算机来解决。为了阐明这一点，我们把计算机的工作看作是计算函数，而计算就是一种调用可计算函数的过程。



## lambda 演算：表示函数的最小逻辑系统

为了表达函数的概念，数学家阿隆佐·邱奇（Alonzo Church）在 1930 年代发明了Lambda 演算，它是一个能用来表示函数本质的最小逻辑系统。

最基本的Lambda 演算系统包括

1. 构造 lambda 项（term）
2. 对lambda 项执行的归约操作（reduction operations）

Lambda 演算的**项**由以下三个规则定义：           

|      语法       |   名称   | 描述                                                        |                  举例                   |
| :-------------: | :------: | :---------------------------------------------------------- | :-------------------------------------: |
|        x        |   变量   | 一个变量 x 本身就是一个$\lambda$  项。                      |               x、y、plus                |
| ($\lambda$ x.M) | 函数抽象 | x为变量，M为函数体，也是一个$\lambda$ 项。                  |          $\lambda$ x.plus x x           |
|       MN        | 函数应用 | 如果 M 和 N 都是$\lambda$ 项，那么 MN也是一个$\lambda$ 项。 | ($\lambda$ x .plus x x) y => (plus y y) |

或者用更简洁的巴科斯范式表示为：

$$ \begin{array}{lll}
M, N, L& = \quad X \\
& |\quad(\lambda X . M) \\
& |\quad(M N) \\
X & = & \text { a variable: } x, y, \ldots
\end{array} $$

重复应用上述三个规则就可以得到所有的$\lambda$ 项。

Lambda 的**规约操作**则有两种：

|                   操作                   |     名称      |                     描述                      |
| :--------------------------------------: | :-----------: | :-------------------------------------------: |
| ($\lambda$ x.M[x]) -> ($\lambda$ y.M[y]) | $\alpha$ 转换 | 重命名表达式中的绑定变量，用于防止命名冲突。  |
|     (($\lambda$ x.M) E) -> (M[x:=E])     | $\beta$ 规约  | 用参数替换表达式中函数抽象（M）中的绑定变量。 |

如果能避免命名冲突（例如之后我们在解释器的实现中巧妙地做到的那样），那么$\alpha$ 转换就不是必须的。而不断运用$\beta$ 规约所得到的最终结果将是一个$\beta$ 规范型，这种情况下将不能再进行规约操作。

Lambda 演算是图灵完备的，这意味着它有着和一般的计算机语言相当的表达能力，但却是用lambda 自身的编码形式。

光看lambda 演算的形式定义未免感到无所适从，我们来举一个例子演示lambda 演算到底是如何工作的。

布尔代数是一组元素（通常为两个，“TRUE”与“FALSE”）和服从定义的公理在这些元素上的运算（通常为与、或、非）。在lambda 演算中，“TRUE” 被编码为接受两个参数并返回第一个，而“FALSE”则是接受两个参数并返回第二个，具体来说就是：

$$TRUE \quad := \quad \lambda{x}.\lambda{y}.{x} \\ {FALSE \quad : = \quad \lambda{x}.\lambda{y}.{y}}$$

相对于C 语言中常用的宏定义（ `TRUE=1 & FALSE=0 ` ）来说，这个对布尔型的定义抽象得令人无法理解，为什么“TRUE”是第一个？难道就不能是第二个吗？

答案是完全可以。事实上，这个定义恰好满足了布尔型的最小条件，即集合论断言子集$A$和它的补集$A^C$的交集为空集：

$$A \cap (A^C) = \emptyset $$

从这个角度上看，C 语言通过宏来定义布尔型的方式反而是不严谨的，因为{1}的补集不是{0}，而是除了1之外的所有整数的集合。

接下来我们可以编码布尔代数上的运算，例如非（NOT）定义为：

$$NOT\quad :=\quad \lambda{a}.{a}\quad FALSE\quad TRUE$$

如果我们使用非进行运算，接收 TRUE 作为参数，计算过程如下 ：

$$ \begin{array}{lll}
\overbrace {NOT}^{函数}\quad \overbrace {TRUE}^{参数}& = \quad \overbrace{(\lambda{b}.b\enspace FALSE \enspace{TRUE})}^{函数} \enspace \overbrace{TRUE}^{参数}    \\
& = \quad (TRUE) \enspace {FALSE}\enspace {TRUE} \\
& =\quad (\lambda{x}.\lambda{y}.{x})\enspace {FALSE} \enspace {TRUE} \\
& = \quad FASLE
\end{array} $$

其结果也是一个$\beta$ 规范型，且符合我们通常对布尔非运算的一般认识。

总而言之，lambda 演算给了我们机制和信心，能够只用函数计算解决其他指令式编程语言（基于图灵有限状态机模型）所能解决的事情。

## 函数式编程：lambda 演算的图灵表述

相对于我们所熟悉的指令式编程，函数式编程是一种完全不同的编程范式，它将计算视为函数的求值，从而避免了状态变更和使用可变数据。它的特点有：

* 函数是"一等公民"：与其他数据类型处于平等地位，可以赋值给其他变量，也可以作为参数传入另一个函数，或者作为别的函数的返回值。
* 只用“表达式”，不用“语句”：每一步都是单纯的运算，而且都有返回值。
* 没有“副作用”：函数要保持独立，所有功能就是返回一个新的值。
* 



在函数式编程中，lambda演算（也就是匿名函数）被用作一个函数生成器：

```lisp
let multipl = λxλy.x*y 
let conbine = λxλy.xy
;;; apply and eval
multipl 2 3 -> 2 * 3
conbine ‘BYV’ ‘oid’ -> ‘BYVoid’
```

这为函数成为一等公民（和其他类型没有区别）提供了实现手段和理论依据。

考虑到大多数函数式语言要运行在依据图灵有限状态机模型设计出来的硬件上，这些硬件对基本的数据操作做了相当多的优化工作，因此在计算的最终阶段（例如`2 * 3`）依然使用lambda 演算系统表达是十分不明智的。
$$
\begin{array}{lll}
2 * 3 & = MULTIPLY \enspace 2\enspace3 \Rightarrow \overbrace{(λ abc.a(bc)) }^{MULTIPLY} \overbrace{(λ sz.s(s(z)))}^{2} \overbrace{(λ xy.x(x(x(y)))) }^{3} \\
& = λ c.(λ sz.s(s(z)))((λ xy.x(x(x(y))))c) \\
& = λ cz.((λ xy.x(x(x(y))))c)(((xy.x(x(x(y))))c)(z)) \\
& = λ cz.(λ y.c(c(c(y)))) (c(c(c(z)))) \\
& = λ cz.c(c(c(c(c(c(z)))))) \\
& = 6 \\
\end{array}
$$

$$
\begin{array}{lll}
2*3 & = MUL \enspace 00010   \enspace 00011 \\
& = ……经过硬件加速的原码一位乘法 \\
& = 6
\end{array}
$$
因此，虽然函数编程语言最重要的基础是lambda 演算，但在具体实现上却也离不开图灵有限状态机。可以说，我们是在用指令式的工具表达函数式的思想。

函数式编程的另一个有争议性的来源是范畴论，这么说的原因是几大函数式语言（lisp、Haskell、Scala）在开发时并没有严格考虑过范畴论，但范畴论中的一些理论恰好能解释函数式编程的很多现象。

范畴论视角下的计算是范畴之间的转换，范畴则是包含了对象及对象之间关系的代数结构。在函数式语言的具体实现中，我们往往会用一个包含了数据和方法的容器来表达范畴的概念。



# Lachesis 语言设计

## S-表达式

表达式求值是程序语言要解决的基本问题之一，例如：

```
(_input) >> 1 + 1
(output) >> 2
```

事实上，鉴于绝大多数函数抽象语法树（我们的也不例外）采用的是波兰表达式的记号方法，为了方便推导，我们索性将Lachesis 也设计成波兰表示法的，也就是操作符置于操作数的前面的表示方法。

```
(_input) >> + 1 1
(output) >> 2
```

这样的好处是可以通过操作符赋值法轻松地实现可变参数个数的运算：

```
(_input) >> + 1 1 1 1 1 1
(output) >> 6
```

这里的 `+` 不再被看成一个二元运算符，而是一个表示加法函数的符号，也就是函数的名字。

这样的设计在涉及多运算符表达式时波兰表示法自身不需要加括号的优点就消失了，例如以下表达式不使用括号会产生歧义：

```
(_input) >> * + 1 1 3 2
(output) >> error!
(output) >> could be 10 ( [ * [+ 1 1 3] 2 ] ) or 12 ( [ * [ + 1 1 ] 3 2] )
```

所以Lachesis 的求值语句被设计成使用方括号作为界定符的符号表达式（S-表达式）



## Q-表达式

S-表达式一旦被读入就会开始求值，有时候我们并不想这样，例如表达式中含有未知变量时直接求解会抛出错误：

```
(_input) >> + x 1 
(output) >> error! x is undefined!
```

而未知变量可能在之后定义，在这种情况下我们希望延迟表达式的推导而仅仅保留其形式（惰性求值），这时我们就需要 Q-表达式，用一对花括号定义：

```
(_input) >> {+ x 1}
(output) >> {+ x 1}
```

Q-表达式惰性求值的特性广泛运用在程序间传递信息，因为它不像S-表达式那样是转瞬即逝的。

##  变量

变量是一个很有误导性的名字，因为函数式语言中的变量实际上是不变的，一旦初始化后就不能更改。

Lachesis 中的变量概念更类似一个符号绑定，我们可以通过Q-表达式来传递变量的名字：

```
(_input) >> def {x} 1 
(_input) >> x
(output) >> 1
```

我们设计的def 函数可以像golang 或Python那样一次给多个变量初始化：

```
(_input) >> def {x y z} 1 2 3
(_input) >> print x y z
(output) >> 1 2 3
```

函数也可以是变量，函数的形式用Q-表达式传递，需要时再用`eval`函数将Q-表达式转换成S-表达式并立刻推导。

```
(_input) >> def {func_name} {+ x 1}
(_input) >> def {x} 1
(_input) >> eval func_name
(output) >> 5
```

## lambda 函数

Lachesis 中的lambda 函数在具体实现上被设计成一个重写系统，但这不是现在要考虑的问题。

将变量名绑定在一个表达式上的做法并不能很好地表达函数的概念，一个显而易见的缺点是会把变量名暴露参数到外部，lambda 函数则解决了这个问题。

```
(_input) >> def {add_two} [\ {x y} {+ x y}]
(_input) >> [ add_two 1 2 ]
(output) >> 3
```

lambda 函数以`\` 为函数名，一个Q-表达式作为参数列表，一个函数类型的S -表达式为函数体。lambda 函数的变量在外部是不可见的，这有助于实现函数式语言纯函数的特性。

## 实用方法

以上就是Lachesis 的核心语法概念，它简单得简直不像话，我们还需要定义一些程序内置的实用方法，如下：

| 接口                                   | 描述                     | 举例                                 |
| :------------------------------------- | :----------------------- | :----------------------------------- |
| `[+-*/==!=><≥≤] <sexpr>*`              | 常见数字运算符和比较符   | `* 1 5 => 5`                         |
| `head <qexpr> => <qexpr>`              | 返回表达式第一个元素     | `head {1 2 3} => {1}`                |
| `tail <qexpr> => <qexpr>`              | 返回表达式除了第一个元素 | `tail {1 2 3} => {2 3}`              |
| `list <sexpr> => <qexpr> `             | 将S表达式转换成Q-表达式  | `list [+ 1 2] => {+ 1 2}`            |
| `join <qexpr> => <qexpr>`              | 将多个Q-表达式合并成一个 | `join {+} {1 2} => {+ 1 2}`          |
| `print <string>* => <string>`          | 打印多个字符串           | `print "hello world" => hello world` |
| `if <bool> <qexpr> <qexpr> => <sexpr>` | 分支判断                 | `if [== x y] {+ x y} {- x y}`        |
| `import <string>*`                     | 导入其他文件             | `import "stdlib"`                    |









# 解释器设计

## 工作模式

本系统支持现代解释器通常支持的两种工作模式，一个是交互式提示符（interactive prompt）式的读取-求值-输出循环（RELP：Read–eval–print loop），用户在命令行直接输入指令后马上返回结果。另一个则由用户事先写好脚本文件，解释器依次读取脚本中的内容并执行。

![inter](/Users/kounarushi/Desktop/paper/build_paper/inter.png)

（两种工作模式）

在UNIX/Linux 设计哲学中，一切皆是文件，用户输入流不过是一个名为”stdin“的文件罢了。因此，除了在输入的捕获阶段要求开发者作不同的处理外，无论是普通文本类型的文件还是用户在终端的输入，都能使用同一套处理逻辑。



![rad](/Users/kounarushi/Desktop/paper/build_paper/rad.png)

## 系统框架



![未命名文件 (1)](/Users/kounarushi/Desktop/paper/build_paper/未命名文件 (1).png)解释器的总体结构如 图上所示，整个解释器以main 函数为程序起点，检查输入的命令行参数后进入相应的工作模式，调用其它模块完成演算任务。图中各模块功能简要介绍如下 :

1. 环境模块（environment）：创建和初始化环境容器，添加符号和函数定义。
2. 内置函数模块（builtin）：系统保留字和预置函数的开发接口，方便未来功能拓展和二次开发。
3. 容器操作模块（lobj）：对Lachesis 对象的一系列初始化、转换、读取、求值、函数调用。
4. 语法解析器和词法分析器模块（mpc）：对输入字符流的捕获和抽象语法树的建立。
5. 调试模块（debug）：打印动态调试信息和用于处理语义异常的函数包。
6. 命令行交互模块（headline）：检查输入参数、导入标准库和帮助系统。
7. 标准库（stdlib）：用Lachesis 语言自身定义的一些实用函数。
8. 日志文件（log）：动态调试信息的默认输出区。



# 解释器实现细节



## 容器



```c
LObject *lobj_number(long number);
LObject *lobj_error(char *fmt, ...);
LObject *lobj_symbol(char *symbol);
LObject *lobj_string(const char *string);
LObject *lobj_sexpr(void);
LObject *lobj_qexpr(void);
……
```

一个枚举类型`enum ltype` 被用来来识别该结构体的类型，如果这个`LObject ` 结构体的类型是S-表达式或者Q-表达式，那么它则会存储了指向其他`LObject` 的指针以及指针的个数，方便作递归式的推导。



## 语法分析器

语法分析器的功能是将大量杂乱无章甚至含有非法输入的字符流通过我们自己指定捕获规则过滤后映射成函数抽象语法树。



![未命名文件 (3)](/Users/kounarushi/Desktop/paper/build_paper/未命名文件 (3).png)

我们没有自己开发专用的语法解析器，那将让整个工程和论文的内容无法控制地膨胀。一个第三方的语法解析库，mpc（Micro Parser Combinators）【mpc】，被用来执行Lachesis 的语法解析工作。

通过调用`mpca_lang()` 函数，我们用正则表达式定义了九种语句的捕获规则，它们是Lachesis 所能识别的全部输入词符。

| 规则        | 定义                               | 解释                                     | 举例                                  |
| ----------- | ---------------------------------- | ---------------------------------------- | ------------------------------------- |
| number      | `/-?[0-9]+/`                       | 0或1个“-”开头，后跟任意个0-9之间的数字   | -1                                    |
| symbol      | `/[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/` | 任意个字母、数字、_+-\* /=<>!&的组合     | apple                                 |
| string      | `/\"(\\\\.| [^\"])*\"/`                            | 以\\开头的单字符或除了"之外的任意字符 | “New\n”   |
| comment     | `/;[^\\r\\n]*/ `                   | 以`;`开头的除了`\r\n` 之外的任意多个字符 | ; comment!                            |
| sexpr       | `'[' <expr>* ']'    `              | 被"[ ]"包裹起来的任意条`<expr>`          | [ + 1 2 ]                             |
| qexpr       | `'{' <expr>* '}'    `              | 被"{ }"包裹起来的任意条`<expr>`          | { + 1 2 }                             |
| expr        | `<number>|<symbol>|<string>|<comment>|<sexpr>|<qexpr> ` | 任意一条六种类型 | print |
| lexpression | `/^/ <expr>* /$/`                  | 任意多条expr                             | print "hello world"                   |



可以看出，expr[ession] 只是前六种类型的囊括，而lexpression 是任意个expr[ession] 的列表。这样做的目的是方便在程序中统一捕获符合`LExpression` 规则的输入并绑定在一个指向函数抽象语法树的指针 raw 上。

（输入流中的捕获）	

```c
mpc_result_t raw;
……
if (mpc_parse("<stdin>", input, LExpression, &raw)) {……}
```

（文件流中的捕获）	

```c
if (mpc_parse_contents(obj->sub_obj[0]->string, LExpression, &raw)) {……}
```



## 函数抽象语法树的j建立与推导

语法分析器捕获的结果是一个多叉树。

```c
typedef struct mpc_ast_t {
    char* tag;
    char* contents;
    mpc_state_t state;
    int children_num;
    struct mpc_ast_t** children;
} mpc_ast_t;
```

其中`tag` 指示了捕获词符的类型，也就是我们所定义的八种捕获规则（number、symbol、string……）外加上 “>”、“regex” 等库定义词符。`contents`是具体捕获的内容，例如`tag` 为“number” 的结构体中`contents` 的内容是“13”。当`tag` 为“>”、“sexpr”或“qexpr”时，说明这是一个树根，需要递归读入其子树。



函数`lobj_read()` 被用来读取一棵抽象语法树的内容并转换成一棵`Lobject` 树。其算法和示意图如下：

```js
function lobj_read(tree){
	switch(tree.tag){
		case "number","symbol","string":
			return lobj_read[number|symbol|string](tree);
		case ">","sexpr":
			new result = lobj_sexpr();
            break;
		case "qexpr":
			new result = lobj_qexpr();
            break;
        for( i in tree.children_num){
            skip tokens "[],{},commnet";
            result = lobj_add(result, lobj_read(tree.children[i]));
        }
        return result;
	}
}
```

![code2flow_4kCaoH](/Users/kounarushi/Desktop/paper/build_paper/code2flow_4kCaoH.png)

（当抽象语法树的`tag` 是“number”、“symbol”或“string”等$\beta $ 规范型时，直接返回含有`content` 的LObject。

当抽象语法树的`tag` 是“>”、“sexpr”或“qexpr” 时，需要首先新建一个新的LObject 作为子树树根`result` ，然后递归调用`lobj_read()` 遍历抽象语法树的子树，将结果加入到`result` 中并返回。）



![code2flow_4kCaoH (1)](/Users/kounarushi/Desktop/paper/build_paper/code2flow_4kCaoH (1).png)

读取之后我们要对得到的`LObject` 树进行推导。只有S-表达式才会被立即推导，其余数据类型不做处理，唯一的例外是符号类型，要从`env` 环境中拷贝以获取其完整的信息（函数指针）。

推导S-表达式的函数是`lobj_eval_sexpr()`  ，它会首先递归调用`lobj_eval()` 推导其子树，使得树高不断缩减，向上传递结果直到最后只剩一层，最后针对这一层的三种情况依次处理：

| S-表达式的孩子个数 | 类型       | 处理方法       | 举例                                              |
| ------------------ | ---------- | -------------- | ------------------------------------------------- |
| 0                  | 空类型     | 返回自身       | `[ ] =>(output): [ ]`                             |
| 1                  | 单元素类型 | 返回第一个元素 | `[1]=>(output): 1`                                |
| >=2                | 函数表达式 | 调用相应函数   | `[+ 1 1]=> func_call(e,"+",[+ 1 1])=>(output): 2` |

算法和流程图如图所示：

```js
function lobj_eval_sexpr(e, obj){
    for (i from 0 to obj.count)
        lobj_eval(e,obj[i]);
    switch (obj.count) {
        case 0: return obj;
        case 1: return obj[0];
    }
    // >=2
    return lobj_call(e, func = obj[0], obj);
}
```

![code2flow_4kCaoH (2)](/Users/kounarushi/Desktop/paper/build_paper/code2flow_4kCaoH (2)-0383538.png)

## 函子、环境与变量

Lachesis 解释器在开始执行时会初始化一个环境变量容器e，然后向其中导入函数符号表，

```c
    lenv* e = lenv_new();// 初始化环境变量容器
    lenv_builtin_init_list(e);// 初始化符号表
```

我们的解释器在在一开始没有预设的全局变量，只有内建函数。所以初始化符号表的函数仅执行一连串的函数添加操作：

```c
void lenv_add_builtin_func(lenv* e, char* symbol_name, lbuiltin func);
void lenv_builtin_init_list(lenv *e)
{
    /*__BASIC_TYPES__*/
    lenv_add_builtin_func(e, "list", built_in_list);
    lenv_add_builtin_func(e, "head", built_in_head);
    lenv_add_builtin_func(e, "tail", built_in_tail);
    lenv_add_builtin_func(e, "eval", built_in_eval);
……
}
```

添加函数的方法的工作流程如图所示：

![code2flow_4kCaoH (3)](/Users/kounarushi/Desktop/paper/build_paper/code2flow_4kCaoH (3).png)

首先，字符串 `symbol_name`  和函数指针 `func` 将会被转化成字符对象和函子。如果符号已经在环境容器中存在了，那么只要用新的字符对象和函子来替换旧的就可以了。如果是新出现的符号，则需要对环境容器进行扩容：

```c
/*if not repeated found, allocate for new obj */
e->count++;
e->func_object_list = realloc(e->func_object_list, sizeof(LObject *) * e->count);
e->symbol_list = realloc(e->symbol_list, sizeof(char *) * e->count);
/*copy func_ptr and symbol*/
e->func_object_list[e->count - 1] = lobj_copy(func_ptr);
e->symbol_list[e->count - 1] = malloc(strlen(func_symbol_obj->symbol) + 1);
strcpy(e->symbol_list[e->count - 1], func_symbol_obj->symbol);
```

这样做的好处是避免了命名冲突，缺点也是显而易见的：所有变量都是全局变量。



## lambda





## 错误处理和动态调试器模式

C 语言是一门没有垃圾回收器的静态语言，这意味着在运行时的发生任何内存错误都可能使程序强制退出。因此，一个能在运行时动态检测并处理错误的系统是非常必要的。

在Lachesis 解释器中，我们设计了一套基于宏替换的异常处理系统，用来检查输入参数的语义合法性。

语法分析器保证了输入数据形式上的合法性（例如满足一个S-表达式的形式），但对于我们自定义函数的一些语义上的错误（例如函数接收参数的个数不足）则无法通过语法分析器检出。

为此，我们定义了一套基于宏替换的异常处理系统，在各个功能模块

```c
#define ERROW_CHECK(args, conditon, fmt, ...)                                \
    if (!(conditon)) {                                                       \
        LObject* err = lobj_error(fmt, ##__VA_ARGS__);                       \
        lobj_del(args);                                                      \
        return err;                                                          \
    }
#endif

#define ERROW_CHECK_TYPE(func, args, index, expect)                          \
    ERROW_CHECK(args, args->sub_obj[index]->type == expect,                  \
        "function '%s' passed incorrect type for argument %i.\nGot %s, "     \
        "Expect %s.",                                                        \
        func, index, lobj_type_name(args->sub_obj[index]->type),             \
        lobj_type_name(expect))

#define ERROW_CHECK_NUM(func, args, num)                                     \
    ERROW_CHECK(args, args->count == num,                                    \
        "Function '%s' passed incorrect number of argument.\nGot %i, Expect "\
        "%i",                                                                \
        func, args->count, num)

#define ERROW_CHECK_NOT_EMPTY(func, args, index)                             \
    ERROW_CHECK(args, args->sub_obj[index]->count != 0,                      \
        "Function '%s' passed {} for argument %i.", func, index)
```





但是这些想法是从哪里冒出来的呢？当然他是一个隐晦的答案是他是从错误中不断反思总结出来的，我们又如何掌握？处在哪里并使用相应的异常处理机制呢？



在看看书，我还有另一套动态调试器结构。他在我们需要的每一个地方打下一个调试装，然后打印出内存的情况。输出到一张表中，方便我们在出错时查看。并找到错误的地方，有时他会比单步调试效率更高，有时候都不会。





## 标准库设计





就此我们的语言解释器部分已经全部完工了。他能够接受我们一起合法收入，并且给出一个令人满意的答案。



这样我们就不用再使用c语言来建立新的功能，而使用母子机。设计的Nexus。



### 从匿名函数到实名函数

我们内置的lambda 函数抽象可以看做一个匿名函数，

```
[def {func} [\ {func_name argument} {
    def [head f] [\ [tail f] b] 
}]]
```


```
[func {not x} {- 1 x}]
```



# 系统结果分析

## 测试用例



# 结论



## 不足与缺陷



有的时候工程的发展偏离最初的目标，建立一个能够解决所有可计算问题的语言系统是几乎不可能的，因为这涉及到“所有”这个无限的概念。帕斯卡圆球说过，只有上帝是无限，其他都是有限的。降低一点的要求是？这个语言系统能够解决其他语言所能解决的功能。在花费一定时间这情况下这是可以的，







# 致谢





# 参考文献

::: thebibliography

[1]https://www.phy.cam.ac.uk/blog/how-make-apple-pie-scratch

[mpc]https://github.com/orangeduck/mpc

【functor】https://www.jianshu.com/p/740a3f5c514e

[函子漫画]https://adit.io/posts/2013-04-17-functors,_applicatives,_and_monads_in_pictures.html

【函数式编程基础[一]--范畴论和Functor】https://zmxiaodao.com/post/fp/functor/



:::

66



# 附录

