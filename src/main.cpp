#include "Converter.h"

extern syntax_tree::AST analize(char* arg);

int main(int argc, char* argv[])
{   
    std::cout << "-----------------------------\n";
    std::cout << "--------Inference tree:------\n";
    std::cout << "-----------------------------\n";
    syntax_tree::AST inferenceTree = analize(argv[1]);
    inferenceTree.print();
    
    Converter* converter = new Converter();
    syntax_tree::AST operationTree = converter->convert(inferenceTree);
    std::cout << "-----------------------------\n";
    std::cout << "--------Operation tree:------\n";
    std::cout << "-----------------------------\n";
    operationTree.print();
}