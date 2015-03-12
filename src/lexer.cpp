#include <cstdio>
#include <cerrno>
#include <cstring>
#include <vector>
#include <array>

#include "utf8.h"
#include "libop/op.h"

using u8str = std::basic_string<uint8_t>;
using u32str = std::basic_string<char32_t>;




static u8str read_file(const char* filename) {
    auto file = std::fopen(filename, "rb");
    if (!file) {
        std::fprintf(stderr, "error: %s: %s\n", filename, std::strerror(errno));
        std::exit(1);
    }

    u8str result;
    std::array<uint8_t, 4096> buf;
    while (true) {
        auto bytes_read = std::fread(buf.data(), 1, buf.size(), file);
        if (std::ferror(file)) {
            std::fprintf(stderr, "error: %s: %s\n", filename, std::strerror(errno));
            std::exit(1);
        }
        
        if (!bytes_read) break;
        result.insert(result.end(), buf.begin(), buf.begin() + bytes_read);
    }

    return result;
}

namespace p {
    struct CompilationError : public virtual op::BaseException {
        explicit CompilationError(const std::string& msg) : op::BaseException(msg) { }
        explicit CompilationError(const char* msg) : op::BaseException(msg) { }
    };

    struct SyntaxError : public virtual CompilationError {
        explicit SyntaxError(const std::string& msg) : op::BaseException(msg) { }
        explicit SyntaxError(const char* msg) : op::BaseException(msg) { }
    };

    class Token {
    public:
        enum Type {
            newline,
            identifier,
            open_paren,
            close_paren,
            number,
            oper
        };

        Token(Type type, u32str value, size_t line, size_t col)
        : type(type), value(value), line(line), col(col) { }

    private:
        Type type;
        u32str value;
        size_t line;
        size_t col;
    };


    std::map<Token::Type, std::string> token_type_names = {
        {Token::Type::newline, "newline"},
        {Token::Type::identifier, "identifier"},
        {Token::Type::open_paren, "open_paren"},
        {Token::Type::close_paren, "close_paren"},
        {Token::Type::number, "number"},
        {Token::Type::oper, "oper"}
    };


    class AST {

    };

    class Lexer {
    public:
        Lexer(u32str::iterator begin, u32str::iterator end)
        : line(1), col(1), it(begin), end(end) { }

        op::optional<Token> get_token() {
            const char32_t operators[] = U"+-*/&|^%<>";
            const std::set<char32_t> operators_set(std::begin(operators), std::end(operators));
            while (it != end && *it == U' ') {
                ++col;
                ++it;
            }

            if (it == end) return {};

            size_t token_col = col;
            size_t token_line = line;

            char32_t c = *it++;
            if (c == U'\n') {
                ++line;
                col = 1;
                return Token(Token::Type::newline, u32str(c, 1), token_col, token_line);
            } else if (c == U'(') {
                return Token(Token::Type::open_paren, u32str(c, 1), token_col, token_line);
            } else if (c == U')') {
                return Token(Token::Type::close_paren, u32str(c, 1), token_col, token_line);
            } else if (operators_set.count(c)) {
                u32str value(c, 1);
                if (it != end) {
                    if (((c == U'<' || c == U'>') && *it == c) || *it == U'=') {
                        value += *it;
                        ++it;
                    }
                }

                return Token(Token::Type::oper, value, token_col, token_line);
            }

            std::string c_str;
            utf32to8(&c, &c + 1, std::back_inserter(c_str));
            throw SyntaxError(std::string("Unknown character '") + c_str);
        }
        
        op::optional<Token> peek_token() { return Lexer(*this).get_token(); }

    private:
        size_t line;
        size_t col;
        u32str::iterator it;
        u32str::iterator end;
    };
    


    AST parse(Lexer& lexer) {

        return AST();
    }

    template<class ForwardIterator>
    AST compile(ForwardIterator begin, ForwardIterator end) {
        u32str input;

        // Decode UTF-8 input into Unicode codepoints and handle line endings.
        size_t line = 1;
        size_t col = 1;
        while (begin != end) {
            try {
                uint32_t c = utf8::next(begin, end);
                if (c == U'\r') {
                    c = U'\n';
                    if (begin != end && utf8::peek_next(begin, end) == U'\n') ++begin;
                }
                
                input += c;
                if (c == U'\n') {
                    ++line;
                    col = 1;
                } else ++col;
            } catch (const utf8::exception& e) {
                std::fprintf(stderr, "error: %zu:%zu: %s\n", line, col, e.what());
                std::exit(1);
            }
        }

        Lexer lexer(input.begin(), input.end());
        return parse(lexer);
    }
}





int main(int argc, char** argv) {
    if (argc < 2) {
        std::fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    auto file = read_file(argv[1]);
    p::compile(file.begin(), file.end());





    return 0;
}


