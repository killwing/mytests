#ifndef RECORDFILTER_H
#define RECORDFILTER_H

#include <boost/smart_ptr.hpp>
#include <boost/spirit/core.hpp>
#include <boost/spirit/tree/tree_to_xml.hpp>
#include <boost/spirit/tree/ast.hpp>
#include <string>


class RecordEvaluator
{
public:
    virtual std::string get(const std::string& key) const = 0;
};


class Calculator;
class RecordFilter
{
public:
    RecordFilter(const std::string& condition, const RecordEvaluator* recEval);
     
    // parse condition
    bool parse();

    // check condition
    bool match();

private:
    typedef boost::spirit::tree_match<const char*>::tree_iterator iterT;

    bool evalExpression(const iterT& i);
    std::string evalString(const iterT& i);

    boost::spirit::tree_parse_info<> parseInfo_;

    const RecordEvaluator* recEval_;
    boost::scoped_ptr<Calculator> calc_;
    std::string condition_;
};

#endif // RecordFilter

