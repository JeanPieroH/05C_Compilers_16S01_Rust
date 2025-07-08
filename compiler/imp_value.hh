#ifndef IMP_VALUE_HH
#define IMP_VALUE_HH

#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

enum ImpVType { NOTYPE = 0, TINT, TBOOL, TFLOAT, TARRAY, TVOID, TSTRING_LITERAL, TI32, TF32 };

class ImpValue {
public:
    ImpVType type;
    int int_value;
    bool bool_value;
    double float_value;
    std::vector<ImpValue>* array_value;
    std::string string_value; 
    
    ImpVType element_type; 

    ImpValue();
    ~ImpValue();
    ImpValue(const ImpValue& other);
    ImpValue& operator=(const ImpValue& other);

    void set_default_value(ImpVType tt);
    void set_int(int val) { type = TINT; int_value = val; }
    void set_bool(bool val) { type = TBOOL; bool_value = val; }
    void set_float(double val) { type = TFLOAT; float_value = val; }
    void set_string_literal(const string& val) { type = TSTRING_LITERAL; string_value = val; }
    void set_array(std::vector<ImpValue>* arr_ptr, ImpVType elem_t); 

    static ImpVType get_basic_type(string s); 

private:
    void copy_array_content(const ImpValue& other);
};

std::ostream& operator << (std::ostream& outs, const ImpValue & v);

#endif // IMP_VALUE_HH