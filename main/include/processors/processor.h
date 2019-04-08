#ifndef _PROCESSOR_H_
#define _PROCESSOR_H_

#include <regex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <boost/iostreams/device/mapped_file.hpp>

#include "../grid.h"

struct processor
{
    using cell_tag_info = std::pair<std::string, unsigned long>;
    using cell_info = std::tuple<std::string, unsigned long, std::vector<cell_tag_info>>;
    using result_type = std::vector<cell_info>;

    processor(processor const &) = delete;

    processor(processor &&) = delete;

    processor & operator=(processor const &) = delete;

    processor & operator=(processor &&) = delete;

    virtual void preprocess() = 0;

    virtual result_type operator()() const = 0;

protected:
    // @formatter:off
    using record_type = std::unordered_map<
        std::string,
        std::pair<
            unsigned long,
            std::unordered_map<std::string, unsigned long>
        >
    >;
    // @formatter:on

    static std::regex const coord_rgx, hash_tags_rgx, hash_tag_rgx;

    static void merge_records(record_type &, record_type &&);

    static bool less_cell_tag_info(cell_tag_info const &, cell_tag_info const &);

    static bool less_cell_info(cell_info const &, cell_info const &);

    grid const & g;
    boost::iostreams::mapped_file_source file;
    record_type record;

    explicit processor(char const *, grid const &);

    virtual void process_line(std::string const &, record_type &) const final;

    virtual void process_block(char const *, char const *, record_type &) const final;

private:
    friend struct processor_tester;
};

#endif