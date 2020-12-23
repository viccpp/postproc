#include<postproc/parser.h>
#include<postproc/context.h>
#include<postproc/conditions.h>
#include<postproc/operations.h>
#include<postproc/exprs.h>
#include<postproc/string_list.h>
#include<postproc/functions.h>
#include<postproc/predicates.h>
#include<postproc/hex.h>
#include<mfisoft/ascii.h>
#include<mfisoft/ascii_string.h>
#include<mfisoft/string_buffer.h>
#include<cstring>
#include<cassert>

namespace postproc {

//----------------------------------------------------------------------------
const parser::funcs_t parser::functions[] =
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
const parser::preds_t parser::predicates[] =
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
        if(!is_space(ch)) break;
        inc_pos(is->get()); // skip();
    }
}
//----------------------------------------------------------------------------
const char *parser::reader::end_of_file::what() const noexcept
{
    return "Unexpected end of file";
}
//----------------------------------------------------------------------------
parser::bad_syntax::bad_syntax(const char *m, const reader &r)
    : simple_exception(mfi::msg(128) <<
        "Line " << r.get_line() << " column " << r.get_column() << ": " << m)
{
}
//----------------------------------------------------------------------------
inline bool parser::is_space(char ch)
{
    return mfi::ascii::isspace(ch);
}
//----------------------------------------------------------------------------
inline bool parser::is_digit(char ch)
{
    return mfi::ascii::isdigit(ch);
}
//----------------------------------------------------------------------------
inline bool parser::is_ident_first_char(char ch)
{
    return mfi::ascii::isalpha(ch) || ch == '_';
}
//----------------------------------------------------------------------------
inline bool parser::is_ident_char(char ch)
{
    return is_ident_first_char(ch) || mfi::ascii::isdigit(ch);
}
//----------------------------------------------------------------------------
inline bool parser::is_integer_literal_first_char(char ch)
{
    return ch == '-' || ch == '+' || mfi::ascii::isdigit(ch);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
script parser::parse(std::istream &is, context &ctx)
{
    script pp;
    r.init(is);
    while(read_script_elements(pp, ctx));
    return pp;
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
    unique_ptr<condition> cond;

    // condition
    if(r.peek() != '{')
        cond = read_cond();
    if(r.eof() && cond)
        throw bad_syntax("Condition without action", r);

    // action
    if(r.peek() != '{')
        throw bad_syntax("Invalid condition", r);
    r.skip();
    auto act = read_action();

    pp.add_rule(std::move(cond), std::move(act));
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
unique_ptr<condition> parser::read_cond()
{
    assert(!is_space(r.peek()));
    unique_ptr<condition> cond;
    switch(r.peek())
    {
        case '(':
            cond = read_cond_parens();
            break;
        case '!':
            cond = read_cond_not();
            break;
        case '?': // predicate
            cond = read_cond_predicate();
            break;
        default: // binary cond-op
        {
            auto expr = read_expr();
            r.skip_ws();
            auto op = read_bin_cond_op();
            r.skip_ws();
            cond = read_rest_of_cond(std::move(expr), op);
        }
    }
    r.skip_ws();
    if(r.eof()) return cond;

    // Check for & or |
    // Warning: no priorities
    switch(r.peek())
    {
        case '&':
        {
            r.skip();
            r.skip_ws();
            auto cond2 = read_cond();
            auto cond1 = std::move(cond);
            cond = make_unique<cond_and>(
                std::move(cond1), std::move(cond2));
            break;
        }
        case '|':
        {
            r.skip();
            r.skip_ws();
            auto cond2 = read_cond();
            auto cond1 = std::move(cond);
            cond = make_unique<cond_or>(
                std::move(cond1), std::move(cond2));
            break;
        }
    }
    return cond;
}
//----------------------------------------------------------------------------
unique_ptr<condition> parser::read_cond_atom()
{
    assert(!is_space(r.peek()));
    switch(r.peek())
    {
        case '(': return read_cond_parens();
        case '!': return read_cond_not();
        case '?': return read_cond_predicate();
    }
    throw bad_syntax("Expected (...) or predicate", r);
}
//----------------------------------------------------------------------------
unique_ptr<condition> parser::read_cond_parens()
{
    assert(r.peek() == '(');
    r.skip();
    r.skip_ws();
    auto cond = read_cond();
    r.skip_ws();
    if(r.peek() != ')') throw bad_syntax("Expected \")\"", r);
    r.skip();
    return cond;
}
//----------------------------------------------------------------------------
unique_ptr<condition> parser::read_cond_not()
{
    assert(r.peek() == '!');
    r.skip();
    r.skip_ws();
    auto cond = read_cond_atom();
    return make_unique<cond_not>(std::move(cond));
}
//----------------------------------------------------------------------------
unique_ptr<condition> parser::read_cond_predicate()
{
    assert(r.peek() == '?');
    r.skip();
    r.skip_ws();
    std::string name = read_identifier();
    r.skip_ws();
    if(r.peek() != '(') throw bad_syntax("Expected \"(\"", r);
    r.skip();
    return read_predicate(mfi::ascii::toupper(name));
}
//----------------------------------------------------------------------------
std::string parser::read_bin_cond_op()
{
    switch(r.peek())
    {
        case '=': // ==
            r.skip();
            if(r.peek() != '=') break;
            r.skip();
            return "==";
        case '~': // ~
            r.skip();
            return "~";
        case '!': // != or !~ or !in
            r.skip();
            switch(r.peek())
            {
                case '=':
                    r.skip();
                    return "!=";
                case '~':
                    r.skip();
                    return "!~";
                case 'i':
                case 'I':
                    r.skip();
                    if(mfi::ascii::tolower(r.peek()) != 'n') break;
                    r.skip();
                    return "!in";
            }
            break;
        case 'i':
        case 'I': // in
            r.skip();
            if(mfi::ascii::tolower(r.peek()) != 'n') break;
            r.skip();
            return "in";
    }
    throw bad_syntax("Expected binary conditional operation", r);
}
//----------------------------------------------------------------------------
unique_ptr<condition> parser::read_rest_of_cond(
    unique_ptr<expression> arg1, const std::string &op)
{
    assert(!is_space(r.peek()));
    if(op == "==")
    {
        auto arg2 = read_expr();
        return make_unique<equal>(std::move(arg1), std::move(arg2));
    }
    else if(op == "!=")
    {
        auto arg2 = read_expr();
        return make_unique<not_equal>(std::move(arg1), std::move(arg2));
    }
    else if(op == "in")
    {
        auto strs = read_string_list();
        return make_unique<in_set>(std::move(arg1), std::move(strs));
    }
    else if(op == "!in")
    {
        auto strs = read_string_list();
        return make_unique<not_in_set>(std::move(arg1), std::move(strs));
    }
    else if(op == "~")
    {
        auto re = read_regexp();
        return make_unique<match_regexp>(std::move(arg1), std::move(re));
    }
    else if(op == "!~")
    {
        auto re = read_regexp();
        return make_unique<not_match_regexp>(std::move(arg1), std::move(re));
    }
    // must not be reached!
    throw bad_syntax("Invalid binary conditional operation", r);
}
//----------------------------------------------------------------------------
action parser::read_action()
{
    r.skip_ws();
    action act;
    for(;;)
    {
        act.push_back(read_op());
        r.skip_ws();
        switch(r.peek())
        {
            case '}':
                r.skip();
                return act;
            case ';':
                r.skip();
                r.skip_ws();
                if(r.peek() != '}') break;
                r.skip();
                return act;
            //default:
            //    throw bad_syntax("Expected \"}\" or \";\" or end-of-line", r);
        }
    }
}
//----------------------------------------------------------------------------
unique_ptr<operation> parser::read_op()
{
    std::string ident;

    try {
        ident = read_identifier();
    } catch(const bad_syntax & ) {
        throw bad_syntax(
            "Expected assignment, swap, \"break()\" or \"discard()\"", r);
    }
    mfi::ascii::toupper(ident);
    r.skip_ws();

    if(ident == "DISCARD")
    {
        if(r.peek() == '(')
        {
            skip_parens();
            return make_unique<discard>();
        }
    }
    else if(ident == "BREAK")
    {
        if(r.peek() == '(')
        {
            skip_parens();
            return make_unique<break_>();
        }
    }

    // ident is a field name (assignment or swap op)
    auto f = make_unique<field>(ident);
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
                auto f2 = make_unique<field>(ident);
                return make_unique<swap_op>(std::move(f), std::move(f2));
            }
            // fall through
        }
        case '=': // assignment
        {
            r.skip();
            r.skip_ws();
            auto expr = read_expr();
            return make_unique<assignment>(std::move(f), std::move(expr));
        }
    }
    throw bad_syntax("Expected \"=\", \"<-\" or \"<->\" after field name", r);
}
//----------------------------------------------------------------------------
unique_ptr<expression> parser::read_expr()
{
    auto expr1 = read_simple_expr();
    r.skip_ws();
    if(r.eof() || r.peek() != '.') return expr1;
    // It's concatenation
    r.skip();
    r.skip_ws();
    auto expr2 = read_expr();
    return make_unique<concatenation>(std::move(expr1), std::move(expr2));
}
//----------------------------------------------------------------------------
// Expression without concatenation
unique_ptr<expression> parser::read_simple_expr()
{
    char ch = r.peek();
    assert(!is_space(ch));
    switch(ch)
    {
        case '"': // string literal
        {
            return read_quoted_string();
        }
        case '$': // session constant
        {
            r.skip();
            return make_unique<session_constant>(read_identifier());
        }
    }
    if(is_ident_first_char(ch)) // field or function
    {
        std::string name = read_identifier();
        r.skip_ws();
        if(r.peek() == '(') // function
        {
            r.skip();
            return read_function(mfi::ascii::toupper(name));
        }
        else // field
            return make_unique<field>(name);
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
    assert(!is_space(r.peek()));
    char ch = r.peek();
    if(!is_ident_first_char(ch))
        throw bad_syntax("Expected identifier", r);

    mfi::string_buffer st(64);
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
    return std::move(st);
}
//----------------------------------------------------------------------------
regexp parser::read_regexp()
{
    if(r.peek() != '/')
        throw bad_syntax("Expected regexp", r);
    r.skip();
    mfi::string_buffer st(128), opts;
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
                return regexp(st, opts);
            }
            case '\\':
                if(r.peek() == '/') { ch = '/'; r.skip(); }
        }
        st += ch;
    }
}
//----------------------------------------------------------------------------
unique_ptr<string_list> parser::read_string_list()
{
    r.skip_ws();
    switch(r.peek())
    {
        case '(':
            return read_list_literal();
        case '@':
            r.skip();
            return make_unique<named_list>(read_identifier());
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
        set.push_back(std::move(*read_string_literal()));
        r.skip_ws();
        char ch = r.get();
        if(ch == ')') break;
        if(ch != ',')
            throw bad_syntax("Expected \")\" or \",\" in list", r);
    }
}
//----------------------------------------------------------------------------
unique_ptr<list_literal> parser::read_list_literal()
{
    std::list<string_literal> set;
    read_list_literal(set);
    return make_unique<list_literal>(std::move(set));
}
//----------------------------------------------------------------------------
unique_ptr<string_literal> parser::read_string_literal()
{
    char ch = r.peek();
    assert(!is_space(ch));
    if(ch == '"')
        return read_quoted_string();
    if(is_integer_literal_first_char(ch))
        return read_integer_literal();
    throw bad_syntax("Expected string literal", r);
}
//----------------------------------------------------------------------------
unique_ptr<string_literal> parser::read_quoted_string()
{
    assert(r.peek() == '"');
    r.skip();
    mfi::string_buffer st(128);
    for(;;)
    {
        char ch = r.get();
        switch(ch)
        {
            case '"':
                return make_unique<string_literal>(std::move(st));
            case '\\':
                ch = r.get();
                if(ch == 'x') // HEX escape sequence
                {
                    int d = xdigit_to_number(r.get());
                    if(d < 0) bad_syntax(
                        "Expected HEX digit in escape sequence", r);
                    ch = d;
                    d = xdigit_to_number(r.peek());
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
unique_ptr<integer_literal> parser::read_integer_literal()
{
    char ch = r.get();
    assert(!is_space(ch));
    if(ch == '-' && !is_digit(r.peek()))
        throw bad_syntax("Invalid integer literal", r);
    mfi::string_buffer st(128);
    st = ch;
    try
    {
        while(is_digit(ch = r.peek()))
        {
            st += ch;
            r.skip();
        }
    }
    catch(const reader::end_of_file & )
    {
        // ignore
    }
    return make_unique<integer_literal>(std::move(st));
}
//----------------------------------------------------------------------------
template<class FuncRec, size_t Sz>
bool parser::is_ascending(const FuncRec (&table)[Sz])
{
    static_assert(Sz != 0, "Non-zero-sized array is excepted");
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
unique_ptr<function> parser::read_function(const std::string &name)
{
    auto func = find_func(functions, name);
    if(!func) throw bad_syntax("Unknown function", r);
    return (this->*func)();
}
//----------------------------------------------------------------------------
unique_ptr<predicate> parser::read_predicate(const std::string &name)
{
    auto func = find_func(predicates, name);
    if(!func) throw bad_syntax("Unknown predicate", r);
    return (this->*func)();
}
//----------------------------------------------------------------------------
inline void parser::throw_invalid_func_arg(const reader &r)
{
    throw bad_syntax("Invalid argument list", r);
}
//----------------------------------------------------------------------------
unique_ptr<expression> parser::read_func_arg(char delim)
{
    r.skip_ws();
    auto arg = read_expr();
    r.skip_ws();
    if(r.peek() != delim) throw_invalid_func_arg(r);
    r.skip();
    return arg;
}
//----------------------------------------------------------------------------
regexp parser::read_func_arg_re(char delim)
{
    r.skip_ws();
    auto arg = read_regexp();
    r.skip_ws();
    if(r.peek() != delim) throw_invalid_func_arg(r);
    r.skip();
    return arg;
}
//----------------------------------------------------------------------------
unique_ptr<string_list> parser::read_func_arg_list(char delim)
{
    auto arg = read_string_list();
    r.skip_ws();
    if(r.peek() != delim) throw_invalid_func_arg(r);
    r.skip();
    return arg;
}
//----------------------------------------------------------------------------
unique_ptr<string_literal> parser::read_func_arg_literal(char delim)
{
    r.skip_ws();
    auto str = read_string_literal();
    r.skip_ws();
    if(r.peek() != delim) throw_invalid_func_arg(r);
    r.skip();
    return str;
}
//----------------------------------------------------------------------------
unique_ptr<predicate> parser::compile_pred_empty()
{
    auto arg = read_func_arg();
    return make_unique<predicates::empty>(std::move(arg));
}
//----------------------------------------------------------------------------
unique_ptr<predicate> parser::compile_pred_starts_with()
{
    auto arg1 = read_func_arg(','),
         arg2 = read_func_arg();
    return make_unique<predicates::starts_with>(
        std::move(arg1), std::move(arg2));
}
//----------------------------------------------------------------------------
unique_ptr<predicate> parser::compile_pred_lt()
{
    auto arg1 = read_func_arg(','),
         arg2 = read_func_arg();
    return make_unique<predicates::lt>(std::move(arg1), std::move(arg2));
}
//----------------------------------------------------------------------------
unique_ptr<predicate> parser::compile_pred_gt()
{
    auto arg1 = read_func_arg(','),
         arg2 = read_func_arg();
    return make_unique<predicates::gt>(std::move(arg1), std::move(arg2));
}
//----------------------------------------------------------------------------
unique_ptr<predicate> parser::compile_pred_le()
{
    auto arg1 = read_func_arg(','),
         arg2 = read_func_arg();
    return make_unique<predicates::le>(std::move(arg1), std::move(arg2));
}
//----------------------------------------------------------------------------
unique_ptr<predicate> parser::compile_pred_ge()
{
    auto arg1 = read_func_arg(','),
         arg2 = read_func_arg();
    return make_unique<predicates::ge>(std::move(arg1), std::move(arg2));
}
//----------------------------------------------------------------------------
unique_ptr<function> parser::compile_func_length()
{
    auto arg = read_func_arg();
    return make_unique<functions::length>(std::move(arg));
}
//----------------------------------------------------------------------------
unique_ptr<function> parser::compile_func_substr()
{
    auto arg1 = read_func_arg(',');
    // 2nd arg may be last may be not
    r.skip_ws();
    auto arg2 = read_expr();
    unique_ptr<expression> arg3;
    r.skip_ws();
    if(r.peek() == ')') r.skip(); // only 2 arguments
    else
    {
        if(r.peek() != ',') throw_invalid_func_arg(r);
        r.skip();
        arg3 = read_func_arg();
    }
    return make_unique<functions::substr>(
        std::move(arg1), std::move(arg2), std::move(arg3));
}
//----------------------------------------------------------------------------
unique_ptr<function> parser::compile_func_replace()
{
    auto arg1 = read_func_arg(',');
    auto arg2 = read_func_arg_re(',');
    auto arg3 = read_func_arg();
    return make_unique<functions::replace>(
        std::move(arg1), std::move(arg2), std::move(arg3));
}
//----------------------------------------------------------------------------
unique_ptr<function> parser::compile_func_replace_all()
{
    auto arg1 = read_func_arg(',');
    auto arg2 = read_func_arg_re(',');
    auto arg3 = read_func_arg();
    return make_unique<functions::replace_all>(
        std::move(arg1), std::move(arg2), std::move(arg3));
}
//----------------------------------------------------------------------------
unique_ptr<function> parser::compile_func_upper()
{
    auto arg = read_func_arg();
    return make_unique<functions::upper>(std::move(arg));
}
//----------------------------------------------------------------------------
unique_ptr<function> parser::compile_func_lower()
{
    auto arg = read_func_arg();
    return make_unique<functions::lower>(std::move(arg));
}
//----------------------------------------------------------------------------
unique_ptr<function> parser::compile_func_reverse()
{
    auto arg = read_func_arg();
    return make_unique<functions::reverse>(std::move(arg));
}
//----------------------------------------------------------------------------
template<class F_1Arg, class F_2Arg>
inline unique_ptr<function> parser::compile_trimlike_func()
{
    auto arg = read_expr();
    r.skip_ws();
    if(r.peek() == ')') // only 1 argument
    {
        r.skip();
        return make_unique<F_1Arg>(std::move(arg));
    }
    if(r.peek() != ',') throw_invalid_func_arg(r);
    r.skip();

    // Read second argument
    auto chars = read_func_arg_literal();
    return make_unique<F_2Arg>(std::move(arg), std::move(*chars));
}
//----------------------------------------------------------------------------
unique_ptr<function> parser::compile_func_ltrim()
{
    return compile_trimlike_func<
        functions::ltrim_spaces,
        functions::ltrim
    >();
}
//----------------------------------------------------------------------------
unique_ptr<function> parser::compile_func_rtrim()
{
    return compile_trimlike_func<
        functions::rtrim_spaces,
        functions::rtrim
    >();
}
//----------------------------------------------------------------------------
unique_ptr<function> parser::compile_func_trim()
{
    return compile_trimlike_func<
        functions::trim_spaces,
        functions::trim
    >();
}
//----------------------------------------------------------------------------
unique_ptr<function> parser::compile_func_lpad()
{
    auto arg1 = read_func_arg(','),
         arg2 = read_func_arg(',');
    auto pad_char = read_func_arg_literal();
    if(pad_char->length() != 1)
        throw bad_syntax("Expected one char string literal", r);
    return make_unique<functions::lpad>(
        std::move(arg1), std::move(arg2), pad_char->front());
}
//----------------------------------------------------------------------------
unique_ptr<function> parser::compile_func_hex2dec()
{
    auto arg = read_func_arg();
    return make_unique<functions::hex2dec>(std::move(arg));
}
//----------------------------------------------------------------------------
unique_ptr<function> parser::compile_func_dec2hex()
{
    auto arg = read_func_arg();
    return make_unique<functions::dec2hex>(std::move(arg));
}
//----------------------------------------------------------------------------
unique_ptr<function> parser::compile_func_map()
{
    auto value = read_func_arg(',');
    mapping::list mapping;

    r.skip_ws();
    if(r.peek() != '(') bad_syntax("Expected list literal", r);
    do {
        mapping.emplace_back();
        auto &m = mapping.back();

        read_list_literal(m.first);
        r.skip_ws();
        if(r.peek() != '-' || (r.skip(), r.peek()) != '>')
            bad_syntax("Expected \"->\"", r);
        r.skip();
        r.skip_ws();
        m.second = read_expr();

        r.skip_ws();
        char ch = r.peek();
        if(ch == ')') break;
        if(ch != ',') bad_syntax("Expected \",\" or \")\"", r);
        r.skip();
        r.skip_ws();
    } while(r.peek() == '(');

    unique_ptr<expression> def_value;
    if(r.peek() != ')') // default value is presented
    {
        def_value = read_expr();
        r.skip_ws();
        if(r.peek() != ')') bad_syntax("Expected \")\"", r);
    }
    r.skip(); // )
    return make_unique<functions::map_func>(
        std::move(value), std::move(mapping), std::move(def_value));
}
//----------------------------------------------------------------------------
unique_ptr<function> parser::compile_func_iadd()
{
    auto arg1 = read_func_arg(','),
         arg2 = read_func_arg();
    return make_unique<functions::iadd>(std::move(arg1), std::move(arg2));
}
//----------------------------------------------------------------------------
unique_ptr<function> parser::compile_func_isub()
{
    auto arg1 = read_func_arg(','),
         arg2 = read_func_arg();
    return make_unique<functions::isub>(std::move(arg1), std::move(arg2));
}
//----------------------------------------------------------------------------
unique_ptr<function> parser::compile_func_idiv()
{
    auto arg1 = read_func_arg(','),
         arg2 = read_func_arg();
    return make_unique<functions::idiv>(std::move(arg1), std::move(arg2));
}
//----------------------------------------------------------------------------
unique_ptr<function> parser::compile_func_idiv_ceil()
{
    auto arg1 = read_func_arg(','),
         arg2 = read_func_arg();
    return make_unique<functions::idiv_ceil>(std::move(arg1), std::move(arg2));
}
//----------------------------------------------------------------------------
unique_ptr<function> parser::compile_func_imul()
{
    auto arg1 = read_func_arg(','),
         arg2 = read_func_arg();
    return make_unique<functions::imul>(std::move(arg1), std::move(arg2));
}
//----------------------------------------------------------------------------
unique_ptr<function> parser::compile_func_sift()
{
    auto arg1 = read_func_arg(',');
    auto chars = read_func_arg_literal();
    return make_unique<functions::sift>(std::move(arg1), std::move(*chars));
}
//----------------------------------------------------------------------------
unique_ptr<function> parser::compile_func_sift_nonprint()
{
    auto arg = read_func_arg();
    return make_unique<functions::sift_nonprint>(std::move(arg));
}
//----------------------------------------------------------------------------
unique_ptr<function> parser::compile_func_bit_and()
{
    auto arg1 = read_func_arg(','),
         arg2 = read_func_arg();
    return make_unique<functions::bit_and>(std::move(arg1), std::move(arg2));
}
//----------------------------------------------------------------------------
unique_ptr<function> parser::compile_func_bit_or()
{
    auto arg1 = read_func_arg(','),
         arg2 = read_func_arg();
    return make_unique<functions::bit_or>(std::move(arg1), std::move(arg2));
}
//----------------------------------------------------------------------------
unique_ptr<function> parser::compile_func_bit_shl()
{
    auto arg1 = read_func_arg(','),
         arg2 = read_func_arg();
    return make_unique<functions::bit_shl>(std::move(arg1), std::move(arg2));
}
//----------------------------------------------------------------------------
unique_ptr<function> parser::compile_func_bit_shr()
{
    auto arg1 = read_func_arg(','),
         arg2 = read_func_arg();
    return make_unique<functions::bit_shr>(std::move(arg1), std::move(arg2));
}
//----------------------------------------------------------------------------
unique_ptr<function> parser::compile_func_bit_not()
{
    auto arg = read_func_arg();
    return make_unique<functions::bit_not>(std::move(arg));
}
//----------------------------------------------------------------------------
unique_ptr<function> parser::compile_func_ip6_to_ip4()
{
    auto arg = read_func_arg();
    return make_unique<functions::ip6_to_ip4>(std::move(arg));
}
//----------------------------------------------------------------------------
unique_ptr<function> parser::compile_func_ip4_to_hex()
{
    auto arg = read_func_arg();
    return make_unique<functions::ip4_to_hex>(std::move(arg));
}
//----------------------------------------------------------------------------
unique_ptr<function> parser::compile_func_uuid()
{
    r.skip_ws();
    if(r.peek() != ')') throw_invalid_func_arg(r);
    r.skip();
    return make_unique<functions::uuid>();
}
//----------------------------------------------------------------------------

} // namespace
