#include<postproc/parser.h>
#include<postproc/context.h>
#include<postproc/conditions.h>
#include<postproc/operations.h>
#include<postproc/exprs.h>
#include<postproc/string_list.h>
#include<postproc/functions.h>
#include<postproc/predicates.h>
#include<mfisoft/january/string.h>
#include<mfisoft/january/memory.h>
#include<mfisoft/january/bits.h>
#include<cstring>
#include<cassert>
#include<cctype>

namespace postproc {

//----------------------------------------------------------------------------
const parser::name2func_t<function*(parser::*)()> parser::functions[] =
{ // !! keep sorted by name
    { "BIT_AND", &parser::compile_func_bit_and },
    { "BIT_NOT", &parser::compile_func_bit_not },
    { "BIT_OR",  &parser::compile_func_bit_or },
    { "BIT_SHL", &parser::compile_func_bit_shl },
    { "BIT_SHR", &parser::compile_func_bit_shr },
    { "DEC2HEX", &parser::compile_func_dec2hex },
    { "HEX2DEC", &parser::compile_func_hex2dec },
    { "IADD", &parser::compile_func_iadd },
    { "IDIV", &parser::compile_func_idiv },
    { "IDIV_CEIL", &parser::compile_func_idiv_ceil },
    { "IMUL", &parser::compile_func_imul },
    { "IP4_TO_HEX", &parser::compile_func_ip4_to_hex },
    { "IP6_TO_IP4", &parser::compile_func_ip6_to_ip4 },
    { "ISUB", &parser::compile_func_isub },
    { "LENGTH", &parser::compile_func_length },
    { "LOWER", &parser::compile_func_lower },
    { "LPAD", &parser::compile_func_lpad },
    { "LTRIM", &parser::compile_func_ltrim },
    { "MAP", &parser::compile_func_map },
    { "REPLACE", &parser::compile_func_replace },
    { "REPLACE_ALL", &parser::compile_func_replace_all },
    { "REVERSE", &parser::compile_func_reverse },
    { "RTRIM", &parser::compile_func_rtrim },
    { "SIFT", &parser::compile_func_sift },
    { "SIFT_NONPRINT", &parser::compile_func_sift_nonprint },
    { "SUBSTR", &parser::compile_func_substr },
    { "TRIM", &parser::compile_func_trim },
    { "UPPER", &parser::compile_func_upper },
    { "UUID", &parser::compile_func_uuid }
};
//----------------------------------------------------------------------------
const parser::name2func_t<predicate*(parser::*)()> parser::predicates[] =
{ // !! keep sorted by name
    { "EMPTY", &parser::compile_pred_empty },
    { "GE", &parser::compile_pred_ge },
    { "GT", &parser::compile_pred_gt },
    { "LE", &parser::compile_pred_le },
    { "LT", &parser::compile_pred_lt },
    { "STARTS_WITH", &parser::compile_pred_starts_with }
};
//----------------------------------------------------------------------------
inline void parser::reader::inc_pos(char ch)
{
    if(ch == '\n')
    {
        line_no++;
        col_no = 1;
    }
    else col_no++;
}
//----------------------------------------------------------------------------
// Precondion: at least one char in stream is available
void parser::reader::skip()
{
    inc_pos(is->get());
    assert(bool(*is));
}
//----------------------------------------------------------------------------
char parser::reader::get()
{
    int ch = is->get();
    if(!std::char_traits<char>::not_eof(ch)) throw end_of_file();
    inc_pos(ch);
    return ch;
}
//----------------------------------------------------------------------------
char parser::reader::peek()
{
    int ch = is->peek();
    if(!std::char_traits<char>::not_eof(ch)) throw end_of_file();
    return ch;
}
//----------------------------------------------------------------------------
void parser::reader::skip_ws()
{
    if(eof()) return;
    for(;;)
    {
        int ch = is->peek();
        if(!std::char_traits<char>::not_eof(ch)) break;
        if(!std::isspace(ch)) break;
        inc_pos(is->get()); // skip();
    }
}
//----------------------------------------------------------------------------
const char *parser::reader::end_of_file::what() const throw()
{
    return "Unexpected end of file";
}
//----------------------------------------------------------------------------
parser::bad_syntax::bad_syntax(const char *m, const reader &r)
    : msg(jan::msg(128) <<
        "Line " << r.get_line() << " column " << r.get_column() << ": " << m)
{
}
//----------------------------------------------------------------------------
const char *parser::bad_syntax::what() const throw()
{
    return msg;
}
//----------------------------------------------------------------------------
inline bool parser::is_ident_first_char(char ch)
{
    return std::isalpha(ch) || ch == '_';
}
//----------------------------------------------------------------------------
inline bool parser::is_ident_char(char ch)
{
    return is_ident_first_char(ch) || std::isdigit(ch);
}
//----------------------------------------------------------------------------
inline bool parser::is_integer_literal_first_char(char ch)
{
    return ch == '-' || ch == '+' || std::isdigit(ch);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void parser::parse(std::istream &is, script &pp, context &ctx)
{
    r.init(is);
    while(read_script_elements(pp, ctx));
}
//----------------------------------------------------------------------------
bool parser::read_script_elements(script &pp, context &ctx)
{
    do {
        r.skip_ws();
        if(r.eof()) return false;
    } while(skip_comment());

    if(r.peek() == '@') read_named_list_decl(ctx);
    else read_rule(pp);
    return true;
}
//----------------------------------------------------------------------------
void parser::read_rule(script &pp)
{
    std::auto_ptr<condition> cond;
    action act;

    // condition
    if(r.peek() != '{')
        cond.reset(read_cond());
    if(r.eof() && cond.get())
        throw bad_syntax("Condition without action", r);

    // action
    if(r.peek() != '{')
        throw bad_syntax("Invalid condition", r);
    r.skip();
    read_action(act);

    pp.add_rule(cond, act);
}
//----------------------------------------------------------------------------
void parser::read_named_list_decl(context &ctx)
{
    assert(r.peek() == '@');
    r.skip();
    std::string name = read_identifier();
    r.skip_ws();
    if(r.peek() != '=') throw bad_syntax("Expected \"=\"", r);
    r.skip();
    r.skip_ws();
    if(r.peek() != '(') throw bad_syntax("Expected \"(\"", r);
    std::list<string_literal> set;
    read_list_literal(set);
    ctx.add_list(name, set);
}
//----------------------------------------------------------------------------
bool parser::skip_comment()
{
    if(r.peek() != '#') return false;
    try {
        while(r.get() != '\n');
    } catch(const reader::end_of_file & ) {}
    return true;
}
//----------------------------------------------------------------------------
void parser::skip_parens()
{
    assert(r.peek() == '(');
    r.skip(); // skip current '('
    r.skip_ws();
    if(r.peek() != ')') throw bad_syntax("Expected \")\"", r);
    r.skip();
}
//----------------------------------------------------------------------------
condition *parser::read_cond()
{
    assert(!std::isspace(r.peek()));
    std::auto_ptr<condition> cond;
    switch(r.peek())
    {
        case '(':
            cond.reset(read_cond_parens());
            break;
        case '!':
            cond.reset(read_cond_not());
            break;
        case '?': // predicate
            cond.reset(read_cond_predicate());
            break;
        default: // binary cond-op
        {
            std::auto_ptr<expression> expr(read_expr());
            r.skip_ws();
            jan::string_buffer op(3);
            read_bin_cond_op(op);
            r.skip_ws();
            cond.reset( read_rest_of_cond(expr, op) );
        }
    }
    r.skip_ws();
    if(r.eof()) return cond.release();

    // Check for & or |
    // Warning: no priorities
    switch(r.peek())
    {
        case '&':
        {
            r.skip();
            r.skip_ws();
            std::auto_ptr<condition> cond2(read_cond());
            std::auto_ptr<condition> cond1(cond);
            cond.reset( new cond_and(cond1, cond2) );
            break;
        }
        case '|':
        {
            r.skip();
            r.skip_ws();
            std::auto_ptr<condition> cond2(read_cond());
            std::auto_ptr<condition> cond1(cond);
            cond.reset( new cond_or(cond1, cond2) );
            break;
        }
    }
    return cond.release();
}
//----------------------------------------------------------------------------
condition *parser::read_cond_atom()
{
    assert(!std::isspace(r.peek()));
    switch(r.peek())
    {
        case '(': return read_cond_parens();
        case '!': return read_cond_not();
        case '?': return read_cond_predicate();
    }
    throw bad_syntax("Expected (...) or predicate", r);
}
//----------------------------------------------------------------------------
condition *parser::read_cond_parens()
{
    assert(r.peek() == '(');
    r.skip();
    r.skip_ws();
    std::auto_ptr<condition> cond(read_cond());
    r.skip_ws();
    if(r.peek() != ')') throw bad_syntax("Expected \")\"", r);
    r.skip();
    return cond.release();
}
//----------------------------------------------------------------------------
condition *parser::read_cond_not()
{
    assert(r.peek() == '!');
    r.skip();
    r.skip_ws();
    std::auto_ptr<condition> cond(read_cond_atom());
    return new cond_not(cond);
}
//----------------------------------------------------------------------------
condition *parser::read_cond_predicate()
{
    assert(r.peek() == '?');
    r.skip();
    r.skip_ws();
    std::string name = read_identifier();
    r.skip_ws();
    if(r.peek() != '(') throw bad_syntax("Expected \"(\"", r);
    r.skip();
    return read_predicate(jan::toupper(name));
}
//----------------------------------------------------------------------------
void parser::read_bin_cond_op(std::string &op)
{
    switch(r.peek())
    {
        case '=': // ==
            r.skip();
            if(r.peek() != '=') break;
            r.skip();
            op = "==";
            return;
        case '~': // ~
            r.skip();
            op = "~";
            return;
        case '!': // != or !~ or !in
            r.skip();
            switch(r.peek())
            {
                case '=':
                    r.skip();
                    op = "!=";
                    return;
                case '~':
                    r.skip();
                    op = "!~";
                    return;
                case 'i':
                case 'I':
                    r.skip();
                    if(std::tolower(r.peek()) != 'n') break;
                    r.skip();
                    op = "!in";
                    return;
            }
            break;
        case 'i':
        case 'I': // in
            r.skip();
            if(std::tolower(r.peek()) != 'n') break;
            r.skip();
            op = "in";
            return;
    }
    throw bad_syntax("Expected binary conditional operation", r);
}
//----------------------------------------------------------------------------
condition *parser::read_rest_of_cond(
    std::auto_ptr<expression> &arg1, const std::string &op)
{
    assert(!std::isspace(r.peek()));
    if(op == "==")
    {
        std::auto_ptr<expression> arg2(read_expr());
        return new equal(arg1, arg2);
    }
    else if(op == "!=")
    {
        std::auto_ptr<expression> arg2(read_expr());
        return new not_equal(arg1, arg2);
    }
    else if(op == "in")
    {
        std::auto_ptr<string_list> strs(read_string_list());
        return new in_set(arg1, strs);
    }
    else if(op == "!in")
    {
        std::auto_ptr<string_list> strs(read_string_list());
        return new not_in_set(arg1, strs);
    }
    else if(op == "~")
    {
        std::auto_ptr<regexp> re(read_regexp());
        return new match_regexp(arg1, re);
    }
    else if(op == "!~")
    {
        std::auto_ptr<regexp> re(read_regexp());
        return new not_match_regexp(arg1, re);
    }
    // must not be reached!
    throw bad_syntax("Invalid binary conditional operation", r);
}
//----------------------------------------------------------------------------
void parser::read_action(action &act)
{
    r.skip_ws();
    for(;;)
    {
        act.push_back(read_op());
        r.skip_ws();
        switch(r.peek())
        {
            case '}':
                r.skip();
                return;
            case ';':
                r.skip();
                r.skip_ws();
                if(r.peek() != '}') break;
                r.skip();
                return;
            //default:
            //    throw bad_syntax("Expected \"}\" or \";\" or end-of-line", r);
        }
    }
}
//----------------------------------------------------------------------------
operation *parser::read_op()
{
    std::string ident;

    try {
        ident = read_identifier();
    } catch(const bad_syntax & ) {
        throw bad_syntax(
            "Expected assignment, swap, \"break()\" or \"discard()\"", r);
    }
    jan::toupper(ident);
    r.skip_ws();

    if(ident == "DISCARD")
    {
        if(r.peek() == '(') { skip_parens(); return new discard; }
    }
    else if(ident == "BREAK")
    {
        if(r.peek() == '(') { skip_parens(); return new break_; }
    }

    // ident is a field name (assignment or swap op)
    std::auto_ptr<field> f( new field(ident) );
    switch(r.peek())
    {
        case '<': // swap or assignment...
        {
            r.skip();
            if(r.peek() != '-') break;
            r.skip();
            if(r.peek() == '>') // swap
            {
                r.skip();
                r.skip_ws();
                ident.clear();
                ident = read_identifier();
                std::auto_ptr<field> f2( new field(ident) );
                return new swap_op(f, f2);
            }
            // fall through
        }
        case '=': // assignment
        {
            r.skip();
            r.skip_ws();
            std::auto_ptr<expression> expr(read_expr());
            return new assignment(f, expr);
        }
    }
    throw bad_syntax("Expected \"=\", \"<-\" or \"<->\" after field name", r);
}
//----------------------------------------------------------------------------
expression *parser::read_expr()
{
    std::auto_ptr<expression> expr1( read_simple_expr() );
    r.skip_ws();
    if(r.eof() || r.peek() != '.') return expr1.release();
    // It's concatenation
    r.skip();
    r.skip_ws();
    std::auto_ptr<expression> expr2( read_expr() );
    return new concatenation(expr1, expr2);
}
//----------------------------------------------------------------------------
// Expression without concatenation
expression *parser::read_simple_expr()
{
    char ch = r.peek();
    assert(!std::isspace(ch));
    switch(ch)
    {
        case '"': // string literal
        {
            return read_quoted_string();
        }
        case '$': // session constant
        {
            r.skip();
            return new session_constant(read_identifier());
        }
    }
    if(is_ident_first_char(ch)) // field or function
    {
        std::string name = read_identifier();
        r.skip_ws();
        if(r.peek() == '(') // function
        {
            r.skip();
            return read_function(jan::toupper(name));
        }
        else // field
            return new field(name);
    }
    if(is_integer_literal_first_char(ch)) // integer literal
    {
        return read_integer_literal();
    }
    throw bad_syntax("Expected expression", r);
}
//----------------------------------------------------------------------------
std::string parser::read_identifier()
{
    assert(!std::isspace(r.peek()));
    char ch = r.peek();
    if(!is_ident_first_char(ch))
        throw bad_syntax("Expected identifier", r);

    jan::string_buffer st(64);
    try
    {
        do {
            r.skip();
            st += ch;
        } while(is_ident_char(ch = r.peek()));
    }
    catch(const reader::end_of_file & )
    {
        // ignore
    }
    return st;
}
//----------------------------------------------------------------------------
regexp *parser::read_regexp()
{
    if(r.peek() != '/')
        throw bad_syntax("Expected regexp", r);
    r.skip();
    jan::string_buffer st(128), opts(4);
    for(;;)
    {
        char ch = r.get();
        switch(ch)
        {
            case '/':
            {
                // Look for regexp options
                while(std::strchr("imsx", ch = r.peek()))
                {
                    r.skip();
                    opts += ch;
                }
                return new regexp(st, opts);
            }
            case '\\':
                if(r.peek() == '/') { ch = '/'; r.skip(); }
        }
        st += ch;
    }
}
//----------------------------------------------------------------------------
string_list *parser::read_string_list()
{
    r.skip_ws();
    switch(r.peek())
    {
        case '(':
            return read_list_literal();
        case '@':
            r.skip();
            return new named_list(read_identifier());
        default:
            throw bad_syntax("Expected list", r);
    }
}
//----------------------------------------------------------------------------
void parser::read_list_literal(std::list<string_literal> &set)
{
    assert(r.peek() == '(');
    r.skip();
    for(;;)
    {
        r.skip_ws();
        std::auto_ptr<string_literal> s(read_string_literal());
        set.push_back(*s);
        s.release();
        r.skip_ws();
        char ch = r.get();
        if(ch == ')') break;
        if(ch != ',')
            throw bad_syntax("Expected \")\" or \",\" in list", r);
    }
}
//----------------------------------------------------------------------------
list_literal *parser::read_list_literal()
{
    std::list<string_literal> set;
    read_list_literal(set);
    return new list_literal(set);
}
//----------------------------------------------------------------------------
string_literal *parser::read_string_literal()
{
    char ch = r.peek();
    assert(!std::isspace(ch));
    if(ch == '"')
        return read_quoted_string();
    if(is_integer_literal_first_char(ch))
    {
        return read_integer_literal();
    }
    throw bad_syntax("Expected string literal", r);
}
//----------------------------------------------------------------------------
string_literal *parser::read_quoted_string()
{
    assert(r.peek() == '"');
    r.skip();
    jan::string_buffer st(128);
    for(;;)
    {
        char ch = r.get();
        switch(ch)
        {
            case '"':
                return new string_literal(st);
            case '\\':
                ch = r.get();
                if(ch == 'x') // HEX escape sequence
                {
                    int d = jan::hex_to_number(r.get());
                    if(d < 0) bad_syntax(
                        "Expected HEX digit in escape sequence", r);
                    ch = d;
                    d = jan::hex_to_number(r.peek());
                    if(d > 0)
                    {
                        r.skip();
                        ch <<= 4; ch |= d;
                    }
                }
                break;
        }
        st += ch;
    }
}
//----------------------------------------------------------------------------
integer_literal *parser::read_integer_literal()
{
    char ch = r.get();
    assert(!std::isspace(ch));
    if(ch == '-' && !std::isdigit(r.peek()))
        throw bad_syntax("Invalid integer literal", r);
    jan::string_buffer st(128);
    st = ch;
    try
    {
        while(std::isdigit(ch = r.peek()))
        {
            st += ch;
            r.skip();
        }
    }
    catch(const reader::end_of_file & )
    {
        // ignore
    }
    return new integer_literal(st);
}
//----------------------------------------------------------------------------
template<class FuncRec, size_t Sz>
bool parser::is_ascending(const FuncRec (&table)[Sz])
{
    typedef char assert_array_has_non_zero_size[Sz != 0 ? 1 : -1];
    const FuncRec *p = table + 1, *end = table + Sz;
    for(; p != end; ++p)
        if(std::strcmp((p-1)->name, p->name) >= 0) return false;
    return true;
}
//----------------------------------------------------------------------------
template<class FuncRec, size_t Sz>
inline typename FuncRec::func_t parser::find_func(
    const FuncRec (&table)[Sz], const std::string &name)
{
    assert(is_ascending(table));
    size_t l = 0, r = Sz - 1;
    while(l <= r)
    {
        size_t i = (l + r) / 2;
        const FuncRec *p = table + i;
        int cmp = name.compare(p->name);
        if(cmp == 0) return p->func; // *p->name == name
        if(cmp > 0) l = i + 1;       // *p->name < name
        else r = i - 1;              // *p->name > name
    }
    return nullptr;
}
//----------------------------------------------------------------------------
function *parser::read_function(const std::string &name)
{
    function *(parser::*func)() = find_func(functions, name);
    if(!func) throw bad_syntax("Unknown function", r);
    return (this->*func)();
}
//----------------------------------------------------------------------------
predicate *parser::read_predicate(const std::string &name)
{
    predicate *(parser::*func)() = find_func(predicates, name);
    if(!func) throw bad_syntax("Unknown predicate", r);
    return (this->*func)();
}
//----------------------------------------------------------------------------
inline void parser::throw_invalid_func_arg(const reader &r)
{
    throw bad_syntax("Invalid argument list", r);
}
//----------------------------------------------------------------------------
std::auto_ptr<expression> parser::read_func_arg(char delim)
{
    r.skip_ws();
    std::auto_ptr<expression> arg(read_expr());
    r.skip_ws();
    if(r.peek() != delim) throw_invalid_func_arg(r);
    r.skip();
    return arg;
}
//----------------------------------------------------------------------------
std::auto_ptr<regexp> parser::read_func_arg_re(char delim)
{
    r.skip_ws();
    std::auto_ptr<regexp> arg(read_regexp());
    r.skip_ws();
    if(r.peek() != delim) throw_invalid_func_arg(r);
    r.skip();
    return arg;
}
//----------------------------------------------------------------------------
std::auto_ptr<string_list> parser::read_func_arg_list(char delim)
{
    std::auto_ptr<string_list> arg(read_string_list());
    r.skip_ws();
    if(r.peek() != delim) throw_invalid_func_arg(r);
    r.skip();
    return arg;
}
//----------------------------------------------------------------------------
std::auto_ptr<string_literal> parser::read_func_arg_literal(char delim)
{
    r.skip_ws();
    std::auto_ptr<string_literal> str(read_string_literal());
    r.skip_ws();
    if(r.peek() != delim) throw_invalid_func_arg(r);
    r.skip();
    return str;
}
//----------------------------------------------------------------------------
predicate *parser::compile_pred_empty()
{
    std::auto_ptr<expression> arg = read_func_arg();
    return new predicates::empty(arg);
}
//----------------------------------------------------------------------------
predicate *parser::compile_pred_starts_with()
{
    std::auto_ptr<expression> arg1 = read_func_arg(','),
                              arg2 = read_func_arg();
    return new predicates::starts_with(arg1, arg2);
}
//----------------------------------------------------------------------------
predicate *parser::compile_pred_lt()
{
    std::auto_ptr<expression> arg1 = read_func_arg(','),
                              arg2 = read_func_arg();
    return new predicates::lt(arg1, arg2);
}
//----------------------------------------------------------------------------
predicate *parser::compile_pred_gt()
{
    std::auto_ptr<expression> arg1 = read_func_arg(','),
                              arg2 = read_func_arg();
    return new predicates::gt(arg1, arg2);
}
//----------------------------------------------------------------------------
predicate *parser::compile_pred_le()
{
    std::auto_ptr<expression> arg1 = read_func_arg(','),
                              arg2 = read_func_arg();
    return new predicates::le(arg1, arg2);
}
//----------------------------------------------------------------------------
predicate *parser::compile_pred_ge()
{
    std::auto_ptr<expression> arg1 = read_func_arg(','),
                              arg2 = read_func_arg();
    return new predicates::ge(arg1, arg2);
}
//----------------------------------------------------------------------------
function *parser::compile_func_length()
{
    std::auto_ptr<expression> arg = read_func_arg();
    return new functions::length(arg);
}
//----------------------------------------------------------------------------
function *parser::compile_func_substr()
{
    std::auto_ptr<expression> arg1 = read_func_arg(',');
    // 2nd arg may be last may be not
    r.skip_ws();
    std::auto_ptr<expression> arg2(read_expr()), arg3;
    r.skip_ws();
    if(r.peek() == ')') r.skip(); // only 2 arguments
    else
    {
        if(r.peek() != ',') throw_invalid_func_arg(r);
        r.skip();
        arg3 = read_func_arg();
    }
    return new functions::substr(arg1, arg2, arg3);
}
//----------------------------------------------------------------------------
function *parser::compile_func_replace()
{
    std::auto_ptr<expression> arg1 = read_func_arg(',');
    std::auto_ptr<regexp>     arg2 = read_func_arg_re(',');
    std::auto_ptr<expression> arg3 = read_func_arg();
    return new functions::replace(arg1, arg2, arg3);
}
//----------------------------------------------------------------------------
function *parser::compile_func_replace_all()
{
    std::auto_ptr<expression> arg1 = read_func_arg(',');
    std::auto_ptr<regexp>     arg2 = read_func_arg_re(',');
    std::auto_ptr<expression> arg3 = read_func_arg();
    return new functions::replace_all(arg1, arg2, arg3);
}
//----------------------------------------------------------------------------
function *parser::compile_func_upper()
{
    std::auto_ptr<expression> arg = read_func_arg();
    return new functions::upper(arg);
}
//----------------------------------------------------------------------------
function *parser::compile_func_lower()
{
    std::auto_ptr<expression> arg = read_func_arg();
    return new functions::lower(arg);
}
//----------------------------------------------------------------------------
function *parser::compile_func_reverse()
{
    std::auto_ptr<expression> arg = read_func_arg();
    return new functions::reverse(arg);
}
//----------------------------------------------------------------------------
template<class F_1Arg, class F_2Arg>
inline function *parser::compile_trimlike_func()
{
    std::auto_ptr<expression> arg(read_expr());
    r.skip_ws();
    if(r.peek() == ')') // only 1 argument
    {
        r.skip();
        return new F_1Arg(arg);
    }
    if(r.peek() != ',') throw_invalid_func_arg(r);
    r.skip();

    // Read second argument
    std::auto_ptr<string_literal> chars = read_func_arg_literal();
    return new F_2Arg(arg, *chars);
}
//----------------------------------------------------------------------------
function *parser::compile_func_ltrim()
{
    return compile_trimlike_func<
        functions::ltrim_spaces,
        functions::ltrim
    >();
}
//----------------------------------------------------------------------------
function *parser::compile_func_rtrim()
{
    return compile_trimlike_func<
        functions::rtrim_spaces,
        functions::rtrim
    >();
}
//----------------------------------------------------------------------------
function *parser::compile_func_trim()
{
    return compile_trimlike_func<
        functions::trim_spaces,
        functions::trim
    >();
}
//----------------------------------------------------------------------------
function *parser::compile_func_lpad()
{
    std::auto_ptr<expression> arg1 = read_func_arg(','),
                              arg2 = read_func_arg(',');
    std::auto_ptr<string_literal> pad_char = read_func_arg_literal();
    if(pad_char->length() != 1)
        throw bad_syntax("Expected one char string literal", r);
    return new functions::lpad(arg1, arg2, *pad_char->begin());
}
//----------------------------------------------------------------------------
function *parser::compile_func_hex2dec()
{
    std::auto_ptr<expression> arg = read_func_arg();
    return new functions::hex2dec(arg);
}
//----------------------------------------------------------------------------
function *parser::compile_func_dec2hex()
{
    std::auto_ptr<expression> arg = read_func_arg();
    return new functions::dec2hex(arg);
}
//----------------------------------------------------------------------------
namespace {
template<class Pair>
struct pair_second_deleter {
    void operator()(const Pair &p) const { delete p.second; } };
}
//----------------------------------------------------------------------------
function *parser::compile_func_map()
{
    std::auto_ptr<expression> value = read_func_arg(',');
    typedef std::list<std::pair<std::list<string_literal>,const expression *> >
        mapping_t;
    mapping_t mapping;
    jan::pointer_container_guard<mapping_t,
        pair_second_deleter<mapping_t::value_type> > mapping_guard(mapping);

    r.skip_ws();
    if(r.peek() != '(') bad_syntax("Expected list literal", r);
    do {
        mapping.resize(mapping.size() + 1);
        mapping.back().second = nullptr;

        read_list_literal(mapping.back().first);
        r.skip_ws();
        if(r.peek() != '-' || (r.skip(), r.peek()) != '>')
            bad_syntax("Expected \"->\"", r);
        r.skip();
        r.skip_ws();
        mapping.back().second = read_expr();

        r.skip_ws();
        char ch = r.peek();
        if(ch == ')') break;
        if(ch != ',') bad_syntax("Expected \",\" or \")\"", r);
        r.skip();
        r.skip_ws();
    } while(r.peek() == '(');

    std::auto_ptr<expression> def_value;
    if(r.peek() != ')') // default value is presented
    {
        def_value.reset(read_expr());
        r.skip_ws();
        if(r.peek() != ')') bad_syntax("Expected \")\"", r);
    }
    r.skip(); // )
    return new functions::map_func(value, mapping, def_value);
}
//----------------------------------------------------------------------------
function *parser::compile_func_iadd()
{
    std::auto_ptr<expression> arg1 = read_func_arg(','),
                              arg2 = read_func_arg();
    return new functions::iadd(arg1, arg2);
}
//----------------------------------------------------------------------------
function *parser::compile_func_isub()
{
    std::auto_ptr<expression> arg1 = read_func_arg(','),
                              arg2 = read_func_arg();
    return new functions::isub(arg1, arg2);
}
//----------------------------------------------------------------------------
function *parser::compile_func_idiv()
{
    std::auto_ptr<expression> arg1 = read_func_arg(','),
                              arg2 = read_func_arg();
    return new functions::idiv(arg1, arg2);
}
//----------------------------------------------------------------------------
function *parser::compile_func_idiv_ceil()
{
    std::auto_ptr<expression> arg1 = read_func_arg(','),
                              arg2 = read_func_arg();
    return new functions::idiv_ceil(arg1, arg2);
}
//----------------------------------------------------------------------------
function *parser::compile_func_imul()
{
    std::auto_ptr<expression> arg1 = read_func_arg(','),
                              arg2 = read_func_arg();
    return new functions::imul(arg1, arg2);
}
//----------------------------------------------------------------------------
function *parser::compile_func_sift()
{
    std::auto_ptr<expression> arg1 = read_func_arg(',');
    std::auto_ptr<string_literal> chars = read_func_arg_literal();
    return new functions::sift(arg1, *chars);
}
//----------------------------------------------------------------------------
function *parser::compile_func_sift_nonprint()
{
    std::auto_ptr<expression> arg = read_func_arg();
    return new functions::sift_nonprint(arg);
}
//----------------------------------------------------------------------------
function *parser::compile_func_bit_and()
{
    std::auto_ptr<expression> arg1 = read_func_arg(','),
                              arg2 = read_func_arg();
    return new functions::bit_and(arg1, arg2);
}
//----------------------------------------------------------------------------
function *parser::compile_func_bit_or()
{
    std::auto_ptr<expression> arg1 = read_func_arg(','),
                              arg2 = read_func_arg();
    return new functions::bit_or(arg1, arg2);
}
//----------------------------------------------------------------------------
function *parser::compile_func_bit_shl()
{
    std::auto_ptr<expression> arg1 = read_func_arg(','),
                              arg2 = read_func_arg();
    return new functions::bit_shl(arg1, arg2);
}
//----------------------------------------------------------------------------
function *parser::compile_func_bit_shr()
{
    std::auto_ptr<expression> arg1 = read_func_arg(','),
                              arg2 = read_func_arg();
    return new functions::bit_shr(arg1, arg2);
}
//----------------------------------------------------------------------------
function *parser::compile_func_bit_not()
{
    std::auto_ptr<expression> arg = read_func_arg();
    return new functions::bit_not(arg);
}
//----------------------------------------------------------------------------
function *parser::compile_func_ip6_to_ip4()
{
    std::auto_ptr<expression> arg = read_func_arg();
    return new functions::ip6_to_ip4(arg);
}
//----------------------------------------------------------------------------
function *parser::compile_func_ip4_to_hex()
{
    std::auto_ptr<expression> arg = read_func_arg();
    return new functions::ip4_to_hex(arg);
}
//----------------------------------------------------------------------------
function *parser::compile_func_uuid()
{
    r.skip_ws();
    if(r.peek() != ')') throw_invalid_func_arg(r);
    r.skip();
    return new functions::uuid;
}
//----------------------------------------------------------------------------

} // namespace
