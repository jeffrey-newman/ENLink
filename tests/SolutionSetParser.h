//
// Created by a1091793 on 26/5/17.
//

#ifndef ENLINK_SOLUTIONSETPARSER_H
#define ENLINK_SOLUTIONSETPARSER_H

#include <boost/spirit/include/qi.hpp>
#include <boost/filesystem.hpp>

/**
	 * qi provides parsing functionality and is part of boost spirit
	 */
namespace qi = boost::spirit::qi;

/**
 * We will be parsing strings. We define a string iterator as Str_it
 */
typedef std::string::const_iterator Str_it; /**< A string const iterator - used for reading the input of the OptFile */
typedef std::vector< std::vector<int> > SolutionSet;

/**
 * @brief this defines what the parser should skip in the optFile
 * @details think of these are grammers - they define the rules of
 * how the input file should be formatted. This one defines how
 * comments should be formatted - because these are skipped over, as well as white space
 * @see Boost spirit documentation
 */
struct SolutionSetSkipper : qi::grammar< Str_it >
{
    SolutionSetSkipper();
    qi::rule< Str_it > skip_it; /**< Rule that does the work. passes over commetns and whitespace in the file */
    qi::rule<Str_it> comment; /**< Rule that detects comments in the file.*/
    char comment_char; /**< Define what the comment character is. I.e. we follow the Epanet inp file tradition of ';' */
};

class SolutionSetParser : qi::grammar<Str_it, SolutionSetSkipper>
{
public:
    SolutionSetParser(SolutionSet & _solutions);
    qi::rule<Str_it , std::vector<int>(), SolutionSetSkipper> soln_rule;
    qi::rule<Str_it , SolutionSetSkipper> start;
private:
    SolutionSet & solutions;
};

bool
readInSolutions(boost::filesystem::path & solutions_file_path, SolutionSet & solutions);



#endif //ENLINK_SOLUTIONSETPARSER_H
