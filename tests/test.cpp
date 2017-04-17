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


#include "../EN2Link.h"


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

    CmdLinePaths opt_file_path;

    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help,h", "produce help message")
            ("opt-file,o", po::value<std::string>(&opt_file_path.first), "Path to the opt file")
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

    int analysis_id = createAnalysis(opt_file_path.second.c_str());
    int numDVs;
    int* dv_bounds = getDVBounds(analysis_id, &numDVs);
    std::vector<int> dv_bounds_vec(dv_bounds, dv_bounds+numDVs);

    //Generate a random dv vals.
    std::vector<int> dv_vals;
    std::default_random_engine generator;
    for (int i = 0; i < numDVs; ++i)
    {
        std::uniform_int_distribution<int> distribution(0, (dv_bounds_vec[i] -1));
        dv_vals.push_back(distribution(generator));
    }

    int err_code = runEN(analysis_id, dv_vals.data());
    double cost = getPipeCapitalCost(analysis_id);
    clear(analysis_id);


    boost::timer::auto_cpu_timer t;

    for (int j = 0; j < 1000; ++j)
    {
        analysis_id = createAnalysis(opt_file_path.second.c_str());

        //Generate a random dv vals.
        dv_vals.clear();
        std::default_random_engine generator;
        for (int i = 0; i < numDVs; ++i)
        {
            std::uniform_int_distribution<int> distribution(0, (dv_bounds_vec[i] -1));
            dv_vals.push_back(distribution(generator));
        }

        int err_code = runEN(analysis_id, dv_vals.data());
        double cost = getPipeCapitalCost(analysis_id);
        clear(analysis_id);
    }
    t.stop();
    t.report();

    boost::timer::auto_cpu_timer t2;
    analysis_id = createAnalysis(opt_file_path.second.c_str());
    for (int j = 0; j < 1000; ++j)
    {
//        int analysis_id = createAnalysis(opt_file_path.second.c_str());
        std::vector<int> dv_vals;
        std::default_random_engine generator;
        for (int i = 0; i < numDVs; ++i)
        {
            std::uniform_int_distribution<int> distribution(0, (dv_bounds_vec[i] -1));
            dv_vals.push_back(distribution(generator));
        }

        int err_code = runEN(analysis_id, dv_vals.data());
        double cost = getPipeCapitalCost(analysis_id);
    }
    t2.stop();
    t2.report();



//    std::cout << "cost is: " << std::endl;


}