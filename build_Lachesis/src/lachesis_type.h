
#pragma once
#ifndef _LACHESIS_TYPE_H
#define _LACHESIS_TYPE_H

/*function interface*/

struct _lobj;
typedef struct _lobj LObject;

struct _lenv;
typedef struct _lenv lenv;

typedef LObject* (*lbuiltin)(lenv*, LObject*);

struct _lobj {
    int type;

    long num;
    char* err;
    char* symbol;
    int count;      // how many pointers to a list of lobj
    LObject** cell; // pointers to a list of lobj
    lbuiltin func;  // function pointers
};

struct _lenv {
    int count;
    char** symbol_list;
    LObject** object_list;
};

enum { LOBJ_NUM, LOBJ_ERR, LOBJ_SYMBOL, LOBJ_SEXPR, LOBJ_QEXPR, LOBJ_FUNC };

// function interfaces
char* lobj_type_name(int t);
#endif
