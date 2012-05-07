// boost 1.32

#define BOOST_SPIRIT_DUMP_PARSETREE_AS_XML

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <stack>
#include <functional>
#include <string>
#include <cassert>
#include <stdexcept>
#include <vector>

#if defined(BOOST_SPIRIT_DUMP_PARSETREE_AS_XML)
#include <map>
#endif

#include "RecordFilter.h"

using namespace std;
using namespace boost;
using namespace boost::spirit;

static bool 
wildcardCmp(const char* p1, const char* p2)
{
    if (*p1 == '\0' && *p2 == '\0')
    {
        return true;
    }

    if ((*p1 == '\0' && *p2 != '*') || *p2 == '\0')
    {
        return false;
    }

    if (*p2 == '?')
    {
        return wildcardCmp(p1 + 1, p2 + 1);
    } 
    else if (*p2 == '*') 
    {
        for (unsigned int i = 0; i <= strlen(p1); i++) 
        {
            if (wildcardCmp(p1 + i, p2 + 1)) 
            {
                return true;
            }
        }
        return false;
    } 
    else
    {
        return (*p1 == *p2) && wildcardCmp(p1 + 1, p2 + 1);
    }
}

class EvalError : public runtime_error
{
public:
    EvalError(const string& info) : runtime_error(info)
    { 
    }
};

struct Calculator : public grammar<Calculator>
{
    static const int keyID = 1;
    static const int valueID = 2;
    static const int pairID = 3;
    static const int factorID = 4;
    static const int expressionID = 5;

    template <typename ScannerT>
    struct definition
    {
        // grammar definition
        definition(const Calculator& /*self*/)
        {
            key = inner_node_d[ch_p('[') >> leaf_node_d[lexeme_d[+alnum_p]] >> ch_p(']')];
            value = inner_node_d[ch_p('\'') >> leaf_node_d[lexeme_d[+(print_p - ch_p('\''))]] >> ch_p('\'')];

            pair = (key >> root_node_d[str_p("==")] >> value)
                 | (key >> root_node_d[str_p("!=")] >> value)
                 | (key >> root_node_d[str_p(">=")] >> value)
                 | (key >> root_node_d[str_p("<=")] >> value)
                 | (key >> root_node_d[ch_p('>')] >> value)
                 | (key >> root_node_d[ch_p('<')] >> value);

            factor = pair | inner_node_d[ch_p('(') >> expression >> ch_p(')')]
                          | (root_node_d[ch_p('!')] >> factor);

            expression = factor >> *((root_node_d[str_p("&&")] >> factor)
                                   | (root_node_d[str_p("||")] >> factor));

            // turn on the debugging info.
            BOOST_SPIRIT_DEBUG_RULE(key);
            BOOST_SPIRIT_DEBUG_RULE(value);
            BOOST_SPIRIT_DEBUG_RULE(pair);
            BOOST_SPIRIT_DEBUG_RULE(factor);
            BOOST_SPIRIT_DEBUG_RULE(expression);
        }

        rule<ScannerT, parser_context<>, parser_tag<keyID> >          key;
        rule<ScannerT, parser_context<>, parser_tag<valueID> >        value;
        rule<ScannerT, parser_context<>, parser_tag<pairID> >         pair;
        rule<ScannerT, parser_context<>, parser_tag<factorID> >       factor;
        rule<ScannerT, parser_context<>, parser_tag<expressionID> >   expression;

        const rule<ScannerT, parser_context<>, parser_tag<expressionID> >& start() const 
        { return expression; }
    };
};


RecordFilter::RecordFilter(const std::string& condition, const RecordEvaluator* recEval)
    : recEval_(recEval), calc_(new Calculator), condition_(condition)
{
}

bool
RecordFilter::parse()
{
    parseInfo_ = ast_parse(condition_.c_str(), *calc_, space_p);
    if (parseInfo_.full)
    {
#if defined(BOOST_SPIRIT_DUMP_PARSETREE_AS_XML)
        // dump parse tree as XML
        std::map<parser_id, std::string> rule_names;
        rule_names[Calculator::keyID] = "key";
        rule_names[Calculator::valueID] = "value";
        rule_names[Calculator::pairID] = "pair";
        rule_names[Calculator::factorID] = "factor";
        rule_names[Calculator::expressionID] = "expression";
        tree_to_xml(cout, parseInfo_.trees, condition_.c_str(), rule_names);
#endif
    }
    return parseInfo_.full;
}

bool
RecordFilter::match()
{
    bool ret = false;
    try
    {
        ret = evalExpression(parseInfo_.trees.begin());
    }
    catch (const EvalError& e)
    {
    }
    return ret;
}


string
RecordFilter::evalString(const iterT& i)
{
    string v = string(i->value.begin(), i->value.end());

    //cout << "In evalExpression. i->value = " << v << " i->children.size() = " << i->children.size() << endl;

    if (i->value.id() == Calculator::keyID)
    {
        return recEval_->get(v);
    }
    else if (i->value.id() == Calculator::valueID)
    {
        return v;
    }
    else
    {
        throw EvalError("invalid node id");
    }
}


bool 
RecordFilter::evalExpression(const iterT& i)
{
    //cout << "In evalExpression. i->value = " <<
    //    string(i->value.begin(), i->value.end()) <<
    //    " i->children.size() = " << i->children.size() << endl;

    if (i->value.id() == Calculator::pairID)
    {
        if (i->children.size() != 2)
        {
            throw EvalError("children of pair is not 2");
        }

        string op(i->value.begin(), i->value.end());
        string lvalue = evalString(i->children.begin());
        string rvalue = evalString(i->children.begin()+1);
        long l;
        long r;
        bool numCmp = true;

        try
        {
            l = lexical_cast<long>(lvalue);    
            r = lexical_cast<long>(rvalue);  
        }
        catch (const bad_lexical_cast&)
        {
            numCmp = false;
        }

        if (op == "==")
        {
            if (numCmp)
            {
                return l == r;
            }
            else
            {
                return wildcardCmp(lvalue.c_str(), rvalue.c_str());
            }
        }
        else if (op == "!=")
        {
            if (numCmp)
            {
                return l != r;
            }
            else
            {
                return !wildcardCmp(lvalue.c_str(), rvalue.c_str());
            }
        }
        else if (op == ">=" && numCmp)
        {
            return l >= r;
        }
        else if (op == "<=" && numCmp)
        {
            return l <= r;
        }
        else if (op == ">" && numCmp)
        {
            return l > r;
        }
        else if (op == "<" && numCmp)
        {
            return l < r;
        }
        else
        {
            throw EvalError("compare failure");
        }
    }
    else if (i->value.id() == Calculator::factorID)
    {
        if (*i->value.begin() != '!')
        {
            throw EvalError("invalid op (!)");
        }
        return !evalExpression(i->children.begin());
    }
    else if (i->value.id() == Calculator::expressionID)
    {
        string op(i->value.begin(), i->value.end());
        if (op == "&&")
        {
            if (i->children.size() != 2)
            {
                throw EvalError("children of op(&&) is not 2");
            }

            return evalExpression(i->children.begin()) &&
                   evalExpression(i->children.begin()+1);
        }
        else if (op == "||")
        {
            if (i->children.size() != 2)
            {
                throw EvalError("children of op(||) is not 2");
            }

            return evalExpression(i->children.begin()) ||
                   evalExpression(i->children.begin()+1);
        }
        else
        {
            throw EvalError("invalid op (&&, ||)");
        }
    }
    else
    {
        throw EvalError("invalid node id");
    }
}


class MyEvaluator : public RecordEvaluator
{
public:
    struct Data
    {
        Data(const string& ss, int ii)
            : s(ss), i(ii)
        {
        }

        string s;
        int i;
    };

    MyEvaluator()
    {
        // init data
        int count = 100;
        while (count--)
        {
            if (count%2)
            {
                data_.push_back(Data("abc", count));
            }
            else
            {
                data_.push_back(Data("xyz", count));
            }
        }
        cur_ = data_.begin();
    }

    std::string get(const std::string& key) const
    {
        if (key == "str")
        {
            return cur_->s;
        }
        else if (key == "num")
        {
            return lexical_cast<string>(cur_->i);
        }
        else
        {
            assert(false);
        }
    }

    bool next()
    {
        ++cur_;
        return cur_ != data_.end();
    }

    void dumpCur()
    {
        cout<<"str: "<<cur_->s<<", num: "<<lexical_cast<string>(cur_->i)<<endl;
    }

private:
    vector<Data> data_;
    vector<Data>::iterator cur_;
};

int
main()
{
    MyEvaluator my;
    RecordFilter filter("!([str]=='xyz' && [num]>='50')", &my);
    if (filter.parse())
    {
        do
        {
            if (filter.match())
            {
                my.dumpCur();
            }
        }
        while (my.next());
    }
    else
    {
        cout<<"parse fail"<<endl;
    }

    return 0;
}
