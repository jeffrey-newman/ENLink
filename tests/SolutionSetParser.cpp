//
// Created by a1091793 on 26/5/17.
//

#include "SolutionSetParser.h"
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/filesystem.hpp>
#include <fstream>


SolutionSetSkipper::SolutionSetSkipper() : SolutionSetSkipper::base_type(skip_it)
{
    comment_char = ';';
    //Skip spaces or comments, except if they are end of line spaces.
    skip_it = ((qi::space - qi::eol) | comment);
    comment = qi::lexeme[qi::lit(comment_char) >> *(qi::char_ - qi::eol)];
}

SolutionSetParser::SolutionSetParser(SolutionSet & _solutions)
: SolutionSetParser::base_type(start), solutions(_solutions)
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;

    this->soln_rule = *qi::int_ >> qi::eol;
    this->start = *(soln_rule[phoenix::push_back(phoenix::ref(this->solutions), qi::_1)]);
}

bool
readInSolutions(boost::filesystem::path & solutions_file_path, SolutionSet & solutions)
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace phoenix = boost::phoenix;
    //Read in the solutions
    //Now we read the file.

    if (!boost::filesystem::exists(solutions_file_path))
    {
        std::cout << "Solutions file did not exist on path: " << solutions_file_path.string() << "\n";
        return false;
    }

    std::ifstream fin(solutions_file_path.string().c_str(), std::ios_base::in);
    if (fin.is_open())
    {
        //Start parsing the file - String to store the contents of the file
        std::string fileContents;
        //Copy contents of file to the string, make sure to include the white space
        fin.unsetf(std::ios::skipws);
        std::copy(std::istream_iterator<char>(fin),
                  std::istream_iterator<char>(),
                  std::back_inserter(fileContents));
        // The parser grammer requires a line ending after some things. Can cause problems
        // if one of these things is on the last line of the file. So we append another
        // new line just in case.
        fileContents.append("     \n");
        //Print the read-in contents to clog.

        //Now we parse the file. WE need (rewindable) iterators at the start and end of the string.
        Str_it it = fileContents.begin();
        Str_it end = fileContents.end();

//            SolutionSetParser parser(dv_vals_set);
        auto soln_rule = *qi::int_ >> qi::eol;
        auto solns_rule = *(soln_rule[phoenix::push_back(phoenix::ref(solutions), qi::_1)]);
        SolutionSetSkipper skipper;
        bool r = boost::spirit::qi::phrase_parse(it, end, solns_rule, skipper);

        if (r && it == end)
        {
            std::cout << "------------------------------\n";
            std::cout << "Parsing solution set succeeded\n";
            std::cout << "------------------------------\n";
        }
        else
        {
            std::string rest(it, end);
            std::cout << "-----------------------------\n";
            std::cout << "Parsing failed\n";
            std::cout << "stopped at: \"" << rest << "\"\n";
            std::cout << "-----------------------------\n";
            return false;
        }
        fin.close();
    }
    else
    {
        std::string errmsg = "Error while opening file. File with name " + solutions_file_path.string() + " does not exist?";
        return  false;
    }
    return true;
}