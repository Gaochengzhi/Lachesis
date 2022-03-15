/**
 * File              : lachesis_environment.c
 * License           : The MIT License (MIT)
 * Author            : Gao Chengzhi <2673730435@qq.com>
 * Date              : 26.02.2022
 * Last Modified Date: 14.03.2022
 * Last Modified By  : Gao Chengzhi <2673730435@qq.com>
 */

#include "lachesis_environment.h"
#include "lachesis_builtin.h"
#include "lachesis_object.h"
#include "lachesis_type.h"
#include <stdlib.h>
#include <string.h>

lenv *lenv_new(void)
{
    lenv *e = malloc(sizeof(lenv));
    e->parent = NULL;
    e->count = 0;
    e->symbol_list = NULL;
    e->object_list = NULL;
    return e;
}

void lenv_del(lenv *e)
{
    for (int i = 0; i < e->count; ++i)
    {
        free(e->symbol_list[i]);
        lobj_del(e->object_list[i]);
    }

    free(e->symbol_list);
    free(e->object_list);
    free(e);
}

lenv *lenv_copy(lenv *e)
{
    lenv *new = malloc(sizeof(lenv));
    new->parent = e->parent;
    new->count = e->count;
    new->symbol_list = malloc(sizeof(char *) * e->count);
    new->object_list = malloc(sizeof(LObject *) * e->count);
    for (int i = 0; i < new->count; ++i)
    {
        new->symbol_list[i] = malloc(strlen(e->symbol_list[i]) + 1);
        strcpy(new->symbol_list[i], e->symbol_list[i]);
        new->object_list[i] = lobj_copy(e->object_list[i]);
    }
    return new;
}

/*get an copy of the LObject in lenv*/
LObject *lenv_get_copy(lenv *mother_env, LObject *son_obj)
{

    for (int i = 0; i < mother_env->count; ++i)
    {
        if (strcmp(mother_env->symbol_list[i], son_obj->symbol) == 0)
        {
            return lobj_copy(mother_env->object_list[i]);
        }
    }

    if (mother_env->parent)
    {
        return lenv_get_copy(mother_env->parent, son_obj);
    }
    else
    {
        return lobj_error("Undefined symbol!");
    }
}

void lenv_put(lenv *e, LObject *o, LObject *func_ptr)
{
    /*iterate e, if o's symbol already in e, delete the old one */
    /*and replace with the new one;*/
    for (int i = 0; i < e->count; ++i)
    {
        if (strcmp(e->symbol_list[i], o->symbol) == 0)
        {
            lobj_del(e->object_list[i]);
            e->object_list[i] = lobj_copy(func_ptr);
            return;
        }
    }

    /*if not repeated found, allocate for new obj */
    e->count++;
    e->object_list = realloc(e->object_list, sizeof(LObject *) * e->count);
    e->symbol_list = realloc(e->symbol_list, sizeof(char *) * e->count);
    /*copy func_ptr and symbol*/
    e->object_list[e->count - 1] = lobj_copy(func_ptr);
    e->symbol_list[e->count - 1] = malloc(strlen(o->symbol) + 1);
    strcpy(e->symbol_list[e->count - 1], o->symbol);
}

void lenv_add_builtin_func(lenv *e, char *name, lbuiltin func)
{
    LObject *o = lobj_symbol(name);
    LObject *func_obj = lobj_func(func);
    lenv_put(e, o, func_obj);
    lobj_del(o);
    lobj_del(func_obj);
}

void lenv_define(lenv *e, LObject *o, LObject *func)
{
    /*find e->parent's parent... */
    while (e->parent)
    {
        e = e->parent;
    }
    lenv_put(e, o, func);
}
void lenv_builtin_init_list(lenv *e)
{
    /*__BASIC_TYPES__*/
    lenv_add_builtin_func(e, "list", built_in_list);
    lenv_add_builtin_func(e, "head", built_in_head);
    lenv_add_builtin_func(e, "tail", built_in_tail);
    lenv_add_builtin_func(e, "eval", built_in_eval);
    lenv_add_builtin_func(e, "join", built_in_join);

    lenv_add_builtin_func(e, "+", built_in_add);
    lenv_add_builtin_func(e, "-", built_in_sub);
    lenv_add_builtin_func(e, "*", built_in_mult);
    lenv_add_builtin_func(e, "/", built_in_div);

    /*__FUNCTION__*/
    lenv_add_builtin_func(e, "def", built_in_define);
    lenv_add_builtin_func(e, "=", built_in_put);
    lenv_add_builtin_func(e, "==", built_in_euql);
    lenv_add_builtin_func(e, "!=", built_in_not_euql);
    lenv_add_builtin_func(e, ">", built_in_great_than);
    lenv_add_builtin_func(e, "<", built_in_less_than);
    lenv_add_builtin_func(e, ">=", built_in_great_and_equal);
    lenv_add_builtin_func(e, "<=", built_in_less_and_equal);

    /*__STRING_FUNCTION__*/
    lenv_add_builtin_func(e, "load", built_in_load);
    lenv_add_builtin_func(e, "error", built_in_error);
    lenv_add_builtin_func(e, "print", built_in_print);
}
