#ifndef VISITOR_H
#define VISITOR_H
#include "exp.h" 
#include <list>

// --- Declaraciones adelantadas de clases del AST ---
class BinaryExp;
class UnaryExp;
class IntExp;
class FloatExp;
class BoolExp;
class ArrayExp;
class IdentifierExp;
class FCallExp;
class AccesoArrayExp; 

class AssignStatement;
class PrintStatement;
class IfStatement;
class WhileStatement;
class ForStatement; // Declaración adelantada para ForStatement
class DecStament;
class BreakStm;
class ReturnStatement;
class FCallStm;
class AssignArrayStatement;
class StatementList;
class Body;
class FunDec;
class FunDecList;
class Program;

// --- Clase base Visitor ---
class Visitor {
public:
    // Métodos visit para expresiones
    virtual int visit(BinaryExp* exp) = 0;
    virtual int visit(UnaryExp* exp) = 0;
    virtual int visit(IntExp* exp) = 0;
    virtual int visit(FloatExp* exp) = 0;
    virtual int visit(BoolExp* exp) = 0;
    virtual int visit(ArrayExp* exp) = 0;
    virtual int visit(IdentifierExp* exp) = 0;
    virtual int visit(FCallExp* exp) = 0;
    virtual int visit(AccesoArrayExp* exp) = 0; 

    // Métodos visit para sentencias
    virtual void visit(AssignStatement* stm) = 0;
    virtual void visit(PrintStatement* stm) = 0;
    virtual void visit(IfStatement* stm) = 0;
    virtual void visit(WhileStatement* stm) = 0;
    virtual void visit(ForStatement* stm) = 0; // Método para ForStatement
    virtual void visit(DecStament* stm) = 0;
    virtual void visit(BreakStm* stm) = 0;
    virtual void visit(ReturnStatement* stm) = 0;
    virtual void visit(FCallStm* stm) = 0;
    virtual void visit(AssignArrayStatement* stm) = 0;
    virtual void visit(StatementList* stm) = 0;
    virtual void visit(Body* b) = 0;
    virtual void visit(FunDec* funDec) = 0;
    virtual void visit(FunDecList* funDecList) = 0;
    virtual void visit(Program* program) = 0;
};

// --- Clase PrintVisitor ---
class PrintVisitor : public Visitor {
private:
    std::ostream& out;
public:
    PrintVisitor(std::ostream& out) : out(out) {}
    void imprimir(Program* program);

    // Implementaciones de métodos visit para expresiones
    int visit(BinaryExp* exp) override;
    int visit(UnaryExp* exp) override;
    int visit(IntExp* exp) override;
    int visit(FloatExp* exp) override;
    int visit(BoolExp* exp) override;
    int visit(ArrayExp* exp) override;
    int visit(IdentifierExp* exp) override;
    int visit(FCallExp* exp) override;
    int visit(AccesoArrayExp* exp) override; 

    // Implementaciones de métodos visit para sentencias
    void visit(AssignStatement* stm) override;
    void visit(PrintStatement* stm) override;
    void visit(IfStatement* stm) override;
    void visit(WhileStatement* stm) override;
    void visit(ForStatement* stm) override; // Implementación para ForStatement
    void visit(DecStament* stm) override;
    void visit(BreakStm* stm) override;
    void visit(ReturnStatement* stm) override;
    void visit(FCallStm* stm) override;
    void visit(AssignArrayStatement* stm) override;
    void visit(StatementList* stm) override;
    void visit(Body* b) override;
    void visit(FunDec* funDec) override;
    void visit(FunDecList* funDecList) override;
    void visit(Program* program) override;
};

#endif