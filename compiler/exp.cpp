#include <iostream>
#include "exp.h" // Incluye tu archivo de cabecera del AST

// --- Implementaciones de Exp ---
Exp::~Exp() { }

std::string Exp::binopToChar(BinaryOp op) {
    std::string c;
    switch(op) {
        case PLUS_OP: c = "+"; break;
        case MINUS_OP: c = "-"; break;
        case MUL_OP: c = "*"; break;
        case DIV_OP: c = "/"; break;
        case LT_OP: c = "<"; break;
        case LE_OP: c = "<="; break;
        case GT_OP: c = ">"; break;
        case GE_OP: c = ">="; break;
        case EQ_OP: c = "=="; break;
        case NEQ_OP: c = "!="; break;
        case AND: c = "&&"; break;
        case OR: c = "||"; break;
        default: c = "$";
    }
    return c;
}

std::string Exp::unopToChar(UnaryOp op) {
    std::string c;
    switch(op) {
        case NOT_OP: c = "!"; break;
        case MINUS_UNARY_OP: c = "-"; break;
        default: c = "$";
    }
    return c;
}

// --- Implementaciones de BinaryExp ---
BinaryExp::BinaryExp(Exp* l, Exp* r, BinaryOp op) : left(l), right(r), op(op) {}

BinaryExp::~BinaryExp() {
    delete left;
    delete right;
}

// --- Implementaciones de UnaryExp ---
UnaryExp::UnaryExp(Exp* operand_val, UnaryOp op_val) : operand(operand_val), op(op_val) {}

UnaryExp::~UnaryExp() {
    delete operand;
}

// --- Implementaciones de IntExp ---
IntExp::IntExp(int v) : value(v) {}

IntExp::~IntExp() { }

// --- Implementaciones de FloatExp ---
FloatExp::FloatExp(double v) : value(v) {}

FloatExp::~FloatExp() { }

// --- Implementaciones de BoolExp ---
BoolExp::BoolExp(bool v) : value(v) {}

BoolExp::~BoolExp() { }

// --- Implementaciones de ArrayExp ---
ArrayExp::ArrayExp() : values() {}

ArrayExp::~ArrayExp() {
    for (Exp* val : values) {
        delete val;
    }
}

// --- Implementaciones de IdentifierExp ---
IdentifierExp::IdentifierExp(const std::string& n) : name(n) {}

IdentifierExp::~IdentifierExp() { }

// --- Implementaciones de FCallExp ---
FCallExp::FCallExp() : nombre(""), argumentos(), ref_mut() {}

FCallExp::~FCallExp() {
    for (Exp* arg : argumentos) {
        delete arg;
    }
}

// --- Implementaciones de AccesoArrayExp ---
AccesoArrayExp::AccesoArrayExp(string id_val, Exp* index_exp_val) : id(id_val), index_exp(index_exp_val) {}

AccesoArrayExp::~AccesoArrayExp() {
    delete index_exp;
}

// --- Implementaciones de Stm ---
Stm::~Stm() { }

// --- Implementaciones de AssignStatement ---
AssignStatement::AssignStatement(std::string id_val, Exp* e_val) : id(id_val), rhs(e_val) {}

AssignStatement::~AssignStatement() {
    delete rhs;
}

// --- Implementaciones de PrintStatement ---
PrintStatement::PrintStatement(std::string c_val, std::vector<Exp*> e_val) : cadena(c_val), e(e_val) {}

PrintStatement::~PrintStatement() {
    for (Exp* exp_to_delete : e) {
        delete exp_to_delete;
    }
}

// --- Implementaciones de IfStatement ---
IfStatement::IfStatement(Exp* c, Body* t, Body* e_val) : condition(c), then(t), els(e_val) {}

IfStatement::~IfStatement() {
    delete condition;
    delete then;
    delete els;
}

// --- Implementaciones de WhileStatement ---
WhileStatement::WhileStatement(Exp* c, Body* b_val) : condition(c), b(b_val) {}

WhileStatement::~WhileStatement() {
    delete condition;
    delete b;
}

// --- Implementaciones de ForStatement ---
ForStatement::ForStatement(string id_val, Exp* init_val, Exp* end_val, Body* b_val):id(id_val), init(init_val), end(end_val), b(b_val) {}

ForStatement::~ForStatement() {
    delete init;
    delete end;
    delete b;
}

// --- Implementaciones de DecStament ---
DecStament::DecStament(bool is_mut_val, std::string type_val, std::string id_val) : is_mut(is_mut_val), type(type_val), id(id_val), value(nullptr) {}

DecStament::~DecStament() {
    delete value;
}


// --- Implementaciones de ReturnStatement ---
ReturnStatement::ReturnStatement() : e(nullptr) {}

ReturnStatement::~ReturnStatement() {
    delete e;
}

// --- Implementaciones de AssignArrayStatement ---
// Se ha modificado el constructor para que coincida con la nueva definición en exp.h
AssignArrayStatement::AssignArrayStatement(string id_val, Exp* index_exp_val, Exp* rhs_val): id(id_val), index_exp(index_exp_val), rhs(rhs_val) {}

AssignArrayStatement::~AssignArrayStatement() {
    delete index_exp;
    delete rhs;
}

// --- Implementaciones de StatementList ---
StatementList::StatementList() : stms() {}

void StatementList::add(Stm* s) {
    stms.push_back(s);
}

StatementList::~StatementList() {
    for (Stm* s : stms) {
        delete s;
    }
}

// --- Implementaciones de Body ---
Body::Body(StatementList* stms_val) : slist(stms_val) {}

Body::~Body() {
    delete slist;
}

// --- Implementaciones de FunDec ---
FunDec::FunDec() : nombre(""), tipo(""), parametros(), tipos(), ref_mut(), cuerpo(nullptr) {}

FunDec::~FunDec() {
    delete cuerpo;
}

// --- Implementaciones de FunDecList ---
FunDecList::FunDecList() : Fundecs() {}

FunDecList::~FunDecList() {
    for (FunDec* func : Fundecs) {
        delete func;
    }
}

// --- Implementaciones de Program ---
Program::Program() : fundecs(new FunDecList()) {} // Initialize with a new FunDecList

Program::~Program() {
    delete fundecs; // This is now safe and correct
}