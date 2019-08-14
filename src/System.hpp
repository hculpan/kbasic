#ifndef _SYSTEM_HPP_
#define _SYSTEM_HPP_

#include "ConsoleOutput.hpp"
#include "Lexer.hpp"

#include <map>

class System {
public:
    System();

    void command(string line, ConsoleOutput *output);

private:
    map<int, string> program;

    ConsoleOutput *m_output;

    bool is_number(const std::string& s);
    bool checkNext(Lexer *l, TokenType type);
    bool checkEol(Lexer *l);
    void swallowNext(Lexer *l);

    void loadCodeLine(Lexer *l, string line);

    // Commands
    void files(Lexer *l);
    void load(Lexer *l);
    void list(Lexer *l);

    void lines();
    
};

extern System *core;

#endif