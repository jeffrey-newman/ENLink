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


struct AnalysisData
{
    AnalysisData(boost::shared_ptr<ENMultiObjEvaluator> _evaluator)
            : evaluator(_evaluator), is_in_use(true), num_dvs(0), num_dv_options(boost::shared_ptr<std::vector<int> >(new std::vector<int>))
    {

    }

    boost::shared_ptr<ENMultiObjEvaluator> evaluator;
    bool is_in_use;
    int num_dvs;
    boost::shared_ptr<std::vector<int> > num_dv_options;
};

int max_analysis_id = -1;
std::map<int, AnalysisData> analysis_map;

int
createAnalysis(const char *opt_cfg_file)
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
    analysis_map.insert(std::make_pair(max_analysis_id, AnalysisData(en2_analysis)));

    return (max_analysis_id);
}

int*
getDVBounds(int analysisID, int* numberDVs)
{
    AnalysisData & analysis = analysis_map[analysisID];
    analysis.num_dv_options = (analysis.evaluator->getPipeDVBounds());
    analysis.num_dvs = analysis.num_dv_options->size();
    *numberDVs = analysis.num_dvs;
    return analysis.num_dv_options->data();
}

int runEN(int analysisID, const int *decision_variables)
{
    AnalysisData & analysis = analysis_map[analysisID];

    return( analysis.evaluator->runEN(decision_variables));
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

void handBackAnalysis(int analysisID)
{
    analysis_map[analysisID].second = false;
}

int clear(int analysisID)
{
    analysis_map.erase(analysisID);
    return 0;
}

int clearAll()
{
    analysis_map.clear();
    return 0;
}
