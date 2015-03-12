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

    class Token {
    public:
        enum Type {
            newline,
        };

        Token(Type type, u32str value, size_t line, size_t col)
        : type(type), value(value), line(line), col(col) { }

    private:
        Type type;
        u32str value;
        size_t line;
        size_t col;
    };

    class AST {

    };

    class Lexer {
    public:
        Lexer(u32str input) : input(std::move(input)), line(1), col(1), it(input.begin()) { }

        op::optional<Token> get_token() {
            while (it != input.end() && *it == U' ') {

            }
            return Token(Token::newline, U"\n", line, col);
        }

    private:
        size_t line;
        size_t col;
        u32str input;
        u32str::iterator it;
    };
    


    template<class ForwardIterator>
    AST parse(ForwardIterator begin, ForwardIterator end) {

        u8str out;
        utf8::utf32to8(begin, end, std::back_inserter(out));
        std::fwrite(out.data(), 1, out.size(), stdout);

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

        return parse(input.begin(), input.end());
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


