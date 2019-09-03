#ifndef _LEXER_HPP_
#define _LEXER_HPP_

#include "main.hpp"

enum TokenType {
    t_unknown, t_identifier, t_string, t_integer, t_real, t_keyword, t_period, t_colon,
    t_plus, t_dash, t_mult, t_div, t_leftparen, t_rightparen, t_equals, t_eol, t_caret,
    t_save, t_load, t_run, t_list, t_scnclr, t_bye, t_files, t_new, t_stat, t_and, t_or,
    t_notequals, t_greater, t_less, t_greaterequal, t_lessequal, t_step, t_comma,
    t_data, t_for, t_to, t_next, t_read, t_let, t_print, t_rem, t_goto, t_not,
    t_clear, t_end, t_semicolon, t_gosub, t_return, t_if, t_then, t_trun, t_function,
    t_input, t_at, t_open, t_as, t_output, t_hash, t_close, t_inkey, t_getkey, t_restore
};

 extern vector<string> functions;

struct LexToken {
    std::string text;
    TokenType type;

    LexToken(string text, TokenType type) {
        this->text = text;
        this->type = type;
    }
};

class Lexer {
    public:
        Lexer(string line);

        struct LexToken *next();
        struct LexToken *peek();
        string line() const { return m_line; }
        bool eol();
        void skipToEnd();
        int currentPos() { return m_currentPos; }
        void pushBack(struct LexToken *t);

    private:
        string m_line;
        int m_currentPos = 0;
        LexToken *m_nextToken = nullptr;

        bool isFunction(string ltext);

        struct LexToken *newToken(string curr, TokenType currType, string text, TokenType type);
        struct LexToken *nextToken();
        char peekAhead();
        struct LexToken *keywords(struct LexToken *token);
};

#endif