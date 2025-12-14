#include "Converter.h"

extern syntax_tree::AST analize(char* arg);

int main(int argc, char* argv[])
{   
    std::cout << "-----------------------------\n";
    std::cout << "----------Output tree:-------\n";
    std::cout << "-----------------------------\n";
    syntax_tree::AST outputTree = analize(argv[1]);
    outputTree.print();
    
    Converter* converter = new Converter();
    syntax_tree::AST ast = converter->convert(outputTree);
    std::cout << "-----------------------------\n";
    std::cout << "--------Operation tree:------\n";
    std::cout << "-----------------------------\n";
    ast.print();
}