#ifndef _CONSOLE_HPP_
#define _CONSOLE_HPP_

#include <string>

using namespace std;

enum PrintAppendMode { pam_none, pam_append, pam_tab };

struct CursorPos {
    int col;
    int row;

    CursorPos(int col, int row)
    {
        this->col = col;
        this->row = row;
    }
};

class Console {
public:
    virtual void addText(string s, PrintAppendMode appendMode = pam_none) = 0;
    virtual void putTextAt(int location, string s, PrintAppendMode appendMode = pam_none) = 0;
    virtual void clearText() = 0;
    virtual void terminate() = 0;
    virtual bool loop() = 0;
    virtual float inputNumber(string prompt) = 0;
    virtual string inputString(string prompt) = 0;
    virtual int lineSize() = 0;
    virtual int lineCount() = 0;
    virtual string getKey() = 0;
    virtual CursorPos getCursorPos() = 0;
    virtual void setCursorPos(const CursorPos &pos) = 0;
};

#endif