// imp_type.hh
#ifndef IMP_TYPE_HH
#define IMP_TYPE_HH

#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <list>
#include <vector>
#include <string>

using namespace std;

class ImpType {
public:
    enum TType { NOTYPE = 0, VOID, INT, BOOL, FLOAT, FUN, ARRAY };
    static const char* type_names[7]; // Ajustado para 7 tipos

    TType ttype;
    vector<TType> types; 

    bool match(const ImpType& other) const;
    bool set_basic_type(string s);
    bool set_basic_type(TType tt);
    bool set_fun_type(list<string> param_types_str, string return_type_str);
    bool set_array_type(string base_type_str);

private:
    TType string_to_type(string s) const;
};

std::ostream& operator << (std::ostream& outs, const ImpType & type);

#endif // IMP_TYPE_HH