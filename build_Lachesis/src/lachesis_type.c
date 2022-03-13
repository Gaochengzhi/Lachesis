
#include "lachesis_type.h"

char* lobj_type_name(int t)
{
    switch (t) {
    case LOBJ_NUM:
        return "Number";
    case LOBJ_ERR:
        return "Error";
    case LOBJ_STR:
        return "String";
    case LOBJ_SYMBOL:
        return "Symbol";
    case LOBJ_SEXPR:
        return "S-expression";
    case LOBJ_QEXPR:
        return "Q-expression";
    case LOBJ_FUNC:
        return "Function";
    default:
        return "Unknown type";
    }
}
