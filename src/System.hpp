#ifndef _SYSTEM_HPP_
#define _SYSTEM_HPP_

#include "ConsoleOutput.hpp"

#include <map>

class System {
public:
    System();

    void command(string line, ConsoleOutput *output);

private:
    map<int, string> program;

    ConsoleOutput *m_output;

    bool is_number(const std::string& s);

    void lines();
    
};

extern System *core;

#endif