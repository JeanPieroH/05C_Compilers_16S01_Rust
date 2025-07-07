#ifndef TOKEN_H
#define TOKEN_H

#include <string>

class Token {
public:
    enum Type {
        PLUS, MINUS, MUL, DIV, ERR, PD, PI,LLD,LLI,CD,CI,STRING, END, ID, PRINT, ASSIGN, PC,COLON,COMA,LT, LE, GT, GE, EQ, NEQ, AND, OR ,NOT, ARROW,LET, MUT,REF, IF, ELSE,WHILE, FOR,IN, DOTDOT, TRUE, FALSE, FUN,RETURN, BREAK,
        // Tipos de datos como palabras clave
        TYPE_I64, TYPE_F64, TYPE_BOOL,
        // Literales
        LITERAL_INT, LITERAL_FLOAT
        // TRUE y FALSE ya cubren los literales booleanos.
        // STRING cubre literales de cadena.
    };

    Type type;
    std::string text;

    Token(Type type);
    Token(Type type, char c);
    Token(Type type, const std::string& source, int first, int last);

    friend std::ostream& operator<<(std::ostream& outs, const Token& tok);
    friend std::ostream& operator<<(std::ostream& outs, const Token* tok);
};

#endif // TOKEN_H