#include "imp_value.hh"

ImpValue::ImpValue() :
    type(NOTYPE),
    int_value(0),
    bool_value(false),
    float_value(0.0),
    array_value(nullptr),
    string_value(""),
    element_type(NOTYPE) {}

ImpValue::~ImpValue() {
    if (type == TARRAY && array_value != nullptr) {
        delete array_value;
        array_value = nullptr;
    }
}

ImpValue::ImpValue(const ImpValue& other) :
    type(other.type),
    int_value(other.int_value),
    bool_value(other.bool_value),
    float_value(other.float_value),
    array_value(nullptr),
    string_value(other.string_value),
    element_type(other.element_type)
{
    copy_array_content(other);
}

ImpValue& ImpValue::operator=(const ImpValue& other) {
    if (this == &other) {
        return *this;
    }

    if (type == TARRAY && array_value != nullptr) {
        delete array_value;
        array_value = nullptr;
    }

    type = other.type;
    int_value = other.int_value;
    bool_value = other.bool_value;
    float_value = other.float_value;
    string_value = other.string_value;
    element_type = other.element_type;

    copy_array_content(other);

    return *this;
}

void ImpValue::copy_array_content(const ImpValue& other) {
    if (other.type == TARRAY && other.array_value != nullptr) {
        array_value = new std::vector<ImpValue>(*other.array_value);
    } else {
        array_value = nullptr;
    }
}

void ImpValue::set_default_value(ImpVType tt) {
    type = tt;
    element_type = NOTYPE;
    switch (tt) {
        case TINT: int_value = 0; break;
        case TI32: int_value = 0; break; // Default for i32
        case TBOOL: bool_value = false; break;
        case TFLOAT: float_value = 0.0; break;
        case TF32: float_value = 0.0f; break; // Default for f32
        case TARRAY:
            if (array_value != nullptr) {
                delete array_value;
            }
            array_value = new std::vector<ImpValue>();
            break;
        case TSTRING_LITERAL: string_value = ""; break;
        case TVOID:
        case NOTYPE:
        default:
            break;
    }
}

void ImpValue::set_array(std::vector<ImpValue>* arr_ptr, ImpVType elem_t) {
    type = TARRAY;
    element_type = elem_t;
    if (array_value != nullptr) {
        delete array_value;
    }
    array_value = arr_ptr;
}

ImpVType ImpValue::get_basic_type(string s) {
    if (s == "i64") return TINT;
    if (s == "f64") return TFLOAT;
    if (s == "bool") return TBOOL;
    if (s == "void" || s == "unit" || s == "()") return TVOID;
    if (s == "i32") return TI32;
    if (s == "f32") return TF32;
    return NOTYPE;
}

std::ostream& operator << (std::ostream& outs, const ImpValue & v) {
    switch (v.type) {
        case TINT: outs << v.int_value; break;
        case TI32: outs << v.int_value << "(i32)"; break; // Distinguish for logging
        case TBOOL: outs << (v.bool_value ? "true" : "false"); break;
        case TFLOAT: outs << v.float_value; break;
        case TF32: outs << v.float_value << "(f32)"; break; // Distinguish for logging
        case TSTRING_LITERAL: outs << "\"" << v.string_value << "\""; break;
        case TARRAY:
            outs << "[";
            if (v.array_value != nullptr) {
                for (size_t i = 0; i < v.array_value->size(); ++i) {
                    outs << (*v.array_value)[i];
                    if (i < v.array_value->size() - 1) {
                        outs << ", ";
                    }
                }
            }
            outs << "]";
            break;
        case TVOID: outs << "void"; break;
        case NOTYPE:
        default: outs << "NOTYPE"; break;
    }
    return outs;
}