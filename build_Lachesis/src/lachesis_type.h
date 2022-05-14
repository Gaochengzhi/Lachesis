/**
 * File              : lachesis_type.h
 * License           : The MIT License (MIT)
 * Author            : Gao Chengzhi <2673730435@qq.com>
 * Date              : 06.05.2022
 * Last Modified Date: 06.05.2022
 * Last Modified By  : Gao Chengzhi <2673730435@qq.com>
 */
#ifndef _LACHESIS_TYPE_H
#define _LACHESIS_TYPE_H

#include "../lib/mpc/mpc.h"
#include <stdbool.h>

struct _lobj;
typedef struct _lobj LObject;

struct _lenv;
typedef struct _lenv lenv;

typedef LObject* (*builtin_func_type)(lenv*, LObject*);

enum ltype {
    LOBJ_NUM,
    LOBJ_DOUBLE,
    LOBJ_ERR,
    LOBJ_SYMBOL,
    LOBJ_STR,
    LOBJ_SEXPR,
    LOBJ_QEXPR,
    LOBJ_FUNC
};
struct _lobj {
    enum ltype type;

    // Basic_types
    long num;
    double double_num;
    char* err;
    char* symbol;
    char* str;

    // sub_object
    int count;         // how many pointers to a list of lobj
    LObject** sub_obj; // pointers to a list of lobj

    // function
    builtin_func_type builtin_func;

    // environmnet
    lenv* env;
    LObject* argument;
    LObject* body;
};

struct _lenv {
    lenv* parent;
    int count;
    char** symbol_list;
    LObject** functor_list;
};

// function interfaces

char* lobj_type_name(int typename);

/*Lists of Parsers*/
#endif
