#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "exp.h"

class Parser {
private:
    std::ostream& out;
    Scanner* scanner;
    Token *current, *previous;
    bool match(Token::Type ttype);
    bool check(Token::Type ttype);
    bool advance();
    bool isAtEnd();
public:
    Parser(Scanner* scanner, std::ostream& out);
    Program* parseProgram();
    // Funciones auxiliares para expresiones (ya existen, solo las listamos para referencia)
    Exp* parseCExp();
    Exp* parseLExp(); // Nueva regla para LExp
    Exp* parseRExp(); // Nueva regla para RExp
    Exp* parseExp();  // Renombrado de parseExpression a parseExp para coincidir con la gramática
    Exp* parseTerm();
    Exp* parseFactor();

    // Funciones auxiliares para sentencias
    Stm* parseControlStatement();
    Stm* parseStatement();
    StatementList* parseStatementList();
    FunDec* parseFunDec(); // Nueva regla para DeclaracionFuncion
    Body* parseBody();

    // Funciones auxiliares para tipos y literales
    string parseType(); // Nueva regla para Tipo
    Exp* parseLiteral(); // Nueva regla para Literal
    Exp* parseLiteralArray(); // Nueva regla para LiteralArray
    list<Exp*> parsePrintExp(); // Nueva regla para PrintExp
    list<pair<bool, Exp*>> parseArgumentList(); // Nueva regla para ListaArgumentos
    list<pair<string, bool>> parseParameterList(); // Nueva regla para ListaParametros

};

#endif // PARSER_H