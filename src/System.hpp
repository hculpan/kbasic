#ifndef _SYSTEM_HPP_
#define _SYSTEM_HPP_

#include "Console.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Value.hpp"

#include <map>
#include <vector>
#include <climits>
#include <stack>
#include <queue>
#include <iostream>
#include <fstream>

struct LineLocation {
    int lineNum;
    Node *node;

    LineLocation(int lineNum, Node *node) 
    {
        this->lineNum = lineNum;
        this->node = node;
    }
};

struct ForLocation {
    int lineNum;
    Node *node;
    int startIndex;
    int endIndex;

    ForLocation() {}

    ForLocation(int lineNum, Node *node, const int startIndex, const int endIndex)
    {
        this->lineNum = lineNum;
        this->node = node;
        this->startIndex = startIndex;
        this->endIndex = endIndex;
    }
};

struct ProgramLine {
    int lineNum;
    string line;
    Node *node;

    ProgramLine() {}

    ProgramLine(const ProgramLine &p2) 
    {
        lineNum = p2.lineNum;
        line = p2.line;
        node = p2.node;
    }

    ~ProgramLine()
    {
        free(node);
    }
};

enum AccessMode { am_input, am_output };

struct FileAccess {
    int number;
    ios *stream = nullptr;
    AccessMode accessMode;
    string filename;
    bool m_open = false;

    inline bool isOpen() const { return m_open; }

    FileAccess(string name, int number, AccessMode accessMode)
    {
        this->filename = name;
        this->number = number;
        this->accessMode = accessMode;
        if (accessMode == am_input) 
        {
            this->stream = new ifstream(name);
            m_open = dynamic_cast<ifstream *>(stream)->is_open();
        }
        if (accessMode == am_output) 
        {
            this->stream = new ofstream(name);
            m_open = dynamic_cast<ofstream *>(stream)->is_open();
        }
    }

    ~FileAccess()
    {
        if (stream && accessMode == am_input) dynamic_cast<ifstream *>(stream)->close();
        if (stream && accessMode == am_output) dynamic_cast<ofstream *>(stream)->close();
    }
};

class System {
public:
    System();
    ~System();

    void command(string line, Console *output);

private:
    const int NO_LINE_NUM = INT_MIN;

    map<int, ProgramLine *> m_program;

    map<string, Value> m_variables;

    map<int, FileAccess *> m_openFiles;

    stack<LineLocation> m_gosub;
    map<string, ForLocation> m_for;
    stack<string> m_forStack;

    queue<Value> m_dataStack;

    vector<string> m_errors;

    Console *m_output;

    int nextLineNo = NO_LINE_NUM;
    int currLine = NO_LINE_NUM;
    bool inAssign = false;
    bool continueStatements = true;
    Node *currNode = nullptr;

    bool is_number(const std::string& s);
    bool checkNext(Lexer *l, TokenType type);
    bool checkEol(Lexer *l);
    void swallowNext(Lexer *l);
    bool isBoolNode(NodeType type);
    bool isComparisonNode(NodeType type);

    int getLineNo(string line);

    void loadCodeLine(string line);

    Value getVariable(Node *node);
    Value getVariable(string id);
    void setVariable(Node *node, Value v);

    void processData();

    void execute(Node *node);
    void load(Node *node);
    void new_(Node *node);
    void stat(Node *node);
    void bye(Node *node);
    void scnclr(Node *node);
    void list(Node *node);
    void files(Node *node);
    void save(Node *node);
    void statements(Node *node);
    bool statement(Node *node);
    void print(Node *node);
    void printfile(Node *node);
    void line(Node *node);
    void run(Node *node);
    void trun(Node *node);
    void goto_(Node *node);
    void gosub(Node *node);
    void return_(Node *node);
    void assign(Node *node);
    Value add(Node *node);
    void clear(Node *node);
    void if_(Node *node);
    void for_(Node *node);
    void next(Node *node);
    void input(Node *node);
    void inputfile(Node *node);
    void open(Node *node);
    void close(Node *node);
    void getkey(Node *node);
    void data(Node *node);
    void read(Node *node);
    void restore(Node *node);

    void branchTo(int lineNum, Node *node);
    void preprocess(Node *node);
    void handleData(Node *node);

    Value expression(Node *node);
    Value boolExpression(Node *node);

    // Function definitions
    Value function(Node *node);
    Value tab(const Value &v);
    Value intFunc(const Value &v);
    Value strFunc(const Value &v);
    Value rnd(const Value &v);
    Value valFunc(const Value &v);
    Value chrFunc(const Value &v);

    void lines();
    
};

extern System *core;

#endif