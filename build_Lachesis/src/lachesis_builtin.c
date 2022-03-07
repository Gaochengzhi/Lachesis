/**
 * File              : lachesis_builtin.c
 * License           : The MIT License (MIT)
 * Author            : Gao Chengzhi <2673730435@qq.com>
 * Date              : 07.03.2022
 * Last Modified Date: 07.03.2022
 * Last Modified By  : Gao Chengzhi <2673730435@qq.com>
 */
#include "lachesis_builtin.h"
#include "lachesis_environment.h"
#include "lachesis_type.h"
#include <math.h>
#include <string.h>

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

LObject* built_in_var(lenv* e, LObject* o, char* func)
{
    ERROW_CHECK_TYPE("def", o, 0, LOBJ_QEXPR);

    /*catch the symbol_list in the first of the list*/
    LObject* symbol_list = o->cell[0];

    /*make sure all elements of list are symbols*/
    for (int i = 0; i < symbol_list->count; ++i) {
        ERROW_CHECK(o, (symbol_list)->cell[i]->type == LOBJ_SYMBOL,
            "Function 'def' cannot define non-symbol.\n Got %s, Expected %s.",
            lobj_type_name(symbol_list->cell[i]->type),
            lobj_type_name(LOBJ_SYMBOL));
    }

    /*check symbol and object values*/
    ERROW_CHECK(o, symbol_list->count == o->count - 1,
        "Function def cannot define incorrect number of objects.\n Got %i, "
        "Expected %i.",
        symbol_list->count, o->count - 1);

    for (int i = 0; i < symbol_list->count; ++i) {
        lenv_put(e, symbol_list->cell[i], o->cell[i + 1]);
        if (strcmp(func, "def") == 0) {
            lenv_define(e, symbol_list->cell[i], o->cell[i + 1]);
        }
        if (strcmp(func, "=") == 0) {
            lenv_put(e, symbol_list->cell[i], o->cell[i + 1]);
        }
    }

    lobj_del(o);
    return lobj_sexpr();
}
LObject* built_in_put(lenv* e, LObject* o) { return built_in_var(e, o, "def"); }

LObject* built_in_define(lenv* e, LObject* o)
{
    return built_in_var(e, o, "def");
}
LObject* built_in_lambda(lenv* e, LObject* a)
{
    ERROW_CHECK_NUM("\\", a, 2);
    ERROW_CHECK_TYPE("\\", a, 0, LOBJ_QEXPR);
    ERROW_CHECK_TYPE("\\", a, 1, LOBJ_QEXPR);

    /*first Q-expression should only contains symbol*/
    for (int i = 0; i < a->cell[0]->count; ++i) {
        ERROW_CHECK(a, (a->cell[0]->cell[i]->type == LOBJ_SYMBOL),
            "Cannot define non-symbol. Got %s, Expected %s.",
            lobj_type_name(a->cell[0]->cell[i]->type),
            lobj_type_name(LOBJ_SYMBOL));
    }

    /*Pop first two arguments and pass them to lobj_lambda*/
    LObject* argument = lobj_pop(a, 0);
    LObject* body = lobj_pop(a, 0);
    lobj_del(a);

    return lobj_lambda(argument, body);
}
