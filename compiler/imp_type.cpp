// imp_type.cpp
#include "imp_type.hh"

// El tamaño si se añaden o quitan tipos en el enum ImpType::TType
const char* ImpType::type_names[7] = { "notype", "void", "i64", "bool", "f64", "fun", "array" };

bool ImpType::match(const ImpType& other) const {
    if (this->ttype != other.ttype) {
        return false;
    }

    if (this->ttype == ImpType::FUN) {
        if (this->types.size() != other.types.size()) {
            return false;
        }
        for (size_t i = 0; i < this->types.size(); ++i) {
            if (this->types[i] != other.types[i]) {
                return false;
            }
        }
    } else if (this->ttype == ImpType::ARRAY) {
        // Para arrays, solo el tipo base debe coincidir
        if (this->types.empty() || other.types.empty() || this->types[0] != other.types[0]) {
            return false;
        }
    }
    return true;
}

bool ImpType::set_basic_type(string s) {
    TType tt = string_to_type(s);
    if (tt == ImpType::NOTYPE) {
        return false;
    }
    this->ttype = tt;
    this->types.clear();
    return true;
}

bool ImpType::set_basic_type(TType tt) {
    switch(tt) {
        case ImpType::INT:
        case ImpType::BOOL:
        case ImpType::VOID:
        case ImpType::FLOAT:
            this->ttype = tt;
            this->types.clear();
            return true;
        default:
            return false;
    }
}

bool ImpType::set_fun_type(list<string> param_types_str, string return_type_str) {
    ttype = ImpType::FUN;
    types.clear();

    for (const string& s : param_types_str) {
        TType param_tt = string_to_type(s);
        // Las funciones no pueden tener funciones o arrays como parámetros directos sin envoltorios
        if (param_tt == ImpType::NOTYPE || param_tt == ImpType::FUN || param_tt == ImpType::ARRAY) {
            types.clear();
            return false;
        }
        types.push_back(param_tt);
    }

    TType return_tt = string_to_type(return_type_str);
    // Las funciones no pueden devolver funciones o arrays directos sin envoltorios
    if (return_tt == ImpType::NOTYPE || return_tt == ImpType::FUN || return_tt == ImpType::ARRAY) {
        types.clear();
        return false;
    }
    types.push_back(return_tt); // El último tipo en 'types' es el tipo de retorno

    return true;
}

bool ImpType::set_array_type(string base_type_str) {
    TType base_tt = string_to_type(base_type_str);
    if (base_tt == ImpType::NOTYPE || base_tt == ImpType::FUN || base_tt == ImpType::ARRAY) {
        return false;
    }
    this->ttype = ImpType::ARRAY;
    this->types.clear();
    this->types.push_back(base_tt); // El primer elemento de 'types' es el tipo base del array
    return true;
}

ImpType::TType ImpType::string_to_type(string s) const {
    if (s == "i64") return ImpType::INT;
    if (s == "bool") return ImpType::BOOL;
    if (s == "f64") return ImpType::FLOAT;
    if (s == "void" || s == "unit" || s == "()") return ImpType::VOID;
    return ImpType::NOTYPE;
}

std::ostream& operator << (std::ostream& outs, const ImpType & type) {
    outs << ImpType::type_names[type.ttype];
    if (type.ttype == ImpType::FUN) {
        size_t num_params = type.types.size() - 1; // Último elemento es el retorno
        ImpType::TType rtype = type.types[num_params];
        outs << "(";
        for (size_t i = 0; i < num_params; ++i) {
            outs << ImpType::type_names[type.types[i]];
            if (i < num_params - 1) {
                outs << ", ";
            }
        }
        outs << ") -> " << ImpType::type_names[rtype];
    } else if (type.ttype == ImpType::ARRAY) {
        if (!type.types.empty()) {
            outs << "[" << ImpType::type_names[type.types[0]] << "]";
        }
    }
    return outs;
}