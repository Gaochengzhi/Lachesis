
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

mpc_parser_t* Number;
mpc_parser_t* Double;
mpc_parser_t* Symbol;
mpc_parser_t* String;
mpc_parser_t* Comment;
mpc_parser_t* Sexpr;
mpc_parser_t* Qexpr;
mpc_parser_t* Expr;
mpc_parser_t* LExpression;

int main(int argc, char** argv)
{
    /*debug mode switch*/

    Number = mpc_new("number");
    Double = mpc_new("double");
    Symbol = mpc_new("symbol");
    String = mpc_new("string");
    Comment = mpc_new("comment");
    Sexpr = mpc_new("sexpr");
    Qexpr = mpc_new("qexpr");
    Expr = mpc_new("expr");
    LExpression = mpc_new("lexpression");
    /*Language Definition*/
    mpca_lang(MPCA_LANG_DEFAULT, "                               \
        number    : /-?[0-9]+/;                             \
        double    :  /`-?[0-9]*\\.?[0-9]+`/      ;                    \
        symbol    : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/;            \
        string    : /\"(\\\\.|[^\"])*\"/;                        \
        comment   : /;[^\\r\\n]*/;                               \
        sexpr     : '[' <expr>* ']' ;                            \
        qexpr     : '{' <expr>* '}' ;                            \
        expr      : <number>|<double>|<symbol>|<string>|<comment>\
                  | <sexpr> | <qexpr> ;                          \
        lexpression     : /^/ <expr>* /$/ ;                      \
        ",
        Number, Double, Symbol, String, Comment, Sexpr, Qexpr, Expr,
        LExpression); // notice, can't place space after '\' !

    /*init the environment*/

    lenv* e = lenv_new();
    lenv_builtin_init_list(e);

    int print_tree = check_commandline_argument(&argc, argv, e);
    if (argc == 1) {

        /*print some welcome words*/
        lec_print_headline();

        while (1) {

            char* input = readline("\033[;32mλ ->\033[0m ");
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

            if (mpc_parse("<stdin>", input, LExpression, &raw)) {
                if (print_tree)
                    mpc_ast_print(raw.output);
                LObject* x = lobj_eval(e, lobj_read(raw.output));
                lobj_print_line(x);
                lobj_delete(x);
                mpc_ast_delete(raw.output);
                if (print_tree)
                    fflush(stderr);
            } else {
                mpc_err_print(raw.error);
                mpc_err_delete(raw.error);
            }
            free(input);
        }
    }
    /*file support*/
    if (argc >= 2) {
        for (int i = 1; i < argc; ++i) {
            LObject* args = lobj_add(lobj_sexpr(), lobj_string(argv[i]));
            LObject* x = built_in_import(e, args);
            if (print_tree)
                fflush(stderr);
            if (x->type == LOBJ_ERR) {
                lobj_print_line(x);
            }
            lobj_delete(x);
        }
    }
    /*before end of code*/
    lenv_delete(e);
    mpc_cleanup(9, Number, Double, Symbol, String, Comment, Sexpr, Qexpr, Expr,
        LExpression);

    return 0;
}
