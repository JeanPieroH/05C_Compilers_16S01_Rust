#ifndef IMP_VALUE_VISITOR_HH
#define IMP_VALUE_VISITOR_HH

#include "exp.h" 
#include "imp_value.hh"

class ImpValueVisitor {
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

    virtual ImpValue visit(BinaryExp* e) = 0;
    virtual ImpValue visit(UnaryExp* e) = 0;
    virtual ImpValue visit(IntExp* e) = 0;
    virtual ImpValue visit(FloatExp* e) = 0;
    virtual ImpValue visit(BoolExp* e) = 0;
    virtual ImpValue visit(ArrayExp* e) = 0;
    virtual ImpValue visit(IdentifierExp* e) = 0;
    virtual ImpValue visit(FCallExp* e) = 0;
    virtual ImpValue visit(AccesoArrayExp* e) = 0; 
};

#endif // IMP_VALUE_VISITOR_HH