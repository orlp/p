#include <array>
#include <cerrno>
#include <cstdio>

#include "utf8/utf8.h"

#include "exception.h"
#include "common.h"
#include "lexer.h"



// Reads a file as binary data.
static u8str read_file(const char* filename) {
    auto file = std::fopen(filename, "rb");
    if (!file) throw p::FilesystemError(std::strerror(errno));

    u8str result;
    std::array<uint8_t, 4096> buf;
    while (true) {
        auto bytes_read = std::fread(buf.data(), 1, buf.size(), file);
        if (std::ferror(file)) throw p::FilesystemError(std::strerror(errno));
        
        if (!bytes_read) break;
        result.insert(result.end(), buf.begin(), buf.begin() + bytes_read);
    }

    return result;
}


// Decodes binary blob as UTF-8 into an Unicode string, or throws EncodingError if there is an
// error. Also normalizes newlines \r | \n | \r\n -> \n.
template<class ForwardIterator>
static u32str decode_utf8(ForwardIterator begin, ForwardIterator end) {
    u32str result;

    size_t line = 1;
    size_t col = 1;
    while (begin != end) {
        try {
            char32_t c = utf8::next(begin, end);
            if (c == U'\r') {
                c = U'\n';
                if (begin != end && utf8::peek_next(begin, end) == U'\n') ++begin;
            }
            
            result += c;
            if (c == U'\n') {
                ++line;
                col = 1;
            } else ++col;
        } catch (const utf8::exception& e) {
            throw p::EncodingError(e.what(), line, col);
        }
    }

    return result;
}


// Returns the requested line, with an arrow at the requested column from the source.
template<class ForwardIterator>
static u32str get_source_context(ForwardIterator begin, ForwardIterator end,
                                 size_t line, size_t col) {
    int cur_line = 1;
    while (cur_line != line && begin != end) {
        if (*begin++ == U'\n') cur_line += 1;
    }

    u32str result;
    while (begin != end && *begin != U'\n') result += *begin++;

    result += U'\n';
    while (--col) result += U' ';
    result += U'^';

    return result;
}


namespace p {
    struct AST { };

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












int main(int argc, char** argv) {
    if (argc < 2) {
        std::fprintf(stdout, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    u32str decoded_file;
    try {
        auto file = read_file(argv[1]);
        decoded_file = decode_utf8(file.begin(), file.end());
        p::compile(decoded_file.begin(), decoded_file.end());
    } catch (const p::SyntaxError& e) {
        std::fprintf(stdout, "%s:%zu:%zu syntax error: %s\n", argv[1], e.line, e.col, e.what());
        std::fprintf(stdout, "%s\n",
            u32_to_string(get_source_context(decoded_file.begin(), decoded_file.end(), e.line, e.col)).c_str());
    } catch (const p::EncodingError& e) {
        std::fprintf(stdout, "%s:%zu:%zu encoding error: %s\n", argv[1], e.line, e.col, e.what());
    } catch (const p::CompilationError& e) {
        std::fprintf(stdout, "%s:%zu:%zu %s\n", argv[1], e.line, e.col, e.what());
    } catch (const p::FilesystemError& e) {
        std::fprintf(stdout, "error: %s: %s\n", argv[1], e.what());
    }


    return 0;
}


