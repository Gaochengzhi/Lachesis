/**
 * File              : lachesis_builtin.c
 * License           : The MIT License (MIT)
 * Author            : Gao Chengzhi <2673730435@qq.com>
 * Date              : 07.03.2022
 * Last Modified Date: 09.03.2022
 * Last Modified By  : Gao Chengzhi <2673730435@qq.com>
 */
#include "lachesis_builtin.h"
#include "lachesis_debug.h"
#include "lachesis_environment.h"
#include "lachesis_object.h"
#include "lachesis_type.h"
#include <math.h>
#include <string.h>

LObject* built_in_op(lenv* e, LObject* o, char* op)
{
    /*Make sure all arguments are numbers*/
    for (int i = 0; i < o->count; ++i) {
        ERROW_CHECK_TYPE(op, o, i, LOBJ_NUM);
    }

    /*Pop the first element*/
    LObject* x = lobj_pop(o, 0);

    /*No arguments and sub will perform the unary operation*/
    /*e.g: y = -4*/
    if ((strcmp(op, "-") == 0) && o->count == 0) {
        x->num = -(x->num);
    }

    /*If there are still remaining numbers*/
    while (o->count > 0) {

        /*Pop the next */
        LObject* y = lobj_pop(o, 0);
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
    lobj_del(o);
    return x;
}

LObject* built_in_add(lenv* e, LObject* o) { return built_in_op(e, o, "+"); }

LObject* built_in_sub(lenv* e, LObject* o) { return built_in_op(e, o, "-"); }

LObject* built_in_mult(lenv* e, LObject* o) { return built_in_op(e, o, "*"); }

LObject* built_in_div(lenv* e, LObject* o) { return built_in_op(e, o, "/"); }

LObject* built_in_great_than(lenv* e, LObject* o)
{
    return built_in_ord(e, o, ">");
}

LObject* built_in_great_and_equal(lenv* e, LObject* o)
{
    return built_in_ord(e, o, ">=");
}

LObject* built_in_less_than(lenv* e, LObject* o)
{
    return built_in_ord(e, o, "<");
}

LObject* built_in_less_and_equal(lenv* e, LObject* o)
{
    return built_in_ord(e, o, "<=");
}

LObject* built_in_ord(lenv* e, LObject* o, char* op)
{
    ERROW_CHECK_NUM(op, o, 2);
    ERROW_CHECK_TYPE(op, o, 0, LOBJ_NUM);
    ERROW_CHECK_TYPE(op, o, 1, LOBJ_NUM);

    int result;

    if (strcmp(op, ">") == 0) {
        result = (o->cell[0]->num > o->cell[1]->num);
    }

    if (strcmp(op, "<") == 0) {
        result = (o->cell[0]->num < o->cell[1]->num);
    }

    if (strcmp(op, ">=") == 0) {
        result = (o->cell[0]->num >= o->cell[1]->num);
    }

    if (strcmp(op, "<=") == 0) {
        result = (o->cell[0]->num <= o->cell[1]->num);
    }
    lobj_del(o);
    return lobj_number(result);
};

LObject* built_in_cmp(lenv* e, LObject* o, char* op)
{
    ERROW_CHECK_NUM(op, o, 2);
    int result;
    if (strcmp(op, "==") == 0) {
        result = lobj_equal(o->cell[0], o->cell[1]);
    }
    if (strcmp(op, "!=") == 0) {
        result = !lobj_equal(o->cell[0], o->cell[1]);
    }

    lobj_del(o);
    return lobj_number(result);
}

LObject* built_in_euql(lenv* e, LObject* o) { return built_in_cmp(e, o, "=="); }
LObject* built_in_not_euql(lenv* e, LObject* o)
{
    return built_in_cmp(e, o, "!=");
}
LObject* built_in_head(lenv* e, LObject* o)
{

    ERROW_CHECK_NUM("head", o, 1);
    ERROW_CHECK_TYPE("head", o, 0, LOBJ_QEXPR);
    ERROW_CHECK_NOT_EMPTY("head", o, 0);

    LObject* v = lobj_take_out(o, 0);
    while (v->count > 1) {
        lobj_del(lobj_pop(v, 1));
    }
    return v;
}

LObject* built_in_tail(lenv* e, LObject* o)
{
    ERROW_CHECK_NUM("tail", o, 1);
    ERROW_CHECK_TYPE("tail", o, 0, LOBJ_QEXPR);
    ERROW_CHECK_NOT_EMPTY("tail", o, 0);

    LObject* v = lobj_take_out(o, 0);
    lobj_del(lobj_pop(v, 0));
    return v;
}

/*convert a s-expression to a q-expression*/
LObject* built_in_list(lenv* e, LObject* o)
{
    o->type = LOBJ_QEXPR;
    return o;
}

/*convert a q-expression to a q-expression*/
LObject* built_in_eval(lenv* e, LObject* o)
{

    ERROW_CHECK_NUM("eval", o, 1);
    ERROW_CHECK_TYPE("eval", o, 0, LOBJ_QEXPR);

    LObject* x = lobj_take_out(o, 0);
    x->type = LOBJ_SEXPR;
    return lobj_eval(e, x);
}

LObject* built_in_join(lenv* e, LObject* o)
{
    for (int i = 0; i < o->count; ++i) {
        ERROW_CHECK_TYPE("join", o, i, LOBJ_QEXPR);
    }

    LObject* x = lobj_pop(o, 0);
    while (o->count) {
        x = lobj_join(x, lobj_pop(o, 0));
    }
    lobj_del(o);
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

LObject* built_in_if(lenv* e, LObject* o)
{
    ERROW_CHECK_NUM("if", o, 3);
    ERROW_CHECK_TYPE("if", o, 0, LOBJ_NUM);
    ERROW_CHECK_TYPE("if", o, 1, LOBJ_QEXPR);
    ERROW_CHECK_TYPE("if", o, 2, LOBJ_QEXPR);

    /*Turn them into an evaluable object*/
    LObject* x;
    o->cell[1]->type = LOBJ_SEXPR;
    o->cell[2]->type = LOBJ_SEXPR;

    if (o->cell[0]->num == TRUE) {
        /*the first argument will determine the run branch*/
        x = lobj_eval(e, lobj_pop(o, 1));
    } else {
        x = lobj_eval(e, lobj_pop(o, 2));
    }
    lobj_del(o);
    return x;
}
LObject* built_in_lambda(lenv* e, LObject* o)
{
    ERROW_CHECK_NUM("\\", o, 2);
    ERROW_CHECK_TYPE("\\", o, 0, LOBJ_QEXPR);
    ERROW_CHECK_TYPE("\\", o, 1, LOBJ_QEXPR);

    /*first Q-expression should only contains symbol*/
    for (int i = 0; i < o->cell[0]->count; ++i) {
        ERROW_CHECK(o, (o->cell[0]->cell[i]->type == LOBJ_SYMBOL),
            "Cannot define non-symbol. Got %s, Expected %s.",
            lobj_type_name(o->cell[0]->cell[i]->type),
            lobj_type_name(LOBJ_SYMBOL));
    }

    /*Pop first two arguments and pass them to lobj_lambda*/
    LObject* argument = lobj_pop(o, 0);
    LObject* body = lobj_pop(o, 0);
    lobj_del(o);

    return lobj_lambda(argument, body);
}
