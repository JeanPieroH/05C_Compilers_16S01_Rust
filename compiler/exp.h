#ifndef EXP_H
#define EXP_H
#include "imp_value.hh"
#include "imp_type.hh"
#include <string>
#include <unordered_map>
#include <list>
#include "visitor.h"
using namespace std;
enum BinaryOp { PLUS_OP, MINUS_OP, MUL_OP, DIV_OP,LT_OP, LE_OP, GT_OP, GE_OP,EQ_OP,NEQ_OP, AND, OR};
enum UnaryOp {NOT_OP,MINUS_UNARY_OP};

class Body;
class ImpValueVisitor;

class Exp {
public:
    virtual int  accept(Visitor* visitor) = 0;
    virtual ImpValue accept(ImpValueVisitor* v) = 0;
    virtual ~Exp() = 0;
    static string binopToChar(BinaryOp op);
    static string unopToChar(UnaryOp op);
};

class BinaryExp : public Exp {
public:
    Exp *left, *right;
    BinaryOp op;
    BinaryExp(Exp* l, Exp* r, BinaryOp op);
    ~BinaryExp();
    int accept(Visitor* visitor);
    ImpValue accept(ImpValueVisitor* v);
};

class UnaryExp : public Exp {
public:
    Exp *operand;
    UnaryOp op;
    UnaryExp(Exp* operand, UnaryOp op);
    ~UnaryExp();
    int accept(Visitor* visitor);
    ImpValue accept(ImpValueVisitor* v);
};

class IntExp : public Exp {
public:
    int value;
    IntExp(int v);
    ~IntExp();
    int accept(Visitor* visitor);
    ImpValue accept(ImpValueVisitor* v);
};

class FloatExp : public Exp {
public:
    double value;
    FloatExp(double v);
    ~FloatExp();
    int accept(Visitor* visitor);
    ImpValue accept(ImpValueVisitor* v);
};

class BoolExp : public Exp {
public:
    bool value;
    BoolExp(bool v);
    ~BoolExp();
    int accept(Visitor* visitor);
    ImpValue accept(ImpValueVisitor* v);
};

class ArrayExp : public Exp {
public:
    vector<Exp*> values;
    ArrayExp();
    ~ArrayExp();
    int accept(Visitor* visitor);
    ImpValue accept(ImpValueVisitor* v);
};

class IdentifierExp : public Exp {
public:
    std::string name;
    IdentifierExp(const std::string& n);
    ~IdentifierExp();
    int accept(Visitor* visitor);
    ImpValue accept(ImpValueVisitor* v);
};

class FCallExp : public Exp {
public:
    string nombre;
    vector<Exp*> argumentos;
    list<bool> ref_mut;
    FCallExp();
    ~FCallExp();
    int accept(Visitor* visitor);
    ImpValue accept(ImpValueVisitor* v);
};

class AccesoArrayExp : public Exp {
public:
    string id;
    Exp* index_exp;
    AccesoArrayExp(string id, Exp* index);
    ~AccesoArrayExp();
    int accept(Visitor* visitor);
    ImpValue accept(ImpValueVisitor* v);
};

class Stm {
public:
    virtual int accept(Visitor* visitor) = 0;
    virtual ~Stm() = 0;
    virtual void accept(ImpValueVisitor* v) = 0;
};

class AssignStatement : public Stm {
public:
    std::string id;
    Exp* rhs;
    AssignStatement(std::string id, Exp* e);
    ~AssignStatement();
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
};

class PrintStatement : public Stm {
public:
    string cadena;
    vector<Exp*> e;
    PrintStatement(string c,vector<Exp*> e);
    ~PrintStatement();    
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
};

class IfStatement : public Stm {
public:
    Exp* condition;
    Body* then;
    Body* els;
    IfStatement(Exp* condition, Body* then, Body* els);
    ~IfStatement();
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
};
class WhileStatement : public Stm {
public:
    Exp* condition;
    Body* b;
    WhileStatement(Exp* condition, Body* b);
    ~WhileStatement();
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
};


class ForStatement : public Stm {
public:
    string id;
    Exp* init;
    Exp* end;
    Body* b;
    ForStatement(string id,Exp* init, Exp* end, Body* b);
    ~ForStatement();
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
};


class DecStament : public Stm {
public:
    string type;
    bool is_mut;
    string id;
    Exp* value;
    DecStament(bool is_mut, string type, string id);
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
    ~DecStament();
};


class BreakStm: public Stm{
public:
    BreakStm(){};
    ~BreakStm(){};
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
};


class FCallStm : public Stm {
public:
    string nombre;
    vector<Exp*> argumentos;
    list<bool> ref_mut;
    FCallStm(){};
    ~FCallStm(){};
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
};

class ReturnStatement: public Stm {
public:
    Exp* e;
    ReturnStatement();
    ~ReturnStatement();
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
};

class AssignArrayStatement : public Stm {
public:
    string id;
    Exp* index_exp;
    Exp* rhs;                     
    AssignArrayStatement(string id, Exp* e, Exp* rhs);
    ~AssignArrayStatement();
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
};

class StatementList {
public:
    list<Stm*> stms;
    void add(Stm* stm);
    StatementList();
    ~StatementList();
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
};

class Body{
public:
    StatementList* slist;
    Body(StatementList* stms);
    ~Body();
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
};

class FunDec {
public:
    string nombre;
    string tipo;
    vector<string> parametros;
    list<string> tipos;
    list<bool> ref_mut;
    Body* cuerpo;
    FunDec();
    ~FunDec();
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
};

class FunDecList{
public:
    list<FunDec*> Fundecs;
    void add(FunDec* fundec) {
        Fundecs.push_back(fundec);
    };
    FunDecList();
    ~FunDecList();
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
};

class Program {
public:
    FunDecList* fundecs;
    Program();
    ~Program();
    int accept(Visitor* v);
    void accept(ImpValueVisitor* v);
};



#endif // EXP_H