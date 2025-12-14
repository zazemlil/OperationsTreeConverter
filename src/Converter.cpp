#include "Converter.h"
    
syntax_tree::AST Converter::convert(syntax_tree::AST inferenceTree) {
    Node root = inferenceTree.getRoot();
    if (!root) return syntax_tree::AST();
    
    while (syntax_tree::utils::hasNonTerminal(root)) {
        root = transform(root);
        std::cin.get();
        std::cout << "#############################\n";
        root->print();
    }
    std::cin.get();
    std::cout << "\n";
    return syntax_tree::AST(root);
}


Node Converter::transform(Node root) {
    // Шаг 2: Найти крайний левый нетерминальный узел
    auto current = syntax_tree::utils::findLeftmostNonTerminal(root);
    
    if (!current) return root;
    
    return processNode(root, current);
}

Node Converter::processNode(Node root, Node current) {
    // Шаг 3: Текущий узел имеет только один дочерний узел
    if (current->getStatementCount() == 1) {
        auto child = current->getStatement(0);
        
        // Если текущий узел - корень
        if (current == root) {
            return child;
        }
        
        // Заменяем текущий узел его дочерним узлом
        if (syntax_tree::utils::replaceNode(root, current, child)) {
            return root;
        }
    }
    
    // Шаг 4: Удаляем семантически пустые терминалы
    bool removedEmpty = false;
    for (size_t i = 0; i < current->getStatementCount(); i++) {
        auto child = current->getStatement(i);
        if (child->isTerminal() && child->isSemanticallyEmpty()) {
            current->removeStatement(i);
            removedEmpty = true;
            i--;
        }
    }
    
    if (removedEmpty) {
        // Возвращаемся к шагу 3
        return processNode(root, current);
    }
    
    // Шаг 5: Проверяем на наличие оператора среди дочерних узлов
    // Ищем оператор как терминальный узел среди детей
    Node operatorNode = nullptr;
    size_t operatorIndex = 0;
    std::vector<Node> operands;
    
    for (size_t i = 0; i < current->getStatementCount(); i++) {
        auto child = current->getStatement(i);
        if (child->isTerminal() && child->isOperator()) {
            operatorNode = child;
            operatorIndex = i;
        } else {
            operands.push_back(child);
        }
    }
    
    if (operatorNode && !operands.empty()) {
        // Удаляем оператор из списка детей
        current->removeStatement(operatorIndex);
        
        // Меняем тип текущего узла на оператор
        current->setNodeType(operatorNode->getNodeType());
        
        // Очищаем старые дочерние узлы и добавляем операнды
        current->clearStatements();
        for (auto& operand : operands) {
            current->addStatement(operand);
        }
        
        return root;
    }
    
    // Шаг 6: Если есть нетерминалы среди дочерних узлов, выбираем крайний левый
    for (size_t i = 0; i < current->getStatementCount(); i++) {
        auto child = current->getStatement(i);
        if (child->isNonTerminal()) {
            return processNode(root, child);
        }
    }
    
    return root;
}