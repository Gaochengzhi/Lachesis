/**
 * File              : main.c
 * License           : The MIT License (MIT)
 * Author            : Gao Chengzhi <2673730435@qq.com>
 * Date              : 11.02.2022
 * Last Modified Date: 14.02.2022
 * Last Modified By  : Gao Chengzhi <2673730435@qq.com>
 */
#include "../libaray/mpc/mpc.h"
#include "headline.h"
#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
/*some definitions*/
/*some parsers*/
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
        Number, Operator, Expr, Lispy); // notice, can't place space after '\' !
    lec_print_headline();
    while (1) {

        char* input = readline("Lachesis> ");
        /*Attention! readline needs -lreadline argument!*/
        add_history(input);

        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Lispy, &r)) {
            puts("Sucess to build the AST tree!");
            mpc_ast_print(r.output);
            mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }
    }
    /*before end of code*/
    mpc_cleanup(4, Number, Operator, Expr, Lispy);
    return 0;
}
