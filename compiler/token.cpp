#include <iostream>
#include "token.h"

using namespace std;

Token::Token(Type type):type(type) { text = ""; }

Token::Token(Type type, char c):type(type) { text = string(1, c); }

Token::Token(Type type, const string& source, int first, int last):type(type) {
    text = source.substr(first, last);
}
        
std::ostream& operator << ( std::ostream& outs, const Token & tok )
{
    switch (tok.type) {
        case Token::PLUS: outs << "TOKEN(PLUS)"; break;
        case Token::MINUS: outs << "TOKEN(MINUS)"; break;
        case Token::MUL: outs << "TOKEN(MUL)"; break;
        case Token::DIV: outs << "TOKEN(DIV)"; break;
        // case Token::INT: outs << "TOKEN(INT)"; break; // Reemplazado por LITERAL_INT
        // case Token::FLOAT: outs << "TOKEN(FLOAT)"; break; // Reemplazado por LITERAL_FLOAT
        // case Token::BOOL: outs << "TOKEN(BOOL)"; break; // Reemplazado por TYPE_BOOL y literales TRUE/FALSE
        case Token::LITERAL_INT: outs << "TOKEN(LITERAL_INT)"; break;
        case Token::LITERAL_FLOAT: outs << "TOKEN(LITERAL_FLOAT)"; break;
        case Token::TYPE_I64: outs << "TOKEN(TYPE_I64)"; break;
        case Token::TYPE_F64: outs << "TOKEN(TYPE_F64)"; break;
        case Token::TYPE_BOOL: outs << "TOKEN(TYPE_BOOL)"; break;
        case Token::ERR: outs << "TOKEN(ERR)"; break;
        case Token::PD: outs << "TOKEN(PD)"; break;
        case Token::PI: outs << "TOKEN(PI)"; break;
        case Token::LLD: outs << "TOKEN(LLD)"; break;
        case Token::LLI: outs << "TOKEN(LLI)"; break;
        case Token::CD: outs << "TOKEN(CD)"; break;
        case Token::CI: outs << "TOKEN(CI)"; break;
        // case Token::QUOTE: outs << "TOKEN(QUOTE)"; break; // Eliminado
        case Token::STRING: outs << "TOKEN(STRING)"; break;
        case Token::END: outs << "TOKEN(END)"; break;
        case Token::ID: outs << "TOKEN(ID)"; break;
        case Token::PRINT: outs << "TOKEN(PRINT)"; break;
        case Token::ASSIGN: outs << "TOKEN(ASSIGN)"; break;
        case Token::PC: outs << "TOKEN(PC)"; break;
        case Token::COLON: outs << "TOKEN(COLON)"; break;
        case Token::COMA: outs << "TOKEN(COMA)"; break;
        case Token::LT: outs << "TOKEN(LT)"; break;
        case Token::LE: outs << "TOKEN(LE)"; break;
        case Token::GT: outs << "TOKEN(GT)"; break;
        case Token::GE: outs << "TOKEN(GE)"; break;
        case Token::EQ: outs << "TOKEN(EQ)"; break;
        case Token::NEQ: outs << "TOKEN(NEQ)"; break;
        case Token::AND: outs << "TOKEN(AND)"; break;
        case Token::OR: outs << "TOKEN(OR)"; break;
        case Token::NOT: outs << "TOKEN(NOT)"; break;
        case Token::ARROW: outs << "TOKEN(ARROW)"; break;
        case Token::LET: outs << "TOKEN(LET)"; break;
        case Token::MUT: outs << "TOKEN(MUT)"; break;
        case Token::REF: outs << "TOKEN(REF)"; break;
        case Token::IF: outs << "TOKEN(IF)"; break;
        case Token::ELSE: outs << "TOKEN(ELSE)"; break;
        case Token::WHILE: outs << "TOKEN(WHILE)"; break;
        case Token::FOR: outs << "TOKEN(FOR)"; break;
        case Token::BREAK: outs << "TOKEN(BREAK)"; break;
        case Token::IN: outs << "TOKEN(IN)"; break;
        case Token::DOTDOT: outs << "TOKEN(DOTDOT)"; break;
        case Token::TRUE: outs << "TOKEN(TRUE)"; break;
        case Token::FALSE: outs << "TOKEN(FALSE)"; break;
        case Token::FUN: outs << "TOKEN(FUN)"; break;
        case Token::RETURN: outs << "TOKEN(RETURN)"; break;
        default: outs << "TOKEN(UNKNOWN)"; break;
    }
    return outs;
}

std::ostream& operator << ( std::ostream& outs, const Token* tok ) {
    return outs << *tok;
}