#include "libop/op.h"

#include "ast.h"
#include "lexer.h"

namespace p {
    AST parse(Lexer& lexer) {
        op::optional<Token> token;
        while (token = lexer.get_token()) {
            if (token->type == Token::Type::newline) op::print(Token::type_names.at(token->type));
            else op::print(Token::type_names.at(token->type), u32_to_string(token->value));

        }

        return AST();
    }

    template<class ForwardIterator>
    AST compile(ForwardIterator begin, ForwardIterator end) {
        Lexer lexer(begin, end);
        return parse(lexer);
    }
}
