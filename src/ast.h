#ifndef P_AST_H
#define P_AST_H

#include "libop/op.h"

#include "exception.h"
#include "common.h"
#include "lexer.h"

namespace p {
    struct AST { };

    AST parse(Lexer& lexer);
    
    template<class ForwardIterator>
    AST compile(ForwardIterator begin, ForwardIterator end);
}

#endif


