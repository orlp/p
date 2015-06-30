#ifndef P_AST_H
#define P_AST_H

namespace p {
    struct AST {
        enum Type {
            block,
            expression,
        }

        std::string value;
        std::vector<std::shared_ptr<AST>> children;
    };
}

#endif


