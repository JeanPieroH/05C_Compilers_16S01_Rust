#include <iostream>
#include <cstring>
#include <string>
#include "token.h"
#include "scanner.h"

using namespace std;

Scanner::Scanner(const char* s, std::ostream& out):input(s),first(0), current(0),out(out) { }


bool is_white_space(char c) {
    return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

Token* Scanner::nextToken() {
    Token* token;
    while (current < input.length()) {
        // Ignorar espacios en blanco
        if (is_white_space(input[current])) {
            current++;
            continue; // Volver a verificar para comentarios o más espacios
        }

        // Manejar comentarios de una sola línea: //
        if (current + 1 < input.length() && input[current] == '/' && input[current + 1] == '/') {
            current += 2; // Consumir los dos caracteres '//'
            while (current < input.length() && input[current] != '\n') {
                current++;
            }
            // Si encontramos un salto de línea, también lo consumimos
            if (current < input.length() && input[current] == '\n') {
                current++;
            }
            continue; // Volver a verificar para el siguiente token
        }
        
        // Si no es espacio en blanco ni comentario, salimos del bucle
        break;
    }
    if (current >= input.length()) return new Token(Token::END);
    char c  = input[current];
    first = current;
    if (isdigit(c)) {
        current++;
        while (current < input.length() && isdigit(input[current])) {
            current++;
        }
        if (current < input.length() && input[current] == '.' && isdigit(input[current+1])) {
            current++; // Consumir el punto decimal
            while (current < input.length() && isdigit(input[current])) {
                current++; 
            }
            token = new Token(Token::FLOAT, input, first, current - first);
        } else {
            // Si no hay punto decimal, es un INT
            token = new Token(Token::INT, input, first, current - first);
        }
    }

    else if (isalpha(c) || c == '_') {
        current++;
        while (current < input.length() && (isalnum(input[current]) || input[current] == '_' || input[current] == '!'))
            current++;
        string word = input.substr(first, current - first);

        if (word == "println!")     token = new Token(Token::PRINT, word, 0, word.length());
        else if (word == "if")      token = new Token(Token::IF, word, 0, word.length());
        else if (word == "else")    token = new Token(Token::ELSE, word, 0, word.length());
        else if (word == "while")   token = new Token(Token::WHILE, word, 0, word.length());
        else if (word == "for")     token = new Token(Token::FOR, word, 0, word.length());
        else if (word == "true")    token = new Token(Token::TRUE, word, 0, word.length());
        else if (word == "false")   token = new Token(Token::FALSE, word, 0, word.length());
        else if (word == "return")  token = new Token(Token::RETURN, word, 0, word.length());
        else if (word == "fn")      token = new Token(Token::FUN, word, 0, word.length());
        else if (word == "let")     token = new Token(Token::LET, word, 0, word.length());
        else if (word == "mut")     token = new Token(Token::MUT, word, 0, word.length());
        else if (word == "i64")     token = new Token(Token::INT, word, 0, word.length());
        else if (word == "f64")     token = new Token(Token::FLOAT, word, 0, word.length());
        else if (word == "bool")     token = new Token(Token::BOOL, word, 0, word.length());
        else if (word == "break")   token = new Token(Token::BREAK, word, 0, word.length());
        else token = new Token(Token::ID, word, 0, word.length());
    } else {
        current++;
        switch (c) {
            case '+': return new Token(Token::PLUS, c);
            case '-':
                if (current < input.length() && input[current] == '>') {
                    current++;
                    return new Token(Token::ARROW, "->", 0, 2);
                }
                return new Token(Token::MINUS, c);
            case '*': return new Token(Token::MUL, c);
            case '/': return new Token(Token::DIV, c);
            case '(': return new Token(Token::PI, c);
            case ')': return new Token(Token::PD, c);
            case '{': return new Token(Token::LLI, c);
            case '}': return new Token(Token::LLD, c);
            case '[': return new Token(Token::CI, c);
            case ']': return new Token(Token::CD, c);
            case ';': return new Token(Token::PC, c);
            case ':': return new Token(Token::COLON, c);
            case ',': return new Token(Token::COMA, c);
            case '"':
                first = current; // El inicio de la cadena es después de la primera comilla
                while (current < input.length() && input[current] != '"') {
                    current++;
                }
                if (current < input.length() && input[current] == '"') {
                    // Hemos encontrado la comilla de cierre
                    string string_value = input.substr(first, current - first);
                    current++; // Consumir la comilla de cierre
                    return new Token(Token::STRING, string_value, 0, string_value.length());
                } else {
                    // Error: string sin cerrar
                    return new Token(Token::ERR, input.substr(first -1, current - (first - 1)), 0, current - (first -1)); // Captura desde la comilla inicial
                }
            case '=':
                if (current < input.length() && input[current] == '=') {
                    current++;
                    return new Token(Token::EQ, "==", 0, 2);
                }
                return new Token(Token::ASSIGN, c);
            case '!':
                if (current < input.length() && input[current] == '=') {
                    current++;
                    return new Token(Token::NEQ, "!=", 0, 2);
                }
                return new Token(Token::NOT, c);
            case '<':
                if (current < input.length() && input[current] == '=') {
                    current++;
                    return new Token(Token::LE, "<=", 0, 2);
                }
                return new Token(Token::LT, c);
            case '>':
                if (current < input.length() && input[current] == '=') {
                    current++;
                    return new Token(Token::GE, ">=", 0, 2);
                }
                return new Token(Token::GT, c);
            case '&':
                if (current < input.length() && input[current] == '&') {
                    current++;
                    return new Token(Token::AND, "&&", 0, 2);
                }
                return new Token(Token::REF, c);
            case '|':
                if (current < input.length() && input[current] == '|') {
                    current++;
                    return new Token(Token::OR, "||", 0, 2);
                }
                return new Token(Token::ERR, c);
            case '.':
                if (current < input.length() && input[current] == '.') {
                    current++;
                    return new Token(Token::DOTDOT, "..", 0, 2);
                }
                return new Token(Token::ERR, c);
            default:
                return new Token(Token::ERR, c);
        }
    }
    return token;
}

void Scanner::reset() {
    first = 0;
    current = 0;
}

Scanner::~Scanner() { }

void test_scanner(Scanner* scanner,  std::ostream& out) {
    Token* current;
    out << "Iniciando Scanner:" << endl<< endl;
    while ((current = scanner->nextToken())->type != Token::END) {
        if (current->type == Token::ERR) {
            out << "Error en scanner - carácter inválido: " << current->text << endl;
            break;
        } else {
            out << *current << endl;
        }
        delete current;
    }
    out << "TOKEN(END)" << endl;
    delete current;
}