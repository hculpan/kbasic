#ifndef _PARSER_HPP_
#define _PARSER_HPP_

#include "Lexer.hpp"

#include <vector>

enum NodeType {
    nt_unknown, nt_command, nt_lines, nt_statements, nt_statement, nt_newline,
    nt_load, nt_new, nt_stat, nt_bye, nt_scnclr, nt_list, nt_integerrange,
    nt_files, nt_save, nt_print, nt_line, nt_run, nt_goto, nt_and, nt_or,
    nt_integer, nt_string, nt_real, nt_not, nt_equal, nt_notequal, nt_greater,
    nt_greaterequal, nt_less, nt_lessequal, nt_minus, nt_add, nt_mult, nt_div, 
    nt_negate, nt_leftparen, nt_rightparen, nt_power, nt_identifier, nt_remark,
    nt_assign, nt_clear, nt_end, nt_semicolon, nt_printlist, nt_gosub, nt_lineNumber,
    nt_return, nt_if, nt_then, nt_trun, nt_for, nt_next, nt_step, nt_to, nt_function,
    nt_input
};

struct Node {
    string text;
    NodeType type;

    string data = "";

    Node *left = nullptr;
    Node *right = nullptr;

    Node(NodeType type, string text) 
    {
        this->type = type;
        this->text = text;
    }

    ~Node()
    {
        free(right);
        free(left);
    }
};

struct ParseError {
    string msg = "";
    int lineNo = 0;

    ParseError(string msg, int lineNo = 0)
    {
        this->msg = msg;
        this->lineNo = lineNo;
    }

    // Copy constructor
    ParseError(const ParseError &p2) 
    {
        msg = p2.msg;
        lineNo = p2.lineNo;
    }
};

class Parser {
    public:
        Parser() {} ;
        Parser(Lexer *l);
        Parser(string line);

        ~Parser();

        Node *parse();
        Node *parseStatements(string line);
        vector<ParseError> errors() { return m_errors; }
        bool hasErrors() { return m_errors.size() > 0; }

    private:
        Lexer *m_lexer;
        bool ownLexer = false;
        vector<ParseError> m_errors;

        bool swallowNext(TokenType type, bool suppressError = false);

        Node *command();
        Node *load(LexToken *token);
        Node *new_(LexToken *token);
        Node *stat(LexToken *token);
        Node *bye(LexToken *token);
        Node *scnclr(LexToken *token);
        Node *list(LexToken *token);
        Node *IntegerRange(LexToken *token);
        Node *integer(LexToken *token);
        Node *files(LexToken *token);
        Node *string_(LexToken *token);
        Node *save(LexToken *token);
        Node *lines(LexToken *token);
        Node *statements(LexToken *token);
        Node *statement(LexToken *token);
        Node *print(LexToken *token);
        Node *printList(LexToken *token);
        Node *expression(LexToken *token);
        Node *real(LexToken *token);
        Node *run(LexToken *token);
        Node *trun(LexToken *token);
        Node *goto_(LexToken *token);
        Node *andExpr(LexToken *token);
        Node *notExpr(LexToken *token);
        Node *funcExpr(LexToken *token);
        Node *compareExpr(LexToken *token);
        Node *addExpr(LexToken *token);
        Node *multExpr(LexToken *token);
        Node *negateExpr(LexToken *token);
        Node *powerExpr(LexToken *token);
        Node *valueExpr(LexToken *token);
        Node *constant(LexToken *token);
        Node *remark(LexToken *token);
        Node *idStmt(LexToken *token);
        Node *clear(LexToken *token);
        Node *end(LexToken *token);
        Node *gosub(LexToken *token);
        Node *return_(LexToken *token);
        Node *if_(LexToken *token);
        Node *then(LexToken *token);
        Node *for_(LexToken *token);
        Node *next(LexToken *token);
        Node *input(LexToken *token);
};

#endif