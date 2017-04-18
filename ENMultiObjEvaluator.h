//
// Created by a1091793 on 2/3/17.
//

#ifndef SRC_ENMULTIOBJEVALUATOR_H
#define SRC_ENMULTIOBJEVALUATOR_H

#include <fstream>
#include <iostream>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_array.hpp>
#include <boost/filesystem.hpp>
#include <boost/function.hpp>
#include "OptParser.h"

#include "Ibanez_EN2/types.h"

struct PipeEvalData
{
    double capCost;
    double trenchCost;
    double repairCost;
    double capEnerg;
    double trenchEnerg;
    double repairEnerg;
    float length;
    float diameter;
    double unitCost;
    double unitRepairCost;
    double unitEE;
    double unitRepairEE;
    int Allots;
    std::map<double, double> lengths;
};

struct PumpStationCosts
{
    PumpStationCosts() :
            avgHead(0), //
            peakHead(0), //
            avgFlow(0), //
            peakFlow(0), //
            pumpHydraulicPower(0), //
            pumpNetPower(0), //
            pumpEfficiency(0), //
            pumpMechanicalCost(0), //
            pumpElectricalCost(0), //
            pumpCivilCost(0), //
            pumpRunningCost(0), //
            pumpRunningEnergy(0), //
            pumpCivilEE(0), //
            pumpMechanicalEE(0)
    {
    }
    std::string name;
    double avgHead;
    double peakHead;
    double avgFlow;
    double peakFlow;

    double pumpHydraulicPower;
    double pumpNetPower;
    double pumpEfficiency;
    double pumpMechanicalCost;
    double pumpElectricalCost;
    double pumpCivilCost;
    double pumpRunningCost;
    double pumpRunningEnergy;
    double pumpCivilEE;
    double pumpMechanicalEE;
};

struct TankResultsInfo
{
    TankResultsInfo() :
            peak_water_demand(0), //
            peak_water_production(0), //
            peak_external_demand(0), //
            peak_internal_demand_rate(0), //
            net_internal_demand(0), //
            net_external_demand(0), //
            avg_internal_demand_rate(0), //
            tankEE(0), //
            tankCost(0), //
            treatmentEE(0), //
            treatmentRunningEnergy(0), //
            treatmentCapitalCost(0), //
            treatmentRunningCost(0), //
            log_reduction(0), //
            other_cost(0), //
            other_EE(0), //
            numAllots(0), //
            tankName("N/A"), //
            externalSupplyCost(0), //
            externalSupplyEnergy(0), //
            externalTreatCost(0), //
            externalTreatEnergy(0)
    {

    }

    double peak_water_demand;
    double peak_water_production;
    double peak_external_demand;
    double peak_internal_demand_rate;
    double net_internal_demand;
    double net_external_demand;
    double avg_internal_demand_rate;
    double tankEE;
    double tankCost;
    double treatmentEE;
    double treatmentRunningEnergy;
    double treatmentCapitalCost;
    double klInPeakDay;
    double buildingCost;
    double treatmentRunningCost;

    double log_reduction;

    double other_cost;
    double other_EE;

    int numAllots;
    std::string tankName;

    double externalSupplyCost;
    double externalSupplyEnergy;
    double externalTreatCost;
    double externalTreatEnergy;

};


struct Results
{
    Results() :
            maxPressureTooHigh(0), //
            minPressureTooLow(0), //
            maxHeadTooHigh(0), //
            minHeadTooLow(0), //
            maxVelocityTooHigh(0),
            sumPressureTooHigh(0), //
            sumPressureTooLow(0), //
            sumHeadTooHigh(0), //
            sumHeadTooLow(0), //
            sumVelocityTooHigh(0),
            networkResilience(0), //
            costPipes(0), //
            pipeRepairCost(0), //
            pipeCapitalCost(0), //
            pipeEmbodiedEnergy(0), //
            pipeRepairEE(0), //
            costPumps(0), //
            pumpElevHeads(0), //
            pumpFlows(0), //
            costPumpsBreakdown(), //
            net_costPumpBreakdown() //
    {

    }

    double maxPressureTooHigh; /**< */
    double minPressureTooLow; /**< */
    double maxHeadTooHigh; /**< */
    double minHeadTooLow; /**< */
    double maxVelocityTooHigh; /**< */

    double sumPressureTooHigh; /**< */
    double sumPressureTooLow; /**< */
    double sumHeadTooHigh; /**< */
    double sumHeadTooLow; /**< */
    double sumVelocityTooHigh; /**< */

    double networkResilience;

    std::string penaltyPresLoc;
    std::string penaltyHeadLoc;
    std::string penaltyVelLoc;

    double costPipes; /**< does not seem to be used...*/
    double pipeRepairCost; /**< */
    double pipeCapitalCost; /**< */
    double pipeEmbodiedEnergy;
    double pipeRepairEE;
    double trenchingCost;
    double trenchingEnergy;
    double lengthAvgDiameter;
    std::map<std::string, PipeEvalData> distSysData;
    std::map<std::string, PipeEvalData> allPipeData;
    std::map<double, double> pipe_lengths_by_dia;

    double costPumps;
    std::vector< std::vector<double> > pumpElevHeads;
    std::vector< std::vector<double> > pumpFlows;
    std::vector<PumpStationCosts> costPumpsBreakdown;
    PumpStationCosts net_costPumpBreakdown;

    std::vector< TankResultsInfo > tankresults_vec;
    TankResultsInfo net_tank_results;
};

enum ENLinkNodeType{ENLNK_JUNCTION, ENLNK_RESERVOIR, ENLNK_TANK};

 struct NodeInfo
{
    std::string id;
    ENLinkNodeType type;
    int index;
    std::vector<int> connectedLinkIndices;
     NodeInfo()
             : id("null"), type(ENLNK_JUNCTION), index(-1), connectedLinkIndices(0)
     {

     }
};

enum ENLinkLinkType{ENLNK_CVPIPE, ENLNK_PIPE, ENLNK_PUMP, ENLNK_PRV, ENLNK_PSV, ENLNK_PBV, ENLNK_FCV, ENLNK_TCV, ENLNK_GPV};

struct LinkInfo
{
    std::string id;
    ENLinkLinkType  type;
    int index;
    int fromNode;
    int toNode;
    LinkInfo()
            :id("null"), type(ENLNK_PIPE), index(-1), fromNode(-1), toNode(-1)
    {

    }
};


typedef void * EN_Project;
typedef struct _ENsimulation_t * ENsimulation_t;


class ENMultiObjEvaluator {

public:
    typedef boost::shared_ptr< ENMultiObjEvaluator > ENMultiObjEvalSPtr;

    /**
     * Cnstr
     */
    ENMultiObjEvaluator();

    /**
     * Dstr
     */
    virtual
    ~ENMultiObjEvaluator();

    void
    initialise(boost::filesystem::path config_file, boost::filesystem::path _working_dir);

    boost::shared_ptr<std::vector<int>  >
    getPipeDVBounds();

    int
    runEN(const int* dvs);

    double
    getPipeCapitalCost();

    double
    getNetworkResilience();

    double
    getSumPressureTooHigh();

    double
    getMaxPressureTooHigh();

    double
    getSumPressureTooLow();

    double
    getMinPressureTooLow();

    double
    getSumHeadTooHigh();

    double
    getMaxHeadTooHigh();

    double
    getSumHeadTooLow();

    double
    getMinHeadTooLow();

    double
    getSumVelocityTooHigh();

    double
    getMaxVelocityTooHigh();



//    void
//    fileSaver(std::string filename, bool doSaveReport,
//              bool doUseInpSpecs, std::string reportname);
//
//    void
//    fileSaver(std::string filename, fs::path & _outDirPath,
//              bool doSaveReport, bool doUseInpSpecs,
//              std::string reportname);
//
//    void
//    fileSaver(std::string filename, std::string _outDirPath,
//              bool doSaveReport, bool doUseInpSpecs,
//              std::string reportname);
//
//    bool writeHeadViolation;



private:

    Results results;

    void
    setPipeChoices(const int* dvs);

    void
    evalPipes(const int* dvs);

    void
    evalPressurePenalty();

    void
    evalHeadPenalty();

    void
    evalVelocityPenalty();

    void
    evalHydraulicConstraints();

    void
    evalResiliency();


    bool
    errors(int err, std::string what = "");

    //Function pointers in the epanet library
//    boost::function<int (char*, char*, char*) > ENopen_f;
//    boost::function<int (void) > ENopenH_f;
// The above functions are now just local to the initialisation routines for each version of en3.

    boost::function<int (char*, int*) > ENgetlinkindex_f;
    boost::function<int (int, char*) > ENgetlinkid_f;
    boost::function<int (int, int*) > ENgetlinktype_f;
    boost::function<int (int, int*, int*) > ENgetlinknodes_f;
    boost::function<int (int, int, double) > ENsetlinkvalue_f;
    boost::function<int (int, int, double *) > ENgetlinkvalue_f;
    boost::function<int (char *, int *) > ENgetnodeindex_f;
    boost::function<int (int, int, double *) > ENgetnodevalue_f;
    boost::function<int (int, char *) > ENgetnodeid_f;
    boost::function<int (int, int*) > ENgetnodetype_f;
    boost::function<int (int, int, double) > ENsetnodevalue_f;
    boost::function<int (int, int *) > ENgetcount_f;
    boost::function<int (char *, int *) > ENgetpatternindex_f;
    boost::function<int (int, float *, int) > ENsetpattern_f;
    boost::function<int (int, int, double) > ENsetpatternvalue_f;
    boost::function<int (void) > ENinitH_f;
    boost::function<int (long *) > ENrunH_f;
    boost::function<int ( long *) > ENnextH_f;
    boost::function<int (void) > ENsaveH_f;
    boost::function<int (void) > ENresetreport_f;
    boost::function<int (char *) > ENsetreport_f;
    boost::function<int (void) > ENreport_f;
    boost::function<int (char *) > ENsaveinpfile_f;
    boost::function<int (int, char *, int) > ENgeterror_f;
    boost::function<int (void) > ENcloseH_f;
    boost::function<int (void) > ENclose_f;

    std::map< std::string, int > linkIndices;
    std::map< std::string, int > nodeIndices;
    OptData_SPtr params;

    boost::filesystem::path working_dir;
    boost::filesystem::path outputDir;
    boost::scoped_array< char > ENFile_cstr;
//    std::string ENFile; /**< The location of the epanet inp file in the system */
    boost::filesystem::path en_inp_path;
    std::string reportFileName;
    std::string binaryFileName;
    boost::scoped_array< char > reportFile_cstr;
    boost::scoped_array< char > binaryFile_cstr;
    unsigned dlOpenCount = 0;

//    bool initParams;
    boost::filesystem::path orig_opt_file_path;
    std::string epanet_dylib_loc; /**< The system path to the location of the epanet shared library **/
    boost::filesystem::path epanet_dylib_path;
    std::string epanet_error_log_name; /**< The location where the error log file will be appended to */
    std::ofstream err_out;
    void * en_lib_handle;

    boost::scoped_array< char > linkID;
    boost::scoped_array< char > nodeID;

    int node_count;
    int link_count;
    std::vector<NodeInfo> nodes;
    std::vector<LinkInfo> links;

    void initialise_OWA_EN2();
    void initialise_OWA_EN3();
    void initialise_Ibanez_EN2(); //http://lopez-ibanez.eu/epanet-thread-safe

    void open_OWA_EN2();
    void open_OWA_EN3();
    void open_Ibanez_EN2(); //http://lopez-ibanez.eu/epanet-thread-safe

    void close_OWA_EN2();
    void close_OWA_EN3();
    void close_Ibanez_EN2(); //http://lopez-ibanez.eu/epanet-thread-safe

    void setLinkIndices();
    void setNodeIndices();
    void getENInfo();

    boost::filesystem::path en_report_path;
    boost::filesystem::path en_bin_path;

    EN_Project en3_simulation;
    ENsimulation_t ibanez_simulation;

    boost::function<EN_Project (void) > EN_createProject_OWA_EN3_f;
    boost::function<int (const char *, EN_Project) > EN_openOutput_OWA_EN3_f;
    boost::function<int (const char *, EN_Project) > EN_openReport_OWA_EN3_f;
    boost::function<int (const char *, EN_Project) > EN_loadProject_OWA_EN3_f;
//    boost::function<int (int, EN_Project) > EN_initSolver_OWA_EN3_f;

    boost::function<int (char*, char*, char*) > ENopen_owa_en2_f;
    boost::function<int (void) > ENopenH_owa_en2_f;

    boost::function<int (char*, char*, char*) > ENopen_Ibanev_f;
    boost::function<int (ENsimulation_t *) > ENopenH_Ibanev_f;

};


#endif //SRC_ENMULTIOBJEVALUATOR_H
