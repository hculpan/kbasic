#ifndef _CONSOLE_OUTPUT_HPP_
#define _CONSOLE_OUTPUT_HPP_

#include <string>

using namespace std;

class ConsoleOutput {
public:
    virtual void addText(string s) = 0;
    virtual void clearText() = 0;
    virtual void terminate() = 0;
};

#endif