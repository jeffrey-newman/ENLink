#include "EN2Link.h"

#include <iostream>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem.hpp>
#include "ENMultiObjEvaluator.h"

/**
 * memory management of EN analysis.
 * int is the analysis ID, which is mapped to the ENMultiObjEvaluator object which is the keeper of the loaded EN2 library.
 * bool is a flag that indicates whether this analysis is currently being used.
 */
int max_analysis_id = -1;
std::map<int, std::pair<boost::shared_ptr<ENMultiObjEvaluator>, bool> > analysis_map;

void hello()
{
    std::cout << "Hello, World!" << std::endl;
}

int createAnalysis(const char *opt_cfg_file)
{
    ++max_analysis_id;
    boost::shared_ptr<ENMultiObjEvaluator> en2_analysis(new ENMultiObjEvaluator);

    boost::filesystem::path config_file_path(opt_cfg_file);

    if (!boost::filesystem::exists(config_file_path))
    {
        std::cerr << "Config file not found on filesystem: " << config_file_path << "\n";
        return (-1);
    }

    en2_analysis->initialise(config_file_path);
    analysis_map.insert(std::make_pair(max_analysis_id, std::make_pair(en2_analysis, true)));

    return (max_analysis_id);
}

int runEN(int analysisID, const int *decision_variables)
{
    return( analysis_map[analysisID].first->runEN(decision_variables));
}

double getCost(int analysisID)
{
//    return(analysis_map[analysisID].first->evalCost());
    return 0;
}

double getPressureConstraint(int analysisID)
{
//    analysis_map[analysisID].first->evalPressurePenalty(results);
//    return results.
    return 0;
}

double getHeadConstraint(int analysisID)
{
//    return(analysis_map[analysisID].first->evalHeadPenalty());
    return 0;
}


double getResilience(int analysisID)
{
    return 0;
}

int clear(int analysisID)
{
    return 0;
}

int clearAll()
{
    return 0;
}
