/**
 * File              : lachesis_object.c
 * License           : The MIT License (MIT)
 * Author            : Gao Chengzhi <2673730435@qq.com>
 * Date              : 18.02.2022
 * Last Modified Date: 05.03.2022
 * Last Modified By  : Gao Chengzhi <2673730435@qq.com>
 */

#include "lachesis_object.h"
#include "lachesis_debug.h"
#include "lachesis_environment.h"
#include "lachesis_type.h"
#include <stdlib.h>

/*give lobj a number and tags it*/
LObject* lobj_number(long x)
{
    LObject* v = malloc(sizeof(LObject));
    lbug_print_slsl("assign number:", x, "on pointer", (long)v); // debug line
    v->type = LOBJ_NUM;
    v->num = x;
    return v;
}

/*sending lobj the error messages and tag it */
LObject* lobj_error(char* fmt, ...)
{

    LObject* v = malloc(sizeof(LObject));
    lbug_print_sssl("assign err:", fmt, "on pointer", (long)v); // debug line
    v->type = LOBJ_ERR;

    /*val_list initializatiojn*/
    va_list val;
    va_start(val, fmt);
    v->err = malloc(512);
    vsnprintf(v->err, 511, fmt, val);
    v->err = realloc(v->err, strlen(v->err) + 1);
    va_end(val);
    return v;
}

LObject* lobj_symbol(char* s)
{
    LObject* v = malloc(sizeof(LObject));
    lbug_print_sssl("assign symbol", s, "on pointer", (long)v); // debug line
    v->type = LOBJ_SYMBOL;
    v->symbol = malloc(strlen(s) + 1);
    strcpy(v->symbol, s);
    return v;
}

/*Initialize a pointer to a new empty LObject*/
LObject* lobj_sexpr(void)
{
    LObject* v = malloc(sizeof(LObject));
    lbug_print_sl("init a s-expression on pointer", (long)v); // debug line
    v->type = LOBJ_SEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

LObject* lobj_qexpr(void)
{
    LObject* v = malloc(sizeof(LObject));
    lbug_print_sl("init a q-expression on pointer", (long)v); // debug line
    v->type = LOBJ_QEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

/*Add lobj x into lobj v*/
LObject* lobj_add(LObject* v, LObject* x)
{

    lbug_print_slsl("add lobj:", (long)v, "on lobj: ", (long)x); // debug line
    v->count++;
    v->cell = realloc(v->cell, sizeof(LObject*) * (v->count));
    v->cell[v->count - 1] = x;
    return v;
}

void lobj_del(LObject* v)
{
    lbug_print_slsl("delete lobj:", (long)v, "type", v->type); // debug line
    switch (v->type) {
        /*number and function type will let it go*/
    case LOBJ_NUM:
        break;
    case LOBJ_FUNC:
        break;
        /*error or symbol will free its string data*/
    case LOBJ_ERR:
        free(v->err);
        break;
    case LOBJ_SYMBOL:
        free(v->symbol);
        break;

        /*q or s-expression will free all its data*/
    case LOBJ_QEXPR:
    case LOBJ_SEXPR:
        for (int i = 0; i < v->count; ++i) {
            lobj_del(v->cell[i]);
        }
        break;
    }
    /*free the memory allocated to contain the pointers*/
    free(v);
}

LObject* lobj_read_num(mpc_ast_t* t)
{

    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    lbug_print_slsl("read num: ", x, "from tree t", (long)t); // debug line

    return errno != ERANGE ? lobj_number(x)
                           : lobj_error("Too long or invalid number!");
}

/*read the parser input and return the conversion of that type*/
LObject* lobj_read(mpc_ast_t* t)
{
    if (strstr(t->tag, "number")) {
        return lobj_read_num(t);
    }

    if (strstr(t->tag, "symbol")) {
        return lobj_symbol(t->contents);
    }

    /*root means '>'! s-expression and root will 1.create an empty list*/
    LObject* x = NULL;
    if (strcmp(t->tag, ">") == 0) {
        x = lobj_sexpr();
    }
    if (strstr(t->tag, "sexpr")) {
        x = lobj_sexpr();
    }

    if (strstr(t->tag, "qexpr")) {
        x = lobj_qexpr();
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
        x = lobj_add(x, lobj_read(t->children[i]));
    }
    return x;
}

/*print a LObject*/

void lobj_print(LObject* v)
{
    switch (v->type) {
    case LOBJ_NUM:
        printf("%li", v->num);
        break;

    case LOBJ_ERR:
        printf("Error: %s", v->err);
        break;

    case LOBJ_FUNC:
        printf("<function>");
        break;

    case LOBJ_SYMBOL:
        printf("%s", v->symbol);
        break;

    case LOBJ_SEXPR:
        lobj_print_expr(v, '[', ']');
        break;

    case LOBJ_QEXPR:
        lobj_print_expr(v, '{', '}');
        break;
    }
}

void lobj_print_expr(LObject* v, char begin_symbol, char end_symbol)
{
    putchar(begin_symbol);
    for (int i = 0; i < v->count; ++i) {
        lobj_print(v->cell[i]);

        /*last element will not have trauling space*/
        if (i != (v->count - 1)) {
            putchar(' ');
        }
    }
    putchar(end_symbol);
}

void lobj_print_line(LObject* v)
{
    lobj_print(v);
    putchar('\n');
}

/*Only evaluate S-expression, all other things will return*/
LObject* lobj_eval(lenv* e, LObject* v)
{
    /*when encountering symbols, delete the input*/
    if (v->type == LOBJ_SYMBOL) {
        LObject* x = lenv_get(e, v);
        lobj_del(v);
        return x;
    }
    if (v->type == LOBJ_SEXPR) {
        return lobj_eval_sexpr(e, v);
    }
    return v;
}

LObject* lobj_eval_sexpr(lenv* e, LObject* v)
{
    /*Evaluate its children*/
    for (int i = 0; i < v->count; ++i) {
        v->cell[i] = lobj_eval(e, v->cell[i]);
    }

    /*Error handling*/
    for (int i = 0; i < v->count; ++i) {
        if (v->cell[i]->type == LOBJ_ERR) {
            return lobj_take_out(v, i);
        }
    }

    /*Empty Cases*/
    if (v->count == 0) {
        return v;
    }

    /*Single Cases*/
    if (v->count == 1) {
        return lobj_take_out(v, 0);
    }

    /*Make sure the first element is a function */
    LObject* f = lobj_pop(v, 0);
    if (f->type != LOBJ_FUNC) {
        lobj_del(f);
        lobj_del(v);
        return lobj_error("first element is not a function");
    }

    LObject* result = f->func(e, v);
    lobj_del(f);
    return result;
}

LObject* built_in_op(lenv* e, LObject* a, char* op)
{
    /*Make sure all arguments are numbers*/
    for (int i = 0; i < a->count; ++i) {
        ERROW_CHECK_TYPE(op, a, i, LOBJ_NUM);
    }

    /*Pop the first element*/
    LObject* x = lobj_pop(a, 0);

    /*No arguments and sub will perform the unary operation*/
    /*e.g: y = -4*/
    if ((strcmp(op, "-") == 0) && a->count == 0) {
        x->num = -(x->num);
    }

    /*If there are still remaining numbers*/
    while (a->count > 0) {

        /*Pop the next */
        LObject* y = lobj_pop(a, 0);
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

                lobj_del(x);
                lobj_del(y);
                x = lobj_error("Division by zero!");
                break;
            }
            x->num /= y->num;
        }
        lobj_del(y);
    }
    lobj_del(a);
    return x;
}

LObject* built_in_add(lenv* e, LObject* a) { return built_in_op(e, a, "+"); }

LObject* built_in_sub(lenv* e, LObject* a) { return built_in_op(e, a, "-"); }

LObject* built_in_mult(lenv* e, LObject* a) { return built_in_op(e, a, "*"); }

LObject* built_in_div(lenv* e, LObject* a) { return built_in_op(e, a, "/"); }

LObject* built_in_head(lenv* e, LObject* a)
{

    ERROW_CHECK_NUM("head", a, 1);
    ERROW_CHECK_TYPE("head", a, 0, LOBJ_QEXPR);
    ERROW_CHECK_NOT_EMPTY("head", a, 0);

    LObject* v = lobj_take_out(a, 0);
    while (v->count > 1) {
        lobj_del(lobj_pop(v, 1));
    }
    return v;
}

LObject* built_in_tail(lenv* e, LObject* a)
{
    ERROW_CHECK_NUM("tail", a, 1);
    ERROW_CHECK_TYPE("tail", a, 0, LOBJ_QEXPR);
    ERROW_CHECK_NOT_EMPTY("tail", a, 0);

    LObject* v = lobj_take_out(a, 0);
    lobj_del(lobj_pop(v, 0));
    return v;
}

/*convert a s-expression to a q-expression*/
LObject* built_in_list(lenv* e, LObject* a)
{
    a->type = LOBJ_QEXPR;
    return a;
}

/*convert a q-expression to a q-expression*/
LObject* built_in_eval(lenv* e, LObject* a)
{

    ERROW_CHECK_NUM("eval", a, 1);
    ERROW_CHECK_TYPE("eval", a, 0, LOBJ_QEXPR);

    LObject* x = lobj_take_out(a, 0);
    x->type = LOBJ_SEXPR;
    return lobj_eval(e, x);
}

LObject* built_in_join(lenv* e, LObject* a)
{
    for (int i = 0; i < a->count; ++i) {
        ERROW_CHECK_TYPE("join", a, i, LOBJ_QEXPR);
    }

    LObject* x = lobj_pop(a, 0);
    while (a->count) {
        x = lobj_join(x, lobj_pop(a, 0));
    }
    lobj_del(a);
    return x;
}

LObject* lobj_join(LObject* x, LObject* y)
{
    for (int i = 0; i < y->count; ++i) {
        x = lobj_add(x, y->cell[i]);
    }
    free(y->cell);
    free(y);
    return x;
}

/*Pop up the i-th item inside the cell, return the poped one*/
LObject* lobj_pop(LObject* v, int i)
{

    lbug_print_sdsl(
        "pop lobj the number:", i, "on pointer", (long)v); // debug line

    LObject* x = v->cell[i];
    /*void *memmove( void *str1, const void *str2, size_t n );*/
    memmove(
        &v->cell[i], &v->cell[i + 1], sizeof(LObject*) * (v->count-- - i - 1));
    // remember to decrease the v->count!

    /*realloc the memory we used*/
    v->cell = realloc(v->cell, sizeof(LObject*) * v->count);
    return x;
}

/*Same as lobj_pop, but delete the original list*/
LObject* lobj_take_out(LObject* v, int i)
{
    LObject* x = lobj_pop(v, i);
    lobj_del(v);
    return x;
}

LObject* lobj_func(lbuiltin func)
{
    LObject* v = malloc(sizeof(LObject));
    v->type = LOBJ_FUNC;
    v->func = func;
    return v;
}

/*copy a new one and return it*/
LObject* lobj_copy(LObject* v)
{
    LObject* x = malloc(sizeof(LObject));
    x->type = v->type;

    switch (v->type) {
    case LOBJ_FUNC:
        x->func = v->func;
        break;
    case LOBJ_NUM:
        x->num = v->num;
        break;

    case LOBJ_ERR:
        x->err = malloc(strlen(v->err) + 1);
        strcpy(x->err, v->err);
        break;
    case LOBJ_SYMBOL:
        x->symbol = malloc(strlen(v->symbol) + 1);
        strcpy(x->symbol, v->symbol);
        break;
    case LOBJ_SEXPR:
    case LOBJ_QEXPR:
        x->count = v->count;
        x->cell = malloc(sizeof(LObject*) * x->count);
        for (int i = 0; i < x->count; ++i) {
            x->cell[i] = lobj_copy(v->cell[i]);
        }
        break;
    }

    return x;
}

LObject* built_in_define(lenv* e, LObject* a)
{
    ERROW_CHECK_TYPE("def", a, 0, LOBJ_QEXPR);

    /*catch the symbol_list in the first of the list*/
    LObject* symbol_list = a->cell[0];

    /*make sure all elements of list are symbols*/
    for (int i = 0; i < symbol_list->count; ++i) {
        ERROW_CHECK(a, (symbol_list)->cell[i]->type == LOBJ_SYMBOL,
            "Function 'def' cannot define non-symbol.\n Got %s, Expected %s.",
            lobj_type_name(symbol_list->cell[i]->type),
            lobj_type_name(LOBJ_SYMBOL));
    }

    /*check symbol and object values*/
    ERROW_CHECK(a, symbol_list->count == a->count - 1,
        "Function def cannot define incorrect number of objects.\n Got %i, "
        "Expected %i.",
        symbol_list->count, a->count - 1);

    for (int i = 0; i < symbol_list->count; ++i) {
        lenv_put(e, symbol_list->cell[i], a->cell[i + 1]);
    }

    lobj_del(a);
    return lobj_sexpr();
}
