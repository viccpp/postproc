#ifndef __POSTPROC_PARSER_H
#define __POSTPROC_PARSER_H

#include<mfisoft/january/defs.h>
#include<mfisoft/january/string_buffer.h>
#include<postproc/script.h>
#include<istream>
#include<memory>
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
class parser : private jan::non_copyable
{
    class reader
    {
        std::istream *is;
        unsigned line_no, col_no;
        void inc_pos(char );
    public:
        struct end_of_file : public std::exception
        {
            const char *what() const throw();
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
    static const name2func_t<function*(parser::*)()> functions[];
    static const name2func_t<predicate*(parser::*)()> predicates[];

    reader r;

    static bool is_ident_first_char(char );
    static bool is_ident_char(char );
    static bool is_integer_literal_first_char(char );

    template<class FuncRec, size_t Sz>
    static bool is_ascending(const FuncRec (&)[Sz]);
    template<class FuncRec, size_t Sz>
    static typename FuncRec::func_t find_func(
        const FuncRec (&)[Sz], const std::string & );

    PP_NORETURN static void throw_invalid_func_arg(const reader & );
    template<class , class > function *compile_trimlike_func();

    predicate *compile_pred_empty();
    predicate *compile_pred_starts_with();
    predicate *compile_pred_lt();
    predicate *compile_pred_gt();
    predicate *compile_pred_le();
    predicate *compile_pred_ge();

    function *compile_func_length();
    function *compile_func_substr();
    function *compile_func_replace();
    function *compile_func_replace_all();
    function *compile_func_upper();
    function *compile_func_lower();
    function *compile_func_reverse();
    function *compile_func_ltrim();
    function *compile_func_rtrim();
    function *compile_func_trim();
    function *compile_func_lpad();
    function *compile_func_hex2dec();
    function *compile_func_dec2hex();
    function *compile_func_map();
    function *compile_func_iadd();
    function *compile_func_isub();
    function *compile_func_idiv();
    function *compile_func_idiv_ceil();
    function *compile_func_imul();
    function *compile_func_sift();
    function *compile_func_sift_nonprint();
    function *compile_func_bit_and();
    function *compile_func_bit_or();
    function *compile_func_bit_shl();
    function *compile_func_bit_shr();
    function *compile_func_bit_not();
    function *compile_func_ip6_to_ip4();
    function *compile_func_ip4_to_hex();
    function *compile_func_uuid();

    bool read_script_elements(script & , context & );
    void read_rule(script & );
    void read_named_list_decl(context & );
    bool skip_comment();
    void skip_parens();
    condition *read_cond();
    condition *read_cond_parens();
    condition *read_cond_not();
    condition *read_cond_predicate();
    condition *read_cond_atom();
    void read_bin_cond_op(std::string & );
    condition *read_rest_of_cond(
        std::auto_ptr<expression> & , const std::string & );
    void read_action(action & );
    operation *read_op();
    expression *read_expr();
    expression *read_simple_expr();
    std::string read_identifier();
    regexp *read_regexp();
    string_list *read_string_list();
    list_literal *read_list_literal();
    void read_list_literal(std::list<string_literal> & );
    string_literal *read_string_literal();
    string_literal *read_quoted_string();
    integer_literal *read_integer_literal();
    std::auto_ptr<expression>  read_func_arg(char = ')');
    std::auto_ptr<regexp>      read_func_arg_re(char = ')');
    std::auto_ptr<string_list> read_func_arg_list(char = ')');
    std::auto_ptr<string_literal> read_func_arg_literal(char = ')');
    predicate *read_predicate(const std::string & );
    function *read_function(const std::string & );
public:
    class bad_syntax : public std::exception
    {
        jan::static_string msg;
    public:
        explicit bad_syntax(const char * , const reader & );
        const char *what() const throw();
    };

    void parse(std::istream & , script & , context & );
};
//////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // header guard
