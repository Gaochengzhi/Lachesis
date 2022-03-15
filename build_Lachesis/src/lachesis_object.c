/**
 * File              : lachesis_object.c
 * License           : The MIT License (MIT)
 * Author            : Gao Chengzhi <2673730435@qq.com>
 * Date              : 18.02.2022
 * Last Modified Date: 14.03.2022
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
LObject *lobj_number(long number)
{
    LObject *obj = malloc(sizeof(LObject));
    lbug_print_slsl("assign number:", number, "on pointer", (long)obj); // debug line
    obj->type = LOBJ_NUM;
    obj->num = number;
    return obj;
}

/*sending lobj the error messages and tag it */
LObject *lobj_error(char *fmt, ...)
{

    LObject *error_obj = malloc(sizeof(LObject));
    lbug_print_sssl("assign err:", fmt, "on pointer", (long)error_obj); // debug line
    error_obj->type = LOBJ_ERR;

    /*val_list initializatiojn*/
    va_list val;
    va_start(val, fmt);
    error_obj->err = malloc(512);
    vsnprintf(error_obj->err, 511, fmt, val);
    error_obj->err = realloc(error_obj->err, strlen(error_obj->err) + 1);
    va_end(val);
    return error_obj;
}

LObject *lobj_symbol(char *symbol)
{
    LObject *symbol_obj = malloc(sizeof(LObject));
    lbug_print_sssl("assign symbol", symbol, "on pointer", (long)symbol_obj); // debug line
    symbol_obj->type = LOBJ_SYMBOL;
    symbol_obj->symbol = malloc(strlen(symbol) + 1);
    strcpy(symbol_obj->symbol, symbol);
    return symbol_obj;
}

LObject *lobj_string(const char *string)
{
    LObject *string_obj = malloc(sizeof(LObject));
    lbug_print_sssl("assign string", string, "on pointer", (long)string_obj); // debug line
    string_obj->type = LOBJ_STR;
    string_obj->string = malloc(sizeof(string) + 1);
    strcpy(string_obj->string, string);
    return string_obj;
}
/*Initialize a pointer to a new empty LObject*/
LObject *lobj_sexpr(void)
{
    LObject *new_obj = malloc(sizeof(LObject));
    lbug_print_sl("init a s-expression on pointer", (long)new_obj); // debug line
    new_obj->type = LOBJ_SEXPR;
    new_obj->count = 0;
    new_obj->sub_obj = NULL;
    return new_obj;
}

LObject *lobj_qexpr(void)
{
    LObject *new_obj = malloc(sizeof(LObject));
    lbug_print_sl("init a q-expression on pointer", (long)new_obj); // debug line
    new_obj->type = LOBJ_QEXPR;
    new_obj->count = 0;
    new_obj->sub_obj = NULL;
    return new_obj;
}

/*Add lobj x into lobj o*/
LObject *lobj_add(LObject *target_obj, LObject *add_obj)
{

    lbug_print_slsl("add lobj:", (long)target_obj, "on lobj: ", (long)add_obj); // debug line
    target_obj->count++;
    target_obj->sub_obj = realloc(target_obj->sub_obj, sizeof(LObject *) * (target_obj->count));
    target_obj->sub_obj[target_obj->count - 1] = add_obj;
    return target_obj;
}

void lobj_del(LObject *o)
{
    lbug_print_slsl("delete lobj:", (long)o, "type", o->type); // debug line
    switch (o->type)
    {
        /*number and function type will let it go*/
    case LOBJ_NUM:
        break;
    case LOBJ_FUNC:
        if (!o->builtin_func)
        {
            /*user-define cases*/
            lenv_del(o->env);
            lobj_del(o->argument);
            lobj_del(o->body);
        }
        break;
        /*error or symbol will free its string data*/
    case LOBJ_STR:
        free(o->string);
        break;
    case LOBJ_ERR:
        free(o->err);
        break;
    case LOBJ_SYMBOL:
        free(o->symbol);
        break;

        /*q or s-expression will free all its data*/
    case LOBJ_QEXPR:
    case LOBJ_SEXPR:
        for (int i = 0; i < o->count; ++i)
        {
            lobj_del(o->sub_obj[i]);
        }
        break;
    }
    /*free the memory allocated to contain the pointers*/
    free(o);
}

LObject *lobj_read_num(mpc_ast_t *t)
{

    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    lbug_print_slsl("read num: ", x, "from tree t", (long)t); // debug line

    return errno != ERANGE ? lobj_number(x)
                           : lobj_error("Too long or invalid number!");
}

LObject *lobj_read_str(mpc_ast_t *t)
{
    /*drop the last \0*/
    t->contents[strlen(t->contents) - 1] = '\0';
    char *unescaped = malloc(strlen(t->contents + 1) + 1);
    strcpy(unescaped, t->contents + 1);
    unescaped = mpcf_unescape(unescaped);
    LObject *string = lobj_string(unescaped);
    free(unescaped);
    return string;
}
/*read the parser input and return the conversion of that type*/
LObject *lobj_read(mpc_ast_t *t)
{

    if (strstr(t->tag, "number"))
    {
        return lobj_read_num(t);
    }

    if (strstr(t->tag, "symbol"))
    {
        return lobj_symbol(t->contents);
    }

    if (strstr(t->tag, "string"))
    {
        return lobj_read_str(t);
    }
    /*root means '>'! s-expression and root will 1.create an empty list*/
    LObject *x = NULL;
    if (strcmp(t->tag, ">") == 0)
    {
        x = lobj_sexpr();
    }
    if (strstr(t->tag, "sexpr"))
    {
        x = lobj_sexpr();
    }

    if (strstr(t->tag, "qexpr"))
    {
        x = lobj_qexpr();
    }
    /*then 2.fill the list with its expression*/
    for (int i = 0; i < t->children_num; ++i)
    {

        if (strcmp(t->children[i]->contents, "[") == 0)
        {
            continue;
        }

        if (strcmp(t->children[i]->contents, "]") == 0)
        {
            continue;
        }
        if (strcmp(t->children[i]->contents, "{") == 0)
        {
            continue;
        }
        if (strcmp(t->children[i]->contents, "}") == 0)
        {
            continue;
        }
        if (strcmp(t->children[i]->tag, "regex") == 0)
        {
            continue;
        }
        x = lobj_add(x, lobj_read(t->children[i]));
    }
    return x;
}

/*print a LObject*/

void lobj_print(LObject *o)
{
    switch (o->type)
    {
    case LOBJ_NUM:
        printf("%li", o->num);
        break;

    case LOBJ_ERR:
        printf("Error: %s", o->err);
        break;

    case LOBJ_STR:
        lobj_print_string(o);
        break;
    case LOBJ_FUNC:
        if (o->builtin_func)
        {
            printf("<built_in_function>");
        }
        else
        {
            printf("(\\ ");
            lobj_print(o->argument);
            putchar(' ');
            lobj_print(o->body);
            putchar(' ');
        }
        break;

    case LOBJ_SYMBOL:
        printf("%s", o->symbol);
        break;

    case LOBJ_SEXPR:
        lobj_print_expr(o, '[', ']');
        break;

    case LOBJ_QEXPR:
        lobj_print_expr(o, '{', '}');
        break;
    }
}

void lobj_print_string(LObject *string_obj)
{
    char *escape_str = malloc(sizeof(string_obj->string) + 1);
    strcpy(escape_str, string_obj->string);
    escape_str = mpcf_escape(escape_str);
    printf("\"%s\"", escape_str);
    free(escape_str);
}
void lobj_print_expr(LObject *obj, char begin_symbol, char end_symbol)
{
    putchar(begin_symbol);
    for (int i = 0; i < obj->count; ++i)
    {
        lobj_print(obj->sub_obj[i]);

        /*last element will not have trauling space*/
        if (i != (obj->count - 1))
        {
            putchar(' ');
        }
    }
    putchar(end_symbol);
}

void lobj_print_line(LObject *o)
{
    lobj_print(o);
    putchar('\n');
}

/*Only evaluate S-expression, all other things will return*/
LObject *lobj_eval(lenv *e, LObject *o)
{
    /*when encountering symbols, delete the input*/
    if (o->type == LOBJ_SYMBOL)
    {
        LObject *x = lenv_get_copy(e, o);
        lobj_del(o);
        return x;
    }
    if (o->type == LOBJ_SEXPR)
    {
        return lobj_eval_sexpr(e, o);
    }
    return o;
}

LObject *lobj_eval_sexpr(lenv *e, LObject *o)
{
    /*Evaluate its children*/
    for (int i = 0; i < o->count; ++i)
    {
        o->sub_obj[i] = lobj_eval(e, o->sub_obj[i]);
    }

    /*Error handling*/
    for (int i = 0; i < o->count; ++i)
    {
        if (o->sub_obj[i]->type == LOBJ_ERR)
        {
            return lobj_take_out(o, i);
        }
    }

    /*Empty Cases*/
    if (o->count == 0)
    {
        return o;
    }

    /*Single Cases*/
    if (o->count == 1)
    {
        return lobj_take_out(o, 0);
    }

    /*Make sure the first element is a function */
    LObject *f = lobj_pop(o, 0);
    if (f->type != LOBJ_FUNC)
    {
        lobj_del(f);
        lobj_del(o);
        return lobj_error("first element is not a function");
    }

    LObject *result = lobj_call(e, f, o);
    lobj_del(f);
    return result;
}

LObject *lobj_join(LObject *x, LObject *y)
{
    for (int i = 0; i < y->count; ++i)
    {
        x = lobj_add(x, y->sub_obj[i]);
    }
    free(y->sub_obj);
    free(y);
    return x;
}
/*Pop up the i-th item inside the cell, return the poped one*/
LObject *lobj_pop(LObject *o, int i)
{

    lbug_print_sdsl(
        "pop lobj the number:", i, "on pointer", (long)o); // debug line

    LObject *x = o->sub_obj[i];
    /*void *memmove( void *str1, const void *str2, size_t n );*/
    memmove(
        &o->sub_obj[i], &o->sub_obj[i + 1], sizeof(LObject *) * (o->count-- - i - 1));
    // remember to decrease the o->count!

    /*realloc the memory we used*/
    o->sub_obj = realloc(o->sub_obj, sizeof(LObject *) * o->count);
    return x;
}

/*Same as lobj_pop, but delete the original list*/
LObject *lobj_take_out(LObject *o, int i)
{
    LObject *x = lobj_pop(o, i);
    lobj_del(o);
    return x;
}

LObject *lobj_func(lbuiltin func)
{
    LObject *o = malloc(sizeof(LObject));
    o->type = LOBJ_FUNC;
    o->builtin_func = func;
    return o;
}

/*copy a new one and return it*/
LObject *lobj_copy(LObject *o)
{
    LObject *x = malloc(sizeof(LObject));
    x->type = o->type;

    switch (o->type)
    {
    case LOBJ_FUNC:
        if (o->builtin_func)
        {
            /*built in function cases*/
            x->builtin_func = o->builtin_func;
        }
        else
        {
            /*self define cases*/
            x->builtin_func = NULL;
            x->env = lenv_copy(o->env);
            x->body = lobj_copy(o->body);
            x->argument = lobj_copy(o->argument);
        }
        break;

    case LOBJ_NUM:
        x->num = o->num;
        break;
    case LOBJ_ERR:
        x->err = malloc(strlen(o->err) + 1);
        strcpy(x->err, o->err);
        break;
    case LOBJ_STR:
        x->string = malloc(strlen(o->string) + 1);
        strcpy(x->string, o->string);
        break;
    case LOBJ_SYMBOL:
        x->symbol = malloc(strlen(o->symbol) + 1);
        strcpy(x->symbol, o->symbol);
        break;
    case LOBJ_SEXPR:
    case LOBJ_QEXPR:
        x->count = o->count;
        x->sub_obj = malloc(sizeof(LObject *) * x->count);
        for (int i = 0; i < x->count; ++i)
        {
            x->sub_obj[i] = lobj_copy(o->sub_obj[i]);
        }
        break;
    }

    return x;
}

LObject *lobj_lambda(LObject *arguments, LObject *body)
{
    LObject *o = malloc(sizeof(LObject));
    o->type = LOBJ_FUNC;

    o->builtin_func = NULL;
    o->env = lenv_new();
    o->argument = arguments;
    o->body = body;
    return o;
}

LObject *lobj_call(lenv *e, LObject *func, LObject *o)
{
    /*built in function will be called directly*/
    if (func->builtin_func)
    {
        return func->builtin_func(e, o);
    }

    /*split arguments into two*/
    int given = o->count;
    int total = func->argument->count;

    while (o->count)
    {
        if (func->argument->count == 0)
        {
            lobj_del(o);
            return lobj_error(
                "Function passed too many arguments.\n Got %d, Expected %d",
                given, total);
        }

        /*Pop the first symbol from the argument;*/
        LObject *symbol = lobj_pop(func->argument, 0);
        /*Pop the next argument from the list*/
        LObject *value = lobj_pop(o, 0);

        lenv_put(func->env, symbol, value);
        lobj_del(symbol);
        lobj_del(value);
    }

    lobj_del(o);

    if (func->argument->count == 0)
    {
        func->env->parent = e;
        return built_in_eval(
            func->env, lobj_add(lobj_sexpr(), lobj_copy(func->body)));
    }
    else
    {
        return lobj_copy(func);
    }
}

bool lobj_equal(LObject *x, LObject *y)
{
    if (x->type != y->type)
    {
        return FALSE;
    }
    switch (x->type)
    {
    case LOBJ_NUM:
        return x->num == y->num;
    case LOBJ_ERR:
        return (strcmp(x->err, y->err) == 0);
    case LOBJ_STR:
        return (strcmp(x->string, y->string) == 0);
    case LOBJ_SYMBOL:
        return (strcmp(x->symbol, y->symbol) == 0);
    case LOBJ_FUNC:
        if (x->builtin_func || y->builtin_func)
        {
            return x->builtin_func == y->builtin_func;
        }
        else
        {
            return lobj_equal(x->argument, y->argument) && lobj_equal(x->body, y->body);
        }
    case LOBJ_QEXPR:
    case LOBJ_SEXPR:
        if (x->count != y->count)
        {
            return FALSE;
        }
        for (int i = 0; i < x->count; ++i)
        {
            if (!lobj_equal(x->sub_obj[i], y->sub_obj[i]))
            {
                return FALSE;
            }
        }
        return TRUE;
        break;
    }
    return FALSE;
}
