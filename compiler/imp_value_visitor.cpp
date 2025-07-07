#include "imp_value_visitor.hh"

// --- Métodos accept para expresiones (devuelven ImpValue) ---

ImpValue BinaryExp::accept(ImpValueVisitor* v) {
    return v->visit(this);
}

ImpValue UnaryExp::accept(ImpValueVisitor* v) {
    return v->visit(this);
}

ImpValue IntExp::accept(ImpValueVisitor* v) {
    return v->visit(this);
}

ImpValue FloatExp::accept(ImpValueVisitor* v) {
    return v->visit(this);
}

ImpValue BoolExp::accept(ImpValueVisitor* v) {
    return v->visit(this);
}

ImpValue ArrayExp::accept(ImpValueVisitor* v) {
    return v->visit(this);
}

ImpValue IdentifierExp::accept(ImpValueVisitor* v) {
    return v->visit(this);
}

ImpValue FCallExp::accept(ImpValueVisitor* v) {
    return v->visit(this);
}

ImpValue AccesoArrayExp::accept(ImpValueVisitor* v) {
    return v->visit(this);
}

// --- Métodos accept para sentencias (devuelven void) ---

void AssignStatement::accept(ImpValueVisitor* v) {
    v->visit(this);
}

void PrintStatement::accept(ImpValueVisitor* v) {
    v->visit(this);
}

void IfStatement::accept(ImpValueVisitor* v) {
    v->visit(this);
}

void WhileStatement::accept(ImpValueVisitor* v) {
    v->visit(this);
}

void ForStatement::accept(ImpValueVisitor* v) { 
    v->visit(this);
}

void DecStament::accept(ImpValueVisitor* v) {
    v->visit(this);
}

void BreakStm::accept(ImpValueVisitor* v) {
    v->visit(this);
}

void ReturnStatement::accept(ImpValueVisitor* v) {
    v->visit(this);
}

void FCallStm::accept(ImpValueVisitor* v) {
    v->visit(this);
}

void StatementList::accept(ImpValueVisitor* v) {
    v->visit(this);
}

void Body::accept(ImpValueVisitor* v) {
    v->visit(this);
}

void FunDec::accept(ImpValueVisitor* v) {
    v->visit(this);
}

void FunDecList::accept(ImpValueVisitor* v) {
    v->visit(this);
}

void Program::accept(ImpValueVisitor* v) {
    v->visit(this); 
}

void AssignArrayStatement::accept(ImpValueVisitor* v) {
    v->visit(this);
}