%{
#include "Scanner.hpp"
%}

%require "3.7.4"
%language "C++"
%defines "Parser.hpp"
%output "Parser.cpp"

%define api.parser.class {Parser}
%define api.namespace {lisp_for_kids}
%define api.value.type variant
%param {yyscan_t scanner}
%parse-param {syntax_tree::AST& result}

%locations

%code requires {
    #include "AST.h"
}

%code provides
{
    #define YY_DECL \
        int yylex(lisp_for_kids::Parser::semantic_type *yylval, \
                lisp_for_kids::Parser::location_type* yylloc, \
                yyscan_t yyscanner)
    YY_DECL;
}

%token <std::shared_ptr<syntax_tree::LiteralFloat>> T_LITERAL_FLOAT
%left <std::shared_ptr<syntax_tree::ASTNode>> T_PLUS T_MINUS // low priority
%left <std::shared_ptr<syntax_tree::ASTNode>> T_MUL T_DIV // high priority
%token <std::shared_ptr<syntax_tree::ASTNode>> T_PARENTHESIS_OPEN
%token <std::shared_ptr<syntax_tree::ASTNode>> T_PARENTHESIS_CLOSE
%token T_END_OF_FILE

%type <std::shared_ptr<syntax_tree::ASTNode>> s expr num

%%

s: expr T_END_OF_FILE {
    auto n = std::make_shared<syntax_tree::ASTNode>("E");
    n->addStatement($1);
    result = syntax_tree::AST(n);
    YYACCEPT;
};

expr: num { 
        auto n = std::make_shared<syntax_tree::ASTNode>("NUM");
        n->addStatement($1);
        $$ = n; 
    }
    | expr T_PLUS expr { 
        auto n = std::make_shared<syntax_tree::ASTNode>("B");
        n->addStatement($1);
        n->addStatement($2);
        n->addStatement($3);
        $$ = n; 
    }
    | expr T_MINUS expr { 
        auto n = std::make_shared<syntax_tree::ASTNode>("B");
        n->addStatement($1);
        n->addStatement($2);
        n->addStatement($3);
        $$ = n; 
    }
    | expr T_MUL expr { 
        auto n = std::make_shared<syntax_tree::ASTNode>("B");
        n->addStatement($1);
        n->addStatement($2);
        n->addStatement($3);
        $$ = n;  
    }
    | expr T_DIV expr { 
        auto n = std::make_shared<syntax_tree::ASTNode>("B");
        n->addStatement($1);
        n->addStatement($2);
        n->addStatement($3);
        $$ = n;  
    }
    | T_MINUS expr { 
        auto n = std::make_shared<syntax_tree::ASTNode>("U");
        n->addStatement($1);
        n->addStatement($2);
        $$ = n;
    }
    | T_PARENTHESIS_OPEN expr T_PARENTHESIS_CLOSE { 
        auto n = std::make_shared<syntax_tree::ASTNode>("E");
        n->addStatement($1);
        n->addStatement($2);
        n->addStatement($3);
        $$ = n; 
    };

num: T_LITERAL_FLOAT { 
    auto n = std::make_shared<syntax_tree::ASTNode>("FLOAT");
    n->addStatement($1);
    $$ = n; 
};

%%

void lisp_for_kids::Parser::error(const location_type& loc, const std::string& msg) {
    const char* text = yyget_text(scanner);
    int length = yyget_leng(scanner);
    
    std::cerr << msg << " at (Line: " << loc.begin.line << ", Column: " << loc.begin.column
            << ", Last token: '" << std::string(text, length) << "')\n";
}