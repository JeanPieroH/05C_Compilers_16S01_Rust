#ifndef TYPE_VISITOR
#define TYPE_VISITOR

#include "exp.h"
#include "imp_type.hh"

class TypeVisitor {
public:
    virtual void visit(Program* p) = 0;
    virtual void visit(Body* b) = 0;
    virtual void visit(StatementList* e) = 0;
    virtual void visit(AssignStatement* e) = 0;
    virtual void visit(PrintStatement* e) = 0;
    virtual void visit(IfStatement* e) = 0;
    virtual void visit(WhileStatement* e) = 0;
    virtual void visit(ForStatement* e) = 0;
    virtual void visit(DecStament* e) = 0;
    virtual void visit(BreakStm* e) = 0;
    virtual void visit(ReturnStatement* e) = 0;
    virtual void visit(FCallStm* e) = 0;
    virtual void visit(AssignArrayStatement* e) = 0; 
    virtual void visit(FunDec* e) = 0;
    virtual void visit(FunDecList* e) = 0;

    virtual ImpType visit(BinaryExp* e) = 0;
    virtual ImpType visit(UnaryExp* e) = 0;
    virtual ImpType visit(IntExp* e) = 0;
    virtual ImpType visit(FloatExp* e) = 0;
    virtual ImpType visit(BoolExp* e) = 0;
    virtual ImpType visit(ArrayExp* e) = 0;
    virtual ImpType visit(IdentifierExp* e) = 0;
    virtual ImpType visit(FCallExp* e) = 0;
    virtual ImpType visit(AccesoArrayExp* e) = 0; 
};

#endif