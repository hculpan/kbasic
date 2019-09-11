#include "Lexer.hpp"

#include <cctype>
#include <algorithm>

vector<string> functions{"tab", "int", "rnd", "str$", "val", "chr$"};

Lexer::Lexer(string line) 
{
    m_line = line;
}

struct LexToken *Lexer::newToken(string curr, TokenType currType, string text, TokenType type) 
{
    if (curr != "")
    {
        m_currentPos -= text.size();
        return new LexToken(curr, currType);
    } else 
    {
        return new LexToken(text, type);
    }

}

char Lexer::peekAhead() 
{
    if (m_currentPos + 1 >= int(m_line.size())) 
    {
        return '\0';
    } else {
        return m_line.at(m_currentPos);
    }
}

struct LexToken *Lexer::keywords(struct LexToken *token) 
{
    if (token && token->type == t_identifier) 
    {
        string ltext = token->text;
        transform(ltext.begin(), ltext.end(), ltext.begin(),
            [](unsigned char c){ return tolower(c); });

        if      (ltext == "save") token->type = t_save;
        else if (ltext == "load") token->type = t_load;
        else if (ltext == "run") token->type = t_run;
        else if (ltext == "trun") token->type = t_trun;
        else if (ltext == "list") token->type = t_list;
        else if (ltext == "data") token->type = t_data;
        else if (ltext == "for") token->type = t_for;
        else if (ltext == "to") token->type = t_to;
        else if (ltext == "next") token->type = t_next;
        else if (ltext == "step") token->type = t_next;
        else if (ltext == "read") token->type = t_read;
        else if (ltext == "let") token->type = t_let;
        else if (ltext == "print") token->type = t_print;
        else if (ltext == "using") token->type = t_using;
        else if (ltext == "rem") token->type = t_rem;
        else if (ltext == "scnclr") token->type = t_scnclr;
        else if (ltext == "cls") token->type = t_scnclr;
        else if (ltext == "bye") token->type = t_bye;
        else if (ltext == "files") token->type = t_files;
        else if (ltext == "new") token->type = t_new;
        else if (ltext == "stat") token->type = t_stat;
        else if (ltext == "goto") token->type = t_goto;
        else if (ltext == "and") token->type = t_and;
        else if (ltext == "or") token->type = t_or;
        else if (ltext == "not") token->type = t_not;
        else if (ltext == "clear") token->type = t_clear;
        else if (ltext == "end") token->type = t_end;
        else if (ltext == "gosub") token->type = t_gosub;
        else if (ltext == "return") token->type = t_return;
        else if (ltext == "if") token->type = t_if;
        else if (ltext == "then") token->type = t_then;
        else if (ltext == "else") token->type = t_else;
        else if (ltext == "input") token->type = t_input;
        else if (ltext == "open") token->type = t_open;
        else if (ltext == "as") token->type = t_as;
        else if (ltext == "output") token->type = t_output;
        else if (ltext == "close") token->type = t_close;
        else if (ltext == "inkey$") token->type = t_inkey;
        else if (ltext == "getkey") token->type = t_getkey;
        else if (ltext == "data") token->type = t_data;
        else if (ltext == "read") token->type = t_read;
        else if (ltext == "restore") token->type = t_restore;
        else if (ltext == "dim") token->type = t_dim;
        else if (isFunction(ltext)) token->type = t_function;
    } 

    return token;
}

bool Lexer::isFunction(string ltext)
{
    vector<string>::iterator it = find(functions.begin(), functions.end(), ltext);
    return (it != functions.end());
}

void Lexer::skipToEnd()
{
    m_currentPos = m_line.size();
}

bool Lexer::eol()
{
    return (peek() == nullptr || peek()->type == t_eol);
}

struct LexToken *Lexer::peek()
{
    if (m_nextToken) return m_nextToken;

    m_nextToken = nextToken();
    if (!m_nextToken)
    {
        return new LexToken("", t_eol);
    }
    return m_nextToken;
}

struct LexToken *Lexer::next() 
{
    if (m_nextToken)
    {
        LexToken *result = m_nextToken;
        m_nextToken = nullptr;
        return result;
    }

    return nextToken();
}

void Lexer::pushBack(struct LexToken *t)
{
    m_nextToken = t;
}

struct LexToken *Lexer::nextToken() 
{
    LexToken *token = nullptr;
    string curr = "";
    TokenType currType = t_unknown;
    bool inString = false;
    while (m_currentPos < int(m_line.size())) 
    {
        char c = m_line.at(m_currentPos);
        m_currentPos++;

        if (c == '\n' && inString)
        {
            token = new LexToken(curr, t_string);
            m_currentPos--;
            break;
        } else if (c == '\n')
        {
            token = newToken(curr, currType, "\n", TokenType::t_eol);
            break;
        } else if (c == '"' && !inString) 
        {
            if (curr != "") 
            {
                token = new LexToken(curr, currType);
                m_currentPos--;
                break;
            } else {
                inString = true;
            }
        } else if (c == '"' && inString) 
        {
            inString = false;
            token = new LexToken(curr, t_string);
            break;
        } else if (inString)
        {
            curr += c;
        } else if (c == ' ' || c == '\t' || c == '\v' || c == '\f' || c == '\r')
        {
            if (curr != "")
            {
                token = new LexToken(curr, currType);
                break;
            }
            continue;
        } else if (c == '+') 
        {
            token = newToken(curr, currType, "+", TokenType::t_plus);
            break;
        } else if (c == '^') 
        {
            token = newToken(curr, currType, "^", TokenType::t_caret);
            break;
        } else if (c == '@') 
        {
            token = newToken(curr, currType, "@", TokenType::t_at);
            break;
        } else if (c == '-') 
        {
            token = newToken(curr, currType, "-", TokenType::t_dash);
            break;
        } else if (c == ':') 
        {
            token = newToken(curr, currType, ":", TokenType::t_colon);
            break;
        } else if (c == ';') 
        {
            token = newToken(curr, currType, ";", TokenType::t_semicolon);
            break;
        } else if (c == '*') 
        {
            token = newToken(curr, currType, "*", TokenType::t_mult);
            break;
        } else if (c == '/') 
        {
            token = newToken(curr, currType, "/", TokenType::t_div);
            break;
        } else if (c == '<' && peekAhead() == '>')
        {
            token = newToken(curr, currType, "<>", TokenType::t_notequals);
            m_currentPos++; // skip extra character
            break;
        } else if (c == '<' && peekAhead() == '=')
        {
            token = newToken(curr, currType, "<=", TokenType::t_lessequal);
            m_currentPos++; // skip extra character
            break;
        } else if (c == '<')
        {
            token = newToken(curr, currType, "<", TokenType::t_less);
            break;
        } else if (c == '>' && peekAhead() == '<')
        {
            token = newToken(curr, currType, "><", TokenType::t_notequals);
            m_currentPos++; // skip extra character
            break;
        } else if (c == '>' && peekAhead() == '=')
        {
            token = newToken(curr, currType, ">=", TokenType::t_greaterequal);
            m_currentPos++; // skip extra character
            break;
        } else if (c == '>')
        {
            token = newToken(curr, currType, ">", TokenType::t_greater);
            break;
        } else if (c == '(') 
        {
            token = newToken(curr, currType, "(", TokenType::t_leftparen);
            break;
        } else if (c == ')') 
        {
            token = newToken(curr, currType, ")", TokenType::t_rightparen);
            break;
        } else if (c == ',')
        {
            token = newToken(curr, currType, ",", TokenType::t_comma);
            break;
        } else if (c == '#')
        {
            token = newToken(curr, currType, "#", TokenType::t_hash);
            break;
        } else if (c == '.') 
        {
            if (currType == t_integer) 
            {
                curr += '.';
                currType = t_real;
            } else {
                token = newToken(curr, currType, ".", TokenType::t_period);
                break;
            }
        } else if (c == '=') 
        {
            token = newToken(curr, currType, "=", TokenType::t_equals);
            break;
        } else if (isdigit(c)) 
        {
            if (curr == "") {
                currType = t_integer;
            }
            curr += c;
        } else if (isprint(c)) 
        {
            if (curr != "" && currType != t_unknown && currType != t_identifier) 
            {
                m_currentPos--;  // backup so we don't lose first char of text
                token = new LexToken(curr, currType);
                break;
            }
            currType = t_identifier;
            curr += c;
        }
    }

    if (!token && curr != "" && inString) {
        token = new LexToken(curr, t_string);
    } else if (!token && curr != "") {
        token = new LexToken(curr, currType);
    }

    return keywords(token);
}

