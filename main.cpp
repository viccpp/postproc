#include<mfisoft/ascii_string.h>
#include<mfisoft/error.h>
#include<iterator>
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<exception>
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

    std::string get(const std::string &name) const override
    {
        if(mfi::ascii::equal_icase(name, "RECNO"))
            return std::to_string(count);
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
postproc::script make_script()
{
    // call_type == "CONNECTION" { digits = called_number }
    // call_type == "MOBILE_ORIGINATED" {
    //    digits = calling_number;
    //    record_id = $RECNO;
    // }
    using namespace postproc;
    script pp;

    pp.add_rule(
        make_unique<equal>(
            make_unique<field>("CALL_TYPE"),
            make_unique<string_literal>("CONNECTION")
        ),
        action{
            make_unique<assignment>(
                make_unique<field>("DIGITS"),
                make_unique<field>("CALLED_NUMBER")
            )
        }
    );
    pp.add_rule(
        make_unique<equal>(
            make_unique<field>("CALL_TYPE"),
            make_unique<string_literal>("MOBILE_ORIGINATED")
        ),
        action{
            make_unique<assignment>(
                make_unique<field>("DIGITS"),
                make_unique<field>("CALLING_NUMBER")
            ),
            make_unique<assignment>(
                make_unique<field>("RECORD_ID"),
                make_unique<session_constant>("RECNO")
            )
        }
    );
    return pp;
}
postproc::script parse_script(std::istream &is, postproc::context &ctx)
{
    postproc::parser p;
    return p.parse(is, ctx);
}
postproc::script parse_script(const std::string &st, postproc::context &ctx)
{
    std::istringstream is(st);
    return parse_script(is, ctx);
}
void run()
{
    postproc::map in_fields =
    {
        {"CALL_TYPE", "MOBILE_ORIGINATED"},
        {"CALLED_NUMBER", "9041234578"},
        {"CALLING_NUMBER", "9107557896"},
        {"DURATION", "360"}
    };
    //postproc::map out_fields(in_fields);

    postproc_constants ppc;
    postproc::context ctx(ppc);

    //auto pp = make_script();
    std::ifstream script("script");
    if(!script.is_open()) throw mfi::exception("Couldn't open script file");
    auto pp = parse_script(script, ctx);
    std::cout << std::distance(pp.begin(), pp.end()) << " rules\n\n";

    if(pp.process(in_fields, in_fields/*out_fields*/, ctx))
    {
        for(auto &f : in_fields) //out_fields
            std::cout << f.first << " : " << f.second << '\n';
    }
    else
        std::cout << "Rejected\n";
}
