#ifndef P_LEXER_H
#define P_LEXER_H

#include "common.h"


namespace p {
    struct Token {
        enum Type {
            open_paren,
            close_paren,
            open_square,
            close_square,
            open_brace,
            close_brace,
            colon,
            comma,
            period,
            comment,
            identifier,
            newline,
            number,
            oper,
            string
        };

        static const std::map<Token::Type, std::string> type_names;

        Token(Type type, u32str value, size_t line, size_t col)
        : type(type), value(value), line(line), col(col) { }

        Type type;
        u32str value;
        size_t line;
        size_t col;
    };


    class Lexer {
    public:
        Lexer(u32str::iterator begin, u32str::iterator end)
        : line(1), col(1), it(begin), end(end) { }

        op::optional<Token> get_token();
        op::optional<Token> peek_token() { return Lexer(*this).get_token(); }

    private:
        size_t line;
        size_t col;
        u32str::iterator it;
        u32str::iterator end;
    };
}

#endif
