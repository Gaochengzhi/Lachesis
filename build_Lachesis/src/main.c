
/**
 * File              : main.c
 * License           : The MIT License (MIT)
 * Author            : Gao Chengzhi <2673730435@qq.com>
 * Date              : 11.02.2022
 * Last Modified Date: 17.02.2022
 * Last Modified By  : Gao Chengzhi <2673730435@qq.com>
 */
#include "../lib/mpc/mpc.h"
#include "headline.h"
#include "lachesis_debug.h"
#include "lachesis_environment.h"
#include "lachesis_object.h"
#include "lachesis_type.h"
#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char strlast(const char* p)
{
    int len = strlen(p);
    const char* t = &p[len - 1];
    return *t;
}
int main(int argc, char const* argv[])
{
    for (int i = 1; i < argc; ++i) {
        switch (strlast(argv[i])) {
        case 'g':
            _debug_mode = 1;
            break;
        }
    }

    /*Lists of Parsers*/
    mpc_parser_t* Number = mpc_new("number");
    mpc_parser_t* Symbol = mpc_new("symbol");
    mpc_parser_t* Sexpr = mpc_new("sexpr");
    mpc_parser_t* Qexpr = mpc_new("qexpr");
    mpc_parser_t* Expr = mpc_new("expr");
    mpc_parser_t* Lispy = mpc_new("lispy");
    /*Language Definition*/
    mpca_lang(MPCA_LANG_DEFAULT, "                          \
        number    : /-?[0-9]+/ ;                            \
        symbol    : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/;       \
        sexpr     : '[' <expr>* ']' ;                       \
        qexpr     : '{' <expr>* '}' ;                       \
        expr      : <number> | <symbol> | <sexpr>| <qexpr> ;\
        lispy     : /^/ <expr>* /$/ ;                       \
        ",
        Number, Symbol, Sexpr, Qexpr, Expr,
        Lispy); // notice, can't place space after '\' !

    /*print some welcome world*/
    lec_print_headline();

    /*inti the environment*/

    lenv* e = lenv_new();
    lenv_builtin_init_list(e);
    while (1) {

        char* input = readline("Lachesis> ");
        /*Attention! readline needs -lreadline argument!*/
        if (strcmp(input, "q") == 0) {
            puts("Bye!");
            exit(0);
        }

        if (strcmp(input, "h") == 0) {
            print_help();
        }
        add_history(input);

        mpc_result_t r;

        if (mpc_parse("<stdin>", input, Lispy, &r)) {
            LObject* x = lobj_eval(e, lobj_read(r.output));
            lobj_print_line(x);
            lobj_del(x);
            mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }
        free(input);
    }
    /*before end of code*/
    lenv_del(e);
    mpc_cleanup(6, Number, Symbol, Sexpr, Qexpr, Expr, Lispy);

    return 0;
}
