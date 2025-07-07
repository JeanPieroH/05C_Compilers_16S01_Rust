#ifndef GENCODE_HH
#define GENCODE_HH

#include <unordered_map>
#include <string>
#include <stack>
#include <iostream>

#include "exp.h"
#include "imp_value_visitor.hh"
#include "environment.hh"
#include "imp_value.hh"   // ImpValue para tipado/constantes

class ImpCODE : public ImpValueVisitor{
private:
    Environment<ImpValue> env_vars;          // Tabla de símbolos para análisis semántico.
    std::ostream& out;                       // Flujo de salida para el código ASM/intermedio.

    std::unordered_map<std::string, std::string> env_fun_labels; // Nombre de función a etiqueta ASM.
    int label_count;                         // Contador para etiquetas únicas.
    long current_offset;                     // Offset actual en el stack para vars locales (función actual).
    std::unordered_map<std::string, long> stack_offsets; // Nombre de var local a offset en stack frame.
    std::stack<int> loop_break_labels;       // Pila de etiquetas para 'break' en bucles anidados.
    int current_function_return_label;       // Etiqueta de retorno para la función actual.

    std::string new_label_str() { return "L" + std::to_string(label_count++); }

public:
    ImpCODE(std::ostream& out_stream) : 
        out(out_stream), 
        env_vars(out_stream), 
        label_count(0), 
        current_offset(0) {}

    void interpret(Program* p); // Punto de entrada para la generación de código.

    // Métodos visit para statements (generan código, no devuelven ImpValue).
    void visit(Program* p);
    void visit(Body* b);
    void visit(StatementList* e);
    void visit(AssignStatement* e);
    void visit(PrintStatement* e);
    void visit(IfStatement* e);
    void visit(WhileStatement* e);
    void visit(ForStatement* e);
    void visit(DecStament* e);
    void visit(BreakStm* e);
    void visit(ReturnStatement* e);
    void visit(FCallStm* e);
    void visit(AssignArrayStatement* e);

    // Métodos visit para declaraciones de funciones.
    void visit(FunDec* e);
    void visit(FunDecList* e);

    // Métodos visit para expresiones (generan código que deja el valor calculado en un registro/stack).
    // ImpValue devuelto representa el TIPO o valor de CONSTANTE literal.
    ImpValue visit(BinaryExp* e);
    ImpValue visit(UnaryExp* e);
    ImpValue visit(IntExp* e);
    ImpValue visit(FloatExp* e);
    ImpValue visit(BoolExp* e);
    ImpValue visit(ArrayExp* e);
    ImpValue visit(IdentifierExp* e);
    ImpValue visit(FCallExp* e);
    ImpValue visit(AccesoArrayExp* e);
};

#endif