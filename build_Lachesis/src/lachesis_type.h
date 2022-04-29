#ifndef _LACHESIS_TYPE_H
#define _LACHESIS_TYPE_H

#include "../lib/mpc/mpc.h"
#include <stdbool.h>

struct _lobj;
typedef struct _lobj LObject;

struct _lenv;
typedef struct _lenv lenv;

typedef LObject* (*lbuiltin)(lenv*, LObject*);

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
    char* string;

    // sub_object
    int count;         // how many pointers to a list of lobj
    LObject** sub_obj; // pointers to a list of lobj

    // function
    lbuiltin builtin_func;

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
