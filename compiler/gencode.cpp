#include "gencode.hh"
#include <iomanip> 
#include <iostream> 
#include <cstdlib>  
#include <vector>   

// Helper function to get type size in bytes
static int get_type_size(ImpVType type) {
    switch (type) {
        case TI32:
        case TF32:
            return 4;
        case TINT:   // i64
        case TFLOAT: // f64
        // Bools are often stored as 1 byte but aligned, using 8 for simplicity here
        // or matching register size if moved directly. Let's assume 8 for now like i64.
        case TBOOL:
        case TARRAY: // Stores a pointer (typically 8 bytes on x86-64)
            return 8;
        case TVOID:
            return 0;
        default:
            // Should not happen for valid variable types
            std::cerr << "Error: Unknown type size for ImpVType: " << type << std::endl;
            exit(1);
            return 8;
    }
}

void ImpCODE::interpret(Program* p) {
    env_vars.clear();
    env_fun_labels.clear();
    label_count = 0;
    current_offset = 0; // Usado para offsets de variables locales en el stack frame.

    out << ".data" << std::endl;
    out << "print_str_fmt: .string \"%s\"" << std::endl;
    out << "print_int_fmt: .string \"%ld\"" << std::endl; // For i64
    out << "print_i32_fmt: .string \"%d\"" << std::endl;  // For i32
    out << "print_float_fmt: .string \"%.17g\"" << std::endl; // For f64
    out << "print_f32_fmt: .string \"%.7g\"" << std::endl;   // For f32
    out << "print_newline: .string \"\\n\"" << std::endl;
    out << ".LC_array_placeholder: .string \"[Array]\"" << std::endl; 

    out << ".text" << std::endl;
    out << ".globl main" << std::endl;

    if (p->fundecs) {
        for (FunDec* func : p->fundecs->Fundecs) {
            env_fun_labels[func->nombre] = "fun_" + func->nombre;
        }
    }

    if (p->fundecs) {
        p->fundecs->accept(this); 
    }

    out << "main:" << std::endl;
    out << "  pushq %rbp" << std::endl;
    out << "  movq %rsp, %rbp" << std::endl;

    env_vars.add_level();
    stack_offsets.clear(); 

    if (env_fun_labels.count("main")) {
        out << "  call " << env_fun_labels["main"] << std::endl; 
    } else {
        out << "  # No se encontró una función 'main' definida por el usuario." << std::endl;
    }

    out << "  movl $0, %eax" << std::endl;
    out << "  leave" << std::endl;
    out << "  ret" << std::endl;
    out << ".section .note.GNU-stack,\"\",@progbits" << std::endl;
}

void ImpCODE::visit(Program* p) {
    if (p->fundecs) {
        p->fundecs->accept(this);
    }
}

void ImpCODE::visit(Body* b) {
    env_vars.add_level();
    if (b->slist) {
        b->slist->accept(this);
    }
    env_vars.remove_level();
}

void ImpCODE::visit(StatementList* s) {
    for (Stm* stm : s->stms) {
        stm->accept(this);
    }
}

void ImpCODE::visit(AssignStatement* s) {
    ImpValue rhs_val_info = s->rhs->accept(this); // RHS result in %rax/%eax or %xmm0

    if (env_vars.check(s->id)) {
        ImpValue var_info = env_vars.lookup(s->id); 
        switch (var_info.type) {
            case TINT: case TBOOL: case TARRAY:
                out << "  movq %rax, " << stack_offsets[s->id] << "(%rbp)" << std::endl;
                break;
            case TI32:
                out << "  movl %eax, " << stack_offsets[s->id] << "(%rbp)" << std::endl;
                break;
            case TFLOAT:
                out << "  movsd %xmm0, " << stack_offsets[s->id] << "(%rbp)" << std::endl;
                break;
            case TF32:
                out << "  movss %xmm0, " << stack_offsets[s->id] << "(%rbp)" << std::endl;
                break;
            default:
                out << "Error: Asignación no soportada para el tipo de variable " << s->id << std::endl;
                exit(1);
        }
    } else {
        out << "Error de generación de código: variable '" << s->id << "' no declarada." << std::endl;
        exit(1);
    }
}

void ImpCODE::visit(PrintStatement* s) {
    // Current version prints one expression per line.
    // Rust's println! is more complex with format strings.
    // This simplified version will print each expression on its own line.
    for (Exp* expr : s->e) {
        ImpValue val_info = expr->accept(this);

        switch(val_info.type) {
            case TINT: // i64
            case TBOOL: // Bools are printed as integers 0 or 1
                out << "  movq %rax, %rsi" << std::endl;
                out << "  leaq print_int_fmt(%rip), %rdi" << std::endl;
                out << "  movl $0, %eax" << std::endl; // No vector args
                out << "  call printf@PLT" << std::endl;
                break;
            case TI32:
                out << "  movslq %eax, %rsi" << std::endl; // Sign-extend i32 to i64 for printf %d/%ld
                out << "  leaq print_i32_fmt(%rip), %rdi" << std::endl;
                out << "  movl $0, %eax" << std::endl;
                out << "  call printf@PLT" << std::endl;
                break;
            case TFLOAT: // f64
                out << "  leaq print_float_fmt(%rip), %rdi" << std::endl;
                out << "  movl $1, %eax" << std::endl; // One vector arg in %xmm0
                out << "  call printf@PLT" << std::endl;
                break;
            case TF32: // f32
                 // Promote f32 to f64 for printf
                out << "  cvtss2sd %xmm0, %xmm0" << std::endl;
                out << "  leaq print_f32_fmt(%rip), %rdi" << std::endl;
                out << "  movl $1, %eax" << std::endl;
                out << "  call printf@PLT" << std::endl;
                break;
            case TARRAY:
                out << "  leaq print_str_fmt(%rip), %rdi" << std::endl;
                out << "  leaq .LC_array_placeholder(%rip), %rsi" << std::endl;
                out << "  movl $0, %eax" << std::endl;
                out << "  call printf@PLT" << std::endl;
                break;
            default:
                 out << "Error: Print no soportado para el tipo " << val_info.type << std::endl;
                 // No exit(1) here to allow other prints to proceed if possible.
                 break;
        }
        // Print a newline after each argument for this simplified version
        out << "  leaq print_newline(%rip), %rdi" << std::endl;
        out << "  movl $0, %eax" << std::endl;
        out << "  call printf@PLT" << std::endl;
    }
}


void ImpCODE::visit(IfStatement* s) {
    int lbl_else = label_count++;
    int lbl_endif = label_count++;

    s->condition->accept(this); // Result of condition in %eax (0 or 1 for bool)
    out << "  cmpl $0, %eax" << std::endl; // Compare with 0
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
    loop_break_labels.push(lbl_end);

    out << "while_" << lbl_cond << ":" << std::endl;
    s->condition->accept(this); // Result of condition in %eax (0 or 1 for bool)
    out << "  cmpl $0, %eax" << std::endl;
    out << "  je endwhile_" << lbl_end << std::endl;  

    s->b->accept(this);                          
    out << "  jmp while_" << lbl_cond << std::endl;   

    out << "endwhile_" << lbl_end << ":" << std::endl;
    loop_break_labels.pop();
}

void ImpCODE::visit(ForStatement* s) {
    env_vars.add_level();

    // Iterator variable (assuming i64 for now, like current FOR loops)
    current_offset -= 8;
    stack_offsets[s->id] = current_offset;
    ImpValue iter_var_info; iter_var_info.type = TINT;
    env_vars.add_var(s->id, iter_var_info);

    s->init->accept(this);
    out << "  movq %rax, " << stack_offsets[s->id] << "(%rbp)" << std::endl; 

    int lbl_cond = label_count++;
    int lbl_end = label_count++;
    loop_break_labels.push(lbl_end);

    out << "for_cond_" << lbl_cond << ":" << std::endl;
    out << "  movq " << stack_offsets[s->id] << "(%rbp), %rax" << std::endl;
    ImpValue end_val_info = s->end->accept(this);
    // Assuming end expression is also i64
    out << "  movq %rax, %rcx" << std::endl;
    out << "  cmpq %rcx, %rax" << std::endl; // iter < end
    out << "  jge endfor_" << lbl_end << std::endl;

    s->b->accept(this);

    out << "  movq " << stack_offsets[s->id] << "(%rbp), %rax" << std::endl;
    out << "  incq %rax" << std::endl; 
    out << "  movq %rax, " << stack_offsets[s->id] << "(%rbp)" << std::endl; 
    out << "  jmp for_cond_" << lbl_cond << std::endl; 

    out << "endfor_" << lbl_end << ":" << std::endl;
    loop_break_labels.pop();

    // Restore current_offset if it was changed *only* for this variable,
    // but current_offset is function-wide. The slot remains "used".
    env_vars.remove_level();
}

void ImpCODE::visit(DecStament* s) {
    ImpVType var_type = NOTYPE;
    ImpValue var_info_for_env; // Stores type and potentially element_type for arrays

    if (!s->type.empty()) {
        var_type = ImpValue::get_basic_type(s->type);
        // This is where explicit array type like `let x: [i32; 5]` needs more info from parser.
        // For example, if parser indicates this is an array declaration via a flag on DecStament node:
        // if (s->is_explicit_array_declaration) { // hypothetical flag
        //    var_info_for_env.element_type = var_type; // var_type from s->type is element type
        //    var_type = TARRAY; // The variable itself is an array
        //    // Size calculation for stack array would use s->array_size_expr here.
        //    // This is currently a GAP for stack-allocated arrays: `let x: [TYPE; SIZE];`
        // }
    }

    if (s->value) { // Variable has an initializer, e.g., `let x = 10;` or `let x: i32 = 10;`
        ImpValue val_info = s->value->accept(this); // Evaluate initializer

        if (var_type == NOTYPE) { // Type is inferred from initializer
            var_type = val_info.type;
            if (var_type == TARRAY) {
                var_info_for_env.element_type = val_info.element_type;
            }
        } else { // Explicit type was given (var_type is set from s->type)
            // If the explicit type was for an array's element (e.g. s->type="i32" for `let x: [i32;N]`),
            // we need to adjust var_type to TARRAY and store element_type.
            // This depends on how parser populates DecStament for array declarations.
            // Assuming for now if s->value is an array, and var_type is not TARRAY, it's an element type.
            if (val_info.type == TARRAY && var_type != TARRAY) { // e.g. let x: i32 = [1,2]; (error) or let x: i32[] (if syntax allowed)
                 out << "# Advertencia: Declaración de tipo simple " << s->id << ":" << s->type
                     << " inicializada con un array. Se asume tipo array." << std::endl;
                 var_info_for_env.element_type = var_type; // Original s->type becomes element_type
                 var_type = TARRAY; // Actual type is array
                 if (var_info_for_env.element_type != val_info.element_type && val_info.element_type != NOTYPE) {
                     out << "# Advertencia: Tipo de elemento de literal de array (" << val_info.element_type
                         << ") difiere del tipo de elemento declarado (" << var_info_for_env.element_type
                         << ") para " << s->id << std::endl;
                 }
            } else if (var_type == TARRAY) { // Explicitly `let x: type[] = ...` (hypothetical full array type)
                 var_info_for_env.element_type = ImpValue::get_basic_type(s->type); // Assuming s->type is element for explicit TARRAY
                 if (val_info.type != TARRAY) { /* error */ }
                 else if (var_info_for_env.element_type != val_info.element_type && val_info.element_type != NOTYPE) { /* warning */ }
            } else { // Explicit simple type matches simple initializer (or needs conversion - TODO)
                if (var_type != val_info.type) {
                     out << "# Advertencia: Tipo de inicializador (" << val_info.type
                         << ") difiere del tipo de variable declarado (" << var_type
                         << ") para " << s->id << std::endl;
                }
            }
        }

        int size_for_storage = get_type_size(var_type); // For TARRAY, this is pointer size
        current_offset -= size_for_storage;
        stack_offsets[s->id] = current_offset;

        switch (var_type) {
            case TINT: case TBOOL: case TARRAY:
                out << "  movq %rax, " << current_offset << "(%rbp)" << std::endl; break;
            case TI32:
                out << "  movl %eax, " << current_offset << "(%rbp)" << std::endl; break;
            case TFLOAT:
                out << "  movsd %xmm0, " << current_offset << "(%rbp)" << std::endl; break;
            case TF32:
                out << "  movss %xmm0, " << current_offset << "(%rbp)" << std::endl; break;
            default:
                out << "Error: Inicialización no soportada para el tipo " << var_type << " de variable " << s->id << std::endl;
                exit(1);
        }
    } else { // No initializer: `let x: type;` or `let x: [type; N];`
        if (var_type == NOTYPE) { // Requires explicit type if no initializer
            out << "Error: Variable " << s->id << " debe tener tipo explícito o valor inicial." << std::endl;
            exit(1);
        }

        // This branch handles `let x: i32;` or `let x: [i32; 5];` (latter not fully supported for stack allocation yet)
        // If parser set s->type to "i32" for `let x: [i32;N]`, we need a flag s->isArray.
        // If (s->isArray) { var_info_for_env.element_type = var_type; var_type = TARRAY;
        //    size = s->num_elements * get_type_size(var_info_for_env.element_type); }
        // else { size = get_type_size(var_type); }
        // This is a GAP for `let x: [TYPE; SIZE];` declarations for stack allocation.

        int size = get_type_size(var_type); // For TARRAY (pointer type), this is 8.
        current_offset -= size;
        stack_offsets[s->id] = current_offset;

        switch (var_type) {
            case TINT: case TBOOL: case TARRAY: // TARRAY here means an uninitialized pointer
                out << "  movq $0, " << current_offset << "(%rbp)" << std::endl; break;
            case TI32:
                out << "  movl $0, " << current_offset << "(%rbp)" << std::endl; break;
            case TFLOAT:
                out << "  pxor %xmm0, %xmm0"; out << "  movsd %xmm0, " << current_offset << "(%rbp)" << std::endl; break;
            case TF32:
                out << "  pxor %xmm0, %xmm0"; out << "  movss %xmm0, " << current_offset << "(%rbp)" << std::endl; break;
            default: /* TVOID or other non-initializable */ break;
        }
         // If it was an explicit stack array decl, var_info_for_env.type should be TARRAY and element_type set.
    }

    var_info_for_env.type = var_type; // Final type for the variable in environment
    env_vars.add_var(s->id, var_info_for_env);
}

void ImpCODE::visit(BreakStm* s) {
    if (loop_break_labels.empty()) {
        out << "Error de generación de código: 'break' fuera de un bucle." << std::endl;
        exit(1);
    }
    // This assumes break always jumps to the end of the current while/for loop.
    // For `for` loops, this might need to jump to `endfor_X`.
    // For `while` loops, this might need to jump to `endwhile_X`.
    // The label name might need to be more generic or specific to the loop type.
    // Current WhileStatement and ForStatement both use "endwhile_" and "endfor_" prefixes.
    // Let's assume loop_break_labels stores the correct end label string part.
    out << "  jmp " << "end" << loop_break_labels.top() << std::endl; // Needs adjustment if label format differs
}


void ImpCODE::visit(ReturnStatement* s) {
    if (s->e) {
        ImpValue ret_val_info = s->e->accept(this);
        // Value is in %rax/%eax or %xmm0. The function epilogue will handle it.
        // No specific mov to %rax/%eax/%xmm0 needed here again unless type conversion/coercion.
    } else { // Return sin expresión (para funciones void)
        out << "  xorq %rax, %rax" << std::endl; // Default return 0, or void
    }
    out << "  jmp end_fun_" << current_function_return_label << std::endl; 
}

void ImpCODE::visit(FCallStm* s) {
    // This is largely the same as FCallExp, but without expecting a return value to be used.
    // Callee-saved registers %r10, %r11 (caller saved by convention for printf like calls)
    out << "  pushq %r10" << std::endl; 
    out << "  pushq %r11" << std::endl;
    
    const std::vector<std::string> arg_gpr_regs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    const std::vector<std::string> arg_xmm_regs = {"%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"};
    
    std::vector<ImpVType> arg_types;
    for (int i = s->argumentos.size() - 1; i >= 0; --i) {
        ImpValue arg_val_info = s->argumentos[i]->accept(this);
        arg_types.push_back(arg_val_info.type); // Store type for later
        int size = get_type_size(arg_val_info.type);
        if (arg_val_info.type == TFLOAT || arg_val_info.type == TF32) {
            out << "  subq $" << size << ", %rsp" << std::endl;
            if (arg_val_info.type == TFLOAT) out << "  movsd %xmm0, (%rsp)" << std::endl;
            else out << "  movss %xmm0, (%rsp)" << std::endl;
        } else { // TINT, TI32, TBOOL, TARRAY (pointer)
            out << "  pushq %rax" << std::endl; // Pushes full %rax; i32 value is in %eax.
        }
    }
    std::reverse(arg_types.begin(), arg_types.end()); // Match argument order

    int gpr_idx = 0;
    int xmm_idx = 0;
    for (size_t i = 0; i < s->argumentos.size(); ++i) {
        ImpVType current_arg_type = arg_types[i];
        int size = get_type_size(current_arg_type);
        if (current_arg_type == TFLOAT || current_arg_type == TF32) {
            if (xmm_idx < arg_xmm_regs.size()) {
                if (current_arg_type == TFLOAT) out << "  movsd (%rsp), " << arg_xmm_regs[xmm_idx++] << std::endl;
                else out << "  movss (%rsp), " << arg_xmm_regs[xmm_idx++] << std::endl;
                out << "  addq $" << size << ", %rsp" << std::endl;
            } else { /* Argument passed on stack, already there */ }
        } else { // TINT, TI32, TBOOL, TARRAY
            if (gpr_idx < arg_gpr_regs.size()) {
                out << "  popq " << arg_gpr_regs[gpr_idx++] << std::endl;
                // For TI32, value is in lower 32 bits of popped GPR. Callee expects this.
            } else { /* Argument passed on stack, already there */ }
        }
    }

    out << "  movl $" << xmm_idx << ", %eax" << std::endl; // Number of XMM registers used

    if (!env_fun_labels.count(s->nombre)) {
        out << "Error: función '" << s->nombre << "' no declarada." << std::endl; exit(1);
    }
    out << "  call " << env_fun_labels[s->nombre] << std::endl;
    // Result of call (if any) is in %rax/%eax or %xmm0, ignored for FCallStm.

    out << "  popq %r11" << std::endl;
    out << "  popq %r10" << std::endl;
}

void ImpCODE::visit(AssignArrayStatement* s) {
    // Get base address of array
    if (!env_vars.check(s->id)) {
        out << "Error: Array '" << s->id << "' no declarado en asignación." << std::endl; exit(1);
    }
    ImpValue array_info = env_vars.lookup(s->id);
    if (array_info.type != TARRAY) {
        out << "Error: Variable '" << s->id << "' no es un array." << std::endl; exit(1);
    }
    out << "  movq " << stack_offsets[s->id] << "(%rbp), %rbx" << std::endl; // %rbx = base_ptr

    // Evaluate index expression
    ImpValue index_val_info = s->index_exp->accept(this); // Index in %rax (assuming i64/i32)
    // Assuming index is i64 or i32, result in %rax or %eax. Use %rax for offset calculation.
    if(index_val_info.type == TI32) out << "  movslq %eax, %rcx" << std::endl; // Sign extend index to 64-bit if i32
    else out << "  movq %rax, %rcx" << std::endl; // %rcx = index

    // Evaluate RHS
    ImpValue rhs_val_info = s->rhs->accept(this); // RHS value in %rax/%eax or %xmm0

    // Check type consistency between RHS and array element type
    if (array_info.element_type != rhs_val_info.type) {
        out << "# Advertencia: Tipo de RHS (" << rhs_val_info.type
            << ") podría no coincidir con tipo de elemento de array (" << array_info.element_type
            << ") para " << s->id << std::endl;
    }

    int element_size = get_type_size(array_info.element_type);
    out << "  movq $" << element_size << ", %rdi" << std::endl; // Element size for imul
    out << "  imulq %rdi, %rcx" << std::endl; // %rcx = index * element_size
    out << "  addq %rbx, %rcx" << std::endl; // %rcx = base_ptr + offset_bytes

    // Store RHS value at address in %rcx
    switch (array_info.element_type) {
        case TINT: case TBOOL: case TARRAY:
            out << "  movq %rax, (%rcx)" << std::endl; break;
        case TI32:
            out << "  movl %eax, (%rcx)" << std::endl; break;
        case TFLOAT:
            out << "  movsd %xmm0, (%rcx)" << std::endl; break;
        case TF32:
            out << "  movss %xmm0, (%rcx)" << std::endl; break;
        default:
            out << "Error: Asignación a array de tipo de elemento no soportado: " << array_info.element_type << std::endl; exit(1);
    }
}

void ImpCODE::visit(FunDec* funDec) {
    std::string fun_label = env_fun_labels[funDec->nombre]; 
    out << fun_label << ":" << std::endl;
    out << "  pushq %rbp" << std::endl;
    out << "  movq %rsp, %rbp" << std::endl;

    env_vars.add_level();
    long previous_offset_before_func = current_offset; // Save global current_offset
    current_offset = 0; // Offset for parameters/locals relative to RBP for this function.
                        // Params are at positive offsets from RBP if passed on stack by caller,
                        // or can be moved from registers to negative offsets.
                        // Locals always at negative offsets.

    // Allocate space for parameters on stack frame and copy from registers/caller's stack
    const std::vector<std::string> arg_gpr_regs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    const std::vector<std::string> arg_xmm_regs = {"%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"};
    int gpr_idx = 0;
    int xmm_idx = 0;
    long caller_stack_arg_offset = 16; // Args pushed by caller start at RBP+16

    auto it_param_type_str = funDec->tipos.begin();
    for (const std::string& param_name : funDec->parametros) {
        ImpVType param_type = ImpValue::get_basic_type(*it_param_type_str);
        int size = get_type_size(param_type);
        current_offset -= size; // Allocate space on our frame for this param
        stack_offsets[param_name] = current_offset; 

        if (param_type == TFLOAT || param_type == TF32) {
            if (xmm_idx < arg_xmm_regs.size()) {
                if (param_type == TFLOAT) out << "  movsd " << arg_xmm_regs[xmm_idx++] << ", " << current_offset << "(%rbp)" << std::endl;
                else out << "  movss " << arg_xmm_regs[xmm_idx++] << ", " << current_offset << "(%rbp)" << std::endl;
            } else { // Passed on stack by caller
                if (param_type == TFLOAT) out << "  movsd " << caller_stack_arg_offset << "(%rbp), %xmm0" << std::endl;
                else out << "  movss " << caller_stack_arg_offset << "(%rbp), %xmm0" << std::endl;

                if (param_type == TFLOAT) out << "  movsd %xmm0, " << current_offset << "(%rbp)" << std::endl;
                else out << "  movss %xmm0, " << current_offset << "(%rbp)" << std::endl;
                caller_stack_arg_offset += size; // Next stack arg
            }
        } else { // TINT, TI32, TBOOL, TARRAY
            if (gpr_idx < arg_gpr_regs.size()) {
                if (param_type == TINT || param_type == TARRAY || param_type == TBOOL)
                    out << "  movq " << arg_gpr_regs[gpr_idx++] << ", " << current_offset << "(%rbp)" << std::endl;
                else // TI32
                    out << "  movl " << "%e" + arg_gpr_regs[gpr_idx++].substr(2) << ", " << current_offset << "(%rbp)" << std::endl;
            } else { // Passed on stack by caller
                out << "  movq " << caller_stack_arg_offset << "(%rbp), %rax" << std::endl; // Load full qword
                if (param_type == TINT || param_type == TARRAY || param_type == TBOOL)
                     out << "  movq %rax, " << current_offset << "(%rbp)" << std::endl;
                else // TI32
                     out << "  movl %eax, " << current_offset << "(%rbp)" << std::endl;
                caller_stack_arg_offset += size;
            }
        }
        ImpValue param_info_env; param_info_env.type = param_type;
        env_vars.add_var(param_name, param_info_env);
        ++it_param_type_str;
    }

    // After params, current_offset is negative, representing space for params copied to our frame.
    // Now, DecStament will further decrement current_offset for locals.
    // The final value of current_offset (most negative) determines total stack needed for params+locals.
    // An explicit subq %rsp could be done here if we knew total size, or rely on pushes/calls to adjust RSP.
    // 'leave' will correctly restore RSP if RBP was the base for all these.

    current_function_return_label = label_count++;
    funDec->cuerpo->accept(this);

    out << "end_fun_" << current_function_return_label << ":" << std::endl;
    // Return value should be in %rax/%eax or %xmm0 before 'leave'.
    // If function is void, %rax content doesn't matter to caller but often zeroed.
    // If function has return type, ReturnStatement should have placed value in correct register.
    out << "  leave" << std::endl; 
    out << "  ret" << std::endl;

    current_offset = previous_offset_before_func; // Restore global current_offset
    env_vars.remove_level(); 
}

void ImpCODE::visit(FunDecList* funDecList) {
    for (FunDec* func : funDecList->Fundecs) {
        func->accept(this);
    }
}

ImpValue ImpCODE::visit(BinaryExp* e) {
    ImpValue left_val_info = e->left->accept(this);
    ImpVType left_type = left_val_info.type;

    int size_left = get_type_size(left_type);
    if (left_type == TFLOAT || left_type == TF32) {
        out << "  subq $" << size_left << ", %rsp" << std::endl;
        if (left_type == TFLOAT) out << "  movsd %xmm0, (%rsp)" << std::endl;
        else out << "  movss %xmm0, (%rsp)" << std::endl;
    } else {
        out << "  pushq %rax" << std::endl; 
    }

    ImpValue right_val_info = e->right->accept(this);
    ImpVType right_type = right_val_info.type;
    ImpValue result_val;

    bool types_compatible = (left_type == right_type);
    // More nuanced compatibility for logical/relational ops if needed:
    if (!types_compatible) {
        bool logical_op = (e->op == AND || e->op == OR);
        bool relational_op = (e->op >= LT_OP && e->op <= NEQ_OP);
        // Allow TBOOL with any int type for logical ops (both sides become effectively bool)
        if (logical_op && ((left_type == TBOOL && (right_type == TINT || right_type == TI32)) || (right_type == TBOOL && (left_type == TINT || left_type == TI32)) )) {
            // types_compatible = true; // This path will be complex, needs promotion of bool to int size or vice-versa
        }
        // Allow comparison between any two numeric types (excluding arrays/void)
        else if (relational_op && !( (left_type == TARRAY || left_type == TVOID || left_type == TSTRING_LITERAL) || (right_type == TARRAY || right_type == TVOID || right_type == TSTRING_LITERAL) )) {
            // types_compatible = true; // This also needs promotion (e.g. i32 vs f64)
        } else { // Strict type matching for now if not covered above
             out << "# Error: Tipos no coincidentes o no soportados para operación binaria: "
                 << left_type << " " << Exp::binopToChar(e->op) << " " << right_type << std::endl;
             // exit(1); // For debugging, allow to proceed to see instruction generation attempt
        }
    }

    result_val.type = left_type; // Default result type. Overridden for relational/logical.

    if (left_type == TFLOAT && right_type == TFLOAT) {
        out << "  movsd %xmm0, %xmm1\n  movsd (%rsp), %xmm0\n  addq $8, %rsp" << std::endl; result_val.type = TFLOAT;
        switch (e->op) {
            case PLUS_OP: out << "  addsd %xmm1, %xmm0" << std::endl; break; case MINUS_OP: out << "  subsd %xmm1, %xmm0" << std::endl; break;
            case MUL_OP: out << "  mulsd %xmm1, %xmm0" << std::endl; break; case DIV_OP: out << "  divsd %xmm1, %xmm0" << std::endl; break;
            case LT_OP: case LE_OP: case EQ_OP: case GT_OP: case GE_OP: case NEQ_OP:
                out << "  comisd %xmm1, %xmm0\n";
                if (e->op==LT_OP) out << "  setb %al\n"; else if (e->op==LE_OP) out << "  setbe %al\n"; else if (e->op==EQ_OP) out << "  sete %al\n";
                else if (e->op==GT_OP) out << "  seta %al\n"; else if (e->op==GE_OP) out << "  setae %al\n"; else out << "  setne %al\n";
                out << "  movzbl %al, %eax\n"; result_val.type = TBOOL; break;
            default: out << "Error: Op binaria " << Exp::binopToChar(e->op) << " no para f64." << std::endl; exit(1);
        }
    } else if (left_type == TF32 && right_type == TF32) {
        out << "  movss %xmm0, %xmm1\n  movss (%rsp), %xmm0\n  addq $4, %rsp" << std::endl; result_val.type = TF32;
        switch (e->op) {
            case PLUS_OP: out << "  addss %xmm1, %xmm0" << std::endl; break; case MINUS_OP: out << "  subss %xmm1, %xmm0" << std::endl; break;
            case MUL_OP: out << "  mulss %xmm1, %xmm0" << std::endl; break; case DIV_OP: out << "  divss %xmm1, %xmm0" << std::endl; break;
            case LT_OP: case LE_OP: case EQ_OP: case GT_OP: case GE_OP: case NEQ_OP:
                out << "  comiss %xmm1, %xmm0\n";
                if (e->op==LT_OP) out << "  setb %al\n"; else if (e->op==LE_OP) out << "  setbe %al\n"; else if (e->op==EQ_OP) out << "  sete %al\n";
                else if (e->op==GT_OP) out << "  seta %al\n"; else if (e->op==GE_OP) out << "  setae %al\n"; else out << "  setne %al\n";
                out << "  movzbl %al, %eax\n"; result_val.type = TBOOL; break;
            default: out << "Error: Op binaria " << Exp::binopToChar(e->op) << " no para f32." << std::endl; exit(1);
        }
    } else if ((left_type == TINT || left_type == TBOOL) && (right_type == TINT || right_type == TBOOL)) {
        out << "  movq %rax, %rcx\n  popq %rax" << std::endl; result_val.type = TINT;
        if (e->op == AND || e->op == OR || (e->op >= LT_OP && e->op <= NEQ_OP)) result_val.type = TBOOL;
        switch (e->op) {
            case PLUS_OP: out << "  addq %rcx, %rax" << std::endl; break; case MINUS_OP: out << "  subq %rcx, %rax" << std::endl; break;
            case MUL_OP: out << "  imulq %rcx, %rax" << std::endl; break; case DIV_OP: out << "  cqto\n  idivq %rcx" << std::endl; break;
            case LT_OP: case LE_OP: case EQ_OP: case GT_OP: case GE_OP: case NEQ_OP:
                out << "  cmpq %rcx, %rax\n";
                if (e->op==LT_OP) out << "  setl %al\n"; else if (e->op==LE_OP) out << "  setle %al\n"; else if (e->op==EQ_OP) out << "  sete %al\n";
                else if (e->op==GT_OP) out << "  setg %al\n"; else if (e->op==GE_OP) out << "  setge %al\n"; else out << "  setne %al\n";
                out << "  movzbl %al, %eax\n"; break;
            case AND: { std::string f=new_label_str(), E=new_label_str(); out << "  cmpq $0,%rax\n  je "<<f<<"\n  movq %rcx,%rax\n  jmp "<<E<<"\n"<<f<<":\n  movq $0,%rax\n"<<E<<":\n  cmpq $0,%rax\n  setne %al\n  movzbl %al,%eax\n"; break;}
            case OR:  { std::string t=new_label_str(), E=new_label_str(); out << "  cmpq $0,%rax\n  jne "<<t<<"\n  movq %rcx,%rax\n  jmp "<<E<<"\n"<<t<<":\n  movq $1,%rax\n"<<E<<":\n  cmpq $0,%rax\n  setne %al\n  movzbl %al,%eax\n"; break;}
            default: out << "Error: Op binaria " << Exp::binopToChar(e->op) << " no para i64/bool." << std::endl; exit(1);
        }
    } else if ((left_type == TI32 || left_type == TBOOL) && (right_type == TI32 || right_type == TBOOL) ) {
        // This assumes if TBOOL is involved with TI32, it's treated as TI32 (0 or 1)
        out << "  movl %eax, %ecx\n  popq %rdx\n  movl %edx, %eax" << std::endl; result_val.type = TI32;
        if (e->op == AND || e->op == OR || (e->op >= LT_OP && e->op <= NEQ_OP)) result_val.type = TBOOL;
        switch (e->op) {
            case PLUS_OP: out << "  addl %ecx, %eax" << std::endl; break; case MINUS_OP: out << "  subl %ecx, %eax" << std::endl; break;
            case MUL_OP: out << "  imull %ecx, %eax" << std::endl; break; case DIV_OP: out << "  cltd\n  idivl %ecx" << std::endl; break;
            case LT_OP: case LE_OP: case EQ_OP: case GT_OP: case GE_OP: case NEQ_OP:
                out << "  cmpl %ecx, %eax\n";
                if (e->op==LT_OP) out << "  setl %al\n"; else if (e->op==LE_OP) out << "  setle %al\n"; else if (e->op==EQ_OP) out << "  sete %al\n";
                else if (e->op==GT_OP) out << "  setg %al\n"; else if (e->op==GE_OP) out << "  setge %al\n"; else out << "  setne %al\n";
                out << "  movzbl %al, %eax\n"; break;
            case AND: { std::string f=new_label_str(), E=new_label_str(); out << "  cmpl $0,%eax\n  je "<<f<<"\n  movl %ecx,%eax\n  jmp "<<E<<"\n"<<f<<":\n  movl $0,%eax\n"<<E<<":\n  cmpl $0,%eax\n  setne %al\n  movzbl %al,%eax\n"; break;}
            case OR:  { std::string t=new_label_str(), E=new_label_str(); out << "  cmpl $0,%eax\n  jne "<<t<<"\n  movl %ecx,%eax\n  jmp "<<E<<"\n"<<t<<":\n  movl $1,%eax\n"<<E<<":\n  cmpl $0,%eax\n  setne %al\n  movzbl %al,%eax\n"; break;}
            default: out << "Error: Op binaria " << Exp::binopToChar(e->op) << " no para i32/bool." << std::endl; exit(1);
        }
    } else {
        out << "Error: Combinación tipos no soportada op binaria: L=" << left_type << ", R=" << right_type << std::endl; exit(1);
    }
    return result_val;
}

ImpValue ImpCODE::visit(UnaryExp* e) {
    ImpValue operand_val_info = e->operand->accept(this);
    ImpVType operand_type = operand_val_info.type;
    ImpValue result_val;
    result_val.type = operand_type;

    switch (e->op) {
        case NOT_OP:
            if (operand_type == TINT || operand_type == TBOOL) {
                 out << "  cmpq $0, %rax" << std::endl;
            } else if (operand_type == TI32) {
                 out << "  cmpl $0, %eax" << std::endl;
            } else {
                 out << "Error: NOT solo para tipos enteros/bool." << std::endl; exit(1);
            }
            out << "  sete %al" << std::endl;     
            out << "  movzbl %al, %eax" << std::endl;
            result_val.type = TBOOL;
            break;
        case MINUS_UNARY_OP:
            if (operand_type == TINT) {
                out << "  negq %rax" << std::endl; 
            } else if (operand_type == TI32) {
                out << "  negl %eax" << std::endl;
            } else if (operand_type == TFLOAT) {
                out << "  movabsq $0x8000000000000000, %rcx" << std::endl;
                out << "  movq %rcx, %xmm1" << std::endl;
                out << "  xorpd %xmm1, %xmm0" << std::endl;
            } else if (operand_type == TF32) {
                out << "  movl $0x80000000, %eax" << std::endl;
                out << "  movd %eax, %xmm1" << std::endl;
                out << "  xorps %xmm1, %xmm0" << std::endl;
            } else {
                out << "Error: '-' unario solo para números." << std::endl;
                exit(1);
            }
            break;
        default:
            out << "Error: Operador unario no soportado." << std::endl;
            exit(1);
    }
    return result_val;
}

ImpValue ImpCODE::visit(IntExp* e) {
    ImpValue v;
    // Assuming integer literals can be contextually typed (e.g. to i32 if assigned to i32 var).
    // For now, all IntExp are loaded into %rax as 64-bit.
    // Code consuming this will use %eax if it's an i32 context.
    v.set_int(e->value); // Sets ImpValue type to TINT (i64)
    out << "  movq $" << e->value << ", %rax" << std::endl; 
    return v; 
}

ImpValue ImpCODE::visit(FloatExp* e) {
    ImpValue v;
    // Assuming float literals are f64 by default.
    // Contextual typing to f32 would require more info here or in parser/AST.
    v.set_float(e->value); // Sets ImpValue type to TFLOAT (f64)
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
    ImpValue array_val;
    array_val.type = TARRAY; 

    // Determine element type and size from the first element for malloc and iteration.
    // This assumes homogeneous arrays, which Rust enforces.
    int element_size = 8; // Default if array is empty or type cannot be inferred.
    if (!e->values.empty()) {
        // Temporarily accept first element to get its type, then get size.
        // This is a simplification; a full type system would propagate this.
        // The code for the first element will be generated here, then again in the loop.
        // This is inefficient but works for type inference for now.
        ImpValue first_elem_info = e->values[0]->accept(this);
        array_val.element_type = first_elem_info.type;
        element_size = get_type_size(array_val.element_type);
        // We need to "undo" the effect of the above accept on %rax/%xmm0 if it's not the one
        // used in the loop. This is complex. A non-code-generating type check is better.
        // For now, let's assume the first element's code generation is idempotent or acceptable.
    } else {
        array_val.element_type = NOTYPE; // Or a default type if specified by context
    }

    int array_size_bytes = e->values.size() * element_size;
    out << "  movq $" << array_size_bytes << ", %rdi" << std::endl; 
    out << "  call malloc@PLT" << std::endl; 
    out << "  movq %rax, %rbx" << std::endl; // %rbx = base_ptr of new array

    // Re-evaluate elements and store them
    for (size_t i = 0; i < e->values.size(); ++i) {
        ImpValue current_element_val_info = e->values[i]->accept(this); // Value in %rax/%eax or %xmm0
        long offset_in_array = i * element_size; 

        // If it's the first element, its type sets the array's element_type if not already set.
        if (i == 0 && array_val.element_type == NOTYPE && !e->values.empty()) {
             array_val.element_type = current_element_val_info.type;
        }
        // TODO: Ensure current_element_val_info.type matches array_val.element_type

        switch (current_element_val_info.type) { // Use evaluated element's actual type for mov
            case TFLOAT: out << "  movsd %xmm0, " << offset_in_array << "(%rbx)" << std::endl; break;
            case TF32:   out << "  movss %xmm0, " << offset_in_array << "(%rbx)" << std::endl; break;
            case TINT: case TBOOL: case TARRAY: /*pointers*/
                         out << "  movq %rax, " << offset_in_array << "(%rbx)" << std::endl; break;
            case TI32:   out << "  movl %eax, " << offset_in_array << "(%rbx)" << std::endl; break;
            default: out << "Error: Tipo de elemento no soportado en literal de array." << std::endl; exit(1);
        }
    }
    out << "  movq %rbx, %rax" << std::endl;
    return array_val; 
}

ImpValue ImpCODE::visit(IdentifierExp* e) {
    if (env_vars.check(e->name)) {
        ImpValue var_info = env_vars.lookup(e->name); 
        switch (var_info.type) {
            case TINT: case TBOOL: case TARRAY:
                out << "  movq " << stack_offsets[e->name] << "(%rbp), %rax" << std::endl; break;
            case TI32:
                out << "  movl " << stack_offsets[e->name] << "(%rbp), %eax" << std::endl; break;
            case TFLOAT:
                out << "  movsd " << stack_offsets[e->name] << "(%rbp), %xmm0" << std::endl; break;
            case TF32:
                out << "  movss " << stack_offsets[e->name] << "(%rbp), %xmm0" << std::endl; break;
            default:
                out << "Error: Carga de identificador no soportada para el tipo " << var_info.type << std::endl; exit(1);
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
    
    const std::vector<std::string> arg_gpr_regs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    const std::vector<std::string> arg_xmm_regs = {"%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"};
    
    std::vector<ImpVType> arg_types; // Store evaluated argument types
    for (int i = e->argumentos.size() - 1; i >= 0; --i) {
        ImpValue arg_val_info = e->argumentos[i]->accept(this);
        arg_types.push_back(arg_val_info.type);
        int size = get_type_size(arg_val_info.type);
        if (arg_val_info.type == TFLOAT || arg_val_info.type == TF32) {
            out << "  subq $" << size << ", %rsp" << std::endl;
            if (arg_val_info.type == TFLOAT) out << "  movsd %xmm0, (%rsp)" << std::endl;
            else out << "  movss %xmm0, (%rsp)" << std::endl;
        } else {
            out << "  pushq %rax" << std::endl;
        }
    }
    std::reverse(arg_types.begin(), arg_types.end());

    int gpr_idx = 0;
    int xmm_idx = 0;
    for (size_t i = 0; i < e->argumentos.size(); ++i) {
        ImpVType current_arg_type = arg_types[i];
        int size = get_type_size(current_arg_type);
        if (current_arg_type == TFLOAT || current_arg_type == TF32) {
            if (xmm_idx < arg_xmm_regs.size()) {
                if (current_arg_type == TFLOAT) out << "  movsd (%rsp), " << arg_xmm_regs[xmm_idx++] << std::endl;
                else out << "  movss (%rsp), " << arg_xmm_regs[xmm_idx++] << std::endl;
                out << "  addq $" << size << ", %rsp" << std::endl;
            }
        } else {
            if (gpr_idx < arg_gpr_regs.size()) {
                out << "  popq " << arg_gpr_regs[gpr_idx++] << std::endl;
            }
        }
    }

    out << "  movl $" << xmm_idx << ", %eax" << std::endl;

    if (!env_fun_labels.count(e->nombre)) {
        out << "Error: función '" << e->nombre << "' no declarada." << std::endl; exit(1);
    }
    out << "  call " << env_fun_labels[e->nombre] << std::endl;

    out << "  popq %r11" << std::endl;
    out << "  popq %r10" << std::endl;

    // Determine return type of function e->nombre from env_fun_labels or a symbol table for functions
    // For now, this is a placeholder. A real compiler needs function signature info.
    // Let's assume it's TINT (i64) if not float, TFLOAT (f64) if float for now.
    // This needs to be properly obtained from FunDec.
    ImpValue result_val_type;
    result_val_type.type = TINT; // Placeholder
    // if (function_returns_float) result_val_type.type = TFLOAT;
    // if (function_returns_i32) result_val_type.type = TI32; etc.
    return result_val_type;
}

ImpValue ImpCODE::visit(AccesoArrayExp* e) {
    if (!env_vars.check(e->id)) {
        out << "Error: Array '" << e->id << "' no declarado en acceso." << std::endl; exit(1);
    }
    ImpValue array_info = env_vars.lookup(e->id);
    if (array_info.type != TARRAY) {
        out << "Error: Variable '" << e->id << "' no es un array." << std::endl; exit(1);
    }
    ImpVType element_type = array_info.element_type; // Crucial!
    if (element_type == NOTYPE) {
        out << "Error: Tipo de elemento de array '" << e->id << "' es desconocido." << std::endl; exit(1);
    }
    int element_size = get_type_size(element_type);

    out << "  movq " << stack_offsets[e->id] << "(%rbp), %rbx" << std::endl; // %rbx = base_ptr

    ImpValue index_val_info = e->index_exp->accept(this); // Index in %rax or %eax
    if(index_val_info.type == TI32) out << "  movslq %eax, %rcx" << std::endl;
    else out << "  movq %rax, %rcx" << std::endl; // %rcx = index

    out << "  movq $" << element_size << ", %rdi" << std::endl;
    out << "  imulq %rdi, %rcx" << std::endl;    // %rcx = index * element_size
    out << "  addq %rbx, %rcx" << std::endl;     // %rcx = base_ptr + offset

    switch (element_type) {
        case TINT: case TBOOL: case TARRAY:
            out << "  movq (%rcx), %rax" << std::endl; break;
        case TI32:
            out << "  movl (%rcx), %eax" << std::endl; break;
        case TFLOAT:
            out << "  movsd (%rcx), %xmm0" << std::endl; break;
        case TF32:
            out << "  movss (%rcx), %xmm0" << std::endl; break;
        default:
            out << "Error: Acceso a array de tipo de elemento no soportado: " << element_type << std::endl; exit(1);
    }

    ImpValue result_val;
    result_val.type = element_type;
    return result_val;
}