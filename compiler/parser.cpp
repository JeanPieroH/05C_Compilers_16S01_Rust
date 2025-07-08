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
    // Intentar imprimir a std::cerr para diagnóstico inmediato
    std::cerr << "DEBUG: Parser::parseProgram() BEGIN" << std::endl;
    out << "Parser::parseProgram() BEGIN" << std::endl; // Mensaje de depuración inicial a out

    Program* prog = new Program();
    if (!prog) {
        std::cerr << "FATAL_ERR: new Program() devolvió nullptr." << std::endl;
        out << "FATAL_OUT: new Program() devolvió nullptr." << std::endl;
        exit(1);
    }
    if (!prog->fundecs) {
        std::cerr << "FATAL_ERR: prog->fundecs es nullptr después de new Program()." << std::endl;
        out << "FATAL_OUT: prog->fundecs es nullptr después de new Program()." << std::endl;
        delete prog; // Evitar fuga si fundecs es el problema pero prog no.
        exit(1);
    }

    // out << "Parser::parseProgram(): new Program() y prog->fundecs parecen OK. Llamando a parseFunDec..." << std::endl;

//    while (check(Token::FUN)) { // <DeclaracionGlobal> ::= <DeclaracionFuncion>
        FunDec* fun = parseFunDec(); // parseFunDec también usa 'out' para errores y hace exit(1)
        if (!fun) {
             // Este caso no debería ocurrir si parseFunDec hace exit(1) en error.
             // Pero si parseFunDec pudiera retornar nullptr sin hacer exit:
             std::cerr << "FATAL_ERR: parseFunDec() devolvió nullptr." << std::endl;
             out << "FATAL_OUT: parseFunDec() devolvió nullptr." << std::endl;
             delete prog;
             exit(1);
        }
        prog->fundecs->add(fun);
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
        list<pair<string, bool>> params_from_parser = parseParameterList(); // Elementos son pair<"id:type", bool>
        for (auto const& [id_colon_type, is_mut] : params_from_parser) {
            size_t colon_pos = id_colon_type.find(':');
            if (colon_pos == std::string::npos) {
                out << "Error interno: formato de parámetro incorrecto desde parseParameterList: " << id_colon_type << endl;
                exit(1);
            }
            string p_id = id_colon_type.substr(0, colon_pos);
            string p_type = id_colon_type.substr(colon_pos + 1);

            fundec->parametros.push_back(p_id); // Almacena solo el nombre del parámetro
            fundec->tipos.push_back(p_type);    // Almacena solo el tipo del parámetro
            fundec->ref_mut.push_back(is_mut);  // Almacena si es &mut
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
    string id;
    string type;
    bool is_mut;

    // Primer parámetro
    is_mut = false;
    if (!match(Token::ID)) {
        out << "Error: se esperaba un identificador de parámetro." << endl;
        exit(1);
    }
    id = previous->text;

    if (!match(Token::COLON)) {
        out << "Error: se esperaba ':' después del identificador del parámetro '" << id << "'." << endl;
        exit(1);
    }

    if (match(Token::REF)) {
        if (!match(Token::MUT)) {
            out << "Error: se esperaba 'mut' después de '&' para el parámetro '" << id << "'." << endl;
            // Considerar si '&' solo es permitido para referencias inmutables (no en gramática actual)
            // Por ahora, se asume que si hay '&', debe ser '&mut'.
            exit(1);
        }
        is_mut = true;
    }
    type = parseType();
    params.push_back({id + ":" + type, is_mut});

    // Parámetros adicionales
    while (match(Token::COMA)) {
        is_mut = false; // Reset para el siguiente parámetro
        if (!match(Token::ID)) {
            out << "Error: se esperaba un identificador de parámetro después de ','." << endl;
            exit(1);
        }
        id = previous->text;

        if (!match(Token::COLON)) {
            out << "Error: se esperaba ':' después del identificador del parámetro '" << id << "'." << endl;
            exit(1);
        }

        if (match(Token::REF)) {
            if (!match(Token::MUT)) {
                out << "Error: se esperaba 'mut' después de '&' para el parámetro '" << id << "'." << endl;
                exit(1);
            }
            is_mut = true;
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

    // <DeclaracionVariable> ::= let [mut] <Identificador> [: <Tipo> | [: [<Tipo>; <LiteralEntero>]]] [= <CExp>];
    if (check(Token::LET)) { // Usar check primero para no consumir si no es LET
        advance(); // Consumir LET
        // out<<"entra"; // Debug
        bool is_mut = match(Token::MUT);
        if (!match(Token::ID)) {
            out << "Error: se esperaba un identificador después de 'let" << (is_mut ? " mut" : "") << "'." << endl;
            exit(1);
        }
        string id_name = previous->text;
        string type_name = ""; // Puede ser string vacía si no se especifica tipo
        Exp* array_size_exp = nullptr;
        Exp* initial_value = nullptr;

        if (match(Token::COLON)) { // Opcional : <Tipo> | [ <Tipo> ; <LiteralEntero> ]
            if (check(Token::CI)) { // Es un array: [ <Tipo> ; <LiteralEntero> ]
                advance(); // Consumir '['
                type_name = parseType(); // Tipo del array
                if (!match(Token::PC)) { // ;
                    out << "Error: se esperaba ';' en la declaración de array ([Type; Size])." << endl;
                    exit(1);
                }
                if (!match(Token::LITERAL_INT)) { // LiteralEntero para el tamaño
                    out << "Error: se esperaba un tamaño entero literal para el array." << endl;
                    exit(1);
                }
                array_size_exp = new IntExp(stoi(previous->text));
                if (!match(Token::CD)) { // ]
                    out << "Error: se esperaba ']' al final de la declaración de tamaño de array." << endl;
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

        DecStament* dec_stm = new DecStament(is_mut, type_name, id_name);
        if (initial_value) {
            dec_stm->value = initial_value;
        }
        // TODO: El AST de DecStament y su manejo en el visitor necesitarán considerar array_size_exp
        // y si la declaración es de un array o un tipo simple.
        // Por ejemplo, se podría añadir: dec_stm->array_size = array_size_exp;
        s = dec_stm;
    }
    // println!(<PrintExp>);
    else if (match(Token::PRINT)) {
        if (!match(Token::PI)) {
            out << "Error: se esperaba '(' después de 'println!'." << endl;
            exit(1);
        }
        string format_string = "";
        vector<Exp*> arg_exps;
        if (!match(Token::STRING)) {
            out << "Error: se esperaba una cadena literal como primer argumento de println!." << endl;
            exit(1);
        }
        format_string = previous->text;
        while (match(Token::COMA)) {
            arg_exps.push_back(parseCExp());
        }
        if (!match(Token::PD)) {
            out << "Error: se esperaba ')' después de los argumentos en println!." << endl;
            exit(1);
        }
        if (!match(Token::PC)) {
            out << "Error: se esperaba ';' al final de la sentencia println!." << endl;
            exit(1);
        }
        s = new PrintStatement(format_string, arg_exps);
    }
    // <SentenciaControl> ::= <IfElseSentencia> | <WhileSentencia> | <ForSentencia>
    else if (check(Token::IF) || check(Token::WHILE) || check(Token::FOR)) {
        s = parseControlStatement();
    }
    // return [<CExp>];
    else if (match(Token::RETURN)) {
        Exp* ret_exp = nullptr;
        if (!check(Token::PC)) { // Si no es directamente un ';', entonces hay una expresión
            ret_exp = parseCExp();
        }
        if (!match(Token::PC)) {
            out << "Error: se esperaba ';' al final de la sentencia 'return'." << endl;
            exit(1);
        }
        ReturnStatement* ret_stm = new ReturnStatement();
        if (ret_exp) { // Asignar la expresión si existe
            ret_stm->e = ret_exp;
        }
        s = ret_stm;
    }
    // break;
    else if (match(Token::BREAK)) {
        if (!match(Token::PC)) {
            out << "Error: se esperaba ';' al final de 'break'." << endl;
            exit(1);
        }
        s = new BreakStm();
    }
    // Sentencias que comienzan con ID:
    // <Asignacion>; -> <Identificador> = <CExp>;  O  <AccesoArray> = <CExp>;
    // <LlamadaFuncion>; -> <Identificador>(<ListaArgumentos>);
    else if (check(Token::ID)) {
        string id_name_val = current->text; // Guardar el texto del ID ANTES de consumirlo
        advance(); // Consumir el ID. 'previous' es ahora el token ID.

        // Caso 1: <LlamadaFuncion> ;  -> ID ( [<ListaArgumentos>] ) ;
        if (match(Token::PI)) { // Consumir PI si es una llamada a función
            FCallStm* fcall_stm = new FCallStm();
            fcall_stm->nombre = id_name_val;
            if (!check(Token::PD)) { // Si no es ')', parsear argumentos
                list<pair<bool, Exp*>> args_list = parseArgumentList();
                for (auto const& [is_mut, arg_exp_item] : args_list) {
                    fcall_stm->argumentos.push_back(arg_exp_item);
                    fcall_stm->ref_mut.push_back(is_mut);
                }
            }
            if (!match(Token::PD)) { // Consumir PD
                out << "Error: se esperaba ')' después de los argumentos de la función '" << id_name_val << "'." << endl;
                delete fcall_stm; exit(1);
            }
            if (!match(Token::PC)) { // Consumir PC
                out << "Error: se esperaba ';' al final de la llamada a función '" << id_name_val << "'." << endl;
                delete fcall_stm; exit(1);
            }
            s = fcall_stm;
        }
        // Caso 2: <Asignacion> ; -> <AccesoArray> = <CExp> ;  -> ID [ <CExp> ] = <CExp> ;
        else if (match(Token::CI)) { // Consumir CI si es acceso a array
            Exp* index_exp = parseCExp();
            if (!match(Token::CD)) { // Consumir CD
                out << "Error: se esperaba ']' después del índice del array para '" << id_name_val << "'." << endl;
                exit(1);
            }
            if (!match(Token::ASSIGN)) { // Consumir ASSIGN
                out << "Error: se esperaba '=' para la asignación al elemento del array '" << id_name_val << "'." << endl;
                exit(1);
            }
            Exp* rhs_exp = parseCExp();
            if (!match(Token::PC)) { // Consumir PC
                out << "Error: se esperaba ';' al final de la asignación de array '" << id_name_val << "'." << endl;
                exit(1);
            }
            s = new AssignArrayStatement(id_name_val, index_exp, rhs_exp);
        }
        // Caso 3: <Asignacion> ; -> <Identificador> = <CExp> ; -> ID = <CExp> ;
        else if (match(Token::ASSIGN)) { // Consumir ASSIGN si es asignación simple
            Exp* rhs_exp = parseCExp();
            if (!match(Token::PC)) { // Consumir PC
                out << "Error: se esperaba ';' al final de la asignación para '" << id_name_val << "'." << endl;
                exit(1);
            }
            s = new AssignStatement(id_name_val, rhs_exp);
        }
        // Error: ID no seguido de (, [, o =
        else {
            out << "Error: Sentencia que comienza con identificador '" << id_name_val
                << "' mal formada. Se esperaba '(', '[', o '=' después del ID. Se encontró: " << *current << endl;
            // 'previous' es el ID, 'current' es lo que le sigue.
            exit(1);
        }
    }
    // Si no es ninguna de las anteriores, es un error.
    else {
        if (isAtEnd()) {
            out << "Error: Se esperaba una sentencia, pero se encontró el final del archivo." << endl;
        } else {
            out << "Error: Sentencia no reconocida o mal formada. Token actual: " << *current << endl;
        }
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

    if (match(Token::LITERAL_INT)) {
        return new IntExp(stoi(previous->text));
    } else if (match(Token::LITERAL_FLOAT)) {
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
    if (match(Token::TYPE_I64)) {
        return "i64";
    } else if (match(Token::TYPE_F64)) {
        return "f64";
    } else if (match(Token::TYPE_BOOL)) {
        return "bool";
    } else if (match(Token::TYPE_I32)) {
        return "i32";
    } else if (match(Token::TYPE_F32)) {
        return "f32";
    }
    out << "Error: se esperaba un tipo (i64, f64, bool, i32, f32)." << endl;
    exit(1);
}

// <Literal> ::= <LiteralEntero> | <LiteralFlotante>
Exp* Parser::parseLiteral() {
    if (match(Token::LITERAL_INT)) {
        return new IntExp(stoi(previous->text));
    } else if (match(Token::LITERAL_FLOAT)) {
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
    bool is_mut_ref;
    Exp* arg_exp;

    // Primer argumento
    is_mut_ref = false;
    if (match(Token::REF)) {
        if (!match(Token::MUT)) {
            out << "Error: se esperaba 'mut' después de '&' en la lista de argumentos." << endl;
            exit(1);
        }
        is_mut_ref = true;
    } else if (match(Token::MUT)) {
        // Permitir 'mut' solo es un error si no va precedido por '&' según la gramática '[&mut]'
        // Sin embargo, si la intención fuera permitir 'mut Identificador' (pasar por valor y permitir mutación local a la función llamada)
        // la gramática y el manejo serían diferentes.
        // Por ahora, un 'mut' suelto aquí sin '&' antes es un error contextual.
        out << "Error: 'mut' inesperado sin '&' precedente en la lista de argumentos." << endl;
        exit(1);
    }
    arg_exp = parseCExp();
    args.push_back({is_mut_ref, arg_exp});

    // Argumentos adicionales
    while (match(Token::COMA)) {
        is_mut_ref = false; // Reset para el siguiente argumento
        if (match(Token::REF)) {
            if (!match(Token::MUT)) {
                out << "Error: se esperaba 'mut' después de '&' en la lista de argumentos (después de coma)." << endl;
                exit(1);
            }
            is_mut_ref = true;
        } else if (match(Token::MUT)) {
            out << "Error: 'mut' inesperado sin '&' precedente en la lista de argumentos (después de coma)." << endl;
            exit(1);
        }
        arg_exp = parseCExp();
        args.push_back({is_mut_ref, arg_exp});
    }
    return args;
}

