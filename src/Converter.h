#pragma once

#include "AST.h"

typedef std::shared_ptr<syntax_tree::ASTNode> Node;
typedef std::shared_ptr<syntax_tree::LiteralFloat> LiteralFloat;

class Converter {
private:
    Node transform(Node root);
    Node processNode(Node root, Node current);
    
public:
    syntax_tree::AST convert(syntax_tree::AST ast);
};