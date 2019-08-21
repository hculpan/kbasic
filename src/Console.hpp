#ifndef _CONSOLE_HPP_
#define _CONSOLE_HPP_

#include <string>

using namespace std;

class Console {
public:
    virtual void addText(string s, bool append = false) = 0;
    virtual void clearText() = 0;
    virtual void terminate() = 0;
    virtual bool loop() = 0;
    virtual float inputNumber(string prompt) = 0;
    virtual string inputString(string prompt) = 0;
};

#endif