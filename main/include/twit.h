#ifndef _TWIT_H_
#define _TWIT_H_

#include <regex>
#include <string>
#include <unordered_set>
#include <utility>

using twit_info = std::tuple<double, double, std::unordered_set<std::string>>;

class twit_parser final
{
    static std::regex const line_rgx, hash_tag_rgx;

    static twit_parser const parser;

    twit_parser() = default;
public:
    static twit_parser const & get_parser();

    ~twit_parser() = default;

    twit_parser(twit_parser const &) = delete;

    twit_parser(twit_parser &&) = delete;

    twit_parser & operator=(twit_parser const &) = delete;

    twit_parser & operator=(twit_parser &&) = delete;

    twit_info parse(std::string const &);
};

#endif
