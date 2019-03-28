#ifndef _SINGLE_THREAD_PROCESSOR_HPP_
#define _SINGLE_THREAD_PROCESSOR_HPP_

#include <algorithm>
#include <fstream>
#include <limits>

#include "processor.hpp"

struct single_thread_processor final : public processor
{
    explicit single_thread_processor(char const * filename, grid const & g) : processor(filename, g) {}

    processor & preprocess() override
    {
        std::ifstream twit_file(filename);
        while (!twit_file.eof())
        {
            std::string buff;
            std::getline(twit_file, buff);

            std::smatch coord_search;
            if (!std::regex_search(buff, coord_search, coord_rgx))
                continue;

            auto h = g.get_horizontal(std::stod(coord_search[1].str()));
            if (!h)
                continue;

            auto v = g.get_vertical(std::stod(coord_search[2].str()));
            if (!v)
                continue;

            if (!g.validate(v, h))
                continue;

            // @formatter:off
            auto & [count, hash_tags_counts] = record[{v, h}];
            // @formatter:on
            ++count;

            std::smatch hash_tags_search;
            if (!std::regex_search(buff, hash_tags_search, hash_tags_rgx))
                continue;

            std::smatch hash_tag_search;
            std::string hash_tags_str = hash_tags_search[0].str();
            if (!std::regex_search(hash_tags_str, hash_tag_search, hash_tag_rgx))
                continue;

            for (auto it = hash_tag_search.begin() + 1; it < hash_tag_search.end(); it += 2)
            {
                auto tmp = it->str();
                boost::algorithm::to_lower(tmp);
                ++hash_tags_counts[std::move(tmp)];
            }
        }

        return *this;
    }

    result_type operator()() const override
    {
        result_type re(record.size());
        auto it = re.begin();
        for (auto & [k, v] : record)
        {
            auto & [ik, iv] = v;
            std::vector<cell_tag_info> tmp(iv.size());
            auto tit = tmp.begin();
            for (auto & [iik, iiv] : iv)
            {
                *tit = {iik, iiv};
                ++tit;
            }
            std::sort(tmp.begin(), tmp.end(), less_cell_tag_info);
            if (tmp.size() > 5)
                tmp.erase(tmp.begin() + 5, tmp.end());
            *it = {k, ik, std::move(tmp)};
            ++it;
        }
        std::sort(re.begin(), re.end(), less_cell_info);
        return re;
    }
};

#endif
