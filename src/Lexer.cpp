#include "Lexer.h"

Lexer::Lexer(){
    tokens.clear();
}

char Lexer::peek(){
    return vector.at(index);
}

char Lexer::advance(){
    return vector.at(++index);
}

