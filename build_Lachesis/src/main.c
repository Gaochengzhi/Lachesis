
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
#include <errno.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*define some errors*/
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };
/*define some lachesis types */
enum { LVAL_NUM, LVAL_ERR, LVAL_SYMBOL, LVAL_SEXPR };

typedef struct lval {
    int type;
    long num;
    char* err;
    char* symbol;
    int count;          // how many pointers to a list of lval
    struct lval** cell; // pointers to a list of lval
} lval;

/*function interface*/
void lval_print_expr(lval* v, char open, char close);
void lval_print();
void lval_del(lval* v);
lval* lval_number(long x);
lval* lval_error(char* m);
lval* lval_symbol(char* s);
lval* lval_sexpr(void);
lval* lval_add(lval* v, lval* x);
lval* lval_eval_sexpr(lval* v);
lval* lval_eval(lval* v);
lval* lval_read_num(mpc_ast_t* t);
lval* lval_take_out(lval* v, int i);
lval* lval_pop(lval* v, int i);
lval* built_in_op(lval* a, char* op);

/*function implementatioms*/

/*give lval a number and tags it*/
lval* lval_number(long x)
{
    lval* v = malloc(sizeof(lval));
    printf("assign number:         (%li) on pointer:[%-15li]\n", x,
        (long)v); // debug line
    v->type = LVAL_NUM;
    v->num = x;
    return v;
}

/*sending lval the error messages and tag it */
lval* lval_error(char* m)
{

    lval* v = malloc(sizeof(lval));
    printf("assign err:            (%s) on pointer:[%15li]\n", m,
        (long)v); // debug line
    v->type = LVAL_ERR;
    v->err = malloc(strlen(m) + 1);
    strcpy(v->err, m);
    return v;
}

lval* lval_symbol(char* s)
{
    lval* v = malloc(sizeof(lval));
    printf("assign symbol:         (%s) on pointer:[%15li]\n", s,
        (long)v); // debug line
    v->type = LVAL_SYMBOL;
    v->symbol = malloc(strlen(s) + 1);
    strcpy(v->symbol, s);
    return v;
}

/*Initialize a  pointer to a new empty lval*/
lval* lval_sexpr(void)
{
    lval* v = malloc(sizeof(lval));
    printf("init a s-expression on pointer:[%15li]\n",
        (long)v); // debug line
    v->type = LVAL_SEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

/*Add lval x into lval v*/
lval* lval_add(lval* v, lval* x)
{

    printf("add lval:              [%15li] to lval:[%15li]\n", (long)v,
        (long)x); // debug line
    v->count++;
    v->cell = realloc(v->cell, sizeof(lval*) * (v->count));
    v->cell[v->count - 1] = x;
    return v;
}
void lval_del(lval* v)
{
    printf("delete lval:           [%15li], type(%d)\n", (long)v,
        v->type); // debug line
    switch (v->type) {
        /*number type will let it go*/
    case LVAL_NUM:
        break;

        /*error or symbol will free its string data*/
    case LVAL_ERR:
        free(v->err);
        break;
    case LVAL_SYMBOL:
        free(v->symbol);
        break;

        /*s-expression will free all its data*/
    case LVAL_SEXPR:
        for (int i = 0; i < v->count; ++i) {
            lval_del(v->cell[i]);
        }
        break;
    }
    /*free the memory allocated to contain the pointers*/
    free(v);
}

lval* lval_read_num(mpc_ast_t* t)
{

    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    printf("read num:              [%li] from tree t[%15li]\n", x,
        (long)t); // debug line

    return errno != ERANGE ? lval_number(x)
                           : lval_error("Too long or invalid number!");
}

/*read the parser input and return the conversion of that type*/
lval* lval_read(mpc_ast_t* t)
{
    if (strstr(t->tag, "number")) {
        return lval_read_num(t);
    }

    if (strstr(t->tag, "symbol")) {
        return lval_symbol(t->contents);
    }

    /*root means '>'! s-expression and root will 1.create an empty list*/
    lval* x = NULL;
    if (strcmp(t->tag, ">") == 0) {
        x = lval_sexpr();
    }
    if (strstr(t->tag, "sexpr")) {
        x = lval_sexpr();
    }

    /*then 2.fill the list with its expression*/
    for (int i = 0; i < t->children_num; ++i) {

        if (strcmp(t->children[i]->contents, "[") == 0) {
            continue;
        }

        if (strcmp(t->children[i]->contents, "]") == 0) {
            continue;
        }
        if (strcmp(t->children[i]->contents, "{") == 0) {
            continue;
        }
        if (strcmp(t->children[i]->contents, "}") == 0) {
            continue;
        }
        if (strcmp(t->children[i]->tag, "regex") == 0) {
            continue;
        }
        x = lval_add(x, lval_read(t->children[i]));
    }
    return x;
}

/*print a lval*/

void lval_print(lval* v)

{
    switch (v->type) {
    case LVAL_NUM:
        printf("%li", v->num);
        break;
    case LVAL_ERR:
        printf("Error: %s", v->err);
        break;

    case LVAL_SYMBOL:
        printf("%s", v->symbol);
        break;

    case LVAL_SEXPR:
        lval_print_expr(v, '[', ']');
        break;
    }
}

void lval_print_expr(lval* v, char begin_symbol, char end_symbol)
{
    putchar(begin_symbol);
    for (int i = 0; i < v->count; ++i) {
        lval_print(v->cell[i]);

        /*last element will not have trauling space*/
        if (i != (v->count - 1)) {
            putchar(' ');
        }
    }
    putchar(end_symbol);
}

void lval_print_line(lval* v)
{
    printf("the result is: ");
    lval_print(v);
    putchar('\n');
}

/*Only evaluate S-expression, all other things will return*/
lval* lval_eval(lval* v)
{
    if (v->type == LVAL_SEXPR) {
        return lval_eval_sexpr(v);
    }
    return v;
}

lval* lval_eval_sexpr(lval* v)
{
    /*Evaluate its children*/
    for (int i = 0; i < v->count; ++i) {
        v->cell[i] = lval_eval(v->cell[i]);
    }

    /*Error handling*/
    for (int i = 0; i < v->count; ++i) {
        if (v->cell[i]->type == LVAL_ERR) {
            return lval_take_out(v, i);
        }
    }

    /*Empty Cases*/
    if (v->count == 0) {
        return v;
    }

    /*Single Cases*/
    if (v->count == 1) {
        return lval_take_out(v, 0);
    }

    /*Make sure the first element is a symbol */
    lval* f = lval_pop(v, 0);
    if (f->type != LVAL_SYMBOL) {
        lval_del(f);
        lval_del(v);
        return lval_error("S-expression should not start with symbol!");
    }

    lval* result = built_in_op(v, f->symbol);
    lval_del(f);
    return result;
}

lval* built_in_op(lval* a, char* op)
{
    /*Make sure all arguments are numbers*/
    for (int i = 0; i < a->count; ++i) {
        if (a->cell[i]->type != LVAL_NUM) {
            lval_del(a);
            return lval_error("Operation on non-numberable element!");
        }
    }

    /*Pop the first element*/
    lval* x = lval_pop(a, 0);

    /*No arguments and sub will perform the unary operation*/
    /*e.g: y = -4*/
    if ((strcmp(op, "-") == 0) && a->count == 0) {
        x->num = -(x->num);
    }

    /*If there are still remaining numbers*/
    while (a->count > 0) {

        /*Pop the next */
        lval* y = lval_pop(a, 0);
        if (strcmp(op, "+") == 0) {
            x->num += y->num;
        }

        if (strcmp(op, "-") == 0) {
            x->num -= y->num;
        }

        if (strcmp(op, "*") == 0) {
            x->num *= y->num;
        }
        if (strcmp(op, "/") == 0) {
            if (y->num == 0) {

                lval_del(x);
                lval_del(y);
                x = lval_error("Division by zero!");
                break;
            }
            x->num /= y->num;
        }
        lval_del(y);
    }
    lval_del(a);
    return x;
}

lval* eval(lval* v)
{
    /*Evaluate Sexpressions*/
    if (v->type == LVAL_SEXPR) {
        return lval_eval_sexpr(v);
    }
    /*Otherwise just remain the same*/
    return v;
}

/*Pop up the i-th item inside the cell, remain the rest of the list*/
lval* lval_pop(lval* v, int i)
{

    printf("pop lval the number:   (%d) on pointer:[%-15li]\n", i,
        (long)v); // debug line
    lval* x = v->cell[i];

    /*void *memmove( void *str1, const void *str2, size_t n );*/
    memmove(&v->cell[i], &v->cell[i + 1], sizeof(lval*) * (v->count-- - i - 1));
    // decrease the v->count

    /*realloc the memory we used*/
    v->cell = realloc(v->cell, sizeof(lval*) * v->count);
    return x;
}

/*Same as lval_pop, but delete the original list*/
lval* lval_take_out(lval* v, int i)
{
    lval* x = lval_pop(v, i);
    lval_del(v);
    return x;
}

int main(int argc, char const* argv[])
{
    /*Lists of Parsers*/
    mpc_parser_t* Number = mpc_new("number");
    mpc_parser_t* Symbol = mpc_new("symbol");
    mpc_parser_t* Expr = mpc_new("expr");
    mpc_parser_t* Sexpr = mpc_new("sexpr");
    mpc_parser_t* Lispy = mpc_new("lispy");
    /*Language Definition*/
    mpca_lang(MPCA_LANG_DEFAULT, "                          \
        number    : /-?[0-9]+/ ;                            \
        symbol    : '+' | '-' | '*' | '/' ;                 \
        sexpr     : '[' <expr>* ']' ;                       \
        expr      : <number> | <symbol> | <sexpr> ;         \
        lispy     : /^/ <expr>* /$/ ;                       \
        ",
        Number, Symbol, Expr, Sexpr,
        Lispy); // notice, can't place space after '\' !

    /*print some welcome world*/
    lec_print_headline();

    while (1) {

        char* input = readline("Lachesis> ");
        /*Attention! readline needs -lreadline argument!*/
        if (strcmp(input, "q") == 0) {
            puts("Bye!");
            exit(0);
        }
        add_history(input);

        mpc_result_t r;

        if (mpc_parse("<stdin>", input, Lispy, &r)) {
            lval* x = lval_eval(lval_read(r.output));
            lval_print_line(x);
            lval_del(x);
            mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }
        free(input);
        char* types = "\nNote:\nLVAL_NUM=0,\
            \nLVAL_ERR=1,\nLVAL_SYMBOL=2,\
            \nLVAL_SEXPR=3\n";
        puts(types);
    }
    /*before end of code*/
    mpc_cleanup(5, Number, Symbol, Expr, Sexpr, Lispy);

    return 0;
}
