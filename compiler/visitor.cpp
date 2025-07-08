#include <iostream>
#include "exp.h"
#include "visitor.h"
using namespace std;

#include <string>

// --- Implementaciones de los métodos accept en las clases AST ---

int BinaryExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int UnaryExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int IntExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int FloatExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int BoolExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int ArrayExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int IdentifierExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int FCallExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int AccesoArrayExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

// --- Métodos accept para sentencias ---

int AssignStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int PrintStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int IfStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int WhileStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int ForStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int DecStament::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int BreakStm::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int ReturnStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int FCallStm::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int AssignArrayStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int StatementList::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int Body::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int FunDec::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int FunDecList::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int Program::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

// --- Implementaciones de PrintVisitor ---

int PrintVisitor::visit(BinaryExp* exp) {
    out << "(";
    exp->left->accept(this);
    out << " " << Exp::binopToChar(exp->op) << " ";
    exp->right->accept(this);
    out << ")";
    return 0;
}

int PrintVisitor::visit(UnaryExp* exp) {
    out << Exp::unopToChar(exp->op);
    exp->operand->accept(this);
    return 0;
}

int PrintVisitor::visit(IntExp* exp) {
    out << exp->value;
    return 0;
}

int PrintVisitor::visit(FloatExp* exp) {
    out << exp->value;
    return 0;
}

int PrintVisitor::visit(BoolExp* exp) {
    if(exp->value) out << "true";
    else out << "false";
    return 0;
}

int PrintVisitor::visit(ArrayExp* exp) {
    out << "[";
    for (size_t i = 0; i < exp->values.size(); ++i) {
        exp->values[i]->accept(this);
        if (i < exp->values.size() - 1) {
            out << ", ";
        }
    }
    out << "]";
    return 0;
}

int PrintVisitor::visit(IdentifierExp* exp) {
    out << exp->name;
    return 0;
}

int PrintVisitor::visit(FCallExp* exp) {
    out << exp->nombre << "(";
    for (size_t i = 0; i < exp->argumentos.size(); ++i) {
        auto it_ref_mut = exp->ref_mut.begin();
        std::advance(it_ref_mut, i);
        if (it_ref_mut != exp->ref_mut.end() && *it_ref_mut) {
            out << "&mut ";
        }
        exp->argumentos[i]->accept(this);
        if (i < exp->argumentos.size() - 1) {
            out << ", ";
        }
    }
    out << ")";
    return 0;
}

int PrintVisitor::visit(AccesoArrayExp* exp) {
    out << exp->id << "["; 
    exp->index_exp->accept(this); 
    out << "]";
    return 0;
}

void PrintVisitor::visit(AssignStatement* stm) {
    out << stm->id << " = ";
    stm->rhs->accept(this);
    out << ";";
}

void PrintVisitor::visit(PrintStatement* stm) {
    out << "println!(\"";
    out << stm->cadena << "\"";
    for (Exp* expr : stm->e) {
        out << ", ";
        expr->accept(this);
    }
    out << ");";
}

void PrintVisitor::visit(IfStatement* stm) {
    out << "if ";
    stm->condition->accept(this);
    out << " ";
    stm->then->accept(this);
    if(stm->els){
        out << " else ";
        stm->els->accept(this);
    }
}

void PrintVisitor::visit(WhileStatement* stm){
    out << "while ";
    stm->condition->accept(this);
    out << " ";
    stm->b->accept(this);
}

void PrintVisitor::visit(ForStatement* stm){
    out << "for " << stm->id << " in ";
    stm->init->accept(this);
    out << " .. ";
    stm->end->accept(this);
    out << " ";
    stm->b->accept(this);
}


void PrintVisitor::visit(DecStament* stm){
    out << "let ";
    if (stm->is_mut) {
        out << "mut ";
    }
    out << stm->id;
    if (!stm->type.empty()) {
        out << ": " << stm->type;
    }
    if (stm->value) {
        out << " = ";
        stm->value->accept(this);
    }
    out << ";";
}

void PrintVisitor::visit(BreakStm* stm) {
    out << "break;";
}

void PrintVisitor::visit(ReturnStatement* stm) {
    out << "return";
    if (stm->e) {
        out << " ";
        stm->e->accept(this);
    }
    out << ";";
}

void PrintVisitor::visit(FCallStm* stm) {
    out << stm->nombre << "(";
    for (size_t i = 0; i < stm->argumentos.size(); ++i) {
        auto it_ref_mut = stm->ref_mut.begin();
        std::advance(it_ref_mut, i);
        if (it_ref_mut != stm->ref_mut.end() && *it_ref_mut) {
            out << "&mut ";
        }
        stm->argumentos[i]->accept(this);
        if (i < stm->argumentos.size() - 1) {
            out << ", ";
        }
    }
    out << ");";
}

void PrintVisitor::visit(AssignArrayStatement* stm) {
    // Aquí usamos stm->id, stm->index_exp para imprimir el acceso al array
    // en lugar de depender de un AccesoArrayExp* completo.
    out << stm->id << "["; 
    stm->index_exp->accept(this);
    out << "] = ";
    stm->rhs->accept(this); 
    out << ";";
}

void PrintVisitor::visit(StatementList* stm){
    for(Stm* s : stm->stms){
        s->accept(this);
        out << endl;
    }
}

void PrintVisitor::visit(Body* b){
    out << "{" << endl;
    if (b->slist) {
        b->slist->accept(this);
    }
    out << "}";
}

void PrintVisitor::visit(FunDec* funDec) {
    out << "fn " << funDec->nombre << "(";
    for (size_t i = 0; i < funDec->parametros.size(); ++i) {
        auto it_tipos = funDec->tipos.begin();
        std::advance(it_tipos, i);
        auto it_ref_mut = funDec->ref_mut.begin();
        std::advance(it_ref_mut, i);

        if (it_ref_mut != funDec->ref_mut.end() && *it_ref_mut) {
            out << "&mut ";
        }
        out << funDec->parametros[i] << ": " << *it_tipos;
        if (i < funDec->parametros.size() - 1) {
            out << ", ";
        }
    }
    out << ")";
    if (!funDec->tipo.empty()) {
        out << " -> " << funDec->tipo;
    }
    out << " ";
    funDec->cuerpo->accept(this);
}

void PrintVisitor::visit(FunDecList* funDecList) {
    for (FunDec* func : funDecList->Fundecs) {
        func->accept(this);
        out << endl << endl;
    }
}

void PrintVisitor::visit(Program* program){
    if (program->fundecs) {
        program->fundecs->accept(this);
    }
}

void PrintVisitor::imprimir(Program* program){
    program->accept(this);
}