#include <memory>

#include "libop/op.h"

#include "ast.h"
#include "parse.h"
#include "lexer.h"

static std::shared_ptr<AST> parse(Lexer& lexer);
static std::shared_ptr<AST> parse_block(Lexer& lexer);
static std::shared_ptr<AST> parse_expression(Lexer& lexer);

static std::shared_ptr<AST> parse(Lexer& lexer) {
    return parse_block(lexer);
}


namespace p {
    std::shared_ptr<AST> compile(u32str::iterator begin, u32str::iterator end) {
        Lexer lexer(begin, end);
        return parse(lexer);
    }
}
