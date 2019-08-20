#include "Parser.hpp"

Parser::Parser(Lexer *l) 
{
    m_lexer = l;
}

Parser::Parser(string line) 
{
    m_lexer = new Lexer(line);
    ownLexer = true;
}

Parser::~Parser() 
{
    if (ownLexer) free(m_lexer);
}

Node *Parser::parse()
{
    m_errors.clear();
    return command();
}

Node *Parser::parseStatements(string line) {
    m_errors.clear();
    m_lexer = new Lexer(line);

    LexToken *t = m_lexer->next();
    if (!t || t->type != t_integer) return nullptr;
    Node *result = new Node(nt_lineNumber, t->text);
    free(t);

    t = m_lexer->next();
    result->left = statements(t);
    free(t);

    return result;
}

bool Parser::swallowNext(TokenType type, bool suppressError)
{
    LexToken *t = m_lexer->next();
    if (t && t->type != type)
    {
        m_lexer->pushBack(t);
        if (!suppressError) m_errors.push_back(ParseError("Unexpected token \"" + t->text + "\""));
        return false;
    } else if (!t && type != t_eol) {
        if (!suppressError) m_errors.push_back(ParseError("Unexpecting token; found EOL"));
        return false;
    } else if (t && t->type == type)
    {
        free(t);
    }

    return true;
}

Node *Parser::command()
{
    Node *result = nullptr;

    LexToken *t = m_lexer->next();
    if (!t) return nullptr;

    if (t->type == t_load) result = load(t);
    else if (t->type == t_new) result = new_(t);
    else if (t->type == t_stat) result = stat(t);
    else if (t->type == t_bye) result = bye(t);
    else if (t->type == t_list) result = list(t);
    else if (t->type == t_files) result = files(t);
    else if (t->type == t_save) result = save(t);
    else if (t->type == t_run) result = run(t);
    else if (t->type == t_trun) result = trun(t);
    else result = lines(t);

    free(t);

    return result;
}

Node *Parser::statements(LexToken *token)
{
    Node *result = statement(token);
    Node *currNode = result;
    while (swallowNext(t_colon, true))
    {
        LexToken *t = m_lexer->next();
        currNode->right = statement(t);
        free(t);
        currNode = currNode->right;
    }

    return result;
}

Node *Parser::remark(LexToken *token)
{
    string line = m_lexer->line().substr(m_lexer->currentPos(), string::npos);
    m_lexer->skipToEnd();
    Node *result = new Node(nt_remark, token->text);
    result->left = new Node(nt_string, line);
    return result;
}

Node *Parser::idStmt(LexToken *token)
{
    Node *result = new Node(nt_assign, token->text);
    swallowNext(t_equals);
    LexToken *t = m_lexer->next();
    result->left = expression(t);
    free(t);
    return result;
}

Node *Parser::statement(LexToken *token)
{
    Node *result = new Node(nt_statement, "statement");

    if (token->type == t_print) result->left = print(token);
    else if (token->type == t_goto) result->left = goto_(token);
    else if (token->type == t_rem) result->left = remark(token);
    else if (token->type == t_clear) result->left = clear(token);
    else if (token->type == t_scnclr) result->left = scnclr(token);
    else if (token->type == t_end) result->left = end(token);
    else if (token->type == t_if) result->left = if_(token);
    else if (token->type == t_for)
    {
        result->left = for_(token);
        if (result->left) result->left->left->right = result;
    } 
    else if (token->type == t_next) result->left = next(token);
    else if (token->type == t_gosub)
    {
        result->left = gosub(token);
        if (result->left) result->left->right = result; 
    } 
    else if (token->type == t_return) result->left = return_(token);
    else if (token->type == t_identifier && m_lexer->peek()->type == t_equals) result->left = idStmt(token);
    else 
    {
        free(result);
        result = nullptr;
        m_errors.push_back(ParseError("Unknown token \"" + token->text + "\""));
    }

    return result;
}

Node *Parser::end(LexToken *token) 
{
    return new Node(nt_end, token->text);
}

Node *Parser::clear(LexToken *token) 
{
    return new Node(nt_clear, token->text);
}

Node *Parser::next(LexToken *token)
{
    Node *result = new Node(nt_next, token->text);
    LexToken *t = m_lexer->next();
    if (!t || t->type != t_identifier)
    {
        m_errors.push_back(ParseError("Identifier expect for NEXT"));
        if (t) m_lexer->pushBack(t);
        return nullptr;
    } else result->left = new Node(nt_identifier, t->text);
    free(t);
    return result;
}

Node *Parser::for_(LexToken *token)
{
    Node *result = new Node(nt_for, token->text);
    LexToken *t = m_lexer->next();
    if (!t || t->type != t_identifier)
    {
        m_errors.push_back(ParseError("Identifier expect for FOR"));
        if (t) m_lexer->pushBack(t);
        return nullptr;
    } else result->left = new Node(nt_identifier, t->text);
    free(t);

    swallowNext(t_equals);

    t = m_lexer->next();
    if (t)
    {
        result->right = new Node(nt_to, "to");
        result->right->left = expression(t);
        free(t);
        swallowNext(t_to);
        t = m_lexer->next();
        if (t)
        {
            result->right->right = expression(t);
            free(t);
        }
    }

    return result;
}

Node *Parser::goto_(LexToken *token)
{
    Node *result = new Node(nt_goto, token->text);

    LexToken *t = m_lexer->next();
    if (!t || t->type == t_colon)
    {
        m_errors.push_back(ParseError("Missing parameter for GOTO"));
    } else 
    {
        result->left = expression(t);
    }
    free(t);

    return result;
}

Node *Parser::return_(LexToken *token)
{
    return new Node(nt_return, token->text);
}

Node *Parser::gosub(LexToken *token)
{
    Node *result = new Node(nt_gosub, token->text);

    LexToken *t = m_lexer->next();
    if (!t || t->type == t_colon)
    {
        m_errors.push_back(ParseError("Missing parameter for GOSUB"));
    } else 
    {
        result->left = expression(t);
    }
    free(t);

    return result;
}

Node *Parser::if_(LexToken *token)
{
    Node *result = new Node(nt_if, token->text);

    LexToken *t = m_lexer->next();
    result->left = expression(t);
    free(t);

    t = m_lexer->next();
    if (!t) m_errors.push_back(ParseError("Missing THEN for IF statement"));
    else if (t->type != t_then) m_errors.push_back(ParseError("Expected THEN; found \"" + t->text + "\""));
    else result->right = then(t);
    free(t);

    return result;
}

Node *Parser::then(LexToken *token)
{
    UNUSED(token)

    Node *result = nullptr;
    LexToken *t = m_lexer->next();
    result = statement(t);
    free(t);
    return result;
}

Node *Parser::print(LexToken *token)
{
    Node *result = new Node(nt_print, token->text);

    LexToken *t = m_lexer->next();
    if (!t)
    {
        return result;
    } else if (t->type == t_colon)
    {
        m_lexer->pushBack(t);
    } else 
    {
        result->left = printList(t);
        free(t);
    }

    return result;
}

Node *Parser::printList(LexToken *token)
{
    Node *result = new Node(nt_printlist, "print-list");
    result->left = expression(token);

    if (m_lexer->peek()->type == t_semicolon)
    {
        result->data = "append";
        swallowNext(t_semicolon);
        LexToken *t = m_lexer->next();
        if (t) result->right = printList(t);
        free(t);
    } else if (!m_lexer->eol() && m_lexer->peek()->type != t_colon)
    {
        m_errors.push_back(ParseError("Unexpected token \"" + m_lexer->peek()->text + "\""));
    }

    return result;
}

Node *Parser::andExpr(LexToken *token)
{
    if (!token)
    {
        m_errors.push_back(ParseError("Missing and expression"));
        return nullptr;
    }

    Node *result = nullptr;

    Node *result1 = notExpr(token);

    LexToken *t = m_lexer->next();
    if (t && t->type == t_and)
    {
        result = new Node(nt_and, t->text);
        result->left = result1;
        free(t);
        t = m_lexer->next();
        result->right = andExpr(t);
        free(t);
    } else 
    {
        result = result1;
        if (t) m_lexer->pushBack(t);
    }

    return result;
}

bool isComparitor(TokenType type) 
{
    return (type == t_equals || type == t_notequals || 
        type == t_greater || type == t_greaterequal ||
        type == t_less || type == t_lessequal);
}

NodeType mapComparitor(TokenType type)
{
    if (type == t_equals) return nt_equal;
    if (type == t_notequals) return nt_notequal;
    if (type == t_greater) return nt_greater;
    if (type == t_greaterequal) return nt_greaterequal;
    if (type == t_less) return nt_less;
    if (type == t_lessequal) return nt_lessequal;
    return nt_unknown;
}

Node *Parser::negateExpr(LexToken *token)
{
    Node *result = nullptr;
    if (token->type == t_minus)
    {
        result = new Node(nt_negate, token->text);
        LexToken *t = m_lexer->next();
        result->left = powerExpr(t);
        free(t);
    } else 
    {
        result = powerExpr(token);
    }
    return result;
}

Node *Parser::multExpr(LexToken *token)
{
    if (!token)
    {
        m_errors.push_back(ParseError("Missing mult expression"));
        return nullptr;
    }

    Node *result = nullptr;

    Node *result1 = negateExpr(token);

    LexToken *t = m_lexer->next();
    if (t && (t->type == t_mult || t->type == t_div))
    {
        result = new Node((t->type == t_mult? nt_mult : nt_div), t->text);
        result->left = result1;
        free(t);
        t = m_lexer->next();
        result->right = multExpr(t);
        free(t);
    } else 
    {
        result = result1;
        if (t) m_lexer->pushBack(t);
    }

    return result;
}

Node *Parser::powerExpr(LexToken *token)
{
    Node *result = nullptr;

    Node *result1 = valueExpr(token);

    if (m_lexer->peek() && m_lexer->peek()->type == t_caret)
    {
        result = new Node(nt_power, "^");
        result->left = result1;
        swallowNext(t_caret);
        LexToken *t = m_lexer->next();
        result->right = valueExpr(t);
        free(t);
    } else 
    {
        result = result1;
    }
    return result;
}

Node *Parser::valueExpr(LexToken *token)
{
    Node *result = nullptr;

    if (token->type == t_leftparen)
    {
        LexToken *t = m_lexer->next();
        result = expression(t);
        free(t);
        swallowNext(t_rightparen);
    } else if (token->type == t_identifier)
    {
        result = new Node(nt_identifier, token->text);
    } else if (token->type == t_integer ||
               token->type == t_real ||
               token->type == t_string)
    {
        result = constant(token);
    } else
    {
        m_errors.push_back(ParseError("Unexpected token \"" + token->text + "\""));
    }
    
    return result;
}

Node *Parser::addExpr(LexToken *token)
{
    if (!token)
    {
        m_errors.push_back(ParseError("Missing add expression"));
        return nullptr;
    }

    Node *result = nullptr;

    Node *result1 = multExpr(token);

    LexToken *t = m_lexer->next();
    if (t && (t->type == t_plus || t->type == t_minus))
    {
        result = new Node((t->type == t_plus ? nt_add : nt_minus), t->text);
        result->left = result1;
        free(t);
        t = m_lexer->next();
        result->right = addExpr(t);
        free(t);
    } else 
    {
        result = result1;
        if (t) m_lexer->pushBack(t);
    }

    return result;
}

Node *Parser::compareExpr(LexToken *token)
{
    if (!token)
    {
        m_errors.push_back(ParseError("Missing compare expression"));
        return nullptr;
    }

    Node *result = nullptr;

    Node *result1 = addExpr(token);

    LexToken *t = m_lexer->next();
    if (t && isComparitor(t->type))
    {
        result = new Node(mapComparitor(t->type), t->text);
        result->left = result1;
        free(t);
        t = m_lexer->next();
        result->right = compareExpr(t);
        free(t);
    } else 
    {
        result = result1;
        if (t) m_lexer->pushBack(t);
    }

    return result;
}

Node *Parser::notExpr(LexToken *token)
{
    if (!token)
    {
        m_errors.push_back(ParseError("Missing not expression"));
        return nullptr;
    }

    Node *result = nullptr;
    if (token->type == t_not) 
    {
        result = new Node(nt_not, token->text);
        LexToken *t = m_lexer->next();
        result->left = compareExpr(t);
        free(t);
    } else 
    {
        result = compareExpr(token);
    }

    return result;
}

Node *Parser::constant(LexToken *token)
{
    if (!token) m_errors.push_back(ParseError("Value missing"));
    else if (token->type == t_integer) return integer(token);
    else if (token->type == t_string) return string_(token);
    else if (token->type == t_real) return real(token);
    else m_errors.push_back(ParseError("Invalid value: \"" + token->text + "\""));

    return nullptr;
}

Node *Parser::expression(LexToken *token)
{
    if (!token)
    {
        m_errors.push_back(ParseError("Missing expression"));
        return nullptr;
    }

    Node *result = nullptr;

    Node *result1 = andExpr(token);

    LexToken *t = m_lexer->next();
    if (t && t->type == t_or)
    {
        result = new Node(nt_or, t->text);
        result->left = result1;
        free(t);
        t = m_lexer->next();
        result->right = expression(t);
        free(t);
    } else 
    {
        result = result1;
        m_lexer->pushBack(t);
    }

    return result;
}

Node *Parser::lines(LexToken *token)
{
    Node *result = nullptr;

    if (token && token->type == t_integer)
    {
        result = new Node(nt_line, m_lexer->line());
    } else 
    {
        result = statements(token);
    }

    return result;
}

Node *Parser::save(LexToken *token) 
{
    Node *result = nullptr;

    LexToken *t = m_lexer->next();
    if (!t) {
        m_errors.push_back(ParseError("File name requried for SAVE"));
    } else if (t->type != t_string)
    {
        m_errors.push_back("Incorrect parameter type for SAVE \"" + t->text + "\"");
    } else 
    {
        result = new Node(nt_save, token->text);
        result->right = new Node(nt_string, t->text);
    } 

    free(t);

    swallowNext(t_eol);

    return result;
}

Node *Parser::files(LexToken *token) 
{
    Node *result = new Node(nt_files, token->text);

    LexToken *t = m_lexer->next();
    if (t && t->type == t_string)
    {
        result->right = string_(t);
    } else 
    {
        m_lexer->pushBack(t);
    }
    free(t);

    swallowNext(t_eol);

    return result;
}

Node *Parser::list(LexToken *token)
{
    Node *result = new Node(nt_list, token->text);

    LexToken *t = m_lexer->next();
    if (t) result->right = IntegerRange(t);
    free(t);

    if (swallowNext(t_eol)) return result;
    return nullptr;
}

Node *Parser::integer(LexToken *token)
{
    if (token && token->type == t_integer) return new Node(nt_integer, token->text);
    return nullptr;
}

Node *Parser::real(LexToken *token)
{
    if (token && token->type == t_real) return new Node(nt_real, token->text);
    return nullptr;
}

Node *Parser::string_(LexToken *token)
{
    if (token && token->type == t_string) return new Node(nt_string, token->text);
    return nullptr;
}

Node *Parser::IntegerRange(LexToken *token)
{
    Node *result = nullptr;

    if (token->type == t_integer) 
    {
        if (swallowNext(t_minus, true))  // have a range with Int -
        {
            result = new Node(nt_integerrange, "integer-range");
            result->left = new Node(nt_integer, token->text);

            LexToken *t = m_lexer->next();
            if (t && t->type == t_integer) result->right = integer(t);
            free(t);
        } else // No dash
        {
            result = new Node(nt_integer, token->text);
        }
    } else if (token->type == t_minus)
    {
        result = new Node(nt_integerrange, "integer-range");
        LexToken *t = m_lexer->next();
        if (t && t->type == t_integer) result->right = integer(t);
        free(t);
    } else 
    {
        m_errors.push_back(ParseError("Expected integer or range; found \"" + token->text + "\""));
    }

    return result;
}


Node *Parser::stat(LexToken *token)
{
    UNUSED(token)

    if (swallowNext(t_eol)) return new Node(nt_stat, token->text);
    return nullptr;
}

Node *Parser::new_(LexToken *token)
{
    UNUSED(token)

    if (swallowNext(t_eol)) return new Node(nt_new, token->text);
    return nullptr;
}

Node *Parser::bye(LexToken *token) 
{
    UNUSED(token)

    if (swallowNext(t_eol)) return new Node(nt_bye, token->text);
    return nullptr;
}

Node *Parser::run(LexToken *token) 
{
    UNUSED(token)

    if (swallowNext(t_eol)) return new Node(nt_run, token->text);
    return nullptr;
}

Node *Parser::trun(LexToken *token) 
{
    UNUSED(token)

    if (swallowNext(t_eol)) return new Node(nt_trun, token->text);
    return nullptr;
}

Node *Parser::scnclr(LexToken *token) 
{
    UNUSED(token)

    return new Node(nt_scnclr, token->text);
}

Node *Parser::load(LexToken *token)
{
    Node *result = nullptr;

    LexToken *t = m_lexer->next();
    if (!t) {
        m_errors.push_back(ParseError("File name requried for LOAD"));
    } else if (t->type != t_string)
    {
        m_errors.push_back("Incorrect parameter type for LOAD \"" + t->text + "\"");
    } else 
    {
        result = new Node(nt_load, token->text);
        Node *param = new Node(nt_string, t->text);
        result->right = param;
    } 

    free(t);

    swallowNext(t_eol);

    return result;
}

