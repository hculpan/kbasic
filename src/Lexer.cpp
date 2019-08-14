#include "Lexer.hpp"

#include <cctype>

Lexer::Lexer(string line) 
{
    m_line = line;
}

struct LexToken *Lexer::newToken(string curr, TokenType currType, string text, TokenType type) 
{
    if (curr != "")
    {
        m_currentPos--;
        return new LexToken(curr, currType);
    } else 
    {
        return new LexToken(text, type);
    }

}

char Lexer::peekAhead() 
{
    if (m_currentPos >= int(m_line.size())) 
    {
        return '\0';
    } else {
        return m_line.at(m_currentPos + 1);
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
        else if (ltext == "list") token->type = t_list;
        else if (ltext == "data") token->type = t_data;
        else if (ltext == "for") token->type = t_for;
        else if (ltext == "to") token->type = t_to;
        else if (ltext == "next") token->type = t_next;
        else if (ltext == "read") token->type = t_read;
        else if (ltext == "let") token->type = t_let;
        else if (ltext == "print") token->type = t_print;
        else if (ltext == "rem") token->type = t_rem;
        else if (ltext == "scnclr") token->type = t_scnclr;
        else if (ltext == "cls") token->type = t_scnclr;
        else if (ltext == "bye") token->type = t_bye;
        else if (ltext == "files") token->type = t_files;
        else if (ltext == "new") token->type = t_new;
        else if (ltext == "stat") token->type = t_stat;
    } 

    return token;
}

bool Lexer::eol()
{
    return (peek() == nullptr || peek()->type == t_eol);
}

struct LexToken *Lexer::peek()
{
    if (m_nextToken) return m_nextToken;

    m_nextToken = nextToken();
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
        } else if (c == '-') 
        {
            token = newToken(curr, currType, "-", TokenType::t_minus);
            break;
        } else if (c == ':') 
        {
            token = newToken(curr, currType, ":", TokenType::t_colon);
            break;
        } else if (c == '*') 
        {
            token = newToken(curr, currType, "+", TokenType::t_mult);
            break;
        } else if (c == '/') 
        {
            token = newToken(curr, currType, "+", TokenType::t_div);
            break;
        } else if (c == '(') 
        {
            token = newToken(curr, currType, "+", TokenType::t_leftparen);
            break;
        } else if (c == ')') 
        {
            token = newToken(curr, currType, "+", TokenType::t_rightparen);
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
        } else if (isalpha(c)) 
        {
            if (curr != "" && currType != t_unknown && currType != t_identifier) 
            {
                m_currentPos--;  // backup so we don't lose first char of text
                token = new LexToken(curr, currType);
                break;
            }
            currType = t_identifier;
            curr += c;
        } else if (isdigit(c)) 
        {
            if (curr == "") {
                currType = t_integer;
            }
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

