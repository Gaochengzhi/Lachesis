#include "lachesis_type.h"

char* lobj_type_name(int t)
{
    switch (t) {
    case LOBJ_NUM:
        return "Number";
    case LOBJ_ERR:
        return "error";
    case LOBJ_SYMBOL:
        return "symbol";
    case LOBJ_SEXPR:
        return "s-expression";
    case LOBJ_QEXPR:
        return "q-expression";
    case LOBJ_FUNC:
        return "function";
    default:
        return "Unknown type";
    }
}
