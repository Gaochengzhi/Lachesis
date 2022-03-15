#ifndef _LACHESIS_TYPE_H
#define _LACHESIS_TYPE_H

#include "../lib/mpc/mpc.h"

struct _lobj;
typedef struct _lobj LObject;

struct _lenv;
typedef struct _lenv lenv;

typedef LObject *(*lbuiltin)(lenv *, LObject *);

struct _lobj
{
    int type;

    // Basic_types
    long num;
    char *err;
    char *symbol;
    char *string;

    // sub_object
    int count;         // how many pointers to a list of lobj
    LObject **sub_obj; // pointers to a list of lobj

    // function
    lbuiltin builtin_func;
    lenv *env;
    LObject *argument;
    LObject *body;
};

struct _lenv
{
    lenv *parent;
    int count;
    char **symbol_list;
    LObject **object_list;
};
enum
{
    LOBJ_NUM,
    LOBJ_ERR,
    LOBJ_SYMBOL,
    LOBJ_SEXPR,
    LOBJ_QEXPR,
    LOBJ_FUNC,
    LOBJ_STR
};

// function interfaces

char *lobj_type_name(int typename);

typedef int bool;
#define TRUE 1
#define FALSE 0

/*Lists of Parsers*/
#endif
