#ifndef IMP_INTERPRETER
#define IMP_INTERPRETER

#include <unordered_map>

#include "exp.h"
#include "imp_value_visitor.hh"
#include "environment.hh"
#include <stack>
#include <unordered_map>


using namespace std;

class ImpInterpreter : public ImpValueVisitor {
private:
    Environment<ImpValue> env;
    std::ostream& out;
    std::unordered_map<std::string, FunDec*> declared_functions; // Mapa para almacenar las definiciones de funciones
    std::stack<ImpValue> return_value_stack;                     // Pila para manejar los valores de retorno de las llamadas a funciones

public:
    ImpInterpreter(std::ostream& out) : out(out), env(out) {}
    void interpret(Program*);

    void visit(Program* p) override;
    void visit(Body* b) override;
    void visit(StatementList* e) override;
    void visit(AssignStatement* e) override;
    void visit(PrintStatement* e) override;
    void visit(IfStatement* e) override;
    void visit(WhileStatement* e) override;
    void visit(ForStatement* e) override;
    void visit(DecStament* e) override;
    void visit(BreakStm* e) override;
    void visit(ReturnStatement* e) override;
    void visit(FCallStm* e) override;
    void visit(AssignArrayStatement* e) override;
    void visit(FunDec* e) override;
    void visit(FunDecList* e) override;

    ImpValue visit(BinaryExp* e) override;
    ImpValue visit(UnaryExp* e) override;
    ImpValue visit(IntExp* e) override;
    ImpValue visit(FloatExp* e) override;
    ImpValue visit(BoolExp* e) override;
    ImpValue visit(ArrayExp* e) override;
    ImpValue visit(IdentifierExp* e) override;
    ImpValue visit(FCallExp* e) override;
    ImpValue visit(AccesoArrayExp* e) override;
};

#endif
