#ifndef P_PARSE_H
#define P_PARSE_H

#include "ast.h"
#include "lexer.h"

namespace p {
    AST parse(Lexer& lexer);
    AST compile(u32str::iterator begin, u32str::iterator end);
}

#endif
