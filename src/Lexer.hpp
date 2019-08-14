#ifndef _LEXER_HPP_
#define _LEXER_HPP_

#include "main.hpp"

enum TokenType {
    t_unknown, t_identifier, t_string, t_integer, t_real, t_keyword, t_period, t_colon,
    t_plus, t_minus, t_mult, t_div, t_leftparen, t_rightparen, t_equals, t_eol, t_caret,
    t_save, t_load, t_run, t_list, t_scnclr, t_bye, t_files, t_new, t_stat,
    t_data, t_for, t_to, t_next, t_read, t_let, t_print, t_rem
};

struct LexToken {
    std::string text;
    TokenType type;

    LexToken(std::string text, TokenType type) {
        this->text = text;
        this->type = type;
    }
};

class Lexer {
    public:
        Lexer(string line);

        struct LexToken *next();
        struct LexToken *peek();
        bool eol();

    private:
        string m_line;
        int m_currentPos = 0;
        LexToken *m_nextToken = nullptr;

        struct LexToken *newToken(string curr, TokenType currType, string text, TokenType type);
        struct LexToken *nextToken();
        char peekAhead();
        struct LexToken *keywords(struct LexToken *token);
};

#endif