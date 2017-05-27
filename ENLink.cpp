#include "ENLink.h"

#include <iostream>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>
#include <boost/range/adaptor/map.hpp>
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
boost::filesystem::path working_dir = "no_path";

int
setWorkingDir(const char* _working_dir)
{
    working_dir = _working_dir;
    if (!boost::filesystem::exists(working_dir))
    {
        return -1;
    }
    return 0;
}

int
createAnalysis(const char *opt_cfg_file)
{
    bool _delete_temp_on_exit = true;
    if (working_dir == "no_path")
    {
        working_dir = boost::filesystem::current_path();
    }

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

        ret_val.first->second.evaluator->initialise(config_file_path, working_dir, _delete_temp_on_exit);


        return (max_analysis_id);
    }
    else
    {
        return (-1);
    }

}

int
doLog(int analysisID)
{
    AnalysisData & analysis = analysis_map[analysisID];
    bool success = analysis.evaluator->log();
    if (success) return 0;
    return -1;
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

int handBackAnalysis(int analysisID)
{
    AnalysisData & analysis = analysis_map[analysisID];
    analysis.is_in_use = false;
    if (analysis.is_in_use == false) return 0;
    return -1;
}

int clear(int analysisID, bool _delete_temp_on_exit)
{
    AnalysisData & analysis = analysis_map[analysisID];
    analysis.evaluator->do_remove_work_dir_on_exit = _delete_temp_on_exit;
    analysis_map.erase(analysisID);
    return 0;
}

int clearAll(bool _delete_temp_on_exit)
{
    BOOST_FOREACH(AnalysisData& analysis, analysis_map | boost::adaptors::map_values)
                {
                    analysis.evaluator->do_remove_work_dir_on_exit = _delete_temp_on_exit;
                }
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

bool isPipeCapitalCostCalculated(int analysisID)
{
    AnalysisData & analysis = analysis_map[analysisID];
    return analysis.evaluator->isPipeCapitalCostCalculated();
}

bool isPressureViolationCalculated(int analysisID)
{
    AnalysisData & analysis = analysis_map[analysisID];
    return analysis.evaluator->isPressureViolationCalculated();
}

bool isHeadViolationCalculated(int analysisID)
{
    AnalysisData & analysis = analysis_map[analysisID];
    return analysis.evaluator->isHeadViolationCalculated();
}

bool isNetworkResilienceCalculated(int analysisID)
{
    AnalysisData & analysis = analysis_map[analysisID];
    return analysis.evaluator->isNetworkResilienceCalculated();
}

int saveAnalysis(int analysisID, const char *save_path)
{
    boost::filesystem::path save_fspath(save_path);
    AnalysisData & analysis = analysis_map[analysisID];
    return analysis.evaluator->saveAnalysis(analysisID, save_fspath);
}

int saveAnalysis(int analysisID)
{
    AnalysisData & analysis = analysis_map[analysisID];
    return analysis.evaluator->saveAnalysis(analysisID);
}


