#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include "token.h"

class Scanner {
private:
    std::string input;
    int first, current;
    std::ostream& out;
public:
    Scanner(const char* in_s, std::ostream& out);
    Token* nextToken();
    void reset();
    ~Scanner();
};

void test_scanner(Scanner* scanner,std::ostream& out);

#endif // SCANNER_H