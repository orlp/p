#ifndef P_EXCEPTION_H
#define P_EXCEPTION_H

#include "libop/op.h"

namespace p {
    struct CompilationError : public virtual op::BaseException {
        size_t line;
        size_t col;

    protected:
        CompilationError();
        CompilationError(size_t line, size_t col) : line(line), col(col) { }
    };

    struct SyntaxError : public virtual CompilationError {
        SyntaxError(std::string msg, size_t line, size_t col)
        : op::BaseException(std::move(msg)), CompilationError(line, col) { }
    protected: SyntaxError() { }
    };

    struct EncodingError : public virtual CompilationError {
        EncodingError(std::string msg, size_t line, size_t col)
        : op::BaseException(std::move(msg)), CompilationError(line, col) { }
    protected: EncodingError() { }
    };

    struct FilesystemError : public virtual op::BaseException {
        FilesystemError(std::string msg) : op::BaseException(std::move(msg)) { }
    protected: FilesystemError() { }
    };
}


#endif
