//
// Created by a1091793 on 13/3/17.
//

#include <iostream>
#include <random>
#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/foreach.hpp>
#include <boost/timer/timer.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>


#include "../ENLink.h"
#include "SolutionSetParser.h"


typedef std::pair<std::string, boost::filesystem::path> CmdLinePaths;

void
pathify(CmdLinePaths & path)
{
    path.second = boost::filesystem::path(path.first);
    if (!(boost::filesystem::exists(path.second)))
    {
        std::cout << "Warning: path " << path.first << " does not exist\n";
    }
}

void
pathify_mk(CmdLinePaths & path)
{
    path.second = boost::filesystem::path(path.first);
    if (!(boost::filesystem::exists(path.second)))
    {
        boost::filesystem::create_directories(path.second);
        std::cout << "path " << path.first << " did not exist, so created\n";
    }
}

std::pair<std::string, std::string> at_option_parser(std::string const&s)
{
    if ('@' == s[0])
        return std::make_pair(std::string("cfg-file"), s.substr(1));
    else
        return std::pair<std::string, std::string>();
}



int
main(int argc, char* argv[])
{
//    boost::filesystem::path deafult_working_dir = boost::filesystem::path(userHomeDir()) / ".geonamicaZonalOpt/working_dir";

    std::cout << "Running" << std::endl;
    CmdLinePaths opt_file_path;
    bool do_log;
    bool test_analysis_reuse;
    CmdLinePaths solutions_file_path;

    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help,h", "produce help message")
            ("opt-file,o", po::value<std::string>(&opt_file_path.first), "Path to the opt file")
            ("log,l", po::value<bool>(&do_log)->default_value(false), "log the calculations?")
            ("test-analysis-reuse", po::value<bool>(&test_analysis_reuse)->default_value(false), "Test reusing analysis vs ruuning each in new analysis")
            ("solutions,s", po::value<std::string>(&solutions_file_path.first)->default_value("none"), "File with solutions (space/blank seperate ints, one solution per line)")
            ("cfg-file,c", po::value<std::string>(), "can be specified with '@name', too");

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).extra_parser(at_option_parser).run(), vm);

    if (vm.count("help"))
    {
        std::cout << desc << "\n";
        return 1;
    }

    if (vm.count("cfg-file")) {
        // Load the file and tokenize it
        std::ifstream ifs(vm["cfg-file"].as<std::string>().c_str());
        if (!ifs) {
            std::cout << "Could not open the cfg file\n";
            return 1;
        }
        po::store(po::parse_config_file(ifs, desc), vm);
    }

    po::notify(vm);
    pathify(opt_file_path);

    int analysis_id = createAnalysis(opt_file_path.second.string().c_str());
    if (do_log) doLog(analysis_id);
    int numDVs;
    int* dv_bounds = getDVBounds(analysis_id, &numDVs);
    std::vector<int> dv_bounds_vec(dv_bounds, dv_bounds+numDVs);
    clear(analysis_id, true);

    /////// The solutions datastructure
    SolutionSet dv_vals_set;

    if (solutions_file_path.first == "none")
    {
        //Generate a random dv vals.
        std::default_random_engine generator;
        for (int j = 0; j < 1000; ++j)
        {
            std::vector<int> dv_vals_inst;
            for (int i = 0; i < numDVs; ++i)
            {
                std::uniform_int_distribution<int> distribution(0, (dv_bounds_vec[i] - 1));
                dv_vals_inst.push_back(distribution(generator));
            }
            dv_vals_set.push_back(dv_vals_inst);
        }
    }
    else
    {
        pathify(solutions_file_path);
        readInSolutions(solutions_file_path.second, dv_vals_set);
    }

    if(test_analysis_reuse)
    {
        boost::timer::auto_cpu_timer t;
        for (int j = 0; j < 1000; ++j)
        {
            analysis_id = createAnalysis(opt_file_path.second.string().c_str());
            if (do_log) doLog(analysis_id);
            int err_code = runEN(analysis_id, dv_vals_set[j].data());
            double cost = getPipeCapitalCost(analysis_id);
            clear(analysis_id, true);
        }
        t.stop();
        t.report();
    }

    boost::timer::auto_cpu_timer t2;
    analysis_id = createAnalysis(opt_file_path.second.string().c_str());
    if (do_log) doLog(analysis_id);
    for (int j = 0; j < 1000; ++j)
    {
        int err_code = runEN(analysis_id, dv_vals_set[j].data());
        double cost = getPipeCapitalCost(analysis_id);
    }
    clear(analysis_id, true);
    t2.stop();
    t2.report();

	char temp;
	std::cout << "Press any key to exit" << std::endl;
	std::cin >> temp;

//    std::cout << "cost is: " << std::endl;


}