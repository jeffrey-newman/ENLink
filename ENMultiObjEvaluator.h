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
#include "OptParser.h"


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

enum NodeType{JUNCTION, RESERVOIR, TANK};

struct NodeInfo
{
    std::string id;
    NodeType type;
    int index;
    std::vector<int> connectedLinkIndices;
};

enum LinkType{CVPIPE, PIPE, PUMP, PRV, PSV, PBV, FCV, TCV, GPV};

struct LinkInfo
{
    std::string id;
    LinkType  type;
    int index;
    int fromNode;
    int toNode;
};

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
    initialise(boost::filesystem::path config_file);

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
    int (*ENopen)(char *, char *, char *);

    int (*ENopenH)(void);

    int (*ENgetlinkindex)(char *, int *);

    int (*ENgetlinkid)(int, char*);

    int (*ENgetlinktype)(int, int*);

    int (*ENgetlinknodes)(int, int*, int*);

    int (*ENsetlinkvalue)(int, int, float);

    int (*ENgetlinkvalue)(int, int, float *);



    int (*ENgetnodeindex)(char *, int *);

    int (*ENgetnodevalue)(int, int, float *);

    int (*ENgetnodeid)(int, char*);

    int (*ENgetnodetype)(int, int*);

    int (*ENsetnodevalue)(int, int, float);

    int (*ENgetcount)(int, int *);

    int (*ENgetpatternindex)(char *, int *);

    int (*ENsetpattern)(int, float *, int);

    int (*ENsetpatternvalue)(int, int, float);

    int (*ENinitH)(int);

    int (*ENrunH)(long *);

    int (*ENnextH)(long *);

    int (*ENsaveH)(void);

    int (*ENresetreport)(void);

    int (*ENsetreport)(char *);

    int (*ENreport)(void);

    int (*ENsaveinpfile)(char *);

    int (*ENgeterror)(int, char *, int);

    int (*ENcloseH)(void);

    int (*ENclose)(void);

    std::map< std::string, int > linkIndices;
    std::map< std::string, int > nodeIndices;
    OptData_SPtr params;

    boost::filesystem::path workingDir;
    boost::filesystem::path outputDir;
    boost::scoped_array< char > ENFile_cstr;
    std::string ENFile; /**< The location of the epanet inp file in the system */
    std::string reportFileName;
    std::string binaryFileName;
    boost::scoped_array< char > reportFile_cstr;
    boost::scoped_array< char > binaryFile_cstr;
    unsigned dlOpenCount;

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

    void setLinkIndices();
    void setNodeIndices();
    void getENInfo();


};


#endif //SRC_ENMULTIOBJEVALUATOR_H
