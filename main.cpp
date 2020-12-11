#include<mfisoft/january/memory.h>
#include<mfisoft/january/string.h>
#include<mfisoft/january/error.h>
#include<iterator>
#include<iostream>
#include<fstream>
#include<sstream>
#include<exception>
#include<map>
#include<string>
#include<memory>
#include<postproc/script.h>
#include<postproc/parser.h>
#include<postproc/conditions.h>
#include<postproc/operations.h>
#include<postproc/expr.h>

//////////////////////////////////////////////////////////////////////////////
class postproc_constants : public postproc::session_constant_extractor
{
    unsigned long count;
public:
    explicit postproc_constants(unsigned long c = 1) : count(c) {}

    void inc_counter() { ++count; }

    // override
    std::string get(const std::string &name) const
    {
        if(jan::equal_icase(name, "RECNO"))
            return jan::string_buffer(8) << count;
        throw_unknown_session_constant(name);
    }
};
//////////////////////////////////////////////////////////////////////////////

void run();

int main()
{
    try
    {
        run();
        return 0;
    }
    catch(const std::exception &ex)
    {
        std::cerr << ex.what() << '\n';
    }
    return 1;
}
void make_script(postproc::script &pp)
{
    // call_type == "CONNECTION" { digits = called_number }
    // call_type == "MOBILE_ORIGINATED" {
    //    digits = calling_number;
    //    record_id = $RECNO;
    // }
    using namespace postproc;

    {
        std::auto_ptr<expression>
            cond_arg1( new field("CALL_TYPE") ),
            cond_arg2( new string_literal("CONNECTION") );

        std::auto_ptr<condition> cond( new equal(cond_arg1, cond_arg2) );

        std::auto_ptr<field> f( new field("DIGITS") );
        std::auto_ptr<expression> expr( new field("CALLED_NUMBER") );

        action act;
        act.push_back( new assignment(f, expr) );

        pp.add_rule(cond, act);
    }
    {
        std::auto_ptr<expression>
            cond_arg1( new field("CALL_TYPE") ),
            cond_arg2( new string_literal("MOBILE_ORIGINATED") );

        std::auto_ptr<condition> cond( new equal(cond_arg1, cond_arg2) );

        std::auto_ptr<field> f( new field("DIGITS") );
        std::auto_ptr<expression> expr( new field("CALLING_NUMBER") );

        action act;
        act.push_back( new assignment(f, expr) );

        f.reset( new field("RECORD_ID") );
        expr.reset( new session_constant("RECNO") );
        act.push_back( new assignment(f, expr) );

        pp.add_rule(cond, act);
    }
}
void parse_script(std::istream &is,
    postproc::script &pp, postproc::context &ctx)
{
    postproc::parser p;
    p.parse(is, pp, ctx);
}
void parse_script(const std::string &st,
    postproc::script &pp, postproc::context &ctx)
{
    std::istringstream is(st);
    parse_script(is, pp, ctx);
}
void run()
{
    postproc::map in_fields;

    in_fields["CALL_TYPE"] = "MOBILE_ORIGINATED";
    in_fields["CALLED_NUMBER"]  = "9041234578";
    in_fields["CALLING_NUMBER"] = "9107557896";
    in_fields["DURATION"] = "360";

    //postproc::map out_fields(in_fields);

    postproc::script pp;
    postproc_constants ppc;
    postproc::context ctx(ppc);

    //make_script(pp);
    std::ifstream script("script");
    if(!script.is_open()) throw jan::exception("Couldn't open script file");
    parse_script(script, pp, ctx);
    std::cout << std::distance(pp.begin(), pp.end()) << " rules\n\n";

    if(pp.process(in_fields, in_fields/*out_fields*/, ctx))
    {
        for(postproc::map::const_iterator it =
                    //out_fields.begin(); it != out_fields.end(); ++it)
                    in_fields.begin(); it != in_fields.end(); ++it)
            std::cout << it->first << " : " << it->second << '\n';
    }
    else
        std::cout << "Rejected\n";
}
