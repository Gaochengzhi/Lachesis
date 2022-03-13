
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
#include "lachesis_builtin.h"
#include "lachesis_debug.h"
#include "lachesis_environment.h"
#include "lachesis_object.h"
#include "lachesis_type.h"
#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const* argv[])
{
    /*debug mode switch*/
    for (int i = 1; i < argc; ++i) {
        switch (strlast(argv[i])) {
        case 'g':
            _debug_mode = 1;
            break;
        }
    }

    mpc_parser_t* Number;
    mpc_parser_t* Symbol;
    mpc_parser_t* String;
    mpc_parser_t* Comment;
    mpc_parser_t* Sexpr;
    mpc_parser_t* Qexpr;
    mpc_parser_t* Expr;
    mpc_parser_t* Lispy;
    Number = mpc_new("number");
    Symbol = mpc_new("symbol");
    String = mpc_new("string");
    Comment = mpc_new("lispy");
    Sexpr = mpc_new("sexpr");
    Qexpr = mpc_new("qexpr");
    Expr = mpc_new("expr");
    Lispy = mpc_new("lispy");
    /*Language Definition*/
    mpca_lang(MPCA_LANG_DEFAULT, "                                      \
        number    : /-?[0-9]+/ ;                                        \
        symbol    : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/;                   \
        string    : /\"(\\\\.|[^\"])*\"/;                               \
        comment   : /;[^\\r\\n]*/;                                      \
        sexpr     : '[' <expr>* ']' ;                                   \
        qexpr     : '{' <expr>* '}' ;                                   \
        expr      : <number> | <symbol> | <string> | <comment>| <sexpr> | <qexpr> ;                     \
        lispy     : /^/ <expr>* /$/ ;                                   \
        ",
        Number, Symbol, String, Comment, Sexpr, Qexpr, Expr,
        Lispy); // notice, can't place space after '\' !

    /*init the environment*/

    lenv* e = lenv_new();
    lenv_builtin_init_list(e);

    if (argc == 1) {

        /*print some welcome words*/
        lec_print_headline();

        while (1) {

            char* input = readline("Lachesis> ");
            /*Attention! readline needs -lreadline argument!*/
            if (strcmp(input, "q") == 0) {
                puts("Bye!");
                exit(0);
            }

            if (strcmp(input, "h") == 0) {
                print_help();
                continue;
            }
            add_history(input);

            mpc_result_t raw;

            if (mpc_parse("<stdin>", input, Lispy, &raw)) {
                LObject* x = lobj_eval(e, lobj_read(raw.output));
                lobj_print_line(x);
                lobj_del(x);
                mpc_ast_delete(raw.output);
            } else {
                mpc_err_print(raw.error);
                mpc_err_delete(raw.error);
            }
            free(input);
        }
    }
    /*file support*/
    /*if (argc >= 2) {*/
    /*for (int i = 1; i < argc; ++i) {*/
    /*LObject* args = lobj_add(lobj_sexpr(), lobj_string(argv[i]));*/
    /*LObject* x = built_in_load(e, args);*/
    /*if (x->type == LOBJ_ERR) {*/
    /*lobj_print_line(x);*/
    /*}*/
    /*lobj_del(x);*/
    /*}*/
    /*}*/
    /*before end of code*/
    lenv_del(e);
    mpc_cleanup(7, Number, Symbol, String, Comment, Sexpr, Qexpr, Expr, Lispy);

    return 0;
}
