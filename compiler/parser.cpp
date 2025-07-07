#include <iostream>
#include <stdexcept>
#include "token.h"
#include "scanner.h"
#include "exp.h"
#include "parser.h"

using namespace std;

bool Parser::match(Token::Type ttype) {
    if (check(ttype)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(Token::Type ttype) {
    if (isAtEnd()) return false;
    return current->type == ttype;
}

bool Parser::advance() {
    if (!isAtEnd()) {
        Token* temp = current;
        if (previous) delete previous;
        current = scanner->nextToken();
        previous = temp;
        if (check(Token::ERR)) {
            out << "Error de análisis, carácter no reconocido: " << current->text << endl;
            exit(1);
        }
        return true;
    }
    return false;
}

bool Parser::isAtEnd() {
    return (current->type == Token::END);
}

Parser::Parser(Scanner* sc, std::ostream& out):scanner(sc),out(out) {
    previous = NULL;
    current = scanner->nextToken();
    if (current->type == Token::ERR) {
        out << "Error en el primer token: " << current->text << endl;
        exit(1);
    }
}


//


Program* Parser::parseProgram() {
                out<<"ento";

    Program* prog = new Program();

//    while (check(Token::FUN)) { // <DeclaracionGlobal> ::= <DeclaracionFuncion>
        prog->fundecs->add(parseFunDec());
//   }
    return prog;
}

FunDec* Parser::parseFunDec() {
    FunDec* fundec = new FunDec();
    if (!match(Token::FUN)) {
        out << "Error: se esperaba 'fn' para la declaración de función." << endl;
        exit(1);
    }
    if (!match(Token::ID)) {
        out << "Error: se esperaba un identificador para el nombre de la función." << endl;
        exit(1);
    }
    fundec->nombre = previous->text;
    if (!match(Token::PI)) { // (
        out << "Error: se esperaba '(' después del nombre de la función." << endl;
        exit(1);
    }

    if (!check(Token::PD)) { // Puede haber parámetros
        list<pair<string, bool>> params = parseParameterList();
        for (auto const& [id, is_mut] : params) {
            fundec->parametros.push_back(id);
            fundec->ref_mut.push_back(is_mut); // Usamos ref_mut para el 'mut' de los parámetros
        }
    }
    if (!match(Token::PD)) { // )
        out << "Error: se esperaba ')' después de la lista de parámetros." << endl;
        exit(1);
    }

    if (match(Token::ARROW)) { // Opcional -> <Tipo>
        fundec->tipo = parseType();
    } else {
        fundec->tipo = "void"; // Si no hay tipo de retorno, asumimos void
    }

    fundec->cuerpo = parseBody(); // <BloqueCodigo>
    out<<"ento";

    return fundec;
}

// <ListaParametros> ::= <Identificador> : [ &mut ] <Tipo> (, <Identificador> : [ &mut ] <Tipo>)*
list<pair<string, bool>> Parser::parseParameterList() {
    list<pair<string, bool>> params;
    bool is_mut = false;

    // Primer parámetro
    if (match(Token::REF)) { // '&' opcional
        is_mut = true;
    }
    if (match(Token::MUT)) { // 'mut' opcional después de '&'
        is_mut = true;
    }
    if (!match(Token::ID)) {
        out << "Error: se esperaba un identificador de parámetro." << endl;
        exit(1);
    }
    string id = previous->text;
    if (!match(Token::COLON)) {
        out << "Error: se esperaba ':' después del identificador del parámetro." << endl;
        exit(1);
    }
    string type = parseType();
    params.push_back({id + ":" + type, is_mut}); // Concatenamos para almacenar id:tipo

    // Parámetros adicionales
    while (match(Token::COMA)) {
        is_mut = false; // Reset para el siguiente parámetro
        if (match(Token::REF)) {
            is_mut = true;
        }
        if (match(Token::MUT)) {
            is_mut = true;
        }
        if (!match(Token::ID)) {
            out << "Error: se esperaba un identificador de parámetro después de ','." << endl;
            exit(1);
        }
        id = previous->text;
        if (!match(Token::COLON)) {
            out << "Error: se esperaba ':' después del identificador del parámetro." << endl;
            exit(1);
        }
        type = parseType();
        params.push_back({id + ":" + type, is_mut});
    }
    return params;
}


Body* Parser::parseBody() {

    if (!match(Token::LLI)) { // {
        out << "Error: se esperaba '{' para el bloque de código." << endl;
        exit(1);
    }
    StatementList* slist = new StatementList();
    while (!check(Token::LLD) && !isAtEnd()) { // Mientras no sea '}'
        slist->add(parseStatement());
    }
    if (!match(Token::LLD)) { // }
        out << "Error: se esperaba '}' al final del bloque de código." << endl;
        exit(1);
    }

    return new Body(slist);
}

// <Sentencia>
Stm* Parser::parseStatement() {
    Stm* s = nullptr;
    string id_name;
    Exp* cond_exp;
    Body* then_body;
    Body* else_body = nullptr; // Opcional

    // <DeclaracionVariable>
    if (match(Token::LET)) {
        out<<"entra";
        bool is_mut = match(Token::MUT);
        if (!match(Token::ID)) {
            out << "Error: se esperaba un identificador después de 'let'." << endl;
            exit(1);
        }
        id_name = previous->text;
        string type_name = "";
        Exp* array_size = nullptr;
        Exp* initial_value = nullptr;

        if (match(Token::COLON)) { // Opcional : <Tipo> | [ <Tipo> ; <LiteralEntero> ]
            if (check(Token::CI)) { // Es un array: [ <Tipo> ; <LiteralEntero> ]
                advance(); // Consumir '['
                type_name = parseType(); // Tipo del array
                if (!match(Token::PC)) { // ;
                    out << "Error: se esperaba ';' en la declaración de array." << endl;
                    exit(1);
                }
                if (!match(Token::INT)) { // LiteralEntero
                    out << "Error: se esperaba un tamaño entero para el array." << endl;
                    exit(1);
                }
                array_size = new IntExp(stoi(previous->text));
                if (!match(Token::CD)) { // ]
                    out << "Error: se esperaba ']' en la declaración de array." << endl;
                    exit(1);
                }
            } else { // Es un tipo simple: <Tipo>
                type_name = parseType();
            }
        }

        if (match(Token::ASSIGN)) { // Opcional = <CExp>
            initial_value = parseCExp();
        }

        if (!match(Token::PC)) { // ;
            out << "Error: se esperaba ';' al final de la declaración de variable." << endl;
            exit(1);
        }
        s = new DecStament(is_mut, type_name, id_name);
        // Aquí necesitarías un constructor de DecStatement que reciba el valor inicial y el tamaño del array
        // Por simplicidad, se asignará después si initial_value no es null.
        if (initial_value) {
            // Esto es un placeholder, deberías manejar la asignación inicial en DecStament o un nodo separado.
            // Para arrays, la lógica es más compleja y se necesitaría un nodo DecArrayStatement
        }
    }
    // <Asignacion> ;
    else if (check(Token::ID)) {
        id_name = current->text; // Peek para ver si es ID o AccesoArray
        advance(); // Consumir el ID

        if (match(Token::CI)) { // AccesoArray
            Exp* index = parseCExp();
            if (!match(Token::CD)) {
                out << "Error: se esperaba ']' después del índice del array." << endl;
                exit(1);
            }
            if (!match(Token::ASSIGN)) {
                out << "Error: se esperaba '=' para la asignación de array." << endl;
                exit(1);
            }
            Exp* rhs_exp = parseCExp();
            s = new AssignArrayStatement(id_name, index, rhs_exp);
        } else if (match(Token::ASSIGN)) { // Asignacion simple
            Exp* rhs_exp = parseCExp();
            s = new AssignStatement(id_name, rhs_exp);
        } else {
            out << "Error: Asignación o acceso a array mal formado." << endl;
            exit(1);
        }
        if (!match(Token::PC)) {
            out << "Error: se esperaba ';' al final de la asignación." << endl;
            exit(1);
        }
    }
    // println! ( <PrintExp> ) ;
    else if (match(Token::PRINT)) {
        if (!match(Token::PI)) {
            out << "Error: se esperaba '(' después de 'println!'." << endl;
            exit(1);
        }

        string format_string = "";
        vector<Exp*> arg_exps; // Argumentos que van después de la cadena de formato

        if (!match(Token::STRING)) {
            out << "Error: se esperaba una cadena literal como primer argumento de println!." << endl;
            exit(1);
        }
        format_string = previous->text; // Captura la cadena de formato

        // Ahora, si hay más argumentos después de la cadena, los parseamos como CExp
        while (match(Token::COMA)) {
            arg_exps.push_back(parseCExp());
        }

        if (!match(Token::PD)) {
            out << "Error: se esperaba ')' después de la expresión(es) en println!." << endl;
            exit(1);
        }
        if (!match(Token::PC)) {
            out << "Error: se esperaba ';' al final de println!." << endl;
            exit(1);
        }
        // Creamos el PrintStatement con la cadena de formato y el vector de expresiones
        s = new PrintStatement(format_string, arg_exps);
    }
    // <SentenciaControl>
    else if (check(Token::IF) || check(Token::WHILE) || check(Token::FOR)) {
        s = parseControlStatement();
    }
    // return [<CExp>] ;
    else if (match(Token::RETURN)) {
        Exp* ret_exp = nullptr;
        if (!check(Token::PC)) { // Hay una expresión de retorno
            ret_exp = parseCExp();
        }
        if (!match(Token::PC)) {
            out << "Error: se esperaba ';' al final de la sentencia 'return'." << endl;
            exit(1);
        }
        s = new ReturnStatement();
        // Si tu ReturnStatement necesita la expresión, adapta el constructor
        if (ret_exp) {
            static_cast<ReturnStatement*>(s)->e = ret_exp;
        }
    }
    // <LlamadaFuncion> ;
    if (check(Token::ID)) { // Solo verificamos, NO consumimos todavía
        string id_name = current->text; // Obtenemos el nombre del ID
        advance(); // Ahora sí, consumimos el Token::ID

        // Después de un ID, podemos tener:
        // 1. (  -> Llamada a función (FCallExp)
        // 2. [  -> Acceso a array (AccesoArrayExp) seguido de una asignación
        // 3. =  -> Asignación simple (AssignStatement)

        if (match(Token::PI)) { // Es una LlamadaFuncion
            // Ya consumimos ID, ahora consumimos PI
            FCallExp* fcall_exp = new FCallExp();
            fcall_exp->nombre = id_name;
            
            if (!check(Token::PD)) { // Puede haber argumentos
                list<pair<bool, Exp*>> args = parseArgumentList();
                for (auto const& [is_mut_ref, arg_exp] : args) {
                    fcall_exp->argumentos.push_back(arg_exp);
                    fcall_exp->ref_mut.push_back(is_mut_ref);
                }
            }
            if (!match(Token::PD)) { // )
                out << "Error: se esperaba ')' después de los argumentos de la función '" << id_name << "'." << endl;
                exit(1);
            }
            if (!match(Token::PC)) { // ;
                out << "Error: se esperaba ';' al final de la llamada a función." << endl;
                exit(1);
            }
            s = new FCallStm();
            static_cast<FCallStm*>(s)->nombre = fcall_exp->nombre;
            static_cast<FCallStm*>(s)->argumentos = fcall_exp->argumentos;
            static_cast<FCallStm*>(s)->ref_mut = fcall_exp->ref_mut;
            delete fcall_exp; // Liberar el FCallExp temporal
        }
        else if (match(Token::CI)) { // Es un AccesoArray que DEBE ser seguido por una asignación
            // Ya consumimos ID, ahora consumimos CI
            Exp* index = parseCExp();
            if (!match(Token::CD)) { // ]
                out << "Error: se esperaba ']' después del índice del array para '" << id_name << "'." << endl;
                exit(1);
            }
            
            // Después del AccesoArray, *debe* venir un '=' para que sea una <Asignacion> válida
            if (match(Token::ASSIGN)) { // AccesoArray = CExp ;
                Exp* rhs_exp = parseCExp();
                s = new AssignArrayStatement(id_name, index, rhs_exp);
            } else {
                // Si llegamos aquí, significa que encontramos AccesoArray pero no un '=',
                // lo cual no es una <Asignacion> válida según la gramática para ser una sentencia.
                out << "Error: se esperaba '=' después del acceso al array '" << id_name << "' para una asignación válida." << endl;
                exit(1);
            }

            if (!match(Token::PC)) { // ;
                out << "Error: se esperaba ';' al final de la asignación de array." << endl;
                exit(1);
            }
        }
        else if (match(Token::ASSIGN)) { // Es una Asignacion (simple)
            // Ya consumimos ID, ahora consumimos ASSIGN
            Exp* rhs_exp = parseCExp();
            if (!match(Token::PC)) { // ;
                out << "Error: se esperaba ';' al final de la asignación." << endl;
                exit(1);
            }
            s = new AssignStatement(id_name, rhs_exp);
        }
        else {
            // Si el ID no fue seguido por '(', '[', o '=', es un error.
            out << "Error: Sentencia que comienza con '" << id_name << "' no reconocida o mal formada. Se esperaba '(', '[', o '='." << endl;
            exit(1);
        }
    }
    // break ;
    else if (match(Token::BREAK)) {
        if (!match(Token::PC)) {
            out << "Error: se esperaba ';' al final de 'break'." << endl;
            exit(1);
        }
        s = new BreakStm();
    }
    else {
        out << "Error: Sentencia no reconocida o mal formada: " << *current << endl;
        exit(1);
    }
    return s;
}


// <SentenciaControl>
Stm* Parser::parseControlStatement() {
    Stm* s = nullptr;
    if (match(Token::IF)) { // <IfElseSentencia>
        Exp* condition = parseCExp();
        Body* then_body = parseBody();
        Body* else_body = nullptr;
        if (match(Token::ELSE)) {
            else_body = parseBody();
        }
        s = new IfStatement(condition, then_body, else_body);
    } else if (match(Token::WHILE)) { // <WhileSentencia>
        Exp* condition = parseCExp();
        Body* loop_body = parseBody();
        s = new WhileStatement(condition, loop_body);
    } else if (match(Token::FOR)) { // <ForSentencia>
        if (!match(Token::ID)) {
            out << "Error: se esperaba un identificador para la variable del bucle 'for'." << endl;
            exit(1);
        }
        string id_name = previous->text;
        if (!match(Token::IN)) { // asumiendo que 'in' es un Token::IN
            out << "Error: se esperaba 'in' en la sentencia 'for'." << endl;
            exit(1);
        }
        Exp* start_exp = parseExp(); // <Exp>
        if (!match(Token::DOTDOT)) {
            out << "Error: se esperaba '..' en la sentencia 'for'." << endl;
            exit(1);
        }
        Exp* end_exp = parseExp(); // <Exp>
        Body* loop_body = parseBody();
        s = new ForStatement(id_name, start_exp, end_exp, loop_body);
    } else {
        out << "Error: Sentencia de control no reconocida." << endl;
        exit(1);
    }
    return s;
}


StatementList* Parser::parseStatementList() {
    StatementList* sl = new StatementList();
    while (!check(Token::LLD) && !isAtEnd()) { // Asumiendo que se llama dentro de un cuerpo que termina con '}'
        sl->add(parseStatement());
    }
    return sl;
}


// <CExp> ::= <LExp>
Exp* Parser::parseCExp() {
    return parseLExp();
}

// <LExp> ::= <RExp> ( (&& | ||) <RExp> )*
Exp* Parser::parseLExp() {
    Exp* left = parseRExp();
    while (match(Token::AND) || match(Token::OR)) {
        BinaryOp op;
        if (previous->type == Token::AND) {
            op = AND;
        } else { // Token::OR
            op = OR;
        }
        Exp* right = parseRExp();
        left = new BinaryExp(left, right, op);
    }
    return left;
}

// <RExp> ::= <Exp> ( <OperadorRelacional> <Exp> )?
Exp* Parser::parseRExp() {
    Exp* left = parseExp();
    if (match(Token::EQ) || match(Token::NEQ) || match(Token::LT) || match(Token::LE) || match(Token::GT) || match(Token::GE)) {
        BinaryOp op;
        if (previous->type == Token::EQ) op = EQ_OP;
        else if (previous->type == Token::NEQ) op = NEQ_OP;
        else if (previous->type == Token::LT) op = LT_OP;
        else if (previous->type == Token::LE) op = LE_OP;
        else if (previous->type == Token::GT) op = GT_OP;
        else if (previous->type == Token::GE) op = GE_OP;
        Exp* right = parseExp();
        left = new BinaryExp(left, right, op);
    }
    return left;
}

// <Exp> ::= <Term> ( <OperadorAditivo> <Term> )*
Exp* Parser::parseExp() { // Renombrado de parseExpression a parseExp
    Exp* left = parseTerm();
    while (match(Token::PLUS) || match(Token::MINUS)) {
        BinaryOp op;
        if (previous->type == Token::PLUS) {
            op = PLUS_OP;
        } else { // Token::MINUS
            op = MINUS_OP;
        }
        Exp* right = parseTerm();
        left = new BinaryExp(left, right, op);
    }
    return left;
}

// <Term> ::= <Factor> ( <OperadorMultiplicativo> <Factor> )*
Exp* Parser::parseTerm() {
    Exp* left = parseFactor();
    while (match(Token::MUL) || match(Token::DIV)) {
        BinaryOp op;
        if (previous->type == Token::MUL) {
            op = MUL_OP;
        } else { // Token::DIV
            op = DIV_OP;
        }
        Exp* right = parseFactor();
        left = new BinaryExp(left, right, op);
    }
    return left;
}

// <Factor> ::= ...
Exp* Parser::parseFactor() {
    Exp* e;
    string id_name;

    if (match(Token::INT)) {
        return new IntExp(stoi(previous->text));
    } else if (match(Token::FLOAT)) {
        return new FloatExp(stod(previous->text));
    } else if (match(Token::TRUE)) {
        return new BoolExp(true);
    } else if (match(Token::FALSE)) {
        return new BoolExp(false);
    } else if (match(Token::MINUS)) { // Expresión unaria para '-'
        Exp* operand = parseFactor();
        return new UnaryExp(operand, MINUS_UNARY_OP);
    } else if (match(Token::NOT)) { // Expresión unaria para '!'
        Exp* operand = parseFactor();
        return new UnaryExp(operand, NOT_OP);
    } else if (match(Token::ID)) {
        id_name = previous->text;
        if (check(Token::PI)) { // Es una llamada a función <LlamadaFuncion>
            advance(); // Consumir '('
            FCallExp* fcall_exp = new FCallExp();
            fcall_exp->nombre = id_name;
            if (!check(Token::PD)) { // Puede haber argumentos
                list<pair<bool, Exp*>> args = parseArgumentList();
                for (auto const& [is_mut_ref, arg_exp] : args) {
                    fcall_exp->argumentos.push_back(arg_exp);
                    fcall_exp->ref_mut.push_back(is_mut_ref);
                }
            }
            if (!match(Token::PD)) { // )
                out << "Error: se esperaba ')' después de los argumentos de la función." << endl;
                exit(1);
            }
            return fcall_exp;
        } else if (check(Token::CI)) { // Es un acceso a array <AccesoArray>
            advance(); // Consumir '['
            Exp* index = parseCExp();
            if (!match(Token::CD)) { // ]
                out << "Error: se esperaba ']' después del índice del array." << endl;
                exit(1);
            }
            return new AccesoArrayExp(id_name, index);
        } else {
            return new IdentifierExp(id_name); // Es solo un identificador
        }
    } else if (match(Token::PI)) { // ( <CExp> )
        e = parseCExp();
        if (!match(Token::PD)) {
            out << "Error: Falta paréntesis derecho." << endl;
            exit(1);
        }
        return e;
    } else if (match(Token::CI)) { // [ <CExp> (, <CExp>)* ] - LiteralArray
        return parseLiteralArray();
    }

    out << "Error: se esperaba una expresión o literal, pero se encontró: " << *current << endl;
    exit(1);
}

// <Tipo> ::= i64 | f64 | bool
string Parser::parseType() {
    if (match(Token::INT)) { // Asumiendo Token::I64_KEYWORD
        return "i64";
    } else if (match(Token::FLOAT)) { // Asumiendo Token::F64_KEYWORD
        return "f64";
    } else if (match(Token::BOOL)) { // Asumiendo Token::BOOL_KEYWORD
        return "bool";
    }
    out << "Error: se esperaba un tipo (i64, f64, bool)." << endl;
    exit(1);
}

// <Literal> ::= <LiteralEntero> | <LiteralFlotante>
Exp* Parser::parseLiteral() {
    if (match(Token::INT)) {
        return new IntExp(stoi(previous->text));
    } else if (match(Token::FLOAT)) {
        return new FloatExp(stod(previous->text));
    }
    out << "Error: se esperaba un literal (entero o flotante)." << endl;
    exit(1);
}

// <LiteralArray> ::= [ <CExp> (, <CExp>)* ]
Exp* Parser::parseLiteralArray() {
    ArrayExp* array_exp = new ArrayExp();
    if (check(Token::CD)) { // Array vacío []
        advance(); // Consumir ']'
        return array_exp;
    }
    array_exp->values.push_back(parseCExp());
    while (match(Token::COMA)) {
        array_exp->values.push_back(parseCExp());
    }
    if (!match(Token::CD)) {
        out << "Error: se esperaba ']' al final del literal de array." << endl;
        exit(1);
    }
    return array_exp;
}


// <ListaArgumentos> ::= [ &mut ] <CExp> (, [ &mut ] <CExp>)*
list<pair<bool, Exp*>> Parser::parseArgumentList() {
    list<pair<bool, Exp*>> args;
    bool is_mut_ref = false;

    // Primer argumento
    if (match(Token::REF)) {
        is_mut_ref = true;
    }
    if (match(Token::MUT)) {
        is_mut_ref = true; // Si es '&mut', 'mut' también se marca como true
    }
    Exp* arg_exp = parseCExp();
    args.push_back({is_mut_ref, arg_exp});

    // Argumentos adicionales
    while (match(Token::COMA)) {
        is_mut_ref = false; // Reset para el siguiente argumento
        if (match(Token::REF)) {
            is_mut_ref = true;
        }
        if (match(Token::MUT)) {
            is_mut_ref = true;
        }
        arg_exp = parseCExp();
        args.push_back({is_mut_ref, arg_exp});
    }
    return args;
}

