#include <cstring>

#include "libop/op.h"

#include "common.h"
#include "exception.h"
#include "lexer.h"


namespace p {
    const std::map<Token::Type, std::string> Token::type_names = {
        {Token::Type::open_paren,   "open_paren"  },
        {Token::Type::close_paren,  "close_paren" },
        {Token::Type::open_square,  "open_square" },
        {Token::Type::close_square, "close_square"},
        {Token::Type::open_brace,   "open_brace"  },
        {Token::Type::close_brace,  "close_brace" },
        {Token::Type::colon,        "colon"       },
        {Token::Type::comma,        "comma"       },
        {Token::Type::period,       "period"      },
        {Token::Type::comment,      "comment"     },
        {Token::Type::identifier,   "identifier"  },
        {Token::Type::newline,      "newline"     },
        {Token::Type::number,       "number"      },
        {Token::Type::oper,         "oper"        },
        {Token::Type::string,       "string"      }
    };


    op::optional<Token> Lexer::get_token() {
        static const char32_t operators[] = U"+-*/&|^%<>";
        static const char32_t num[] = U"0123456789";
        static const char32_t alpha[] =
            U"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
        static const char32_t alphanum[] =
            U"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789";
        static const char32_t brackets[] = U"(){}[]";
        static const std::set<char32_t> operators_set(std::begin(operators), std::end(operators));
        static const std::set<char32_t> num_set(std::begin(num), std::end(num));
        static const std::set<char32_t> alpha_set(std::begin(alpha), std::end(alpha));
        static const std::set<char32_t> alphanum_set(std::begin(alphanum), std::end(alphanum));
        static const std::set<char32_t> brackets_set(std::begin(brackets), std::end(brackets));

        static const std::map<char32_t, Token::Type> bracket_types = {
            {U'(', Token::Type::open_paren},
            {U')', Token::Type::close_paren},
            {U'{', Token::Type::open_brace},
            {U'}', Token::Type::close_brace},
            {U'[', Token::Type::open_square},
            {U']', Token::Type::close_square}
        };

        static const std::set<u32str> int_suffixes_set = {
            U"f32", U"f64",
            U"i8", U"i16", U"i32", U"i64",
            U"u8", U"u16", U"u32", U"u64",
            U"i"
        };

        // Skip whitespace.
        while (it != end && *it == U' ') {
            ++it; ++col;
        }

        if (it == end) return {};

        size_t token_col = col;
        size_t token_line = line;

        char32_t c = *it++; ++col;
        if (c == U'\n') {
            ++line;
            col = 1;
            return Token(Token::Type::newline, u32str(1, c), token_line, token_col);
        }

        if (brackets_set.count(c)) {
            return Token(bracket_types.at(c), u32str(1, c), token_line, token_col);
        } else if (c == U':') {
            return Token(Token::Type::colon, u32str(1, c), token_line, token_col);
        } else if (c == U',') {
            return Token(Token::Type::comma, u32str(1, c), token_line, token_col);
        } else if (c == U'.') {
            return Token(Token::Type::period, u32str(1, c), token_line, token_col);
        } else if (c == U'#') {
            u32str value(1, c);
            while (it != end && *it != U'\n') {
                value += *it++; ++col;
            }

            return Token(Token::Type::comment, value, token_line, token_col);
        } else if (c == U'"') {
            u32str value;

            while (true) {
                if (it == end) {
                    throw SyntaxError("EOF encountered in string.", token_line, col);
                }

                if (*it == U'\n') {
                    throw SyntaxError("Newline encountered in string.", token_line, col);
                }

                if (*it == U'"') {
                    ++it; ++col;
                    break;
                }

                if (*it == U'\\') {
                    ++it; ++col;
                    if (it == end) {
                        throw SyntaxError("EOF encountered in string.", token_line, col);
                    }

                    if (*it == U'"') {
                        value += *it++; ++col;
                    } else {
                        value += U'\\';
                    }
                } else {
                    value += *it++; ++col;
                }
            }

            return Token(Token::Type::string, value, token_line, token_col);
        } else if (operators_set.count(c)) {
            u32str value(1, c);
            if (it != end) {
                if (((c == U'<' || c == U'>' || c == U'/') && *it == c) || *it == U'=') {
                    value += *it++; ++col;
                }
            }

            return Token(Token::Type::oper, value, token_line, token_col);
        } else if (alpha_set.count(c)) {
            u32str value(1, c);
            while (it != end && alphanum_set.count(*it)) {
                value += *it++; ++col;
            }

            return Token(Token::Type::identifier, value, token_line, token_col);
        } else if (num_set.count(c)) {
            u32str value(1, c);
            bool base = false;
            bool floating = false;

            if (c == U'0' && it != end && (*it == U'b' || *it == U'o' || *it == U'x')) {
                base = true;
                value += *it++; ++col;
            }

            while (it != end && num_set.count(*it)) {
                value += *it++; ++col;
            }

            if (!base) {
                if (it != end && *it == U'.') {
                    floating = true;
                    value += *it++; ++col;
                }
                
                while (it != end && num_set.count(*it)) {
                    value += *it++; ++col;
                }
            }

            u32str suffix;
            size_t suffix_col = col;
            while (it != end && alphanum_set.count(*it)) {
                suffix += *it++; ++col;
            }

            if (suffix.size()) {
                if (floating && suffix != U"f32" && suffix != U"f64") {
                    throw SyntaxError(
                        std::string("Invalid float suffix '") + u32_to_string(suffix) + "'",
                        token_line, suffix_col
                    );
                } else if (!floating && !int_suffixes_set.count(suffix)) {
                    throw SyntaxError(
                        std::string("Invalid integer suffix '") + u32_to_string(suffix) + "'",
                        token_line, suffix_col
                    );
                }
            }

            return Token(Token::Type::number, value + suffix, token_line, token_col);
        }

        std::string c_str;
        utf8::utf32to8(&c, &c + 1, std::back_inserter(c_str));
        throw SyntaxError(std::string("Unknown character '") + c_str + "'",
                          token_line, token_col);
    }
}
