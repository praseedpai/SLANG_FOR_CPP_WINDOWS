#ifndef AST_INCLUDED
#define AST_INCLUDED

class Runtime_Context;
class Compilation_Context;
class Execution_Context;
class Procedure;

#include "common.h"
#include "codegen.h"

using namespace std;

class Expression {

public:
    virtual SymbolInfo *evaluate(Runtime_Context *ctx) = 0;
    virtual Value *codegen(Execution_Context *ctx)=0;
    virtual TypeInfo typecheck(Compilation_Context *ctx) = 0;
    virtual TypeInfo get_type() = 0;
    virtual string evaluate_string(Execution_Context *ctx)=0;
    virtual SymbolInfo * generate_js(Runtime_Context *ctx) = 0;
};

class BooleanConstant: public Expression {
    SymbolInfo *info;

public:
    BooleanConstant(bool val);
    SymbolInfo *evaluate(Runtime_Context *ctx);
    TypeInfo typecheck(Compilation_Context *ctx);
    TypeInfo get_type();
    Value *codegen(Execution_Context *ctx);
    ~BooleanConstant();
    string evaluate_string(Execution_Context *ctx) {
           return string("");

    }
    ;
    SymbolInfo * generate_js(Runtime_Context *ctx);
};

class NumericConstant: public Expression {

    SymbolInfo *info;

public:
    NumericConstant(double _value);
    SymbolInfo *evaluate(Runtime_Context *ctx);
    TypeInfo typecheck(Compilation_Context *ctx);
    TypeInfo get_type();
    Value *codegen(Execution_Context *ctx);
    string evaluate_string(Execution_Context *ctx) {
          return string("");

    }
    ;
    ~NumericConstant();
    SymbolInfo * generate_js(Runtime_Context *ctx);
};

class StringLiteral: public Expression {

    SymbolInfo *info;

public:
    StringLiteral(std::string _value);
    SymbolInfo *evaluate(Runtime_Context *ctx);
    TypeInfo typecheck(Compilation_Context *ctx);
    TypeInfo get_type();
    Value *codegen(Execution_Context *ctx);
    string evaluate_string(Execution_Context *ctx);
    ~StringLiteral();
    SymbolInfo * generate_js(Runtime_Context *ctx);
};

class Variable: public Expression {

    TypeInfo type;
    std::string name;
    SymbolInfo *info;
public:

    Variable(SymbolInfo *_info);
    Variable(Compilation_Context *ctx, std::string _name, double _value);
    Variable(Compilation_Context *ctx, std::string _name, std::string _value);
    Variable(Compilation_Context *ctx, std::string _name, bool _value);
    std::string get_name();
    SymbolInfo *evaluate(Runtime_Context *ctx);
    TypeInfo typecheck(Compilation_Context *ctx);
    TypeInfo get_type();
    Value *codegen(Execution_Context *ctx);
    string evaluate_string(Execution_Context *ctx);
    ~Variable();
    SymbolInfo * generate_js(Runtime_Context *ctx);
};

class BinaryPlus: public Expression {
    Expression *exp1, *exp2;
    TypeInfo type;

public:
    BinaryPlus(Expression *e1, Expression *e2);
    SymbolInfo *evaluate(Runtime_Context *ctx);
    TypeInfo typecheck(Compilation_Context *ctx);
    TypeInfo get_type();
    Value *codegen(Execution_Context *ctx);
    string evaluate_string(Execution_Context *ctx);
    ~BinaryPlus();
    SymbolInfo * generate_js(Runtime_Context *ctx);
};

class BinaryMinus: public Expression {
    Expression *exp1, *exp2;
    TypeInfo type;

public:
    BinaryMinus(Expression *e1, Expression *e2);
    SymbolInfo *evaluate(Runtime_Context *ctx);
    TypeInfo typecheck(Compilation_Context *ctx);
    TypeInfo get_type();
    Value *codegen(Execution_Context *ctx);
    ~BinaryMinus();
    string evaluate_string(Execution_Context *ctx) {
           return string("");

    }
    ;
    SymbolInfo * generate_js(Runtime_Context *ctx);
};

class Mult: public Expression {
    Expression *exp1, *exp2;
    TypeInfo type;

public:
    Mult(Expression *e1, Expression *e2);
    SymbolInfo *evaluate(Runtime_Context *ctx);
    TypeInfo typecheck(Compilation_Context *ctx);
    TypeInfo get_type();
    Value *codegen(Execution_Context *ctx);
    ~Mult();
    string evaluate_string(Execution_Context *ctx) {
          return string("");

    }
    ;
    SymbolInfo * generate_js(Runtime_Context *ctx);
  
};

class Div: public Expression {
    Expression *exp1, *exp2;
    TypeInfo type;

public:
    Div(Expression *e1, Expression *e2);
    SymbolInfo *evaluate(Runtime_Context *ctx);
    TypeInfo typecheck(Compilation_Context *ctx);
    TypeInfo get_type();
    Value *codegen(Execution_Context *ctx);
    ~Div();
    string evaluate_string(Execution_Context *ctx) {
         return string("");

    }
    ;
    SymbolInfo * generate_js(Runtime_Context *ctx);
};

class UnaryPlus: public Expression {
    Expression *exp1;
    TypeInfo type;

public:
    UnaryPlus(Expression *e1);
    SymbolInfo *evaluate(Runtime_Context *ctx);
    TypeInfo typecheck(Compilation_Context *ctx);
    TypeInfo get_type();
    Value *codegen(Execution_Context *ctx);
    ~UnaryPlus();
    string evaluate_string(Execution_Context *ctx) {
           return string("");

    }
    ;
    SymbolInfo * generate_js(Runtime_Context *ctx);
};

class UnaryMinus: public Expression {
    Expression *exp1;
    TypeInfo type;

public:
    UnaryMinus(Expression *e1);
    SymbolInfo *evaluate(Runtime_Context *ctx);
    TypeInfo typecheck(Compilation_Context *ctx);
    TypeInfo get_type();
    Value *codegen(Execution_Context *ctx);
    ~UnaryMinus();
    string evaluate_string(Execution_Context *ctx) {
           return string("");
    }
    ;
    SymbolInfo * generate_js(Runtime_Context *ctx);
};

class RelationalExpression: public Expression {
    Expression *exp1, *exp2;
    TypeInfo type, operand_type;
    RelationalOperator optr;

public:
    RelationalExpression(Expression *e1, Expression *e2,
                         RelationalOperator _op);
    SymbolInfo *evaluate(Runtime_Context *ctx);
    TypeInfo typecheck(Compilation_Context *ctx);
    TypeInfo get_type();
    Value *codegen(Execution_Context *ctx);
    ~RelationalExpression();
    string evaluate_string(Execution_Context *ctx) {
            return string("");

    }
    ;
    SymbolInfo * generate_js(Runtime_Context *ctx);
};

class LogicalExpression: public Expression {
    Expression *exp1, *exp2;
    TypeInfo type;
    Token optr;

public:
    LogicalExpression(Expression *e1, Expression *e2, Token _op);
    SymbolInfo *evaluate(Runtime_Context *ctx);
    TypeInfo typecheck(Compilation_Context *ctx);
    TypeInfo get_type();
    Value *codegen(Execution_Context *ctx);
    ~LogicalExpression();
    string evaluate_string(Execution_Context *ctx) {
         return string("");

    }
    ;
    SymbolInfo * generate_js(Runtime_Context *ctx);
};

class LogicalNot: public Expression {
    Expression *exp;
    TypeInfo type;

public:
    LogicalNot(Expression *e1);
    SymbolInfo *evaluate(Runtime_Context *ctx);
    TypeInfo typecheck(Compilation_Context *ctx);
    TypeInfo get_type();
    ~LogicalNot();
    Value *codegen(Execution_Context *ctx);
    string evaluate_string(Execution_Context *ctx) {
           return string("");

    }
    ;
    SymbolInfo * generate_js(Runtime_Context *ctx);
};

class CallExpression: public Expression {
    Procedure *proc;
    vector<Expression *> actuals;
    string procname;
    bool isrecurse;
    TypeInfo type;

public:
    CallExpression(Procedure *_proc, vector<Expression *> _actuals);
    CallExpression(string _name, vector<Expression *> _actuals,
                   bool _isrecurse, TypeInfo _type);
    SymbolInfo *evaluate(Runtime_Context *ctx);
    TypeInfo typecheck(Compilation_Context *ctx);
    TypeInfo get_type();
    Value *codegen(Execution_Context *ctx);
    ~CallExpression();
    string evaluate_string(Execution_Context *ctx) {
         return string("");
    }
    ;
    SymbolInfo * generate_js(Runtime_Context *ctx);

};

#endif

