#include "src/Lexer.h"

int main(int argc, char const *argv[]) {
    Lexer lexer;

    std::string code = "1234.123421";

    for(token t : lexer.lex(code))
        std::cout << t.val << std::endl;

    return 0;
}
