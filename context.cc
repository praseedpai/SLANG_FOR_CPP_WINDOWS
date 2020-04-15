#include "Stdafx.h"
#include "context.h"
#include "compilation_unit.h"

// Runtime Context

Runtime_Context::Runtime_Context() {
	st = new SymbolInfoTable();
}
;

Runtime_Context::Runtime_Context(Tmodule *mod) {
	prog = mod;
	st = new SymbolInfoTable();
}
Tmodule *Runtime_Context::get_program() {
	return prog;
}

SymbolInfoTable *Runtime_Context::get_symboltable() {
	return st;
}

void Runtime_Context::set_symboltable(SymbolInfoTable *_st) {
	st = _st;
}
void Runtime_Context::assign_symbol(Variable *var, SymbolInfo *value) {
	st->assign(var, value);
}

void Runtime_Context::add_symbol(SymbolInfo *info) {
	st->add(info);
}

SymbolInfo * Runtime_Context::get_symbol(string name) {
	return st->get(name);
}
void Runtime_Context::setup_js_codegen(FILE *fd) {
    js_fd = fd;
}

void Runtime_Context::update_stream(string str) {
    fwrite(str.c_str(),1,str.length(),js_fd);    
}

Runtime_Context::~Runtime_Context() {
  
}
// Compilation Context

Compilation_Context::Compilation_Context() {
	st = new SymbolInfoTable();
}
SymbolInfoTable * Compilation_Context::get_symboltable() {
	return st;
}

void Compilation_Context::assign_symbol(Variable *var, SymbolInfo *value) {
	st->assign(var, value);
}

void Compilation_Context::set_symboltable(SymbolInfoTable *_st) {
	st = _st;
}

void Compilation_Context::add_symbol(SymbolInfo *info) {
	st->add(info);
}

SymbolInfo * Compilation_Context::get_symbol(string name) {
	return st->get(name);
}


Compilation_Context::~Compilation_Context() {
}
//Execution context

Execution_Context::Execution_Context() {
	st = new AllocaInstTable();
}
AllocaInstTable *Execution_Context::get_symboltable() {
	return st;
}
void Execution_Context::set_symboltable(AllocaInstTable *_st) {
	st = _st;
}
void Execution_Context::add_symbol(string key, AllocaInst *info) {
	st->add(key, info);
}
void Execution_Context::assign_symbol(Variable *var, AllocaInst *value) {
	st->assign(var, value);
}
AllocaInst *Execution_Context::get_symbol(string name) {
	return st->get(name);
}
void Execution_Context::add_procedure(string name, Function *func)
{
   string name1 = name;
   std::transform(name1.begin(), name1.end(), name1.begin(), ::tolower);
   function_table.insert(std::make_pair(name, func));
}

Function *Execution_Context::get_procedure(string name)
{
   string name1 = name;
   std::transform(name1.begin(), name1.end(), name1.begin(), ::tolower);
   map<string, Function *>::iterator it =  function_table.find(name1);
   return it->second;
}

void Execution_Context::set_current_procedure(Procedure *_proc)
{
   //clear  symbol table
   st->clear();

   //set new proc
   current_proc = _proc;
}

Procedure *Execution_Context::current_procedure()
{
   return current_proc;
}

void Execution_Context::update_return_value(Value * val)
{
   current_proc->update_return_value(val);
}

BasicBlock * Execution_Context::get_proc_exitblock(Procedure *proc)
{
   return proc->exitblock();
}
Execution_Context::~Execution_Context() {
}
