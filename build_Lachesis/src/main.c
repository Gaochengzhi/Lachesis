/**
 * File              : main.c
 * License           : The MIT License (MIT)
 * Author            : Gao Chengzhi <2673730435@qq.com>
 * Date              : 11.02.2022
 * Last Modified Date: 15.02.2022
 * Last Modified By  : Gao Chengzhi <2673730435@qq.com>
 */
#include "../lib/mpc/mpc.h"
#include "headline.h"
#include <errno.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*define some errors*/
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };
/*define some lach types */
enum { LVAL_NUM, LVAL_ERR };
/*some parsers*/

typedef struct {
    int type;
    long num;
    int err;
} lval;
lval lval_num(long x)
{
    lval v;
    v.type = LVAL_NUM;
    v.num = x;
    return v;
}

lval lval_err(int x)
{
    lval v;
    v.type = LVAL_ERR;
    v.err = x;
    return v;
}

/*print a lval*/

void lval_print(lval v)
{
    switch (v.type) {
    case LVAL_NUM:
        printf("%li", v.num);
        break;
    case LVAL_ERR:
        if (v.err == LERR_DIV_ZERO) {
            printf("Error: Divided by zero!");
        }
        if (v.err == LERR_BAD_OP) {
            printf("Error: Invalid operator!");
        }

        if (v.err == LERR_BAD_NUM) {
            printf("Error: Invalid number!");
        }
        break;
    }
}
void lval_print_line(lval v)
{
    lval_print(v);
    putchar('\n');
}
lval eval_op(lval x, char* op, lval y)
{

    if (y.type == LVAL_ERR) {
        return y;
    }
    if (x.type == LVAL_ERR) {
        return x;
    }
    if (strcmp(op, "+") == 0) {

        return lval_num(x.num + y.num);
    }
    if (strcmp(op, "-") == 0) {
        return lval_num(x.num - y.num);
    }
    if (strcmp(op, "*") == 0) {
        return lval_num(x.num * y.num);
    }
    if (strcmp(op, "/") == 0) {

        return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num / y.num);
    }
    return lval_err(LERR_BAD_OP);
}

lval eval(mpc_ast_t* t)
{
    if (strstr(t->tag, "number")) {
        errno = 0;
        long x = strtol(t->contents, NULL, 10);

        /*long int strtol (const char* str, char** endptr, int base);*/
        return errno != ERANGE // #define ERANGE 34 <Result too large>
            ? lval_num(x)      // number returns itself
            : lval_err(LERR_BAD_NUM);
    }
    char* op = t->children[1]->contents;
    lval x = eval(t->children[2]);
    int i = 3;
    while (strstr(t->children[i]->tag, "expr")) {
        x = eval_op(x, op, eval(t->children[i++]));
    }
    return x;
}
int main(int argc, char const* argv[])
{
    /*Lists of Parsers*/
    mpc_parser_t* Number = mpc_new("number");
    mpc_parser_t* Operator = mpc_new("operator");
    mpc_parser_t* Expr = mpc_new("expr");
    mpc_parser_t* Lispy = mpc_new("lispy");
    /*Language Definition*/
    mpca_lang(MPCA_LANG_DEFAULT, "                              \
        number    : /-?[0-9]+/ ;                            \
        operator  : '+' | '-' | '*' | '/' ;                 \
        expr      : <number> | '[' <operator> <expr>+ ']' ; \
        lispy     : /^/ <operator> <expr>+ /$/ ;            \
        ",
        Number, Operator, Expr,
        Lispy); // notice, can't place space after '\' !
    lec_print_headline();
    while (1) {

        char* input = readline("Lachesis> ");
        /*Attention! readline needs -lreadline argument!*/
        add_history(input);

        mpc_result_t r;

        if (mpc_parse("<stdin>", input, Lispy, &r)) {
            lval result = eval(r.output);
            lval_print_line(result);
            mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }
        free(input);
    }
    /*before end of code*/
    mpc_cleanup(4, Number, Operator, Expr, Lispy);
    return 0;
}
