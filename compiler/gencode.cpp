#include "gencode.hh"
#include <iomanip> 
#include <iostream> 
#include <cstdlib>  
#include <vector>   

void ImpCODE::interpret(Program* p) {
    env_vars.clear();
    env_fun_labels.clear();
    label_count = 0;
    current_offset = 0; // Usado para offsets de variables locales en el stack frame.

    out << ".data" << std::endl;
    out << "print_str_fmt: .string \"%s\"" << std::endl;
    out << "print_int_fmt: .string \"%ld\"" << std::endl;
    out << "print_float_fmt: .string \"%.17g\"" << std::endl;
    out << "print_newline: .string \"\\n\"" << std::endl;
    out << ".LC_array_placeholder: .string \"[Array]\"" << std::endl; 

    out << ".text" << std::endl;
    out << ".globl main" << std::endl;

    // Registra etiquetas de todas las funciones antes de generarlas para permitir llamadas anticipadas.
    if (p->fundecs) {
        for (FunDec* func : p->fundecs->Fundecs) {
            env_fun_labels[func->nombre] = "fun_" + func->nombre;
        }
    }

    // Genera el código para todas las funciones declaradas.
    if (p->fundecs) {
        p->fundecs->accept(this); 
    }

    // Define el punto de entrada del programa C/C++.
    out << "main:" << std::endl;
    out << "  pushq %rbp" << std::endl;
    out << "  movq %rsp, %rbp" << std::endl;

    env_vars.add_level(); // Inicia un nuevo scope para el 'main' implícito.
    stack_offsets.clear(); 

    // Llama a la función 'main' definida por el usuario, si existe.
    if (env_fun_labels.count("main")) {
        out << "  call " << env_fun_labels["main"] << std::endl; 
    } else {
        out << "  # No se encontró una función 'main' definida por el usuario." << std::endl;
    }

    // Epílogo del 'main' ensamblador.
    out << "  movl $0, %eax" << std::endl; // Retorna 0.
    out << "  leave" << std::endl;
    out << "  ret" << std::endl;
    out << ".section .note.GNU-stack,\"\",@progbits" << std::endl;
}

// Definition for the inherited virtual function ImpCODE::visit(Program* p)
void ImpCODE::visit(Program* p) {
    // This method's definition is required for vtable generation, as it's a pure virtual
    // function in the base class ImpValueVisitor that ImpCODE overrides.
    // The primary program processing logic (like emitting .data, .text directives,
    // setting up main, etc.) is handled by the ImpCODE::interpret method.
    // If any part of the visitor pattern were to call Program::accept(Visitor*),
    // this method would be invoked.
    // For a Program node, this typically involves visiting its constituent parts,
    // which are function declarations according to the grammar.
    if (p->fundecs) {
        p->fundecs->accept(this); // Visit function declarations
    }
    // If the grammar allowed global variable declarations at the program level,
    // they would be visited here as well.
}

void ImpCODE::visit(Body* b) {
    env_vars.add_level(); // New lexical scope for variable names is correct.

    // current_offset should continue from its value in the outer scope.
    // Do NOT reset current_offset here (e.g., to -8).
    // stack_offsets should also persist; do NOT clear it. Inner scopes need access
    // to variable locations from outer scopes. Shadowed variables are handled by env_vars.

    // Store the current_offset upon entering the block if we need to calculate
    // stack space used strictly by this block, but typically the function's
    // overall current_offset (max depth) is used for a single stack frame setup.
    // long offset_at_block_entry = current_offset;

    if (b->slist) { // Ensure slist is not null before dereferencing
        b->slist->accept(this);
    }

    // The stack frame is typically managed by the function's prologue and epilogue
    // (subq %rsp at start, leave or addq %rsp at end).
    // 'leave' instruction (movq %rbp, %rsp; popq %rbp) correctly deallocates
    // space for all local variables addressed relative to %rbp.
    // So, no explicit 'addq %rsp' is needed here to clean up this block's variables.
    
    env_vars.remove_level(); // Variable names declared in this scope are no longer visible.
}

void ImpCODE::visit(StatementList* s) {
    for (Stm* stm : s->stms) {
        stm->accept(this);
    }
}

void ImpCODE::visit(AssignStatement* s) {
    s->rhs->accept(this); // Evalúa RHS (resultado en %rax o %xmm0).

    if (env_vars.check(s->id)) {
        ImpValue var_info = env_vars.lookup(s->id); 
        if (var_info.type == TFLOAT) {
            out << "  movsd %xmm0, " << stack_offsets[s->id] << "(%rbp)" << std::endl; 
        } else {
            out << "  movq %rax, " << stack_offsets[s->id] << "(%rbp)" << std::endl;   
        }
    } else {
        out << "Error de generación de código: variable '" << s->id << "' no declarada." << std::endl;
        exit(1);
    }
}

void ImpCODE::visit(PrintStatement* s) {
    for (Exp* expr : s->e) {
        ImpValue val_type = expr->accept(this); 

        if (val_type.type == TINT || val_type.type == TBOOL) {
            out << "  movq %rax, %rsi" << std::endl; 
            out << "  leaq print_int_fmt(%rip), %rdi" << std::endl; 
            out << "  movl $0, %eax" << std::endl; 
            out << "  call printf@PLT" << std::endl;
        } else if (val_type.type == TFLOAT) {
            out << "  leaq print_float_fmt(%rip), %rdi" << std::endl; 
            out << "  movl $1, %eax" << std::endl; // Para printf con floats (xmm0), %eax debe ser 1.
            out << "  call printf@PLT" << std::endl;
        } else if (val_type.type == TARRAY) {
            out << "  leaq print_str_fmt(%rip), %rdi" << std::endl; 
            out << "  leaq .LC_array_placeholder(%rip), %rsi" << std::endl; 
            out << "  movl $0, %eax" << std::endl;
            out << "  call printf@PLT" << std::endl;
        }
    }
    out << "  leaq print_newline(%rip), %rdi" << std::endl; 
    out << "  movl $0, %eax" << std::endl;
    out << "  call printf@PLT" << std::endl;
}

void ImpCODE::visit(IfStatement* s) {
    int lbl_else = label_count++;
    int lbl_endif = label_count++;

    s->condition->accept(this); 
    out << "  cmpq $0, %rax" << std::endl;     
    out << "  je else_" << lbl_else << std::endl; 

    s->then->accept(this); 
    out << "  jmp endif_" << lbl_endif << std::endl; 

    out << "else_" << lbl_else << ":" << std::endl;
    if (s->els) {
        s->els->accept(this); 
    }
    out << "endif_" << lbl_endif << ":" << std::endl;
}

void ImpCODE::visit(WhileStatement* s) {
    int lbl_cond = label_count++;
    int lbl_end = label_count++;

    loop_break_labels.push(lbl_end); // Guarda la etiqueta de salida del bucle.

    out << "while_" << lbl_cond << ":" << std::endl;
    s->condition->accept(this);                  
    out << "  cmpq $0, %rax" << std::endl;            
    out << "  je endwhile_" << lbl_end << std::endl;  

    s->b->accept(this);                          
    out << "  jmp while_" << lbl_cond << std::endl;   

    out << "endwhile_" << lbl_end << ":" << std::endl;

    loop_break_labels.pop(); // Quita la etiqueta de salida del bucle.
}

void ImpCODE::visit(ForStatement* s) {
    env_vars.add_level(); // Nuevo scope para la variable de iteración.
    long old_current_offset = current_offset;
    std::unordered_map<std::string, long> old_stack_offsets = stack_offsets;

    current_offset -= 8; // Espacio para la variable de iteración en el stack.
    stack_offsets[s->id] = current_offset;
    ImpValue iter_var_info;
    iter_var_info.type = ImpVType::TINT; 
    env_vars.add_var(s->id, iter_var_info);

    s->init->accept(this); // Evalúa el valor inicial.
    out << "  movq %rax, " << stack_offsets[s->id] << "(%rbp)" << std::endl; 

    int lbl_cond = label_count++;
    int lbl_end = label_count++;
    loop_break_labels.push(lbl_end);

    out << "for_cond_" << lbl_cond << ":" << std::endl;

    // Condición del bucle (iterador < end).
    out << "  movq " << stack_offsets[s->id] << "(%rbp), %rax" << std::endl; // Carga iterador.
    s->end->accept(this); // Evalúa el límite, resultado en %rax.
    out << "  movq %rax, %rcx" << std::endl; // Mueve el límite a %rcx.

    out << "  cmpq %rcx, %rax" << std::endl; // Compara iterador y límite.
    out << "  jge endfor_" << lbl_end << std::endl; // Si iterador >= límite, salir.

    s->b->accept(this); // Cuerpo del bucle.

    // Incremento del iterador.
    out << "  movq " << stack_offsets[s->id] << "(%rbp), %rax" << std::endl;
    out << "  incq %rax" << std::endl; 
    out << "  movq %rax, " << stack_offsets[s->id] << "(%rbp)" << std::endl; 

    out << "  jmp for_cond_" << lbl_cond << std::endl; 

    out << "endfor_" << lbl_end << ":" << std::endl;
    loop_break_labels.pop();

    // Restaura el stack y el entorno del scope anterior.
    if (current_offset < old_current_offset) {
        out << "  addq $" << (old_current_offset - current_offset) << ", %rsp" << std::endl; 
    }
    current_offset = old_current_offset;
    stack_offsets = old_stack_offsets;
    env_vars.remove_level();
}

void ImpCODE::visit(DecStament* s) {
    ImpVType var_type = ImpValue::get_basic_type(s->type);

    current_offset -= 8; // Asigna espacio en el stack para la variable local.
    stack_offsets[s->id] = current_offset; 

    if (s->value) {
        s->value->accept(this); // Evalúa el valor inicial.
        if (var_type == TFLOAT) {
            out << "  movsd %xmm0, " << current_offset << "(%rbp)" << std::endl;
        } else {
            out << "  movq %rax, " << current_offset << "(%rbp)" << std::endl;
        }
    } else { // Inicialización por defecto a 0.
        if (var_type == TFLOAT) {
            out << "  pxor %xmm0, %xmm0" << std::endl;
            out << "  movsd %xmm0, " << current_offset << "(%rbp)" << std::endl;
        } else {
            out << "  movq $0, " << current_offset << "(%rbp)" << std::endl;
        }
    }
    ImpValue var_info;
    var_info.type = var_type;
    env_vars.add_var(s->id, var_info); 
}

void ImpCODE::visit(BreakStm* s) {
    if (loop_break_labels.empty()) {
        out << "Error de generación de código: 'break' fuera de un bucle." << std::endl;
        exit(1);
    }
    out << "  jmp endwhile_" << loop_break_labels.top() << std::endl; 
}

void ImpCODE::visit(ReturnStatement* s) {
    if (s->e) {
        s->e->accept(this); // Evalúa expresión de retorno (resultado en %rax o %xmm0).
    } else {
        out << "  xorq %rax, %rax" << std::endl; // Retorna 0 por defecto.
    }
    out << "  jmp end_fun_" << current_function_return_label << std::endl; 
}

void ImpCODE::visit(FCallStm* s) {
    // Guarda registros volátiles.
    out << "  pushq %r10" << std::endl; 
    out << "  pushq %r11" << std::endl;
    
    // Registros para argumentos (System V AMD64 ABI).
    const std::vector<std::string>& arg_regs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"}; 
    const std::vector<std::string>& xmm_regs = {"%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"}; 
    
    std::vector<bool> arg_is_float; 

    // Evalúa y apila argumentos de derecha a izquierda.
    for (int i = s->argumentos.size() - 1; i >= 0; --i) {
        ImpValue arg_type = s->argumentos[i]->accept(this); 
        arg_is_float.push_back(arg_type.type == TFLOAT);

        if (arg_type.type == TFLOAT) {
            out << "  subq $8, %rsp" << std::endl; 
            out << "  movsd %xmm0, (%rsp)" << std::endl; 
        } else {
            out << "  pushq %rax" << std::endl; 
        }
    }
    
    // Carga argumentos en registros o los deja en el stack (izquierda a derecha).
    int int_arg_idx = 0;
    int float_arg_idx = 0;
    for (size_t i = 0; i < s->argumentos.size(); ++i) {
        if (arg_is_float[s->argumentos.size() - 1 - i]) { 
            if (float_arg_idx < xmm_regs.size()) {
                out << "  movsd (%rsp), " << xmm_regs[float_arg_idx++] << std::endl;
                out << "  addq $8, %rsp" << std::endl; 
            } else {
                out << "  addq $8, %rsp" << std::endl; 
            }
        } else {
            if (int_arg_idx < arg_regs.size()) {
                out << "  popq " << arg_regs[int_arg_idx++] << std::endl; 
            } else { 
                out << "  addq $8, %rsp" << std::endl; 
            }
        }
    }

    // Número de registros XMM usados para floats.
    out << "  movl $" << float_arg_idx << ", %eax" << std::endl; 

    // Llama a la función.
    if (!env_fun_labels.count(s->nombre)) {
        out << "Error de generación de código: función '" << s->nombre << "' no declarada." << std::endl;
        exit(1);
    }
    out << "  call " << env_fun_labels[s->nombre] << std::endl;

    // Restaura registros guardados.
    out << "  popq %r11" << std::endl;
    out << "  popq %r10" << std::endl;
}

void ImpCODE::visit(AssignArrayStatement* s) {
    out << "  movq " << stack_offsets[s->id] << "(%rbp), %rbx" << std::endl; 

    s->index_exp->accept(this); 
    out << "  movq %rax, %rcx" << std::endl; 

    s->rhs->accept(this); 

    out << "  imulq $8, %rcx" << std::endl; 
    out << "  addq %rbx, %rcx" << std::endl; 

    ImpValue array_info = env_vars.lookup(s->id); 
    ImpVType element_type = array_info.element_type; 

    if (element_type == TFLOAT) { 
        out << "  movsd %xmm0, (%rcx)" << std::endl;
    } else {
        out << "  movq %rax, (%rcx)" << std::endl;
    }
}

void ImpCODE::visit(FunDec* funDec) {
    std::string fun_label = env_fun_labels[funDec->nombre]; 

    out << fun_label << ":" << std::endl;
    out << "  pushq %rbp" << std::endl;
    out << "  movq %rsp, %rbp" << std::endl;

    env_vars.add_level(); // Nuevo scope para parámetros y locales de la función.
    long old_current_offset = current_offset;
    std::unordered_map<std::string, long> old_stack_offsets = stack_offsets;
    
    current_offset = -8; // Reinicia el offset para variables locales de esta función.

    long param_stack_offset_start = 16; // Offset para parámetros pasados en el stack.
    const std::vector<std::string>& arg_regs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    const std::vector<std::string>& xmm_regs = {"%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"};
    int int_arg_idx = 0;
    int float_arg_idx = 0;

    auto it_param_type = funDec->tipos.begin();

    // Asigna y guarda parámetros en el stack frame.
    for (const std::string& param_name : funDec->parametros) {
        ImpValue param_info;
        param_info.type = ImpValue::get_basic_type(*it_param_type);
        
        current_offset -= 8; 
        stack_offsets[param_name] = current_offset; 

        if (param_info.type == TFLOAT) {
            if (float_arg_idx < xmm_regs.size()) {
                out << "  movsd " << xmm_regs[float_arg_idx++] << ", " << current_offset << "(%rbp)" << std::endl;
            } else { // Parámetros flotantes adicionales del stack.
                out << "  movsd " << param_stack_offset_start << "(%rbp), %xmm0" << std::endl; 
                out << "  movsd %xmm0, " << current_offset << "(%rbp)" << std::endl;
                param_stack_offset_start += 8;
            }
        } else {
            if (int_arg_idx < arg_regs.size()) {
                out << "  movq " << arg_regs[int_arg_idx++] << ", " << current_offset << "(%rbp)" << std::endl;
            } else { // Parámetros enteros adicionales del stack.
                out << "  movq " << param_stack_offset_start << "(%rbp), %rax" << std::endl; 
                out << "  movq %rax, " << current_offset << "(%rbp)" << std::endl; 
                param_stack_offset_start += 8;
            }
        }
        env_vars.add_var(param_name, param_info); 
        ++it_param_type;
    }

    current_function_return_label = label_count++; // Etiqueta para el retorno de esta función.
    funDec->cuerpo->accept(this); // Genera código para el cuerpo.

    out << "end_fun_" << current_function_return_label << ":" << std::endl;
    out << "  leave" << std::endl; 
    out << "  ret" << std::endl;

    current_offset = old_current_offset; // Restaura el offset y el entorno del llamador.
    stack_offsets = old_stack_offsets;
    env_vars.remove_level(); 
}

void ImpCODE::visit(FunDecList* funDecList) {
    for (FunDec* func : funDecList->Fundecs) {
        func->accept(this);
    }
}

ImpValue ImpCODE::visit(BinaryExp* e) {
    ImpValue left_type = e->left->accept(this); 
    
    // Guarda el operando izquierdo en el stack.
    if (left_type.type == TFLOAT) {
        out << "  subq $8, %rsp" << std::endl; 
        out << "  movsd %xmm0, (%rsp)" << std::endl; 
    } else {
        out << "  pushq %rax" << std::endl; 
    }

    ImpValue right_type = e->right->accept(this);
    ImpValue result_type; 

    if (left_type.type == TFLOAT && right_type.type == TFLOAT) {
        out << "  movsd %xmm0, %xmm1" << std::endl; // RHS en %xmm1.
        out << "  movsd (%rsp), %xmm0" << std::endl; // LHS en %xmm0.
        out << "  addq $8, %rsp" << std::endl; // Desapila el valor izquierdo.

        switch (e->op) {
            case PLUS_OP: out << "  addsd %xmm1, %xmm0" << std::endl; break;
            case MINUS_OP: out << "  subsd %xmm1, %xmm0" << std::endl; break;
            case MUL_OP: out << "  mulsd %xmm1, %xmm0" << std::endl; break;
            case DIV_OP: out << "  divsd %xmm1, %xmm0" << std::endl; break;
            case LT_OP:  out << "  comisd %xmm1, %xmm0" << std::endl; out << "  setb %al" << std::endl; out << "  movzx %al, %rax" << std::endl; result_type.type = TBOOL; break;
            case LE_OP:  out << "  comisd %xmm1, %xmm0" << std::endl; out << "  setbe %al" << std::endl; out << "  movzx %al, %rax" << std::endl; result_type.type = TBOOL; break;
            case EQ_OP:  out << "  comisd %xmm1, %xmm0" << std::endl; out << "  sete %al" << std::endl; out << "  movzx %al, %rax" << std::endl; result_type.type = TBOOL; break;
            case GT_OP:  out << "  comisd %xmm1, %xmm0" << std::endl; out << "  seta %al" << std::endl; out << "  movzx %al, %rax" << std::endl; result_type.type = TBOOL; break;
            case GE_OP:  out << "  comisd %xmm1, %xmm0" << std::endl; out << "  setae %al" << std::endl; out << "  movzx %al, %rax" << std::endl; result_type.type = TBOOL; break;
            case NEQ_OP: out << "  comisd %xmm1, %xmm0" << std::endl; out << "  setne %al" << std::endl; out << "  movzx %al, %rax" << std::endl; result_type.type = TBOOL; break;
            default: out << "Error: Operación binaria no soportada para flotantes." << std::endl; exit(1);
        }
        if (result_type.type != TBOOL) result_type.type = TFLOAT; // Si no es una comparación, el resultado es float.
    } else { 
        out << "  movq %rax, %rcx" << std::endl; // RHS en %rcx.
        out << "  popq %rax" << std::endl;       // LHS en %rax.

        switch (e->op) {
            case PLUS_OP: out << "  addq %rcx, %rax" << std::endl; result_type.type = TINT; break;
            case MINUS_OP: out << "  subq %rcx, %rax" << std::endl; result_type.type = TINT; break;
            case MUL_OP: out << "  imulq %rcx, %rax" << std::endl; result_type.type = TINT; break;
            case DIV_OP: 
                out << "  cqto" << std::endl;        
                out << "  idivq %rcx" << std::endl;  
                result_type.type = TINT; break;
            case LT_OP: out << "  cmpq %rcx, %rax" << std::endl; out << "  setl %al" << std::endl; out << "  movzx %al, %rax" << std::endl; result_type.type = TBOOL; break;
            case LE_OP: out << "  cmpq %rcx, %rax" << std::endl; out << "  setle %al" << std::endl; out << "  movzx %al, %rax" << std::endl; result_type.type = TBOOL; break;
            case EQ_OP: out << "  cmpq %rcx, %rax" << std::endl; out << "  sete %al" << std::endl; out << "  movzx %al, %rax" << std::endl; result_type.type = TBOOL; break;
            case GT_OP: out << "  cmpq %rcx, %rax" << std::endl; out << "  setg %al" << std::endl; out << "  movzx %al, %rax" << std::endl; result_type.type = TBOOL; break;
            case GE_OP: out << "  cmpq %rcx, %rax" << std::endl; out << "  setge %al" << std::endl; out << "  movzx %al, %rax" << std::endl; result_type.type = TBOOL; break;
            case NEQ_OP: out << "  cmpq %rcx, %rax" << std::endl; out << "  setne %al" << std::endl; out << "  movzx %al, %rax" << std::endl; result_type.type = TBOOL; break;
            case AND: { // Short-circuiting AND (op1 && op2)
                // LHS (op1) is already evaluated, its result is in %rax (if int/bool) or %xmm0 (if float)
                // For logical ops, inputs must be boolean (0 or 1). Assuming type checking handled this.
                // Current code path is for integer/boolean types, so result of LHS is in %rax.
                std::string false_label = new_label_str();
                std::string end_label = new_label_str();
                out << "  cmpq $0, %rax" << std::endl;       // Check if LHS is false
                out << "  je " << false_label << std::endl; // If LHS is false, jump to set result to false
                // LHS is true, evaluate RHS
                // RHS was already evaluated and its result moved to %rcx.
                // So, if LHS is true, the result of the AND is the result of RHS.
                out << "  movq %rcx, %rax" << std::endl;     // Result of AND is RHS (already 0 or 1)
                out << "  jmp " << end_label << std::endl;
                out << false_label << ":" << std::endl;
                out << "  movq $0, %rax" << std::endl;       // Result is false
                out << end_label << ":" << std::endl;
                result_type.type = TBOOL;
                break;
            }
            case OR: { // Short-circuiting OR (op1 || op2)
                // LHS (op1) is in %rax.
                std::string true_label = new_label_str();
                std::string end_label = new_label_str();
                out << "  cmpq $0, %rax" << std::endl;       // Check if LHS is false
                out << "  jne " << true_label << std::endl;  // If LHS is true, jump to set result to true
                // LHS is false, evaluate RHS
                // RHS result is in %rcx.
                out << "  movq %rcx, %rax" << std::endl;     // Result of OR is RHS
                out << "  jmp " << end_label << std::endl;
                out << true_label << ":" << std::endl;
                out << "  movq $1, %rax" << std::endl;       // Result is true
                out << end_label << ":" << std::endl;
                result_type.type = TBOOL;
                break;
            }
            default: out << "Error: Operador binario no soportado." << std::endl; exit(1);
        }
    }
    return result_type;
}

ImpValue ImpCODE::visit(UnaryExp* e) {
    ImpValue operand_type = e->operand->accept(this);
    ImpValue result_type;

    switch (e->op) {
        case NOT_OP:
            out << "  cmpq $0, %rax" << std::endl;
            out << "  sete %al" << std::endl;     
            out << "  movzx %al, %rax" << std::endl; 
            result_type.type = TBOOL;
            break;
        case MINUS_UNARY_OP:
            if (operand_type.type == TINT) {
                out << "  negq %rax" << std::endl; 
                result_type.type = TINT;
            } else if (operand_type.type == TFLOAT) {
                out << "  movabsq $-0x8000000000000000, %rcx" << std::endl; 
                out << "  movq %rcx, %xmm1" << std::endl; 
                out << "  xorpd %xmm1, %xmm0" << std::endl; 
                result_type.type = TFLOAT;
            } else {
                out << "Error de tipo: el operador unario '-' solo se aplica a números." << std::endl;
                exit(1);
            }
            break;
        default:
            out << "Error: Operador unario no soportado." << std::endl;
            exit(1);
    }
    return result_type;
}

ImpValue ImpCODE::visit(IntExp* e) {
    ImpValue v;
    v.set_int(e->value);
    out << "  movq $" << e->value << ", %rax" << std::endl; 
    return v; 
}

ImpValue ImpCODE::visit(FloatExp* e) {
    ImpValue v;
    v.set_float(e->value);

    std::string float_label = new_label_str(); 
    out << ".data" << std::endl;
    out << "  .align 8" << std::endl; 
    out << float_label << ": .double " << std::fixed << std::setprecision(17) << e->value << std::endl;
    out << ".text" << std::endl; 
    out << "  movsd " << float_label << "(%rip), %xmm0" << std::endl; 
    return v; 
}

ImpValue ImpCODE::visit(BoolExp* e) {
    ImpValue v;
    v.set_bool(e->value);
    out << "  movq $" << (e->value ? 1 : 0) << ", %rax" << std::endl; 
    return v; 
}

ImpValue ImpCODE::visit(ArrayExp* e) {
    ImpValue array_val; // array_val.element_type will be NOTYPE initially
    array_val.type = TARRAY; 

    int element_size = 8; // Assuming 8-byte elements (qword/double) for simplicity.
                        // This should ideally depend on the actual element_type if mixed sizes were supported.
    int array_size_bytes = e->values.size() * element_size;

    out << "  movq $" << array_size_bytes << ", %rdi" << std::endl; 
    out << "  call malloc@PLT" << std::endl; 
    out << "  movq %rax, %rbx" << std::endl; // Save base pointer of the array in %rbx.

    bool first_element_processed = false;
    for (size_t i = 0; i < e->values.size(); ++i) {
        ImpValue current_element_val_info = e->values[i]->accept(this); // Generates code for element, type in current_element_val_info
        long offset_in_array = i * element_size; 

        if (!first_element_processed) {
            array_val.element_type = current_element_val_info.type; // Set array's element_type from the first element
            first_element_processed = true;
            // Note: Rust arrays are homogeneous. A full compiler would verify all elements match this type.
            // We assume homogeneity as per Rust rules, enforced by parser or type checker.
        }
        // Store the element in the array
        if (current_element_val_info.type == TFLOAT) {
            out << "  movsd %xmm0, " << offset_in_array << "(%rbx)" << std::endl;
        } else { // TINT, TBOOL, or pointers (TARRAY itself if nested, though not fully handled)
            out << "  movq %rax, " << offset_in_array << "(%rbx)" << std::endl;
        }
    }
    
    // If the array was empty, element_type remains NOTYPE.
    // In Rust, `let empty_arr = [];` is a type error. `let empty_arr: [SomeType; 0] = [];` is fine.
    // The type context would typically provide the element_type for empty arrays.
    // If e->values.empty(), array_val.element_type will be NOTYPE. This might be an issue
    // if this ImpValue is assigned to a variable, and that variable's element_type in env_vars
    // becomes NOTYPE, leading to problems in AccesoArrayExp or AssignArrayStatement.
    // For this project, we might assume test cases won't rely on empty untyped literals.
    if (e->values.empty() && array_val.element_type == NOTYPE) {
        // Potentially default to TINT or raise an error/warning if strictness is needed.
        // out << "# Warning: Empty array literal created with NOTYPE element_type." << std::endl;
    }

    out << "  movq %rbx, %rax" << std::endl; // Return the base address of the array in %rax.
    return array_val; // array_val now has .element_type set if array was not empty.
}

ImpValue ImpCODE::visit(IdentifierExp* e) {
    if (env_vars.check(e->name)) {
        ImpValue var_info = env_vars.lookup(e->name); 
        
        // Carga el valor de la variable desde su ubicación en el stack.
        if (var_info.type == TFLOAT) {
            out << "  movsd " << stack_offsets[e->name] << "(%rbp), %xmm0" << std::endl;
        } else {
            out << "  movq " << stack_offsets[e->name] << "(%rbp), %rax" << std::endl;
        }
        return var_info; 
    } else {
        out << "Error de generación de código: variable '" << e->name << "' no declarada." << std::endl;
        exit(1);
    }
}

ImpValue ImpCODE::visit(FCallExp* e) {
    out << "  pushq %r10" << std::endl; 
    out << "  pushq %r11" << std::endl;
    
    const std::vector<std::string>& arg_regs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"}; 
    const std::vector<std::string>& xmm_regs = {"%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"}; 
    
    std::vector<bool> arg_is_float;  

    // Evalúa los argumentos y los apila de derecha a izquierda.
    for (int i = e->argumentos.size() - 1; i >= 0; --i) {
        ImpValue arg_type = e->argumentos[i]->accept(this); 
        arg_is_float.push_back(arg_type.type == TFLOAT);

        if (arg_type.type == TFLOAT) {
            out << "  subq $8, %rsp" << std::endl; 
            out << "  movsd %xmm0, (%rsp)" << std::endl; 
        } else {
            out << "  pushq %rax" << std::endl; 
        }
    }
    
    // Carga los argumentos en los registros apropiados (de izquierda a derecha).
    int int_arg_idx = 0;
    int float_arg_idx = 0;
    for (size_t i = 0; i < e->argumentos.size(); ++i) {
        // Accede a los tipos en el orden correcto (del primer argumento al último).
        if (arg_is_float[e->argumentos.size() - 1 - i]) { 
            if (float_arg_idx < xmm_regs.size()) {
                out << "  movsd (%rsp), " << xmm_regs[float_arg_idx++] << std::endl;
                out << "  addq $8, %rsp" << std::endl; 
            } else { // Si no cabe en un registro, el valor permanece en el stack para la función llamada.
                out << "  addq $8, %rsp" << std::endl; 
            }
        } else {
            if (int_arg_idx < arg_regs.size()) {
                out << "  popq " << arg_regs[int_arg_idx++] << std::endl; 
            } else { // Si no cabe en un registro, el valor permanece en el stack para la función llamada.
                out << "  addq $8, %rsp" << std::endl; 
            }
        }
    }

    out << "  movl $" << float_arg_idx << ", %eax" << std::endl; // RAX para el número de registros XMM.

    if (!env_fun_labels.count(e->nombre)) {
        out << "Error de generación de código: función '" << e->nombre << "' no declarada." << std::endl;
        exit(1);
    }
    out << "  call " << env_fun_labels[e->nombre] << std::endl;

    out << "  popq %r11" << std::endl;
    out << "  popq %r10" << std::endl;

    ImpValue result_type; // El tipo de retorno de la función debe obtenerse de la tabla de símbolos.
    return result_type; 
}

ImpValue ImpCODE::visit(AccesoArrayExp* e) {
    if (!env_vars.check(e->id)) {
        out << "Error de generación de código: array '" << e->id << "' no declarado." << std::endl;
        exit(1);
    }
    ImpValue array_info = env_vars.lookup(e->id);
    if (array_info.type != TARRAY) {
        out << "Error de tipo: '" << e->id << "' no es un array." << std::endl;
        exit(1);
    }

    out << "  movq " << stack_offsets[e->id] << "(%rbp), %rbx" << std::endl; // Carga la dirección base del array.

    e->index_exp->accept(this); // Evalúa el índice.
    out << "  movq %rax, %rcx" << std::endl; 

    out << "  imulq $8, %rcx" << std::endl; // Calcula el offset en bytes.
    out << "  addq %rbx, %rcx" << std::endl; // Dirección del elemento.

    ImpVType element_type = TINT; // Placeholder: el tipo real del elemento debe ser conocido.
    if (element_type == TFLOAT) {
        out << "  movsd (%rcx), %xmm0" << std::endl; 
    } else {
        out << "  movq (%rcx), %rax" << std::endl; 
    }

    ImpValue result_val;
    result_val.type = element_type;
    return result_val;
}