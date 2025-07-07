#include "imp_interpreter.hh"
#include "imp_value_visitor.hh"

#include <stdexcept>
#include <string>

void ImpInterpreter::interpret(Program* p) {
    env.clear();
    p->accept(this);
}

void ImpInterpreter::visit(Program* p) {
    env.add_level();
    if (p->fundecs) {
        p->fundecs->accept(this);
    }
    env.remove_level();
}

void ImpInterpreter::visit(Body* b) {
    env.add_level();
    if (b->slist) {
        b->slist->accept(this);
    }
    env.remove_level();
}

void ImpInterpreter::visit(StatementList* s) {
    for (Stm* stm : s->stms) {
        stm->accept(this);
    }
}

void ImpInterpreter::visit(AssignStatement* s) {
    ImpValue v = s->rhs->accept(this);
    if (!env.check(s->id)) {
        out << "Error: Variable '" << s->id << "' no declarada." << endl;
        exit(1);
    }
    ImpValue& target = env.lookup_ref(s->id); // Usar lookup_ref para obtener referencia
    if (target.type != v.type) {
        out << "Error de tipo en asignación: la variable '" << s->id << "' es de tipo " << target.type << " y se intenta asignar un valor de tipo " << v.type << "." << endl;
        exit(1);
    }
    target = v;
}

void ImpInterpreter::visit(PrintStatement* s) {
    string output_string = s->cadena;
    size_t last_pos = 0;
    size_t next_arg = 0;

    for (size_t i = 0; i < output_string.length(); ++i) {
        if (output_string[i] == '{' && i + 1 < output_string.length() && output_string[i+1] == '}') {
            out << output_string.substr(last_pos, i - last_pos);
            if (next_arg < s->e.size()) {
                out << s->e[next_arg++]->accept(this);
            } else {
                out << "Error: Faltan argumentos para println!." << endl;
                exit(1);
            }
            i++; 
            last_pos = i + 1;
        }
    }
    out << output_string.substr(last_pos) << endl;
}

void ImpInterpreter::visit(IfStatement* s) {
    ImpValue condition_val = s->condition->accept(this);
    if (condition_val.type != TBOOL) {
        out << "Error de tipo: La condición del 'if' debe ser booleana." << endl;
        exit(1);
    }
    if (condition_val.bool_value) {
        s->then->accept(this);
    } else if (s->els) {
        s->els->accept(this);
    }
}

void ImpInterpreter::visit(WhileStatement* s) {
    while (true) {
        ImpValue condition_val = s->condition->accept(this);
        if (condition_val.type != TBOOL) {
            out << "Error de tipo: La condición del 'while' debe ser booleana." << endl;
            exit(1);
        }
        if (!condition_val.bool_value) {
            break;
        }
        try {
            s->b->accept(this);
        } catch (const string& control_flow) {
            if (control_flow == "break") {
                break;
            } else {
                throw;
            }
        }
    }
}

void ImpInterpreter::visit(ForStatement* s) {
    ImpValue init_val = s->init->accept(this);
    ImpValue end_val = s->end->accept(this);

    if (init_val.type != TINT || end_val.type != TINT) {
        out << "Error de tipo: Los rangos del 'for' deben ser enteros." << endl;
        exit(1);
    }

    env.add_level();
    env.add_var(s->id, init_val); 

    for (int i = init_val.int_value; i < end_val.int_value; ++i) {
        ImpValue current_i_val;
        current_i_val.set_int(i); 
        env.update(s->id, current_i_val);
        
        try {
            s->b->accept(this);
        } catch (const string& control_flow) {
            if (control_flow == "break") {
                break;
            } else {
                throw;
            }
        }
    }
    env.remove_level();
}

void ImpInterpreter::visit(DecStament* s) {
    ImpVType var_type = ImpValue::get_basic_type(s->type);
    if (var_type == NOTYPE) {
        out << "Error: Tipo '" << s->type << "' no reconocido para declaración de variable." << endl;
        exit(1);
    }

    ImpValue initial_val;
    if (s->value) {
        initial_val = s->value->accept(this);
        if (initial_val.type != var_type) {
            out << "Error de tipo en declaración: La expresión inicial no coincide con el tipo declarado para '" << s->id << "'." << endl;
            exit(1);
        }
    } else {
        initial_val.set_default_value(var_type);
    }
    env.add_var(s->id, initial_val);
}

void ImpInterpreter::visit(BreakStm* s) {
    throw string("break");
}

void ImpInterpreter::visit(ReturnStatement* s) {
    if (s->e) {
        return_value_stack.push(s->e->accept(this));
    } else {
        ImpValue void_val;
        void_val.set_default_value(TVOID);
        return_value_stack.push(void_val);
    }
    throw string("return");
}

void ImpInterpreter::visit(FCallStm* s) {
    if (declared_functions.find(s->nombre) == declared_functions.end()) {
        out << "Error: Función '" << s->nombre << "' no declarada." << endl;
        exit(1);
    }

    FunDec* func_def = declared_functions[s->nombre];
    if (func_def->parametros.size() != s->argumentos.size()) {
        out << "Error: Número incorrecto de argumentos para la función '" << s->nombre << "'." << endl;
        exit(1);
    }

    env.add_level();

    auto it_param_type = func_def->tipos.begin();
    auto it_ref_mut = s->ref_mut.begin();

    for (size_t i = 0; i < s->argumentos.size(); ++i) {
        string param_name = func_def->parametros[i];
        string expected_type_str = *it_param_type;
        ImpVType expected_type = ImpValue::get_basic_type(expected_type_str);
        
        if (it_ref_mut != s->ref_mut.end() && *it_ref_mut) {
            IdentifierExp* id_exp = dynamic_cast<IdentifierExp*>(s->argumentos[i]);
            if (!id_exp) {
                out << "Error: El argumento " << i << " para la función '" << s->nombre << "' debe ser una variable para ser referencia mutable." << endl;
                exit(1);
            }
            if (!env.check(id_exp->name)) {
                out << "Error: La variable '" << id_exp->name << "' pasada por referencia mutable no está declarada." << endl;
                exit(1);
            }
            ImpValue& ref_val = env.lookup_ref(id_exp->name); // Usar lookup_ref para obtener la referencia
            if (ref_val.type != expected_type) {
                out << "Error de tipo: El tipo del argumento '" << id_exp->name << "' (" << ref_val.type << ") no coincide con el tipo esperado (" << expected_type << ") para la referencia mutable." << endl;
                exit(1);
            }
            env.add_var_ref(param_name, ref_val); // add_var_ref debe almacenar la referencia
        } else {
            ImpValue arg_val = s->argumentos[i]->accept(this);
            if (arg_val.type != expected_type) {
                out << "Error de tipo: El tipo del argumento " << i << " (" << arg_val.type << ") no coincide con el tipo esperado (" << expected_type << ") para la función '" << s->nombre << "'." << endl;
                exit(1);
            }
            env.add_var(param_name, arg_val);
        }
        ++it_param_type;
        if(it_ref_mut != s->ref_mut.end()) ++it_ref_mut;
    }

    try {
        func_def->cuerpo->accept(this);
        if (ImpValue::get_basic_type(func_def->tipo) != TVOID && return_value_stack.empty()) {
            out << "Error: Función '" << s->nombre << "' de tipo no-void no retornó un valor." << endl;
            exit(1);
        }
    } catch (const string& control_flow) {
        if (control_flow != "return") {
            throw;
        }
    }

    env.remove_level();
}


void ImpInterpreter::visit(AssignArrayStatement* s) {
    if (!env.check(s->id)) {
        out << "Error: Array '" << s->id << "' no declarado." << endl;
        exit(1);
    }
    ImpValue& array_val = env.lookup_ref(s->id); // Usar lookup_ref para obtener referencia
    if (array_val.type != TARRAY || array_val.array_value == nullptr) {
        out << "Error: La variable '" << s->id << "' no es un array válido." << endl;
        exit(1);
    }

    ImpValue index_val = s->index_exp->accept(this);
    if (index_val.type != TINT) {
        out << "Error de tipo: El índice del array debe ser un entero." << endl;
        exit(1);
    }
    int index = index_val.int_value;

    if (index < 0 || index >= array_val.array_value->size()) {
        out << "Error en tiempo de ejecución: Índice de array fuera de límites para '" << s->id << "' (índice: " << index << ", tamaño: " << array_val.array_value->size() << ")." << endl;
        exit(1);
    }

    ImpValue rhs_val = s->rhs->accept(this);
    // Verificar si el array tiene elementos antes de acceder a .at(0).type
    // Y verificar el tipo del elemento si el array no está vacío.
    if (array_val.element_type != NOTYPE && array_val.element_type != rhs_val.type) {
        out << "Error de tipo: El valor asignado al array no coincide con el tipo de sus elementos." << endl;
        exit(1);
    }
    // Si el array está vacío y se asigna el primer elemento, se establece el tipo del elemento.
    // Aunque tu gramática de inicialización de arrays no permite arrays vacíos de tipo implícito,
    // es una buena práctica considerar cómo se manejaría si fuera posible.
    // Por ahora, 'array_val.element_type' ya debería estar establecido si el array se inicializó con elementos.

    array_val.array_value->at(index) = rhs_val;
}


void ImpInterpreter::visit(FunDec* f) {
    if (declared_functions.count(f->nombre)) {
        out << "Error: Redefinición de la función '" << f->nombre << "'." << endl;
        exit(1);
    }
    declared_functions[f->nombre] = f;
}

void ImpInterpreter::visit(FunDecList* fdl) {
    for (FunDec* func : fdl->Fundecs) {
        func->accept(this);
    }
}


ImpValue ImpInterpreter::visit(BinaryExp* e) {
    ImpValue v1 = e->left->accept(this);
    ImpValue v2 = e->right->accept(this);
    ImpValue result;

    if ((v1.type == TINT && v2.type == TFLOAT) || (v1.type == TFLOAT && v2.type == TINT)) {
        out << "Error de tipo: No se permite operar directamente entre i64 y f64. Se requiere conversión explícita." << endl;
        exit(1);
    }

    if (v1.type == TINT && v2.type == TINT) {
        int iv1 = v1.int_value;
        int iv2 = v2.int_value;
        switch (e->op) {
            case PLUS_OP: result.set_int(iv1 + iv2); break;
            case MINUS_OP: result.set_int(iv1 - iv2); break;
            case MUL_OP: result.set_int(iv1 * iv2); break;
            case DIV_OP: 
                if (iv2 == 0) { out << "Error: División por cero." << endl; exit(1); }
                result.set_int(iv1 / iv2); 
                break;
            case LT_OP: result.set_bool(iv1 < iv2); break;
            case LE_OP: result.set_bool(iv1 <= iv2); break;
            case GT_OP: result.set_bool(iv1 > iv2); break;
            case GE_OP: result.set_bool(iv1 >= iv2); break;
            case EQ_OP: result.set_bool(iv1 == iv2); break;
            case NEQ_OP: result.set_bool(iv1 != iv2); break;
            case AND: 
            case OR: 
                out << "Error: Operación lógica AND/OR no soportada para enteros." << endl; exit(1);
            default: out << "Error: Operación binaria no soportada para enteros." << endl; exit(1);
        }
    } 
    else if (v1.type == TFLOAT && v2.type == TFLOAT) {
        double fv1 = v1.float_value;
        double fv2 = v2.float_value;
        switch (e->op) {
            case PLUS_OP: result.set_float(fv1 + fv2); break;
            case MINUS_OP: result.set_float(fv1 - fv2); break;
            case MUL_OP: result.set_float(fv1 * fv2); break;
            case DIV_OP: 
                if (fv2 == 0.0) { out << "Error: División por cero." << endl; exit(1); }
                result.set_float(fv1 / fv2); 
                break;
            case LT_OP: result.set_bool(fv1 < fv2); break;
            case LE_OP: result.set_bool(fv1 <= fv2); break;
            case GT_OP: result.set_bool(fv1 > fv2); break;
            case GE_OP: result.set_bool(fv1 >= fv2); break;
            case EQ_OP: result.set_bool(fv1 == fv2); break;
            case NEQ_OP: result.set_bool(fv1 != fv2); break;
            case AND: 
            case OR: 
                out << "Error: Operación lógica AND/OR no soportada para flotantes." << endl; exit(1);
            default: out << "Error: Operación binaria no soportada para flotantes." << endl; exit(1);
        }
    }
    else if (v1.type == TBOOL && v2.type == TBOOL) {
        bool bv1 = v1.bool_value;
        bool bv2 = v2.bool_value;
        switch (e->op) {
            case AND: result.set_bool(bv1 && bv2); break;
            case OR: result.set_bool(bv1 || bv2); break;
            case EQ_OP: result.set_bool(bv1 == bv2); break;
            case NEQ_OP: result.set_bool(bv1 != bv2); break;
            case PLUS_OP: 
            case MINUS_OP:
            case MUL_OP:
            case DIV_OP:
            case LT_OP:
            case LE_OP:
            case GT_OP:
            case GE_OP:
                out << "Error: Operación aritmética/relacional no soportada para booleanos." << endl; exit(1);
            default: out << "Error: Operación binaria no soportada para booleanos." << endl; exit(1);
        }
    }
    else {
        out << "Error de tipos en operación binaria: Tipos de operandos no válidos o incompatibles (" << v1.type << " y " << v2.type << ")." << endl;
        exit(1);
    }
    return result;
}

ImpValue ImpInterpreter::visit(UnaryExp* e) {
    ImpValue operand_val = e->operand->accept(this);
    ImpValue result;

    if (e->op == NOT_OP) {
        if (operand_val.type != TBOOL) {
            out << "Error de tipo: El operador '!' solo puede aplicarse a booleanos." << endl;
            exit(1);
        }
        result.set_bool(!operand_val.bool_value);
    } else if (e->op == MINUS_UNARY_OP) {
        if (operand_val.type == TINT) {
            result.set_int(-operand_val.int_value);
        } else if (operand_val.type == TFLOAT) {
            result.set_float(-operand_val.float_value);
        } else {
            out << "Error de tipo: El operador unario '-' solo puede aplicarse a enteros o flotantes." << endl;
            exit(1);
        }
    } else {
        out << "Error: Operador unario no reconocido." << endl;
        exit(1);
    }
    return result;
}

ImpValue ImpInterpreter::visit(IntExp* e) {
    ImpValue v;
    v.set_int(e->value);
    return v;
}

ImpValue ImpInterpreter::visit(FloatExp* e) {
    ImpValue v;
    v.set_float(e->value);
    return v;
}

ImpValue ImpInterpreter::visit(BoolExp* e) {
    ImpValue v;
    v.set_bool(e->value);
    return v;
}

ImpValue ImpInterpreter::visit(ArrayExp* e) {
    std::vector<ImpValue>* arr_values = new std::vector<ImpValue>();
    ImpVType base_type = NOTYPE; 

    if (!e->values.empty()) {
        ImpValue first_elem_val = e->values[0]->accept(this);
        base_type = first_elem_val.type;
        arr_values->push_back(first_elem_val); 
    }

    for (size_t i = (e->values.empty() ? 0 : 1); i < e->values.size(); ++i) {
        Exp* exp_elem = e->values[i];
        ImpValue elem_val = exp_elem->accept(this);

        if (base_type == NOTYPE) { 
             base_type = elem_val.type;
        } else if (elem_val.type != base_type) {
            out << "Error de tipo: Todos los elementos de un array deben ser del mismo tipo." << std::endl;
            delete arr_values; 
            exit(1);
        }
        arr_values->push_back(elem_val);
    }
    
    ImpValue v;
    v.set_array(arr_values, base_type); 
    return v;
}

ImpValue ImpInterpreter::visit(IdentifierExp* e) {
    if (env.check(e->name)) {
        return env.lookup_ref(e->name); // Devolver referencia al valor
    } else {
        out << "Error: Variable '" << e->name << "' no declarada." << endl;
        exit(1);
    }
}

ImpValue ImpInterpreter::visit(FCallExp* e) {
    if (declared_functions.find(e->nombre) == declared_functions.end()) {
        out << "Error: Función '" << e->nombre << "' no declarada." << endl;
        exit(1);
    }

    FunDec* func_def = declared_functions[e->nombre];
    if (func_def->parametros.size() != e->argumentos.size()) {
        out << "Error: Número incorrecto de argumentos para la función '" << e->nombre << "'." << endl;
        exit(1);
    }

    env.add_level();

    auto it_param_type_str = func_def->tipos.begin();
    auto it_ref_mut = e->ref_mut.begin();

    for (size_t i = 0; i < e->argumentos.size(); ++i) {
        string param_name = func_def->parametros[i];
        ImpVType expected_type = ImpValue::get_basic_type(*it_param_type_str);
        
        if (it_ref_mut != e->ref_mut.end() && *it_ref_mut) {
            IdentifierExp* id_exp = dynamic_cast<IdentifierExp*>(e->argumentos[i]);
            if (!id_exp) {
                out << "Error: El argumento " << i << " para la función '" << e->nombre << "' debe ser una variable para ser referencia mutable." << endl;
                exit(1);
            }
            if (!env.check(id_exp->name)) {
                out << "Error: La variable '" << id_exp->name << "' pasada por referencia mutable no está declarada." << endl;
                exit(1);
            }
            ImpValue& ref_val = env.lookup_ref(id_exp->name);
            if (ref_val.type != expected_type) {
                out << "Error de tipo: El tipo del argumento '" << id_exp->name << "' (" << ref_val.type << ") no coincide con el tipo esperado (" << expected_type << ") para la referencia mutable." << endl;
                exit(1);
            }
            env.add_var_ref(param_name, ref_val);
        } else {
            ImpValue arg_val = e->argumentos[i]->accept(this);
            if (arg_val.type != expected_type) {
                out << "Error de tipo: El tipo del argumento " << i << " (" << arg_val.type << ") no coincide con el tipo esperado (" << expected_type << ") para la función '" << e->nombre << "'." << endl;
                exit(1);
            }
            env.add_var(param_name, arg_val);
        }
        ++it_param_type_str;
        if(it_ref_mut != e->ref_mut.end()) ++it_ref_mut;
    }

    ImpValue return_val;
    try {
        func_def->cuerpo->accept(this);
        if (ImpValue::get_basic_type(func_def->tipo) != TVOID) {
            if (return_value_stack.empty()) {
                out << "Error: Función '" << e->nombre << "' de tipo no-void no retornó un valor." << endl;
                exit(1);
            }
            return_val = return_value_stack.top();
            return_value_stack.pop();
        } else {
            if (!return_value_stack.empty()) {
                return_value_stack.pop();
            }
            return_val.set_default_value(TVOID);
        }

    } catch (const string& control_flow) {
        if (control_flow == "return") {
            if (ImpValue::get_basic_type(func_def->tipo) != TVOID) {
                if (return_value_stack.empty()) {
                    out << "Error: Función '" << e->nombre << "' de tipo no-void intentó retornar sin valor." << endl;
                    exit(1);
                }
                return_val = return_value_stack.top();
                return_value_stack.pop();
            } else {
                if (!return_value_stack.empty()) {
                    return_value_stack.pop();
                }
                return_val.set_default_value(TVOID);
            }
        } else {
            throw;
        }
    }

    env.remove_level();
    return return_val;
}

ImpValue ImpInterpreter::visit(AccesoArrayExp* e) {
    if (!env.check(e->id)) {
        out << "Error: Array '" << e->id << "' no declarado." << endl;
        exit(1);
    }
    // Para acceder al valor de un array, si no se va a modificar, no es estrictamente necesario
    // usar lookup_ref. Pero es buena práctica si luego se desea encadenar operaciones que sí modifiquen.
    // Para simple lectura, lookup (que devuelve copia) o lookup_const_ref (si tienes una) bastaría.
    // Usaremos lookup_ref para consistencia con la mutabilidad potencial y evitar copias si ImpValue es grande.
    ImpValue& array_val = env.lookup_ref(e->id); 
    if (array_val.type != TARRAY || array_val.array_value == nullptr) {
        out << "Error: La variable '" << e->id << "' no es un array válido." << endl;
        exit(1);
    }

    ImpValue index_val = e->index_exp->accept(this);
    if (index_val.type != TINT) {
        out << "Error de tipo: El índice del array debe ser un entero." << endl;
        exit(1);
    }
    int index = index_val.int_value;

    try {
        return array_val.array_value->at(index);
    } catch (const std::out_of_range& oor) {
        out << "Error en tiempo de ejecución: Índice de array fuera de límites para '" << e->id << "' (índice: " << index << ", tamaño: " << array_val.array_value->size() << ")." << endl;
        exit(1);
    }
}