/**
 * File              : lachesis_builtin.c
 * License           : The MIT License (MIT)
 * Author            : Gao Chengzhi <2673730435@qq.com>
 * Date              : 07.03.2022
 * Last Modified Date: 07.05.2022
 * Last Modified By  : Gao Chengzhi <2673730435@qq.com>
 */

#include "lachesis_builtin.h"
#include "lachesis_debug.h"
#include "lachesis_environment.h"
#include "lachesis_object.h"
#include "lachesis_type.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern mpc_parser_t* Number;
extern mpc_parser_t* Double;
extern mpc_parser_t* Symbol;
extern mpc_parser_t* String;
extern mpc_parser_t* Comment;
extern mpc_parser_t* Sexpr;
extern mpc_parser_t* Qexpr;
extern mpc_parser_t* Expr;
extern mpc_parser_t* LExpression;

LObject* built_in_op(lenv* e, LObject* obj, char* op)
{
    /*Make sure all arguments are number type*/
    for (int i = 0; i < obj->count; ++i) {
        if (!(obj->sub_obj[i]->type == LOBJ_DOUBLE
                || obj->sub_obj[i]->type == LOBJ_NUM))
            return lobj_error(
                "operator %s could only apply on number type, get %s", op,
                lobj_type_name(obj->sub_obj[i]->type));
    }

    int is_number = 0;
    /*Pop the first element*/
    LObject* x = lobj_pop(obj, 0);

    if (x->type == LOBJ_NUM)
        x->double_num = (double)(x->num);
    else
        ++is_number;

    /*No arguments and sub will perform the unary operation*/
    /*e.g: y = -4*/
    if ((strcmp(op, "-") == 0) && obj->count == 0) {
        x->double_num = -(x->double_num);
    }

    /*If there are still remaining numbers*/
    while (obj->count > 0) {

        /*Pop the next */
        LObject* y = lobj_pop(obj, 0);

        if (y->type == LOBJ_NUM)
            y->double_num = (double)(y->num);
        else
            ++is_number;
        if (strcmp(op, "+") == 0)
            x->double_num += y->double_num;

        if (strcmp(op, "-") == 0)
            x->double_num -= y->double_num;

        if (strcmp(op, "*") == 0)
            x->double_num *= y->double_num;
        if (strcmp(op, "/") == 0) {
            if (y->double_num == 0) {

                lobj_delete(x);
                lobj_delete(y);
                x = lobj_error("Division by zero!");
                break;
            }
            x->double_num /= y->double_num;
        }

        lobj_delete(y);
    }
    lobj_delete(obj);
    if (is_number)
        x->type = LOBJ_DOUBLE;
    else
        x->num = (int)(x->double_num);
    return x;
}

LObject* built_in_add(lenv* e, LObject* obj)
{
    return built_in_op(e, obj, "+");
}

LObject* built_in_sub(lenv* e, LObject* obj)
{
    return built_in_op(e, obj, "-");
}

LObject* built_in_mult(lenv* e, LObject* obj)
{
    return built_in_op(e, obj, "*");
}

LObject* built_in_div(lenv* e, LObject* obj)
{
    return built_in_op(e, obj, "/");
}

LObject* built_in_great_than(lenv* e, LObject* obj)
{
    return built_in_ord(e, obj, ">");
}

LObject* built_in_great_and_equal(lenv* e, LObject* obj)
{
    return built_in_ord(e, obj, ">=");
}

LObject* built_in_less_than(lenv* e, LObject* obj)
{
    return built_in_ord(e, obj, "<");
}

LObject* built_in_less_and_equal(lenv* e, LObject* obj)
{
    return built_in_ord(e, obj, "<=");
}

LObject* built_in_ord(lenv* e, LObject* obj, char* op)
{
    ERROW_CHECK_NUM(op, obj, 2);

    int result;

    if (strcmp(op, ">") == 0) {

        result = (obj->sub_obj[0]->type == LOBJ_DOUBLE)
            ? (obj->sub_obj[0]->double_num > obj->sub_obj[1]->double_num)
            : (obj->sub_obj[0]->num > obj->sub_obj[1]->num);
    }

    if (strcmp(op, "<") == 0) {
        result = (obj->sub_obj[0]->type == LOBJ_DOUBLE)
            ? (obj->sub_obj[0]->double_num < obj->sub_obj[1]->double_num)
            : (obj->sub_obj[0]->num > obj->sub_obj[1]->num);
    }

    if (strcmp(op, ">=") == 0) {
        result = (obj->sub_obj[0]->type == LOBJ_DOUBLE)
            ? (obj->sub_obj[0]->double_num >= obj->sub_obj[1]->double_num)
            : (obj->sub_obj[0]->num > obj->sub_obj[1]->num);
    }

    if (strcmp(op, "<=") == 0) {
        result = (obj->sub_obj[0]->type == LOBJ_DOUBLE)
            ? (obj->sub_obj[0]->double_num <= obj->sub_obj[1]->double_num)
            : (obj->sub_obj[0]->num > obj->sub_obj[1]->num);
    }
    lobj_delete(obj);
    return lobj_number(result);
};

LObject* built_in_cmp(lenv* e, LObject* obj, char* op)
{
    ERROW_CHECK_NUM(op, obj, 2);
    int result;
    if (strcmp(op, "==") == 0) {
        result = lobj_equal(obj->sub_obj[0], obj->sub_obj[1]);
    }
    if (strcmp(op, "!=") == 0) {
        result = !lobj_equal(obj->sub_obj[0], obj->sub_obj[1]);
    }

    lobj_delete(obj);
    return lobj_number(result);
}

LObject* built_in_euql(lenv* e, LObject* obj)
{
    return built_in_cmp(e, obj, "==");
}
LObject* built_in_not_euql(lenv* e, LObject* obj)
{
    return built_in_cmp(e, obj, "!=");
}

LObject* built_in_head(lenv* e, LObject* obj)
{

    ERROW_CHECK_NUM("head", obj, 1);
    ERROW_CHECK_TYPE("head", obj, 0, LOBJ_QEXPR);
    ERROW_CHECK_NOT_EMPTY("head", obj, 0);

    LObject* head_obj = lobj_take_out(obj, 0);
    while (head_obj->count > 1) {
        lobj_delete(lobj_pop(head_obj, 1));
    }
    return head_obj;
}

LObject* built_in_tail(lenv* e, LObject* obj)
{
    ERROW_CHECK_NUM("tail", obj, 1);
    ERROW_CHECK_TYPE("tail", obj, 0, LOBJ_QEXPR);
    ERROW_CHECK_NOT_EMPTY("tail", obj, 0);

    LObject* v = lobj_take_out(obj, 0);
    lobj_delete(lobj_pop(v, 0));
    return v;
}

/*convert a s-expression to a q-expression*/
LObject* built_in_list(lenv* e, LObject* obj)
{
    obj->type = LOBJ_QEXPR;
    return obj;
}

/*convert a q-expression to a s-expression*/
LObject* built_in_eval(lenv* e, LObject* obj)
{

    ERROW_CHECK_NUM("eval", obj, 1);
    ERROW_CHECK_TYPE("eval", obj, 0, LOBJ_QEXPR);

    LObject* x = lobj_take_out(obj, 0);
    x->type = LOBJ_SEXPR;
    return lobj_eval(e, x);
}

LObject* built_in_join(lenv* e, LObject* obj)
{
    for (int i = 0; i < obj->count; ++i)
        ERROW_CHECK_TYPE("join", obj, i, LOBJ_QEXPR);

    LObject* x = lobj_pop(obj, 0);
    while (obj->count) {
        LObject* tmp = lobj_pop(obj, 0);
        x = lobj_join(x, tmp);
    }
    lobj_delete(obj);
    return x;
}

LObject* built_in_var(lenv* e, LObject* obj, char* func_name)
{
    ERROW_CHECK_TYPE(func_name, obj, 0, LOBJ_QEXPR);

    /*catch the symbol_list in the first of the list*/
    LObject* symbol_list = obj->sub_obj[0];

    /*make sure all elements of symbol_list are symbols*/
    for (int i = 0; i < symbol_list->count; ++i) {
        ERROW_CHECK(obj, symbol_list->sub_obj[i]->type == LOBJ_SYMBOL,
            "Function %s cannot define non-symbol.\n Got %s, Expected %s.",
            func_name, lobj_type_name(symbol_list->sub_obj[i]->type),
            lobj_type_name(LOBJ_SYMBOL));
    }

    /*check symbol and object values*/
    ERROW_CHECK(obj, symbol_list->count == obj->count - 1,
        "Function %s cannot define incorrect number of objects.\n Got %i, ",
        func_name, "Expected %i.", symbol_list->count, obj->count - 1);

    for (int i = 0; i < symbol_list->count; ++i) {
        if (strcmp(func_name, "def") == 0) {
            lenv_define(e, symbol_list->sub_obj[i], obj->sub_obj[i + 1]);
        }
        if (strcmp(func_name, "=") == 0) {
            lenv_put_symbol(e, symbol_list->sub_obj[i], obj->sub_obj[i + 1]);
        }
    }

    lobj_delete(obj);
    return lobj_sexpr();
}
LObject* built_in_put(lenv* e, LObject* obj)
{
    return built_in_var(e, obj, "=");
}

LObject* built_in_define(lenv* e, LObject* obj)
{
    return built_in_var(e, obj, "def");
}

LObject* built_in_if(lenv* e, LObject* obj)
{
    ERROW_CHECK_NUM("if", obj, 3);
    ERROW_CHECK_TYPE("if", obj, 0, LOBJ_NUM);
    ERROW_CHECK_TYPE("if", obj, 1, LOBJ_QEXPR);
    ERROW_CHECK_TYPE("if", obj, 2, LOBJ_QEXPR);

    /*Turn them into an evaluable object*/
    LObject* x;
    obj->sub_obj[1]->type = LOBJ_SEXPR;
    obj->sub_obj[2]->type = LOBJ_SEXPR;

    if (obj->sub_obj[0]->num == true) {
        /*the first argument will determine the run branch*/
        x = lobj_eval(e, lobj_pop(obj, 1));
    } else {
        x = lobj_eval(e, lobj_pop(obj, 2));
    }
    lobj_delete(obj);
    return x;
}
LObject* built_in_lambda(lenv* e, LObject* obj)
{
    ERROW_CHECK_NUM("\\", obj, 2);
    ERROW_CHECK_TYPE("\\", obj, 0, LOBJ_QEXPR);
    ERROW_CHECK_TYPE("\\", obj, 1, LOBJ_QEXPR);

    /*first Q-expression should only contains symbol*/
    for (int i = 0; i < obj->sub_obj[0]->count; ++i) {
        ERROW_CHECK(obj, (obj->sub_obj[0]->sub_obj[i]->type == LOBJ_SYMBOL),
            "Cannot define non-symbol. Got %s, Expected %s.",
            lobj_type_name(obj->sub_obj[0]->sub_obj[i]->type),
            lobj_type_name(LOBJ_SYMBOL));
    }

    /*Pop first two arguments and pass them to lobj_lambda*/
    LObject* argument = lobj_pop(obj, 0);
    LObject* body = lobj_pop(obj, 0);
    lobj_delete(obj);

    return lobj_lambda(argument, body);
}

LObject* built_in_import(lenv* e, LObject* obj)
{
    ERROW_CHECK_NUM("import", obj, 1);
    ERROW_CHECK_TYPE("import", obj, 0, LOBJ_STR);
    mpc_result_t raw;
    /*int mpc_parse_contents(const char *filename, mpc_parser_t *p, mpc_result_t
     * *r) */
    if (mpc_parse_contents(obj->sub_obj[0]->str, LExpression, &raw)) {
        LObject* expression = lobj_read(raw.output);
        mpc_ast_delete(raw.output);

        while (expression->count) {
            LObject* x = lobj_eval(e, lobj_pop(expression, 0));
            if (x->type == LOBJ_ERR)
                lobj_print_line(x);
        }

        lobj_delete(expression);
        lobj_delete(obj);
        return lobj_sexpr();
    } else {
        char* error_message = mpc_err_string(raw.error);
        mpc_err_delete(raw.error);
        LObject* error_obj
            = lobj_error("Could not import the library %s", error_message);
        free(error_message);
        lobj_delete(obj);
        return error_obj;
    }
}
LObject* built_in_print(lenv* e, LObject* obj)
{
    for (int i = 0; i < obj->count; ++i)
        lobj_print(obj->sub_obj[i]);
    lobj_delete(obj);
    return lobj_sexpr();
}
LObject* built_in_fprint(lenv* e, LObject* obj)
{
    ERROW_CHECK_TYPE("fprint", obj, 0, LOBJ_STR);
    char* file_name = obj->sub_obj[0]->str;
    FILE* ptr = fopen(file_name, "a");
    for (int i = 1; i < obj->count; ++i)
        lobj_fprint(ptr, obj->sub_obj[i]);
    lobj_delete(obj);
    fclose(ptr);
    return lobj_sexpr();
}
LObject* built_in_error(lenv* e, LObject* obj)
{
    ERROW_CHECK_NUM("error", obj, 1);
    ERROW_CHECK_TYPE("error", obj, 1, LOBJ_STR);

    LObject* error_obj = lobj_error(obj->sub_obj[0]->str);
    lobj_delete(obj);
    return error_obj;
}
LObject* built_in_loop(lenv* e, LObject* obj)
{
    ERROW_CHECK_NUM("loop", obj, 2);
    ERROW_CHECK_TYPE("loop", obj, 0, LOBJ_NUM);
    ERROW_CHECK_TYPE("loop", obj, 1, LOBJ_QEXPR);
    obj->sub_obj[1]->type = LOBJ_SEXPR;
    int _count = (int)obj->sub_obj[0]->num;
    LObject* res;
    LObject* x;
    LObject* y;
    int i = 0;
    LObject* l[_count];
    while (_count > i) {
        y = lobj_copy(obj);
        res = lobj_pop(y, 1);
        x = lobj_eval(e, res);
        l[i] = x;
        lobj_print_line(x);
        x = NULL;
        y = NULL;
        res = NULL;
        i++;
    }
    lobj_delete(obj);
    return l[1];
}
