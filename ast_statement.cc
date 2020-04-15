#include "Stdafx.h"
#include "common.h"
#include "ast_statement.h"
#include "codegen.h"

using namespace std;
using namespace CodeGen;

// Print Statement

PrintStatement::PrintStatement(Expression *_exp) {
    exp = _exp;
    stmt_type = StatementTypePrint;
}

SymbolInfo *PrintStatement::execute(Runtime_Context *ctx) {
    SymbolInfo *info = exp->evaluate(ctx);

    if (info->type == TYPE_STRING) {
        std::cout << info->string_val;
    } else if (info->type == TYPE_NUMERIC) {
        std::cout << info->double_val;
    } else if (info->type == TYPE_BOOL) {
        std::cout << info->bool_val;
    }

    return NULL;
}

PrintStatement::~PrintStatement() {
  delete exp;
}

Value* PrintStatement::codegen(Execution_Context *ctx) {

    Value *val = exp->codegen(ctx);
    TypeInfo type = exp->get_type();
    emit_print_stmt(val, type);
    return NULL;

}

SymbolInfo * PrintStatement::generate_js(Runtime_Context *ctx) {
    ctx->update_stream("print(");
    exp->generate_js(ctx);
    ctx->update_stream(");\n");
    return 0;
}

//PrintLine Statement

PrintLineStatement::PrintLineStatement(Expression *_exp) {
    exp = _exp;
    stmt_type = StatementTypePrintLine;
}

SymbolInfo * PrintLineStatement::execute(Runtime_Context *ctx) {
    SymbolInfo *info = exp->evaluate(ctx);
    if (info->type == TYPE_STRING) {
        std::cout << info->string_val << "\n";
    } else if (info->type == TYPE_NUMERIC) {
        std::cout << info->double_val << "\n";
    } else if (info->type == TYPE_BOOL) {
        std::cout << info->bool_val << "\n";
    }

    return NULL;
}

PrintLineStatement::~PrintLineStatement() {
  delete exp;
}

Value* PrintLineStatement::codegen(Execution_Context *ctx) {
    Value *val = exp->codegen(ctx);
    TypeInfo type = exp->get_type();
    emit_printline_stmt(val, type);
    return NULL;
}

SymbolInfo * PrintLineStatement::generate_js(Runtime_Context *ctx) {
    ctx->update_stream("print(");
    exp->generate_js(ctx);
    ctx->update_stream(");\n");
    return 0;
}

// Variable Declaration

VariableDeclStatement::VariableDeclStatement(SymbolInfo *_info) {
    info = _info;
    stmt_type = StatementTypeDeclaration;
}
SymbolInfo *VariableDeclStatement::execute(Runtime_Context *ctx) {
    ctx->add_symbol(info);
    var = new Variable(info);
    return NULL;
}

Value* VariableDeclStatement::codegen(Execution_Context *ctx) {
    string name = info->symbol_name;
    AllocaInst *alcInst = emit_stack_variable(info);
    ctx->add_symbol(info->symbol_name, alcInst);

    return NULL;
}

VariableDeclStatement::~VariableDeclStatement() {
  delete info;
  delete var;
}

SymbolInfo * VariableDeclStatement::generate_js(Runtime_Context *ctx) {
    ctx->update_stream("var ");
    ctx->update_stream(info->symbol_name);
    ctx->update_stream(";\n");
    return 0;
}


// Assignment Statement

AssignmentStatement::AssignmentStatement(Variable *_var, Expression *_exp) {
    var = _var;
    exp = _exp;
    stmt_type = StatementTypeAssignment;
}
AssignmentStatement::AssignmentStatement(SymbolInfo *info, Expression *_exp) {
    var = new Variable(info);
    exp = _exp;
    stmt_type = StatementTypeAssignment;
}

SymbolInfo *AssignmentStatement::execute(Runtime_Context *ctx) {
    SymbolInfo *info = exp->evaluate(ctx);
    ctx->assign_symbol(var, info);
    return NULL;
}

Value* AssignmentStatement::codegen(Execution_Context *ctx) {
    Value *v = exp->codegen(ctx);
    AllocaInst *alcInst = ctx->get_symbol(var->get_name());
    emit_store_Instruction(alcInst, v);
    return NULL;
}

AssignmentStatement::~AssignmentStatement() {
  delete exp;
  delete var;
}

SymbolInfo * AssignmentStatement::generate_js(Runtime_Context *ctx) {

    var->generate_js(ctx);
    ctx->update_stream(" = ");
    exp->generate_js(ctx);
    ctx->update_stream(";\n");
    return 0;
}

//If Statement

IfStatement::IfStatement(Expression *_exp, vector<Statement *> v1, vector<
                         Statement *> v2) {
    condition = _exp;
    if_statements = v1;
    else_statements = v2;
    stmt_type = StatementTypeIf;
}

SymbolInfo *IfStatement::execute(Runtime_Context *ctx) {
    SymbolInfo *info = condition->evaluate(ctx);

    SymbolInfo *ret = NULL;

    if (info->type == TYPE_BOOL) {

        vector<Statement *> *statements = &if_statements;
        if (info->bool_val == false) {
            statements = &else_statements;
        }

        for (int i = 0; i < (*statements).size(); ++i) {
            Statement *st = (*statements).at(i);
            ret = st->execute(ctx);
            if (ret != NULL) {
                return ret;
            }
        }

    }

    return NULL;
}

Value* IfStatement::codegen(Execution_Context *ctx) {
    Value *condV = condition->codegen(ctx);

    Value *thenV = ConstantFP::get(getGlobalContext(), APFloat(1.0));
    Value *elseV = ConstantFP::get(getGlobalContext(), APFloat(2.0));

    if (condV == NULL) {
        cout << "null";
        return 0;
    };

    // Create condition ,single bit integer is used since relational operators returns bool

    condV = builder.CreateICmpEQ(condV, ConstantInt::get(getGlobalContext(),
                                                         APInt(1, 1)), "ifcond");

    Function *TheFunction = builder.GetInsertBlock()->getParent();

    // create block for if , else and merge blocks

    BasicBlock *thenBB = BasicBlock::Create(getGlobalContext(), "then",
                                            TheFunction);
    BasicBlock *elseBB = BasicBlock::Create(getGlobalContext(), "else");
    BasicBlock *mergeBB = BasicBlock::Create(getGlobalContext(), "merge");

    builder.CreateCondBr(condV, thenBB, elseBB);

    builder.SetInsertPoint(thenBB);

    // emit code for all statements in if

    bool skip_branch;

    for (int i = 0; i < if_statements.size(); ++i) {

        Statement *st = if_statements.at(i);
        st->codegen(ctx);
        /// if current statement is 'return' , then no need to generate
        /// remaining code in the block

        if(st->stmt_type == StatementTypeReturn) {
            Procedure *p = ctx->current_procedure();
            BasicBlock *bb = ctx->get_proc_exitblock(p);
            builder.CreateBr(bb);
            skip_branch = true;
            break;
        }

    }

    if(!skip_branch) {
        builder.CreateBr(mergeBB);
    }
    thenBB = builder.GetInsertBlock();

    TheFunction->getBasicBlockList().push_back(elseBB);
    builder.SetInsertPoint(elseBB);
    skip_branch = false;
    // emit code for all statements in else

    for (int i = 0; i < else_statements.size(); ++i) {

        Statement *st = else_statements.at(i);
        st->codegen(ctx);
        /// if current statement is 'return' , then no need to generate
        /// remaining code in the block

        if(st->stmt_type == StatementTypeReturn) {
            Procedure *p = ctx->current_procedure();
            BasicBlock *bb = ctx->get_proc_exitblock(p);
            builder.CreateBr(bb);
            skip_branch = true;
            break;
        }
    }
    if(!skip_branch) {
        builder.CreateBr(mergeBB);
    }

    elseBB = builder.GetInsertBlock();

    // Emit merge block.

    TheFunction->getBasicBlockList().push_back(mergeBB);
    builder.SetInsertPoint(mergeBB);

    return NULL;

}

IfStatement::~IfStatement() {
  delete condition;
}

SymbolInfo * IfStatement::generate_js(Runtime_Context *ctx) {

    ctx->update_stream("if(");
    condition->generate_js(ctx);
    ctx->update_stream(")");
    ctx->update_stream(" {\n");

    for (int i = 0; i < if_statements.size(); ++i) {
        Statement *st = if_statements.at(i);
        st->generate_js(ctx);
    }

    ctx->update_stream(" }\n");

    if(else_statements.size() > 0) {
        ctx->update_stream("else {\n");
        for (int i = 0; i < else_statements.size(); ++i) {
            Statement *st = else_statements.at(i);
            st->generate_js(ctx);
        }
        ctx->update_stream(" }\n");
        
    }
    return 0;
    
}

// While Statement

WhileStatement::WhileStatement(Expression *_exp, vector<Statement *> v) {
    condition = _exp;
    statements = v;
    stmt_type = StatementTypeWhile;
}
SymbolInfo *WhileStatement::execute(Runtime_Context *ctx) {
    SymbolInfo *ret = NULL;
    SymbolInfo *info = condition->evaluate(ctx);

    while (info->type == TYPE_BOOL && info->bool_val == true) {

        for (int i = 0; i < statements.size(); ++i) {
            Statement *st = statements.at(i);
            st->execute(ctx);
            if (ret != NULL) {
                return ret;
            }
        }

        info = condition->evaluate(ctx);
    }

    return NULL;
}

Value* WhileStatement::codegen(Execution_Context *ctx) {

    //
    // Create blocks for loop header , body and exit .
    // make explicit branch to loop header , conditional branch to body and
    // another explicit branch from end of body to loop header
    //

    Value *thenV = ConstantFP::get(getGlobalContext(), APFloat(1.0));
    Value *elseV = ConstantFP::get(getGlobalContext(), APFloat(2.0));

    Function *TheFunction = builder.GetInsertBlock()->getParent();

    BasicBlock *loopBB = BasicBlock::Create(getGlobalContext(), "loop",
                                            TheFunction);
    BasicBlock *bodyBB = BasicBlock::Create(getGlobalContext(), "body",
                                            TheFunction);
    BasicBlock *exitBB = BasicBlock::Create(getGlobalContext(), "exit",
                                            TheFunction);
    builder.CreateBr(loopBB);

    // emit loop header

    builder.SetInsertPoint(loopBB);

    Value *condV = condition->codegen(ctx);
    condV = builder.CreateICmpEQ(condV, ConstantInt::get(getGlobalContext(),
                                                         APInt(1, 1)), "condition");

    builder.CreateCondBr(condV, bodyBB, exitBB);

    builder.SetInsertPoint(bodyBB);

    // emit code for loop body
    bool hasreturn = false;
    for (int i = 0; i < statements.size(); ++i) {

        Statement *st = statements.at(i);
        st->codegen(ctx);

        /// if current statement is 'return' , then no need to generate
        /// remaining code in the block

        if(st->stmt_type == StatementTypeReturn) {
            hasreturn = true;
            break;
        }
    }

    builder.CreateBr(loopBB); // back to loop header.
    builder.SetInsertPoint(exitBB);

    // emit exit block

    if(hasreturn) {

        Procedure *p = ctx->current_procedure();
        BasicBlock *bb = ctx->get_proc_exitblock(p);
        builder.CreateBr(bb);

    }

    return 0;  

}

WhileStatement::~WhileStatement() {
  
  delete condition;
}

SymbolInfo * WhileStatement::generate_js(Runtime_Context *ctx) {

    ctx->update_stream("while(");
    condition->generate_js(ctx);

    ctx->update_stream(")");
    ctx->update_stream(" {\n");

    for (int i = 0; i < statements.size(); ++i) {
        Statement *st = statements.at(i);
        st->generate_js(ctx);
    }

    ctx->update_stream(" }\n");
    return 0;

}

ReturnStatement::ReturnStatement(Expression *_exp) {
    stmt_type = StatementTypeReturn;
    exp = _exp;
}

SymbolInfo *ReturnStatement::execute(Runtime_Context *ctx) {
    info = exp->evaluate(ctx);
    return info;
}

Value* ReturnStatement::codegen(Execution_Context *ctx) {
    Value *val = exp->codegen(ctx);
    ctx->update_return_value(val);
    return val;
}

ReturnStatement::~ReturnStatement() {
  delete exp;
  delete info;
}

SymbolInfo * ReturnStatement::generate_js(Runtime_Context *ctx) {

    ctx->update_stream("return ");
    exp->generate_js(ctx);
    ctx->update_stream(";\n");
    return 0;
}

CallStatement::CallStatement(Expression *_exp) {
    exp = _exp;
    stmt_type = StatementTypeCall;
}

SymbolInfo *CallStatement::execute(Runtime_Context *ctx) {
    return exp->evaluate(ctx);
}

Value* CallStatement::codegen(Execution_Context *ctx) {
    return exp->codegen(ctx);
}

CallStatement::~CallStatement() {
  delete exp;
}

SymbolInfo * CallStatement::generate_js(Runtime_Context *ctx) {

    exp->generate_js(ctx);
    ctx->update_stream(";\n");
    return 0;
}

