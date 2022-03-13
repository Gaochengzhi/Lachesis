/**
 * File              : lachesis_object.c
 * License           : The MIT License (MIT)
 * Author            : Gao Chengzhi <2673730435@qq.com>
 * Date              : 18.02.2022
 * Last Modified Date: 13.03.2022
 * Last Modified By  : Gao Chengzhi <2673730435@qq.com>
 */

#include "lachesis_object.h"
#include "lachesis_debug.h"
#include "lachesis_environment.h"
#include "lachesis_type.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

LObject* lobj_string(const char* s)
{
    LObject* v = malloc(sizeof(LObject));
    lbug_print_sssl("assign string", s, "on pointer", (long)v); // debug line
    v->type = LOBJ_STR;
    v->string = malloc(sizeof(s) + 1);
    strcpy(v->string, s);
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
        if (!v->func) {
            /*self-define cases*/
            lenv_del(v->env);
            lobj_del(v->argument);
            lobj_del(v->body);
        }
        break;
        /*error or symbol will free its string data*/
    case LOBJ_STR:
        free(v->string);
        break;
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

LObject* lobj_read_str(mpc_ast_t* t)
{
    /*drop the last \0*/
    t->contents[strlen(t->contents) - 1] = '\0';
    char* unescaped = malloc(strlen(t->contents + 1) + 1);
    strcpy(unescaped, t->contents + 1);
    unescaped = mpcf_unescape(unescaped);
    LObject* string = lobj_string(unescaped);
    free(unescaped);
    return string;
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

    if (strstr(t->tag, "string")) {
        return lobj_read_str(t);
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

    case LOBJ_STR:
        lobj_print_string(v);
        break;
    case LOBJ_FUNC:
        if (v->func) {
            printf("<built_in_function>");
        } else {
            printf("(\\ ");
            lobj_print(v->argument);
            putchar(' ');
            lobj_print(v->body);
            putchar(' ');
        }
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

void lobj_print_string(LObject* v)
{
    char* escape_str = malloc(sizeof(v->string) + 1);
    strcpy(escape_str, v->string);
    escape_str = mpcf_escape(escape_str);
    printf("\"%s\"", escape_str);
    free(escape_str);
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

    LObject* result = lobj_call(e, f, v);
    lobj_del(f);
    return result;
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
        if (v->func) {
            /*built in function cases*/
            x->func = v->func;
        } else {
            /*self define cases*/
            x->func = NULL;
            x->env = lenv_copy(v->env);
            x->body = lobj_copy(v->body);
            x->argument = lobj_copy(v->argument);
        }
        break;
    case LOBJ_NUM:
        x->num = v->num;
        break;

    case LOBJ_ERR:
        x->err = malloc(strlen(v->err) + 1);
        strcpy(x->err, v->err);
        break;
    case LOBJ_STR:
        x->string = malloc(strlen(v->string) + 1);
        strcpy(x->string, v->string);
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

LObject* lobj_lambda(LObject* arguments, LObject* body)
{
    LObject* v = malloc(sizeof(LObject));
    v->type = LOBJ_FUNC;

    v->func = NULL;
    v->env = lenv_new();
    v->argument = arguments;
    v->body = body;
    return v;
}

LObject* lobj_call(lenv* e, LObject* func, LObject* o)
{
    /*built in function will be called directly*/
    if (func->func) {
        return func->func(e, o);
    }

    /*split arguments into two*/
    int given = o->count;
    int total = func->argument->count;

    while (o->count) {
        if (func->argument->count == 0) {
            lobj_del(o);
            return lobj_error(
                "Function passed too many arguments.\n Got %d, Expected %d",
                given, total);
        }

        /*Pop the first symbol from the argument;*/
        LObject* symbol = lobj_pop(func->argument, 0);
        /*Pop the next argument from the list*/
        LObject* value = lobj_pop(o, 0);

        lenv_put(func->env, symbol, value);
        lobj_del(symbol);
        lobj_del(value);
    }

    lobj_del(o);

    if (func->argument->count == 0) {
        func->env->parent = e;
        return built_in_eval(
            func->env, lobj_add(lobj_sexpr(), lobj_copy(func->body)));
    } else {
        return lobj_copy(func);
    }
}

int lobj_equal(LObject* x, LObject* y)
{
    if (x->type != y->type) {
        return 0;
    }
    switch (x->type) {
    case LOBJ_NUM:
        return x->num == y->num;
    case LOBJ_ERR:
        return (strcmp(x->err, y->err) == 0);
    case LOBJ_STR:
        return (strcmp(x->string, y->string) == 0);
    case LOBJ_SYMBOL:
        return (strcmp(x->symbol, y->symbol) == 0);
    case LOBJ_FUNC:
        if (x->func || y->func) {
            return x->func == y->func;
        } else {
            return lobj_equal(x->argument, y->argument)
                && lobj_equal(x->body, y->body);
        }
    case LOBJ_QEXPR:
    case LOBJ_SEXPR:
        if (x->count != y->count) {
            return 0;
        }
        for (int i = 0; i < x->count; ++i) {
            if (!lobj_equal(x->cell[i], y->cell[i])) {
                return 0;
            }
        }
        return 1;
        break;
    }
    return 0;
}
