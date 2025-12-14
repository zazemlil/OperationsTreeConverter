#pragma once
#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

namespace syntax_tree {

class ASTNode : public std::enable_shared_from_this<ASTNode> {
    std::string node_type;
    int token;
    std::vector<std::shared_ptr<ASTNode>> statements;

public:
    ASTNode() {}
    ASTNode(std::string t) : node_type(t) {}
    ASTNode(std::string t, int tok) : node_type(t), token(tok) {}
    
    virtual ~ASTNode() = default;

    std::string getNodeType() const { return node_type; }
    void setNodeType(const std::string& type) { node_type = type; }
    
    int getToken() const { return token; }
    size_t getStatementCount() const { return statements.size(); }

    std::shared_ptr<ASTNode>& getStatement(size_t index) { return statements.at(index); }
    const std::shared_ptr<ASTNode>& getStatement(size_t index) const { return statements.at(index); }
    
    void addStatement(std::shared_ptr<ASTNode> stmt) { statements.push_back(stmt); }
    
    // Новые методы для работы с алгоритмом
    void removeStatement(size_t index) {
        if (index < statements.size()) {
            statements.erase(statements.begin() + index);
        }
    }
    
    void replaceStatement(size_t index, std::shared_ptr<ASTNode> newStmt) {
        if (index < statements.size()) {
            statements[index] = newStmt;
        }
    }
    
    void clearStatements() {
        statements.clear();
    }
    
    virtual bool isTerminal() const {
        return isOperator() || node_type == "(" || node_type == ")";
    }
    
    bool isNonTerminal() const {
        return !isTerminal();
    }
    
    virtual bool isOperator() const {
        return node_type == "+" || node_type == "-" 
            || node_type == "*" || node_type == "/";
    }
    
    // Проверка, является ли узел семантически пустым
    virtual bool isSemanticallyEmpty() const {
        return node_type == "(" || node_type == ")";
    }

    void setStatements(std::vector<std::shared_ptr<ASTNode>>& new_statements) { 
        this->statements = new_statements; 
    }
    
    std::vector<std::shared_ptr<ASTNode>>& getStatements() { 
        return statements; 
    }
    
    const std::vector<std::shared_ptr<ASTNode>>& getStatements() const { 
        return statements; 
    }
    
    void addStatements(std::vector<std::shared_ptr<ASTNode>>& new_statements) {
        this->statements.insert(
            std::end(this->statements),
            std::begin(new_statements),
            std::end(new_statements)
        );
    }

    virtual void printValue(std::ostream& os = std::cout) const { 
        os << node_type; 
        if (token != 0) {
            os << " [" << token << "]";
        }
    }

    void print(int indent = 0, std::ostream& os = std::cout) const {
        std::string indentStr = ""; 
        for (int i = 0; i < indent-1; i++) {indentStr += "    ";} 
        
        os << indentStr << "";
        this->printValue(os);
        os << '\n';
        
        for (const auto& stmt : statements) {
            if (stmt) {
                stmt->print(indent + 2, os);
            }
        }
    }
};


class AST {
private:
    std::shared_ptr<ASTNode> root = nullptr;

public:
    AST() {}
    AST(std::shared_ptr<ASTNode> rootNode) : root(rootNode) {}

    std::shared_ptr<ASTNode> getRoot() { return root; }
    const std::shared_ptr<ASTNode> getRoot() const { return root; }
    
    void setRoot(std::shared_ptr<ASTNode> newRoot) { root = newRoot; }
    
    bool isEmpty() const { return root == nullptr; }

    void print(std::ostream& os = std::cout) const {
        if (root) {
            root->print(0);
            std::cout << "\n";
        } else {
            std::cout << "AST is empty.\n\n";
        }
    }
};


class LiteralFloat : public ASTNode {
    float value;
public:
    void printValue(std::ostream& os = std::cout) const override { 
        os << value; 
    }
    
    float getValue() const { return value; }
    
    LiteralFloat(std::string t, float v) : ASTNode(t), value(v) {}
    LiteralFloat(std::string t, int tok, float v) : ASTNode(t, tok), value(v) {}
    
    bool isTerminal() const override { return true; }
    bool isOperator() const override { return false; }
    bool isSemanticallyEmpty() const override { return false; }
};

namespace utils {
    // Поиск крайнего левого нетерминального узла в поддереве
    inline std::shared_ptr<ASTNode> findLeftmostNonTerminal(std::shared_ptr<ASTNode> node) {
        if (!node) return nullptr;
        
        if (node->isNonTerminal()) {
            return node;
        }
        
        for (auto& child : node->getStatements()) {
            auto result = findLeftmostNonTerminal(child);
            if (result) return result;
        }
        
        return nullptr;
    }
    
    // Замена узла в дереве
    inline bool replaceNode(std::shared_ptr<ASTNode> parent, 
                     std::shared_ptr<ASTNode> oldNode, 
                     std::shared_ptr<ASTNode> newNode) {
        if (!parent || !oldNode) return false;
        
        auto& statements = parent->getStatements();
        for (size_t i = 0; i < statements.size(); i++) {
            if (statements[i] == oldNode) {
                statements[i] = newNode;
                return true;
            }
            
            if (replaceNode(statements[i], oldNode, newNode)) {
                return true;
            }
        }
        
        return false;
    }
    
    // Проверка, содержит ли дерево нетерминальные узлы
    inline bool hasNonTerminal(std::shared_ptr<ASTNode> node) {
        if (!node) return false;
        
        if (node->isNonTerminal()) {
            return true;
        }
        
        for (auto& child : node->getStatements()) {
            if (hasNonTerminal(child)) {
                return true;
            }
        }
        
        return false;
    }
}

}