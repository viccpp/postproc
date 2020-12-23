#ifndef POSTPROC_PARSER_H
#define POSTPROC_PARSER_H

#include<postproc/defs.h>
#include<postproc/regexp.h>
#include<postproc/script.h>
#include<istream>
#include<list>

namespace postproc {

// forward declarations
class condition;
class operation;
class expression;
class string_literal;
class integer_literal;
class string_list;
class list_literal;
class function;
class predicate;

//////////////////////////////////////////////////////////////////////////////
class parser
{
    class reader
    {
        std::istream *is;
        unsigned line_no, col_no;
        void inc_pos(char );
    public:
        struct end_of_file : public std::exception
        {
            const char *what() const noexcept;
        };

        void init(std::istream &s) { is = &s; line_no = col_no = 1; }

        void skip_ws();
        void skip();
        char peek();
        char get();

        char eof() const { return is->eof(); }
        unsigned get_line() const { return line_no; }
        unsigned get_column() const { return col_no; }
    };

    template<class FuncPtr>
    struct name2func_t
    {
        typedef FuncPtr func_t;
        const char *name;
        FuncPtr func;
    };
    typedef name2func_t<unique_ptr<function>(parser::*)()> funcs_t;
    typedef name2func_t<unique_ptr<predicate>(parser::*)()> preds_t;
    static const funcs_t functions[];
    static const preds_t predicates[];

    reader r;

    static bool is_space(char );
    static bool is_digit(char );
    static bool is_ident_first_char(char );
    static bool is_ident_char(char );
    static bool is_integer_literal_first_char(char );

    template<class FuncRec, size_t Sz>
    static bool is_ascending(const FuncRec (&)[Sz]);
    template<class FuncRec, size_t Sz>
    static typename FuncRec::func_t find_func(
        const FuncRec (&)[Sz], const std::string & );

    [[noreturn]] static void throw_invalid_func_arg(const reader & );
    template<class , class >
    unique_ptr<function> compile_trimlike_func();

    unique_ptr<predicate> compile_pred_empty();
    unique_ptr<predicate> compile_pred_starts_with();
    unique_ptr<predicate> compile_pred_lt();
    unique_ptr<predicate> compile_pred_gt();
    unique_ptr<predicate> compile_pred_le();
    unique_ptr<predicate> compile_pred_ge();

    unique_ptr<function> compile_func_length();
    unique_ptr<function> compile_func_substr();
    unique_ptr<function> compile_func_replace();
    unique_ptr<function> compile_func_replace_all();
    unique_ptr<function> compile_func_upper();
    unique_ptr<function> compile_func_lower();
    unique_ptr<function> compile_func_reverse();
    unique_ptr<function> compile_func_ltrim();
    unique_ptr<function> compile_func_rtrim();
    unique_ptr<function> compile_func_trim();
    unique_ptr<function> compile_func_lpad();
    unique_ptr<function> compile_func_hex2dec();
    unique_ptr<function> compile_func_dec2hex();
    unique_ptr<function> compile_func_map();
    unique_ptr<function> compile_func_iadd();
    unique_ptr<function> compile_func_isub();
    unique_ptr<function> compile_func_idiv();
    unique_ptr<function> compile_func_idiv_ceil();
    unique_ptr<function> compile_func_imul();
    unique_ptr<function> compile_func_sift();
    unique_ptr<function> compile_func_sift_nonprint();
    unique_ptr<function> compile_func_bit_and();
    unique_ptr<function> compile_func_bit_or();
    unique_ptr<function> compile_func_bit_shl();
    unique_ptr<function> compile_func_bit_shr();
    unique_ptr<function> compile_func_bit_not();
    unique_ptr<function> compile_func_ip6_to_ip4();
    unique_ptr<function> compile_func_ip4_to_hex();
    unique_ptr<function> compile_func_uuid();

    bool read_script_elements(script & , context & );
    void read_rule(script & );
    void read_named_list_decl(context & );
    bool skip_comment();
    void skip_parens();
    unique_ptr<condition> read_cond();
    unique_ptr<condition> read_cond_parens();
    unique_ptr<condition> read_cond_not();
    unique_ptr<condition> read_cond_predicate();
    unique_ptr<condition> read_cond_atom();
    std::string read_bin_cond_op();
    unique_ptr<condition> read_rest_of_cond(
        unique_ptr<expression> , const std::string & );
    action read_action();
    unique_ptr<operation> read_op();
    unique_ptr<expression> read_expr();
    unique_ptr<expression> read_simple_expr();
    std::string read_identifier();
    regexp read_regexp();
    unique_ptr<string_list> read_string_list();
    unique_ptr<list_literal> read_list_literal();
    void read_list_literal(std::list<string_literal> & );
    unique_ptr<string_literal> read_string_literal();
    unique_ptr<string_literal> read_quoted_string();
    unique_ptr<integer_literal> read_integer_literal();
    unique_ptr<expression>  read_func_arg(char = ')');
    regexp                  read_func_arg_re(char = ')');
    unique_ptr<string_list> read_func_arg_list(char = ')');
    unique_ptr<string_literal> read_func_arg_literal(char = ')');
    unique_ptr<predicate> read_predicate(const std::string & );
    unique_ptr<function> read_function(const std::string & );
public:
    struct bad_syntax : public simple_exception
    {
        bad_syntax(const char * , const reader & );
    };

    parser() = default;
    parser(const parser &) = delete;
    parser &operator=(const parser &) = delete;

    script parse(std::istream & , context & );
};
//////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // header guard
