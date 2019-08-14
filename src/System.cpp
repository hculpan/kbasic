#include "System.hpp"

#include <iostream>
#include <iterator>
#include <sstream>
#include <fstream>
#include <vector>
#include <climits>

#include <dirent.h>


System *core = new System();

System::System() {

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

void System::files(Lexer *l) 
{
    if (!l->peek()) {
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir (".")) != NULL) {
            /* print all the files and directories within directory */
            while ((ent = readdir (dir)) != NULL) {
                if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")) m_output->addText(ent->d_name);
            }
            closedir (dir);
        } else {
            /* could not open directory */
            m_output->addText("Error: Could not read current directory");
        }
    } else if (l->peek()->type != t_string)
    {
        LexToken *t = l->next();
        m_output->addText("Syntax error: Incorrect parameter with LOAD \"" + t->text + "\"");
        free(t);
    } else 
    {
        LexToken *t = l->next();
        m_output->addText("LOADING " + t->text);
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir (t->text.c_str())) != NULL) {
            /* print all the files and directories within directory */
            while ((ent = readdir (dir)) != NULL) {
                if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")) m_output->addText(ent->d_name);
            }
            closedir (dir);
        } else {
            /* could not open directory */
            m_output->addText("Error: Could not read directory \"" + t->text + "\"");
        }
        free(t);
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

void System::loadCodeLine(Lexer *l, string line) 
{
    UNUSED(l)

    istringstream iss(line);
    vector<string> results((istream_iterator<string>(iss)),
                            istream_iterator<string>());
    
    if (is_number(results[0])) {
        if (results.size() == 1) {
            program.erase(stoi(results[0]));
        } else {
            program[stoi(results[0])] = line;
        }
    }
}

void System::load(Lexer *l)
{
    if (!l->peek()) {
        m_output->addText("Syntax error: Require parameter with LOAD");
    } else if (l->peek()->type != t_string)
    {
        LexToken *t = l->next();
        m_output->addText("Syntax error: Incorrect parameter with LOAD \"" + t->text + "\"");
        free(t);
    } else 
    {
        program.clear();
        LexToken *t = l->next();
        m_output->addText("Loading \"" + t->text + "\"");
        string line;
        ifstream myfile (t->text);
        if (myfile.is_open())
        {
            while ( getline (myfile,line) )
            {
                loadCodeLine(l, line);
            }
            myfile.close();
            m_output->addText("Load complete, " + to_string(program.size()) + " lines loaded.");
        } else 
        {
            m_output->addText("Unable to open file \"" + t->text + "\"");
        }

        free(t);
    } 
}

void System::list(Lexer *l) 
{
    int startIndex = 0;
    int endIndex = INT_MAX;
    LexToken *t = l->peek();
    if (t && t->type == t_integer)
    {
        startIndex = stoi(t->text);
        endIndex = startIndex;
        swallowNext(l);
        if (l->peek())
        {
            if (l->peek()->type != t_minus)
            {
                t = l->next();
                m_output->addText("Syntax error: Invalid token \"" + t->text + "\"");
                free(t);
                return;
            } else
            {
                swallowNext(l);
                if (l->peek()) 
                {
                    t = l->next();
                    if (t->type != t_integer) 
                    {
                        m_output->addText("Syntax error: Invalid token \"" + t->text + "\"");
                        free(t);
                        return;
                    } else 
                    {
                        endIndex = stoi(t->text);
                        free(t);
                    }
                } else 
                {
                    endIndex = INT_MAX;
                }
            }
        }
    } else if (t && t->type == t_minus)
    {
        swallowNext(l);
        if (!l->peek())
        {
            m_output->addText("Syntax error: No end given for range");
            return;
        } else if (l->peek()->type != t_integer)
        {
            t = l->next();
            m_output->addText("Syntax error: Invalid token \"" + t->text + "\"");
            free(t);
            return;
        } else 
        {
            t = l->next();
            endIndex = stoi(t->text);
            free(t);
        }
    }


    if (!checkEol(l)) 
    {
        LexToken *t = l->next();
        m_output->addText("Syntax error: Extra token(s) with LIST starting with \"" + t->text + "\"");
        free(t);
        return;
    } 

    for (map<int, string>::iterator it = program.begin(); it != program.end(); it++ )
    {
        if (it->first >= startIndex && it->first <= endIndex) m_output->addText(it->second);
    }
}

void System::command(string line, ConsoleOutput *output) {
    this->m_output = output;

    Lexer *l = new Lexer(line);
    LexToken *t = l->next();
    if (t->type == t_integer) 
    {
        free(t);
        loadCodeLine(l, line);
    } else if (t->type == t_list)
    {
        free(t);
        list(l);
    } else if (t->type == t_scnclr)
    {
        free(t);
        if (!checkEol(l)) {
            t = l->next();
            m_output->addText("Syntax error: Extra token(s) with SCNCLR starting with " + t->text);
            free(t);
        } else 
        {
            m_output->clearText();
         } 
    } else if (t->type == t_bye)
    {
        free(t);
        if (!checkEol(l)) {
            t = l->next();
            m_output->addText("Syntax error: Extra token(s) with BYE starting with " + t->text);
            free(t);
        } else 
        {
            m_output->terminate();
         } 
    } else if (t->type == t_load)
    {
        free(t);
        load(l);
    } else if (t->type == t_files) 
    {
        free(t);
        files(l);
    } else if (t->type == t_new) 
    {
        free(t);
        program.clear();
        m_output->addText("Ok");
    } else if (t->type == t_stat) 
    {
        free(t);
        m_output->addText("Program lines in memory: " + to_string(program.size()));
    } else 
    {
        free(t);
        m_output->addText("Unknown command: \"" + line + "\"");
    }

    free(l);

/*
    } else {
        istringstream iss(line);
        vector<string> results((istream_iterator<string>(iss)),
                                istream_iterator<string>());
        
        if (is_number(results[0])) {
            if (results.size() == 1) {
                program.erase(stoi(results[0]));
            } else {
                program[stoi(results[0])] = line;
            }
        } else if (line == "cls") {
        } else if (line == "test") {
            Lexer *l = new Lexer("10 PRINT \"Hello, world!:This is a test\"\n20 print \"Another string\":REM this is a remark");
            LexToken *t = l->next();
            while (t)
            {
                m_output->addText(t->text + " [" + to_string(t->type) + "]");
                free(t);
                t = l->next();
            }
            free(l);
        } else if (line == "bye") {
            m_output->terminate();
        } else {
            map<int, string> program_backup = program;
            program.clear();
            program[0] = line;
            lines();
            program = program_backup;
        }*/

}

bool System::is_number(const std::string& s)
{
    return !s.empty() && std::find_if(s.begin(), 
        s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}

void System::lines() {
    for (map<int, string>::iterator it = program.begin(); it != program.end(); it++ )
    {
        cout << "Processing line: " << it->second << endl;
    }
}