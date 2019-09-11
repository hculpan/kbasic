#include "System.hpp"

#include <iostream>
#include <iterator>
#include <sstream>
#include <fstream>
#include <vector>
#include <climits>
#include <cmath>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <iomanip>

#include <dirent.h>

System *core = new System();

System::System() {
    srand(time(NULL));
}

System::~System()
{
    for (map<int, FileAccess*>::iterator it = m_openFiles.begin(); it != m_openFiles.end(); it++)
    {
        free(it->second);
    }
}

bool System::checkNext(Lexer *l, TokenType type)
{
    bool result = false;
    LexToken *t = l->peek();
    if (t && t->type == type)
    {
        l->next();
        free(t);
        result = true;
    }

    return result;
}

void System::swallowNext(Lexer *l)
{
    LexToken *t = l->next();
    if (t) free(t);
}

void System::files(Node *node) 
{
    string dirname = ".";
    if (node->right && node->right->type == nt_string)
    {
        dirname = node->right->text;
    }

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (dirname.c_str())) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")) m_output->addText(ent->d_name);
        }
        closedir (dir);
    } else {
        /* could not open directory */
        m_output->addText("Error: Could not read current directory");
    }
}

bool System::checkEol(Lexer *l)
{
    bool result = false;
    LexToken *t = l->peek();
    if (!t || t->type == t_eol)
    {
        if (t)
        {
            l->next();
            free(t);
        }
        result = true;
    }

    return result;
}

void System::loadCodeLine(string line) 
{
    istringstream iss(line);
    vector<string> results((istream_iterator<string>(iss)),
                            istream_iterator<string>());
    
    if (is_number(results[0])) {
        if (results.size() == 1) {
            m_program.erase(stoi(results[0]));
        } else {
            ProgramLine *p = new ProgramLine();
            p->lineNum = stoi(results[0]);
            p->line = line;
            Parser *parser = new Parser();
            p->node = parser->parseStatements(line);
            vector<ParseError> errors = parser->errors();
            if (errors.size() > 0) 
            {
                m_output->addText("Errors in line " + to_string(p->lineNum));
                for (vector<ParseError>::iterator it = errors.begin(); it != errors.end(); it++)
                {
                    m_output->addText(it->msg);
                }
                free(p->node);
                free(p);
            } else
            {
                m_program[p->lineNum] = p;
            }
        }
    }
}

void System::execute(Node *node)
{
    executionStatus = ex_executing;

    m_errors.clear();

    if (node->type == nt_load) load(node);
    else if (node->type == nt_new) new_(node);
    else if (node->type == nt_stat) stat(node);
    else if (node->type == nt_bye) bye(node);
    else if (node->type == nt_list) list(node);
    else if (node->type == nt_files) files(node);
    else if (node->type == nt_save) save(node);
    else if (node->type == nt_statement) statements(node);
    else if (node->type == nt_line) line(node);
    else if (node->type == nt_run) run(node);
    else if (node->type == nt_trun) trun(node);

    if (m_errors.size() > 0)
    {
        if (currLine != NO_LINE_NUM) m_output->addText("Runtime error at line " + to_string(currLine));
        for (vector<string>::iterator it = m_errors.begin(); it != m_errors.end(); it++) 
        {
            m_output->addText(*it);
        }
    }

    executionStatus = ex_done;
}

void System::statements(Node *node) 
{
    currNode = node;
    continueStatements = true;
    ifState = ifs_none;
    triggerElse = false;
    while (currNode && loopResult != l_end)
    {
        if (statement(currNode))
        {
            if (currNode) currNode = currNode->right;

            if (m_errors.size() > 0) break;

            if (!continueStatements)
            {
                continueStatements = true;
                break;
            }
        } else 
        {
            break;
        }
    }
}

bool System::statement(Node *node) 
{
    continueStatements = true;

    //if (node->left->type != nt_else) triggerElse = false;
    if (ifState == ifs_yes && node->left->type == nt_else)
    {
        ifState = ifs_none;
        continueStatements = false;
        return false;
    } else if (ifState == ifs_no && node->left->type != nt_else && triggerElse)
    {
        return true;
    }

    if      (node->left->type == nt_print) print(node->left);
    else if (node->left->type == nt_scnclr) scnclr(node->left);
    else if (node->left->type == nt_assign) assign(node->left); 
    else if (node->left->type == nt_clear) clear(node->left);
    else if (node->left->type == nt_return) return_(node->left);
    else if (node->left->type == nt_if) if_(node->left);
    else if (node->left->type == nt_else) else_(node->left);
    else if (node->left->type == nt_for) for_(node->left);
    else if (node->left->type == nt_next) next(node->left);
    else if (node->left->type == nt_input) input(node->left);
    else if (node->left->type == nt_open) open(node->left);
    else if (node->left->type == nt_close) close(node->left);
    else if (node->left->type == nt_printfile) printfile(node->left);
    else if (node->left->type == nt_inputfile) inputfile(node->left);
    else if (node->left->type == nt_getkey) getkey(node->left);
    else if (node->left->type == nt_data) data(node->left);
    else if (node->left->type == nt_read) read(node->left);
    else if (node->left->type == nt_restore) restore(node->left);
    else if (node->left->type == nt_goto) 
    {
        goto_(node->left);
        return false;
    }
    else if (node->left->type == nt_gosub) 
    {
        gosub(node->left);
        return false;
    }
    else if (node->left->type == nt_end) 
    {
        loopResult = l_end;
        return false;
    }

    if (loopResult == l_end || loopResult == l_escape) 
    {
        m_output->addText("Break");
        continueStatements = false;
    }

    return continueStatements;
}

void System::else_(Node *node)
{
    if (triggerElse)
    {
        ifState = ifs_none;
        triggerElse = false;
        continueStatements = statement(node->left);
    }
}

void System::if_(Node *node)
{
    Value v = expression(node->left);
    ifState = (v.boolean() ? ifs_yes : ifs_no );
    triggerElse = !v.boolean();
    continueStatements = statement(node->right);
}

void System::clear(Node *node) 
{
    UNUSED(node)

    m_variables.clear();
}

void System::getkey(Node *node) 
{
    if (!node->left) return;

    inAssign = true;

    waitForClearKeyboard();

    string s = "";
    int index = 0;
    while (index < 50 && s == "")
    {
        singleLoop();
        s = m_output->getKey();
        index++;
    }
    if (s == "return") s = string(1, '\r');

    setVariable(node->left, Value(s));
    inAssign = false;
}

void System::assign(Node *node) 
{
    if (!node->left) return;

    inAssign = true;
    string id = node->text;
    Value v;
    if (node->left->type == nt_inkey)
    {
        waitForClearKeyboard();
        v = Value(m_output->getKey());
    } else
    {
        v = expression(node->left);
        if ((v.isString() && (id.size() == 1 || id.substr(id.size() - 1, 1) != "$")) ||
            (v.isNumeric() && (id.size() > 1 && id.substr(id.size() - 1, 1) == "$")))
        {
            m_errors.push_back("Type mismatch");
            return;
        }
    }
    
    setVariable(node, v);
    inAssign = false;
}

void System::read(Node *node) 
{
    Node *currNode = node->left;
    while (currNode)
    {
        if (m_dataStack.empty())
        {
            m_errors.push_back("Out of DATA");
            return;
        }

        Value v = m_dataStack.front();
        m_dataStack.pop();
        setVariable(currNode->left, v);
        currNode = currNode->right;
    }
}

void System::data(Node *node) 
{
    UNUSED(node)

    // This intentionally left empty; should do nothing if executed as part of a program
    // DATA statements are handled as part of the pre-process     
}

void System::close(Node *node) 
{
    int number = stoi(node->left->text);

    map<int, FileAccess *>::iterator it = m_openFiles.find(number);
    if (it == m_openFiles.end())
    {
        m_errors.push_back("File number " + to_string(number) + " has not been opened.");
        return;
    }

    if (it->second->accessMode == am_output) dynamic_cast<ofstream *>(it->second->stream)->close();
    if (it->second->accessMode == am_input) dynamic_cast<ifstream *>(it->second->stream)->close();

    free(it->second);
    m_openFiles.erase(it);
}

void System::open(Node *node) 
{
    string file = node->left->text;
    int number = stoi(node->right->right->text);

    if (m_openFiles.find(number) != m_openFiles.end())
    {
        m_errors.push_back("File number " + to_string(number) + " already in use.");
        return;
    }

    AccessMode accessMode = am_output;
    if (node->right->left->type == nt_input) accessMode = am_input;

    FileAccess *f = new FileAccess(file, number, accessMode);
    if (!f->isOpen())
    {
        m_errors.push_back("Unable to open file \"" + file + "\"");
        return;
    }

    m_openFiles[number] = f;
}

void System::inputfile(Node *node) 
{
    int filenum = stoi(node->right->text);
    map<int, FileAccess *>::iterator it = m_openFiles.find(filenum);
    if (it == m_openFiles.end())
    {
        m_errors.push_back("File number " + to_string(filenum) + " undefined");
        return;
    } else if (it->second->accessMode != am_input)
    {
        m_errors.push_back("Cannot read from a file that was opened for OUTPUT access");
        return;
    }

    ifstream *input = dynamic_cast<ifstream *>(it->second->stream);
    char c;
    string s = "";
    while ((c = input->get()))
    {
        if (c == ',' || c == ':' || c == ';' || c == '\n') 
        {
            break;
        } else if (c != '\r') s += c;
    }

    string var = node->left->text;
    Value result;
    if (var.back() == '$') result = Value(s);
    else if (is_number(s)) result = Value(stoi(s));
    else 
    {
        m_errors.push_back("Type mismatch");
        return;
    }
    setVariable(node->left, result);
}

void System::input(Node *node) 
{
    string var = node->left->text;
    string prompt = (node->right ? node->right->text : "");
    Value result;
    if (var.back() == '$') result = Value(m_output->inputString(prompt));
    else result = Value(m_output->inputNumber(prompt));
    setVariable(node->left, result);
}

void System::goto_(Node *node) 
{
    Value v = expression(node->left);
    if (v.type() == vt_integer)
    {
        nextLineNo = v.integer();
    } else 
    {
        m_errors.push_back("Invalid value for GOTO: \"" + v.string() + "\"");
    }
}

void System::branchTo(int lineNum, Node *node)
{
    this->currLine = lineNum;
    this->currNode = node;
    map<int, ProgramLine *>::iterator it = m_program.find(lineNum);
    if (it == m_program.end()) 
    {
        m_errors.push_back("Invalid branch to " + to_string(lineNum));
        return;
    }

    it++;
    if (it != m_program.end())
    {
        nextLineNo = it->second->lineNum;
    }
}

void System::return_(Node *node) 
{
    UNUSED(node);

    if (m_gosub.size() == 0) m_errors.push_back("RETURN without GOSUB error");
    else 
    {
        LineLocation l = m_gosub.top();
        m_gosub.pop();
        branchTo(l.lineNum, l.node);
    }
}

void System::restore(Node *node) 
{
    UNUSED(node)
    
    processData();
}

void System::next(Node *node) 
{
    string var = "";
    if (!node->left)
    {
        while (var == "" && !m_forStack.empty())
        {
            string temp = m_forStack.back();
            map<string, ForLocation>::iterator it = m_for.find(temp);
            if (it != m_for.end()) var = it->first;
        }
    } else var = node->left->text;
    map<string, ForLocation>::iterator it = m_for.find(var);
    if (var == "" || it == m_for.end())
    {
        m_errors.push_back("NEXT without matching FOR");
        return;
    }

    ForLocation fl = m_for[var];
    int value = getVariable(var).integer();
    setVariable(var, value + 1);
    if (fl.endIndex > value) branchTo(it->second.lineNum, it->second.node);
    else 
    {
        for (vector<string>::iterator it = m_forStack.begin(); it != m_forStack.end(); it++)
        {
            if (*it == var) 
            {
                m_forStack.erase(it);
                break;
            }
        }
        m_for.erase(var);
    }
}

void System::for_(Node *node) 
{
    string var = node->left->text;
    Value v1 = expression(node->right->left);
    Value v2 = expression(node->right->right);
    if (!v1.isInteger() || !v1.isInteger())
    {
        m_errors.push_back("Type mismatch in FOR");
        return;
    }

    m_for[var] = ForLocation();
    m_for[var].lineNum = currLine;
    m_for[var].node = node->parent;
    m_for[var].startIndex = v1.integer();
    m_for[var].endIndex = v2.integer();
    setVariable(node->left, v1.integer());
    m_forStack.push_back(var);
}

void System::gosub(Node *node) 
{
    Value v = expression(node->left);
    if (v.type() == vt_integer)
    {
        m_gosub.push(LineLocation(currLine, node->right));
        nextLineNo = v.integer();
    } else 
    {
        m_errors.push_back("Invalid value for GOSUB: \"" + v.string() + "\"");
    }
}

void System::printfile(Node *node) 
{
    int filenum = stoi(node->right->text);
    map<int, FileAccess *>::iterator it = m_openFiles.find(filenum);
    if (it == m_openFiles.end())
    {
        m_errors.push_back("File number " + to_string(filenum) + " undefined");
        return;
    } else if (it->second->accessMode != am_output)
    {
        m_errors.push_back("Cannot write to a file that was opened for INPUT access");
        return;
    }

    Node *currNode = node->left;
    string s = "";
    bool append;
    while (currNode)
    {
        append = (currNode->data == "append");
        s += expression(currNode->left).string();
        currNode = currNode->right;
    }

    *(dynamic_cast<ofstream *>(it->second->stream)) << s;
    if (!append) *(dynamic_cast<ofstream *>(it->second->stream)) << endl;
}

string System::formatString(string s, string format)
{
    class comma_numpunct : public std::numpunct<char>
    {
        public:
            comma_numpunct(bool grouping)
            {
                this->m_grouping = grouping;
            }

        protected:
            bool m_grouping = false;

            virtual char do_thousands_sep() const
            {
                return ',';
            }

            virtual std::string do_grouping() const
            {
                return (m_grouping ? "\03" : "");
            }
    };

    if (format == "") return s;
    if (!isInteger(s) && !isFloat(s)) return s;

    float num = stof(s, NULL);

    int dotLoc = format.find('.');
    int commaLoc = format.find(",");
    bool dollarSign = format.at(0) == '$';

    string fa = (dotLoc > -1 ? format.substr(0, dotLoc) : format);
    string fb = (dotLoc > -1 ? format.substr(dotLoc + 1, INT_MAX) : "");
    
    size_t firstSize = std::count(fa.begin(), fa.end(), '#');
    size_t lastSize = std::count(fb.begin(), fb.end(), '#');

    stringstream ss;
    locale comma_locale(locale(), new comma_numpunct((commaLoc < dotLoc) || (commaLoc > -1 && dotLoc == -1)));

    // tell cout to use our new locale.
    ss.imbue(comma_locale);

    ss << setprecision(lastSize) << fixed << num;

    string result = ss.str();
    if (dollarSign) result = "$" + result;
    dotLoc = result.find('.');
    if (dotLoc > -1 && firstSize > static_cast<size_t>(dotLoc)) result = string(firstSize - dotLoc, ' ') + result;

    return result;
}

void System::print(Node *node) 
{
    if (!node->left)
    {
        m_output->addText("");
    } else
    {
        int loc = -1;
        string format = "";
        if (node->right && node->right->type == nt_at)
        {
            Value v = expression(node->right->left);
            if (!v.isInteger()) 
            {
                m_errors.push_back("Type mismatch for PRINT @");
                return;
            }
            loc = v.integer();
        } else if (node->right && node->right->type == nt_using)
        {
            Value v = expression(node->right->left);
            if (!v.isString()) 
            {
                m_errors.push_back("Type mismatch for PRINT USING");
                return;
            }
            format = v.string();
        }

        Node *currNode = node->left;
        while (currNode)
        {
            PrintAppendMode pmode = pam_none;
            if (currNode->data == "append") pmode = pam_append;
            else if (currNode->data == "append-tab") pmode = pam_tab;
            string s = formatString(expression(currNode->left).string(), format);
            if (loc >= 0)
            {
                m_output->putTextAt(loc, s, pmode);
                loc += s.size();
            } else 
            {
                m_output->addText(s, pmode);
            }
            currNode = currNode->right;
        }
    }
}

bool System::isBoolNode(NodeType type)
{
    return (type == nt_and || type == nt_or || type == nt_not || isComparisonNode(type));
}

bool System::isComparisonNode(NodeType type)
{
    return ((!inAssign && type == nt_equal) || type == nt_notequal || type == nt_greater ||
            type == nt_greaterequal || type == nt_less || type == nt_lessequal);
}

Value System::boolExpression(Node *node)
{
    if (!node) throw "Missing parameter";
    if (node->type == nt_string)
        throw "Type mismatch: Expecting boolean, found \"" + node->text + "\"";

    if (node->type == nt_integer) return Value(stoi(node->text) != 0);

    if (node->type == nt_real) return Value(stof(node->text) != 0.0);

    if (node->type == nt_identifier) return getVariable(node).boolean();

    if (node->type == nt_and || node->type == nt_or)
    {
        Value v1 = boolExpression(node->left);
        Value v2 = boolExpression(node->right);
        if (node->type == nt_and) return Value(v1.boolean() && v2.boolean());
        if (node->type == nt_or) return Value(v1.boolean() || v2.boolean());
    } else if (node->type == nt_not)
    {
        Value v1 = boolExpression(node->left);
        return Value(!v1.boolean());
    } else if (node->type == nt_equal) return add(node->left).equals(add(node->right));
    else if (node->type == nt_greater) return add(node->left).isGreaterThan(add(node->right));
    else if (node->type == nt_less) return add(node->left).isLessThan(add(node->right));
    else if (node->type == nt_notequal) return !add(node->left).equals(add(node->right));
    else if (node->type == nt_greaterequal) 
    {
        Value v1 = add(node->left);
        Value v2 = add(node->right);
        return v1.isGreaterThan(v2) || v1.equals(v2);
    } else if (node->type == nt_lessequal) 
    {
        Value v1 = add(node->left);
        Value v2 = add(node->right);
        return v1.isLessThan(v2) || v1.equals(v2);
    }

    return Value();
}

bool isNumeric(NodeType type) 
{
    return (type == nt_integer || nt_real);
}

Value System::add(Node *node)
{
    if (node->type == nt_integer) return Value(stoi(node->text));
    if (node->type == nt_real) return Value(stof(node->text));
    if (node->type == nt_string) return Value(node->text);
    if (node->type == nt_identifier) return getVariable(node);
    if (node->type == nt_function) return function(node);

    if (node->type == nt_add)
    {
        Value v1 = add(node->left);
        Value v2 = add(node->right);

        if (v1.type() == vt_integer && v2.type() == vt_integer) return Value(v1.integer() + v2.integer());    
        if (v1.type() == vt_integer && v2.type() == vt_real) return Value(v1.integer() + v2.real());    
        if (v1.type() == vt_real && v2.type() == vt_real) return Value(v1.real() + v2.real());    
        if (v1.type() == vt_real && v2.type() == vt_integer) return Value(v1.real() + v2.integer());    
        if (v1.type() == vt_string && v2.type() == vt_string) return Value(v1.string() + v2.string());   
    } else if (node->type == nt_minus)
    {
        Value v1 = add(node->left);
        Value v2 = add(node->right);

        if (v1.type() == vt_integer && v2.type() == vt_integer) return Value(v1.integer() - v2.integer());    
        if (v1.type() == vt_integer && v2.type() == vt_real) return Value(v1.integer() - v2.real());    
        if (v1.type() == vt_real && v2.type() == vt_real) return Value(v1.real() - v2.real());    
        if (v1.type() == vt_real && v2.type() == vt_integer) return Value(v1.real() - v2.integer());    
    } else if (node->type == nt_mult)
    {
        Value v1 = add(node->left);
        Value v2 = add(node->right);

        if (v1.type() == vt_integer && v2.type() == vt_integer) return Value(v1.integer() * v2.integer());    
        if (v1.type() == vt_integer && v2.type() == vt_real) return Value(v1.integer() * v2.real());    
        if (v1.type() == vt_real && v2.type() == vt_real) return Value(v1.real() * v2.real());    
        if (v1.type() == vt_real && v2.type() == vt_integer) return Value(v1.real() * v2.integer());    
    } else if (node->type == nt_div)
    {
        Value v1 = add(node->left);
        Value v2 = add(node->right);

        if (v1.type() == vt_integer && v2.type() == vt_integer) return Value(float(v1.integer()) / float(v2.integer()));    
        if (v1.type() == vt_integer && v2.type() == vt_real) return Value(float(v1.integer()) / v2.real());    
        if (v1.type() == vt_real && v2.type() == vt_real) return Value(v1.real() / v2.real());    
        if (v1.type() == vt_real && v2.type() == vt_integer) return Value(v1.real() / float(v2.integer()));    
    } else if (node->type == nt_negate)
    {
        Value v1 = add(node->left);

        if (v1.type() == vt_integer) return Value(v1.integer() * -1);    
        if (v1.type() == vt_real) return Value(float(v1.real() * -1.0));    
    } else if (node->type == nt_power)
    {
        Value v1 = add(node->left);
        Value v2 = add(node->right);

        if (v1.type() == vt_integer && v2.type() == vt_integer) return Value(pow(v1.integer(), v2.integer()));    
        if (v1.type() == vt_integer && v2.type() == vt_real) return Value(pow(v1.integer(), v2.real()));    
        if (v1.type() == vt_real && v2.type() == vt_real) return Value(pow(v1.real(), v2.real()));    
        if (v1.type() == vt_real && v2.type() == vt_integer) return Value(pow(v1.real(), v2.integer()));    
    }
    m_errors.push_back("Type mismatch");
    return Value();
}

Value System::getVariable(string id)
{
    transform(id.begin(), id.end(), id.begin(),
    [](unsigned char c){ return tolower(c); });

    if (m_variables.find(id) == m_variables.end())
    {
        return Value(0);
    }

    return m_variables[id];
}

Value System::getVariable(Node *node)
{
    string id = node->text;

    Node *currNode = node->right;
    while (currNode && currNode->type == nt_arrayid)
    {
        Value v = expression(currNode->left);
        id = id + "__" + v.string();
        currNode = currNode->right;
    }

    return getVariable(id);
}

void System::setVariable(Node *node, Value v)
{
    if (node->text == "") return;

    string id = node->text;
    Node *currNode = node->right;
    while (currNode && currNode->type == nt_arrayid)
    {
        Value v = expression(currNode->left);
        id = id + "__" + v.string();
        currNode = currNode->right;
    }
    setVariable(id, v);
}

void System::setVariable(string id, Value v)
{
    cout << "Setting " << id << " to " << v.string() << endl;
    transform(id.begin(), id.end(), id.begin(),
    [](unsigned char c){ return tolower(c); });

    m_variables[id] = v;
}

Value System::expression(Node *node)
{
    if (!node) return Value();

    if (node->type == nt_string) return Value(node->text);
    else if (node->type == nt_integer) return Value(stoi(node->text));
    else if (node->type == nt_real) return Value(stof(node->text));
    else if (node->type == nt_function) return function(node);
    else if (isBoolNode(node->type)) return boolExpression(node);
    else return add(node);
}

Value System::tab(const Value &v)
{
    if (!v.isInteger())
    {
        m_errors.push_back("Type mismatch in call to TAB()");
        return Value();
    }
    CursorPos cpos = m_output->getCursorPos();
    cpos.col = v.integer();
    m_output->setCursorPos(cpos);
    return string("");
}

Value System::intFunc(const Value &v)
{
    if (v.isString() && isFloat(v.string()))
    {
        return Value(int(stof(v.string())));
    }
    if (!v.isNumeric())
    {
        m_errors.push_back("Type mismatch in call to INT()");
        return Value();
    }
    return Value(int(v.real()));
}

Value System::strFunc(const Value &v)
{
    if (!v.isNumeric() && !v.isString())
    {
        m_errors.push_back("Type mismatch in call to STR$()");
        return Value();
    }
    return Value(v.string());
}

Value System::chrFunc(const Value &v)
{
    if (v.isInteger() && v.integer() > -1 && v.integer() < 256)
        return Value("" + string(1, static_cast<char>(v.integer())));
    else return Value("");
}

Value System::valFunc(const Value &v)
{
    if (v.isNumeric()) return v;
    if (isInteger(v.string())) return Value(stoi(v.string()));
    if (isFloat(v.string())) return Value(stof(v.string()));
    else return Value(0);
}

Value System::rnd(const Value &v)
{
    if (!v.isInteger())
    {
        m_errors.push_back("Type mismatch in call to RND()");
        return Value();
    }

    if (v.integer() == 0)
    {
        return Value(float(rand())/float(RAND_MAX));
    } else
    {
        int n = rand();
        int m = (v.integer() == 0.0 ? 1 : v.integer());
        Value result = Value((n % m) + 1);
        return result;
    }
}

Value System::function(Node *node)
{
    const Value param = expression(node->left);
    string ltext = node->text;
    transform(ltext.begin(), ltext.end(), ltext.begin(), [](unsigned char c){ return tolower(c); });

    if (ltext == "tab") return tab(param);
    if (ltext == "int") return intFunc(param);
    if (ltext == "rnd") return rnd(param);
    if (ltext == "str$") return strFunc(param);
    if (ltext == "val") return valFunc(param);
    if (ltext == "chr$") return chrFunc(param);
    else return Value();
}

bool is_number(const std::string& s)
{
    return !s.empty() && std::find_if(s.begin(), 
        s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}

int System::getLineNo(string line) {
    int result = NO_LINE_NUM;

    int pos = line.find(' ');
    if (pos > 0)
    {
        string s = line.substr(0, pos);
        if (is_number(s))
        {
            result = stoi(s);
        }
    }

    return result;
}

void System::trun(Node *node) 
{
    clock_t start;
    double duration;

    start = clock();

    run(node);

    duration = (clock() - start ) / (double) CLOCKS_PER_SEC;

    m_output->addText("Execution duration: " + to_string(duration));
}

void System::handleData(Node *node)
{
    Node *currNode = node->left;
    while (currNode)
    {
        Value v;
        if (currNode->type == nt_integer) v = Value(stoi(currNode->text));
        if (currNode->type == nt_real) v = Value(stof(currNode->text));
        if (currNode->type == nt_string) v = Value(currNode->text);
        m_dataStack.push(v);
        currNode = currNode->right;
    }
}

void System::processData()
{
    queue<Value> empty;
    swap(m_dataStack, empty);
    
    for (map<int, ProgramLine *>::iterator it = m_program.begin(); it != m_program.end(); it++)
    {
        Node *currNode = it->second->node->left;
        while (currNode)
        {
            if (currNode->left && currNode->left->type == nt_data) handleData(currNode->left);
            currNode = currNode ->right;
        }
    }
}

void System::waitForClearKeyboard()
{
    string s = m_output->getKey();
    while (s != "")
    {
        singleLoop();
        s = m_output->getKey();
    }
}

void System::preprocess(Node *node)
{
    UNUSED(node)

    waitForClearKeyboard();

    processData();    
}

void System::run(Node *node) 
{
    UNUSED(node)

    if (m_program.size() == 0) return;

    preprocess(node);

    map<int, ProgramLine *>::iterator it = m_program.begin();
    nextLineNo = NO_LINE_NUM;
    currLine = NO_LINE_NUM;
    m_forStack.clear();
    m_for.clear();
    m_errors.clear();
    loopResult = l_runningProgram;  // clear out any prior ESC
    Parser *p = new Parser();
    while (it != m_program.end())
    {
        // parse statements
        if (!it->second->node)
        {
            it->second->node = p->parseStatements(it->second->line);
        }
        Node *line = it->second->node;
        if (!line || !line->left) 
        {
            currLine = NO_LINE_NUM;
            m_errors.push_back("Unknown system error!");
            break;
        } else if (line->type != nt_lineNumber)
        {
            currLine = NO_LINE_NUM;
            m_errors.push_back("Parse error: Invalid line number \"" + line->text + "\"");
            break;
        }
        currLine = it->second->lineNum;
        Node *stmts = line->left;

        if (p->hasErrors())
        {
            vector<ParseError> errors = p->errors();
            for (vector<ParseError>::iterator it = errors.begin(); it != errors.end(); it++ )
            {
                m_errors.push_back("Parse error: " + it->msg);
            }

            break;
        }

        singleLoop();

        // execute statements
        if (loopResult == l_runningProgram) statements(stmts);

        if (m_errors.size() > 0) break;

        if (nextLineNo == NO_LINE_NUM) it++;
        else 
        {
            it = m_program.find(nextLineNo);
            if (it == m_program.end())
            {
                m_errors.push_back("Invalid line number in GOTO/GOSUB");
                break;
            }
            nextLineNo = NO_LINE_NUM;
        }

        if (m_output->loop() != l_runningProgram) 
        {
//            if (loopResult == l_escape) m_output->addText("Break");
            loopResult = l_running;
            break;
        }
    }

    free(p);
    loopResult = l_running;
}

void System::bye(Node *node) 
{
    UNUSED(node)

    m_output->terminate();
}

void System::scnclr(Node *node)
{
    UNUSED(node)

    m_output->clearText();
}

void System::stat(Node *node) 
{
    UNUSED(node)

    m_output->addText("m_program. lines in memory: " + to_string(m_program.size()));
    if (m_variables.size() > 0)
    {
        m_output->addText("Variables:");
        for (map<string, Value>::iterator it = m_variables.begin(); it != m_variables.end(); it++) 
        {
            string type = "";
            switch (it->second.type())
            {
                case vt_bool:
                    type = "boolean";
                    break;
                case vt_integer:
                    type = "integer";
                    break;
                case vt_real:
                    type = "real";
                    break;
                case vt_string:
                    type = "string";
                    break;
                case vt_null:
                    type = "null";
                    break;
                default:
                    type = "unknown";
            }
            m_output->addText("  " + it->first + ": " + it->second.string() + " [" + type + "]");
        } 
    } else 
    {
        m_output->addText("Variables: [None defined]");
    }
}

void System::new_(Node *node)
{
    UNUSED(node)

    m_program.clear();
    m_output->addText("Ok");
}

void System::load(Node *node)
{
    m_program.clear();
    Node *filename = node->right;

    m_output->addText("Loading \"" + filename->text + "\"");
    string line;
    ifstream myfile (filename->text);
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            loadCodeLine(line);
        }
        myfile.close();
        m_output->addText("Load complete, " + to_string(m_program.size()) + " lines loaded.");
    } else 
    {
        m_output->addText("Unable to open file \"" + filename->text + "\"");
    }
}

void System::save(Node *node)
{
    Node *filename = node->right;

    m_output->addText("Saving \"" + filename->text + "\"");
    string line;
    ofstream myfile (filename->text);
    if (myfile.is_open())
    {
        for (map<int, ProgramLine *>::iterator it = m_program.begin(); it != m_program.end(); it++ )
        {
            myfile << it->second->line << endl;
        }
        myfile.close();
        m_output->addText("Save complete, " + to_string(m_program.size()) + " lines written to \"" + filename->text + "\"");
    }
    else m_output->addText("Unable to open file \"" + filename->text + "\"");
}

void System::line(Node *node) 
{
    loadCodeLine(node->text);
}

void System::list(Node *node) 
{
    int startIndex = 0;
    int endIndex = INT_MAX;

    if (node->right && node->right->type == nt_integer) 
    {
        startIndex = stoi(node->right->text);
        endIndex = startIndex;
    } else if (node->right && node->right->type == nt_integerrange)
    {
        if (node->right->left && node->right->left->type == nt_integer)
        {
            startIndex = stoi(node->right->left->text);
        }
        if (node->right->right && node->right->right->type == nt_integer)
        {
            endIndex = stoi(node->right->right->text);
        }
    }

    for (map<int, ProgramLine *>::iterator it = m_program.begin(); it != m_program.end(); it++ )
    {
        if (it->first >= startIndex && it->first <= endIndex) 
        {
            int offset = 0;
            string l = it->second->line;
            do 
            {
                m_output->addText(l.substr(offset, m_output->lineSize()));
                offset += m_output->lineSize();
            } while (offset < int(l.size()));
        }
    }
}

void System::command(string line, Console *output) {
    this->m_output = output;

    Parser *p = new Parser(line);
    Node *n = p->parse();
    if (p->hasErrors())
    {
        vector<ParseError> errors = p->errors();
        for (vector<ParseError>::iterator it = errors.begin(); it != errors.end(); it++ )
        {
            output->addText(it->msg);
        }
    } else if (n)
    {
        execute(n);
    }

    free(n);
}

bool System::is_number(const std::string& s)
{
    return !s.empty() && std::find_if(s.begin(), 
        s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}

void System::lines() {
    for (map<int, ProgramLine *>::iterator it = m_program.begin(); it != m_program.end(); it++ )
    {
        cout << "Processing line: " << it->second->lineNum << endl;
    }
}