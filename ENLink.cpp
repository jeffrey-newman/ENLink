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
            : evaluator(_evaluator), is_in_use(true), num_dvs(0), num_dv_options(new std::vector<int>)
    {

    }

    AnalysisData()
            : evaluator(nullptr), is_in_use(false), num_dvs(0), num_dv_options(nullptr)
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
    std::pair<std::map<int, AnalysisData>::iterator, bool> ret_val = analysis_map.insert(std::make_pair(max_analysis_id, AnalysisData(en2_analysis)));
    if (ret_val.second)
    {
        boost::filesystem::path config_file_path(opt_cfg_file);

        if (!boost::filesystem::exists(config_file_path))
        {
            std::cerr << "Config file not found on filesystem: " << config_file_path << "\n";
            return (-1);
        }

        ret_val.first->second.evaluator->initialise(config_file_path);


        return (max_analysis_id);
    }
    else
    {
        return (-1);
    }

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

double getPipeCapitalCost(int analysisID)
{
    AnalysisData & analysis = analysis_map[analysisID];
    return( analysis.evaluator->getPipeCapitalCost());
}

double getSumPressureTooHigh(int analysisID)
{
    AnalysisData & analysis = analysis_map[analysisID];
    return( analysis.evaluator->getSumPressureTooHigh());
}

double getMaxPressureTooHigh(int analysisID)
{
    AnalysisData & analysis = analysis_map[analysisID];
    return( analysis.evaluator->getMaxPressureTooHigh());
}

double getSumPressureTooLow(int analysisID)
{
    AnalysisData & analysis = analysis_map[analysisID];
    return( analysis.evaluator->getSumPressureTooLow());
}

double getMinPressureTooLow(int analysisID)
{
    AnalysisData & analysis = analysis_map[analysisID];
    return( analysis.evaluator->getMinPressureTooLow());
}

double getSumHeadTooHigh(int analysisID)
{
    AnalysisData & analysis = analysis_map[analysisID];
    return( analysis.evaluator->getSumHeadTooHigh());
}

double getMaxHeadTooHigh(int analysisID)
{
    AnalysisData & analysis = analysis_map[analysisID];
    return( analysis.evaluator->getMaxHeadTooHigh());
}

double getSumHeadTooLow(int analysisID)
{
    AnalysisData & analysis = analysis_map[analysisID];
    return( analysis.evaluator->getSumHeadTooLow());
}

double getMinHeadTooLow(int analysisID)
{
    AnalysisData & analysis = analysis_map[analysisID];
    return( analysis.evaluator->getMinHeadTooLow());
}

double getSumVelocityTooHigh(int analysisID)
{
    AnalysisData & analysis = analysis_map[analysisID];
    return( analysis.evaluator->getSumVelocityTooHigh());
}

double getMaxVelocityTooHigh(int analysisID)
{
    AnalysisData & analysis = analysis_map[analysisID];
    return( analysis.evaluator->getMaxVelocityTooHigh());
}


double getNetworkResilience(int analysisID)
{
    AnalysisData & analysis = analysis_map[analysisID];
    return( analysis.evaluator->getNetworkResilience());
}

void handBackAnalysis(int analysisID)
{
    AnalysisData & analysis = analysis_map[analysisID];
    analysis.is_in_use = false;
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

int getAnAnalysis()
{
    typedef std::pair<const int, AnalysisData> AnalysisDatum;
    BOOST_FOREACH(AnalysisDatum & analysis, analysis_map)
                {
                    if (analysis.second.is_in_use == false)
                    {
                        analysis.second.is_in_use = true;
                        return  analysis.first;
                    }

                }
    return -1;
}


