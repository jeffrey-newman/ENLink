/* 
 * File:   OptParser.h
 * Author: jnewman
 *
 * Created on June 7, 2010, 6:21 PM
 */

#ifndef _OPTPARSER_H
#define    _OPTPARSER_H

#include <vector>
#include <map>

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/filesystem.hpp>
//#include <bits/stl_algobase.h>


/**
	 * qi provides parsing functionality and is part of boost spirit
	 */
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

/**
 * We will be parsing strings. We define a string iterator as Str_it
 */
typedef std::string::const_iterator Str_it; /**< A string const iterator - used for reading the input of the OptFile */

/**
 * @brief this defines what the parser should skip in the optFile
 * @details think of these are grammers - they define the rules of
 * how the input file should be formatted. This one defines how
 * comments should be formatted - because these are skipped over, as well as white space
 * @see Boost spirit documentation
 */
struct Skipper : qi::grammar< Str_it >
{
    Skipper();
    qi::rule< Str_it > skip_it; /**< Rule that does the work. passes over commetns and whitespace in the file */
    qi::rule<Str_it> comment; /**< Rule that detects comments in the file.*/
    char comment_char; /**< Define what the comment character is. I.e. we follow the Epanet inp file tradition of ';' */
};

/**
 * @brief a custom skipper that skips over blanks but does not skip over end of lines
 * @details think of these as grammers - they define the rules of
 * how the input file should be formatted. This one defines how
 * comments should be formatted - because these are skipped over, as well as white space, but not end of lines
 * @see Boost spirit documentation
 */
struct No_eol_skip : qi::grammar< Str_it > /**< Rule that does the work. passes over commetns and whitespace in the file */
{
    No_eol_skip();
    qi::rule< Str_it > skip_it; /**< Rule that does the work. passes over commetns and whitespace in the file */
    qi::rule<Str_it> comment; /**< Rule that detects comments in the file.*/
    char comment_char; /**< Define what the comment character is. I.e. we follow the Epanet inp file tradition of ';' */
};

class IFException : public std::runtime_error
{
public:
    IFException();
    IFException(std::string what);
};

class ObjectiveInputError : public IFException
{
public:
    ObjectiveInputError();
    ObjectiveInputError(std::string what);
};


//----------------------------------------------------------------------
// DATA ON AVAILABLE PIPES
//----------------------------------------------------------------------

struct PipeDataProperties {
    //std::string id;
    std::string description;
    double roughnessHeight;
    double diameter;
    double cost;
    double repairCost;
    double embodiedEnergy;
    double repairEE;
    double ghg;
    double topCover;
    double sideCover;
    double bottomCover;
};

struct TreatmentProperties {
    std::string description;
    double waterload;
    double capitalcost;
    double annualOpCost;
};

struct PumpProperties {
    std::string description;
    double head;
    //double capitalcost;
    double efficiency;
};

struct VSPStationDetails {
    std::string description;
    std::string resID;
    std::string prvID;
    std::string linkID;
    std::string interNode;
    std::string outletNode;
    std::string determineAvgBy;
    int timestep;
    std::string NodeGroupID;
};

struct TankProperties {
    double init_storage;
    double surface_area;
    double height;
    double peakAllotmentDemand;
    double peakSupply;
    double peakExternalSupplyPerAllot;
    double avgSupply;
    double avgAllotmentDemand;
    double avgExternalSupplyPerAllot;
    double netExternalSupplyPerAllot;
    double cost;
    double embodied_energy;
    double other_cost;
    double other_EE;
    int numAllots;

};

struct TankChoices {
    std::string potDemandPatternID;
    std::string potNodeGroup;
    std::string rcylDemandPatternID;
    std::string rcylNodeGroup;
    std::vector<std::string> tankOptionIDs;
};

struct DistSysPipeGroups {
    int numAllotments;
    std::vector<std::string> pipeGroupIDs;
};


BOOST_FUSION_ADAPT_STRUCT( PipeDataProperties,
//(std::string, id)
(std::string, description
)//
(double, roughnessHeight)//
(double, diameter)//
(double, cost)//
(double, repairCost)//
(double, embodiedEnergy)//
(double, repairEE)//
(double, ghg)//
(double, topCover)//
(double, sideCover)//
(double, bottomCover)//
)

BOOST_FUSION_ADAPT_STRUCT( TreatmentProperties,
(std::string, description
) //
(double, waterload)//
(double, capitalcost)//
(double, annualOpCost)//
)

BOOST_FUSION_ADAPT_STRUCT( PumpProperties, //
(std::string, description
)//
(double, head)//
//(double, capitalcost)
(double, efficiency)//
)

BOOST_FUSION_ADAPT_STRUCT( VSPStationDetails, //
(std::string, description
)//
(std::string, resID)//
(std::string, prvID)//
(std::string, linkID)//
(std::string, interNode)//
(std::string, outletNode)//
(std::string, determineAvgBy)//
(int, timestep)//
(std::string, NodeGroupID)//
)

BOOST_FUSION_ADAPT_STRUCT( TankProperties, //
(
double, init_storage)//
(double, surface_area)//
(double, height)//
(double, peakAllotmentDemand)//
(double, peakSupply)//
(double, peakExternalSupplyPerAllot)//
(double, avgSupply)//
(double, avgAllotmentDemand)//
(double, avgExternalSupplyPerAllot)//
(double, netExternalSupplyPerAllot)//
(double, cost)//
(double, embodied_energy)//
(double, other_cost)//
(double, other_EE)//
(int, numAllots)//
)

BOOST_FUSION_ADAPT_STRUCT( TankChoices, //
(std::string, potDemandPatternID
)//
(std::string, potNodeGroup)//
(std::string, rcylDemandPatternID)//
(std::string, rcylNodeGroup)//
(std::vector<std::string>, tankOptionIDs)//
)

BOOST_FUSION_ADAPT_STRUCT( DistSysPipeGroups, //
(
int, numAllotments)//
(std::vector<std::string>, pipeGroupIDs)//
)


typedef std::string::const_iterator Str_it; /**< A string const iterator - used for reading the input of the OptFile */

typedef std::map<std::string, PipeDataProperties> PipeDataType;

struct PipeData {

    PipeData(PipeDataType::iterator &it) :
            id(it->first), details(it->second) {
    }

    const std::string &id;
    PipeDataProperties &details;
};

typedef std::map<std::string, TreatmentProperties> TreatmentsType;

struct TreatmentData {

    TreatmentData(TreatmentsType::iterator &it) :
            id(it->first), details(it->second) {
    }

    const std::string &id;
    TreatmentProperties &details;
};

typedef std::vector<std::pair<std::string, PumpProperties> > PumpingOptionsType;

struct PumpOptionsData {

    PumpOptionsData(PumpingOptionsType::iterator &it) :
            id(it->first), details(it->second) {
    }

    const std::string &id;
    PumpProperties &details;
};

typedef std::map<std::string, VSPStationDetails> VSPType;

struct vspData {

    vspData(VSPType::iterator &it) :
            id(it->first), details(it->second) {
    }

    const std::string &id;
    VSPStationDetails &details;
};

typedef std::map<std::string, TankProperties> TanksType;

struct TankData {
    TankData(TanksType::iterator &it) :
            id(it->first), details(it->second) {

    }

    const std::string &id;
    TankProperties &details;
};

//----------------------------------------------------------------------
// EPANET PIPES ARE GROUPED; EACH GROUP HAS A PIPE DIAMETER USED FOR ALL PIPES IN THE GROUP
// ONE DECISION PER GROUP - WHAT SHOULD THE DIAMETER BE?
//----------------------------------------------------------------------

typedef std::map<std::string, std::vector<std::string> > LinkGroupsType;

struct LinkGroups {
    typedef std::vector<std::string> PipeIdVec;
    typedef std::vector<std::string>::iterator iterator;

    LinkGroups(LinkGroupsType::iterator &it) :
            groupId(it->first), epanetLinkIds(it->second) {
    }

    const std::string &groupId;
    PipeIdVec &epanetLinkIds;
};

typedef std::map<std::string, std::pair<double, double> > NodeConstraintsT;

struct Pressures {

    Pressures(NodeConstraintsT::iterator &it) :
            id(it->first), minPressure(it->second.first), maxPressure(
            it->second.second) {
    }

    const std::string &id;
    double &minPressure;
    double &maxPressure;

};

struct Heads {

    Heads(NodeConstraintsT::iterator &it) :
            id(it->first), minHead(it->second.first), maxHead(
            it->second.second) {
    }

    const std::string &id;
    double &minHead;
    double &maxHead;

};

typedef std::map<std::string, double> VelocityConstraintsT;

struct Velocities {

    Velocities(VelocityConstraintsT::iterator &it) :
            id(it->first), maxVelocity(it->second) {
    }

    const std::string &id;
    double &maxVelocity;
};

//----------------------------------------------------------------------
// Each Group has a list of pipes; one of these pipes must be chosen for the groups
// One pipe per group;
//----------------------------------------------------------------------
typedef std::map<std::string, std::vector<std::string> > PipeAvailOptionsType;

struct PipeAvailOptions {
    typedef std::vector<std::string> PipeDataIdVec;
    typedef std::vector<std::string>::iterator iterator;

    PipeAvailOptions(PipeAvailOptionsType::iterator &it) :
            groupId(it->first), pipeDataIds(it->second) {
    }

    const std::string &groupId;
    PipeDataIdVec &pipeDataIds;
};

//Each storage has a list of avialble tanks; on eof these must be chosen for the groups
typedef std::map<std::string, TankChoices> TankAvailableOptionsType;

struct TankAvailableOptions {
    TankAvailableOptions(TankAvailableOptionsType::iterator &it) :
            storageId(it->first), details(it->second) {
    }

    const std::string &storageId;
    TankChoices &details;
};

typedef std::map<std::string, DistSysPipeGroups> DistSysPipeGroupsType;

struct DistSystems {
    DistSystems(DistSysPipeGroupsType::iterator &it) :
            distSysId(it->first), details(it->second) {
    }

    const std::string &distSysId;
    DistSysPipeGroups &details;
};

typedef std::map<std::string, std::vector<std::string> > NodeGroupsType;

struct NodeGroups {
    typedef std::vector<std::string> NodeIdVec;
    typedef std::vector<std::string>::iterator iterator;

    NodeGroups(NodeGroupsType::iterator &it) :
            groupId(it->first), nodeIds(it->second) {
    }

    const std::string &groupId;
    NodeIdVec &nodeIds;
};

typedef std::pair<int, double> TarifPair;
typedef std::map<int, double> TarifStructure;

struct OptData {
    OptData();

    PipeDataType pipe_data_map;
    LinkGroupsType link_groups_data;
    NodeGroupsType node_groups_data;
    PipeAvailOptionsType pipe_availOpt_data;
    TankAvailableOptionsType tank_availOpt_data;
    VSPType vsp_data;
    NodeConstraintsT pressure_constraints;
    VelocityConstraintsT velocity_constraints;
    NodeConstraintsT head_constraints;
    double PressurePenaltyCost;
    double PressurePenaltyEE;
    double PressurePenaltyWtr;
    double HeadPenaltyCost;
    double HeadPenaltyEE;
    double HeadPenaltyWtr;
    double VelocityPenaltyCost;
    double VelocityPenaltyEE;
    double VelocityPenaltyWtr;
    double TreatmentPenaltyCost;
    double TreatmentPenaltyEE;
    double TreatmentPenaltyWtr;
    double TrenchingCost;
    double FittingsCost;
    double FittingsEE;
    double GreywaterSysCost;
    double HouseConnectionsCost;
    double GreywaterEE;
    std::string epanet_dylib_loc;
    std::string iuwmod_nodeext_loc;
    std::string iuwmod_linkext_loc;
    double trenchEE;
    double drateEE;
    double drateCost;
    double netWaterDemand;
    double greywaterContrib;
    double energyCost;
    int planHorizon;
    int numAllotments;
    TreatmentsType treatments_data;
    PumpingOptionsType pump_options_data;
    // Those relating to pumping stations:
    double peakPumpEfficiency; //NOT APPARENTLY USED
    double avgPumpEfficiency;
    int pumpElectricsLife;
    int pumpMechanicsLife;
    int pumpCivilLife;
    int treatmentLife;
    TanksType storage_tank_data;
    DistSysPipeGroupsType dist_groups_data;
    std::string pot_topup_node_name;
    std::string en_pat_id;
    double treatementLogReduction;

    TarifStructure waterTariffs;
    double externalWaterEnergy;
    double externalTreatmentCost;
    double externalTreatmentEnergy;

    bool isDistSystem;
    bool isCollSystem;
    bool isWtrBalSystem;
    bool isOptPipes;
    bool isOptVSP;
    bool isOptTanks;
    bool isCostObj;
    bool isNetworkResiliencyObj;
    bool isEnergyObj;
    bool isWtrObj;
    bool isPressureConstraint;
    bool isHeadConstraint;
    bool isVelocityConstraint;
    bool allTankChoicesTheSame;

    std::string epanetFile;
    std::string iuwmodFile;

    double dailyPumpOpHours;
    double dayPeak_hourPeakRatio;

    std::vector<std::string> *
    getOrNewLinkGroup(const std::string &name);

    std::vector<std::string> *
    getOrNewPipeOptions(const std::string &groupName);

    void
    setOrNewPressureConstraint(const std::string &name,
                               const double &minP, const double &maxP);

    void
    setOrNewHeadConstraint(const std::string &name,
                           const double &minP, const double &maxP);

    void
    setOrNewVelocityConstraint(const std::string &name,
                               const double &maxV);

};

typedef boost::shared_ptr <OptData> OptData_SPtr;

/**
 * @brief Defines the grammer (format) of the data to be read into the program, and holds this data also.
 * @details. This is more then a grammer (format) specification of the input file). It actually stores
 *  the data that it reads in
 */
struct OptFileParser : qi::grammar<Str_it, Skipper> {
public:
    OptFileParser();

    OptFileParser(OptData_SPtr _optData);

    //This is the highest-level rule.
    qi::rule <Str_it, Skipper> start;

    qi::rule <Str_it, Skipper> specification_rule;
    qi::rule <Str_it, Skipper> specification_header;
    qi::rule <Str_it, Skipper> systemRule;
    qi::rule <Str_it, Skipper> OptimiseRule;
    qi::rule <Str_it, Skipper> ObjectivesRule;
    qi::rule <Str_it, Skipper> ConstraintsRule;
    qi::rule<Str_it, bool(), Skipper> isDistSystem_rule;
    qi::rule<Str_it, bool(), Skipper> isCollSystem_rule;
    qi::rule<Str_it, bool(), Skipper> isWtrBalSystem_rule;
    qi::rule<Str_it, bool(), Skipper> isOptPipes_rule;
    qi::rule<Str_it, bool(), Skipper> isOptVSP_rule;
    qi::rule<Str_it, bool(), Skipper> isOptTanks_rule;
    qi::rule<Str_it, bool(), Skipper> allTanksSameSize_rule;
    qi::rule<Str_it, bool(), Skipper> allowVariableSize_rule;
    qi::rule<Str_it, bool(), Skipper> isCostObj_rule;
    qi::rule<Str_it, bool(), Skipper> isNetworkResiliency_rule;
    qi::rule<Str_it, bool(), Skipper> isEnergyObj_rule;
    qi::rule<Str_it, bool
            (), Skipper> isWtrObj_rule;
    qi::rule<Str_it, bool
            (), Skipper> isPressureConstraint_rule;
    qi::rule<Str_it, bool
            (), Skipper> isHeadConstraint_rule;
    qi::rule<Str_it, bool
            (), Skipper> isVelocityConstraint_rule;
    qi::rule<Str_it, std::string
            (), Skipper> en_inp_rule;
    qi::rule<Str_it, std::string
            (), Skipper> iuwmod_inp_rule;

    //Generic rules, used a lot.
    qi::rule<Str_it, std::string
            (), Skipper> white_seperated_string;
    qi::rule<Str_it, std::string
            (), Skipper> white_seperated_string_then_eol;
    qi::rule<Str_it, std::vector<std::string>
            (), Skipper> list_white_seperated_string;
    qi::rule<Str_it, double
            (), Skipper> double_then_end_of_line;
    qi::rule<Str_it, int
            (), Skipper> int_then_end_of_line;
    qi::rule <Str_it, Skipper> headers;

    //Rules that define the Pipe data section of the input file
    qi::rule <Str_it, Skipper> pipe_data_header;
    qi::rule<Str_it, std::string
            (), Skipper> pipe_datum_id;
    qi::rule<Str_it, std::string
            (), Skipper> pipe_data_description;
    qi::rule<Str_it, PipeDataProperties
            (), Skipper> pipe_datum_info;
    qi::rule<Str_it, std::pair<std::string, PipeDataProperties>
            (), Skipper> pipe_datum;
    qi::rule<Str_it, PipeDataType
            (), Skipper> pipe_data;

    //rules that define the treatments section of the input file
    qi::rule <Str_it, Skipper> treatments_header;
    qi::rule<Str_it, std::string
            (), Skipper> treatment_id;
    qi::rule<Str_it, std::string
            (), Skipper> treatment_description;
    qi::rule<Str_it, double
            (), Skipper> capacity;
    qi::rule<Str_it, double
            (), Skipper> capital_cost;
    qi::rule<Str_it, double
            (), Skipper> operate_cost;
    qi::rule<Str_it, TreatmentProperties
            (), Skipper> treatment_info;
    qi::rule<Str_it, std::pair<std::string, TreatmentProperties>
            (), Skipper> treatment_datum;
    qi::rule<Str_it, TreatmentsType
            (), Skipper> treatment_data;

    //rules that define the tank properties part of the input file
    qi::rule <Str_it, Skipper> tanks_header;
    qi::rule<Str_it, std::string
            (), Skipper> tank_id;
//            qi::rule< Str_it, TankProperties
//            (), Skipper > tank_info1;
//            qi::rule< Str_it, TankProperties
//            (), Skipper > tank_info2;

    qi::rule<Str_it, TankProperties
            (), Skipper> tank_info3;
    qi::rule<Str_it, std::pair<std::string, TankProperties>
            (), Skipper> tank_datum;
    qi::rule<Str_it, TanksType
            (), Skipper> tank_data;

    //rules that define the pumping section of the input file
    qi::rule <Str_it, Skipper> pumping_header;
    qi::rule<Str_it, std::string
            (), Skipper> pump_id;
    qi::rule<Str_it, std::string
            (), Skipper> pump_description;
    qi::rule<Str_it, double
            (), Skipper> head;
    qi::rule<Str_it, double
            (), Skipper> efficiency;
    qi::rule<Str_it, PumpProperties
            (), Skipper> pump_info;
    qi::rule<Str_it, std::pair<std::string, PumpProperties>
            (), Skipper> pump_datum;
    qi::rule<Str_it, PumpingOptionsType
            (), Skipper> pump_data;

    //Rules that define the pipe group section of the input file
    qi::rule <Str_it, Skipper> pipe_group_header;
    qi::rule<Str_it, std::string
            (), Skipper> pipe_group_id;
    qi::rule<Str_it, std::vector<std::string>
            (), Skipper> epanet_pipe_ids;
    qi::rule<Str_it,
            std::pair<std::string, std::vector<std::string> >
                    (), Skipper> pipe_group;
    qi::rule<Str_it, LinkGroupsType
            (), Skipper> pipe_groups;

    //Rules that define the pipe group section of the input file
    qi::rule <Str_it, Skipper> node_group_header;
    qi::rule<Str_it, std::string
            (), Skipper> node_group_id;
    qi::rule<Str_it, std::vector<std::string>
            (), Skipper> epanet_node_ids;
    qi::rule<Str_it,
            std::pair<std::string, std::vector<std::string> >
                    (), Skipper> node_group;
    qi::rule<Str_it, NodeGroupsType
            (), Skipper> node_groups;

    // rules that define the pipe choices section of the input file
    qi::rule <Str_it, Skipper> pipe_choices_header;
    qi::rule<Str_it, std::vector<std::string>
            (), Skipper> pipe_data_ids;
    qi::rule<Str_it,
            std::pair<std::string, std::vector<std::string> >
                    (), Skipper> pipe_choice;
    qi::rule<Str_it, PipeAvailOptionsType
            (), Skipper> pipe_choices;

    // rules that define the tank choices section of the input file
    qi::rule <Str_it, Skipper> tank_choices_header;
    qi::rule<Str_it, std::vector<std::string>
            (), Skipper> tank_data_ids;
    qi::rule<Str_it, std::string
            (), Skipper> potDemandPatternID_rule;
    qi::rule<Str_it, std::string
            (), Skipper> potNodeGroup_rule;
    qi::rule<Str_it, std::string
            (), Skipper> rcylDemandPatternID_rule;
    qi::rule<Str_it, std::string
            (), Skipper> rcylNodeGroup_rule;
    qi::rule<Str_it, TankChoices
            (), Skipper> tank_choice_info;
    qi::rule<Str_it, std::pair<std::string, TankChoices>
            (), Skipper> tank_choice_datum;
    qi::rule<Str_it, PipeAvailOptionsType
            (), Skipper> tank_choices;


    //rules that define the pipe groups within each dist. system
    qi::rule <Str_it, Skipper> dist_groups_header;
    qi::rule<Str_it, std::string
            (), Skipper> dist_group_ID_rule;
    qi::rule<Str_it, std::vector<std::string>
            (), Skipper> groups_in_sys_ids_rule;
    qi::rule<Str_it, DistSysPipeGroups
            (), Skipper> dist_groups_info;
    qi::rule<Str_it, std::pair<std::string, DistSysPipeGroups>
            (), Skipper> dist_groups_datum;
//             qi::rule< Str_it, DistSysPipeGroupsType
//             (), Skipper > dist_groups;
    qi::rule <Str_it, Skipper> dist_groups;


    //Rules that define the penalities part of the input file
    qi::rule <Str_it, Skipper> penalties_header;
    qi::rule <Str_it, Skipper> pressureConstraintPenalty_rule;
    qi::rule <Str_it, Skipper> headConstraintPenalty_rule;
    qi::rule <Str_it, Skipper> velocityConstraintPenalty_rule;
    qi::rule <Str_it, Skipper> treatmentConstraintPenalty_rule;
    qi::rule <Str_it, Skipper> penalties;

    //Rules that define the variable speed pump part of the specificiation file
    qi::rule <Str_it, Skipper> vsp_header;
    qi::rule<Str_it, VSPStationDetails
            (), Skipper> vsp_info1;
    qi::rule<Str_it, VSPStationDetails
            (), Skipper> vsp_info2;
    qi::rule<Str_it, std::pair<std::string, VSPStationDetails>
            (), Skipper> vsp_datum1;
    qi::rule<Str_it, std::pair<std::string, VSPStationDetails>
            (), Skipper> vsp_datum2;
    qi::rule<Str_it, std::string
            (), Skipper> vsp_avg_rule;
    qi::rule<Str_it, std::string
            (), Skipper> vsp_ts_rule;
    qi::rule<Str_it, VSPType
            (), Skipper> vsp_data_rule;

    //rules that define the evaluation part of the input file.
    qi::rule <Str_it, Skipper> evaluation_header;
    qi::rule<Str_it, double
            (), Skipper> trenchingCost_rule;
    qi::rule<Str_it, double
            (), Skipper> fittingsCost_rule;
    qi::rule<Str_it, double
            (), Skipper> fittingsEE_rule;
    qi::rule<Str_it, double
            (), Skipper> greywaterSysCost_rule;
    qi::rule<Str_it, double
            (), Skipper> houseConnectionsCost_rule;
    qi::rule<Str_it, double
            (), Skipper> greywaterEE_rule;
    qi::rule <Str_it, Skipper> evaluation;
    qi::rule<Str_it, std::string
            (), Skipper> en_dll_rule;

    qi::rule<Str_it, std::string
            (), Skipper> iuwm_nodeext_rule;
    qi::rule<Str_it, std::string
            (), Skipper> iuwm_linkext_rule;

    qi::rule<Str_it, double
            (), Skipper> trenchEE_rule;
    qi::rule<Str_it, double
            (), Skipper> discountEE_rule;
    qi::rule<Str_it, double
            (), Skipper> discountCost_rule;
    qi::rule<Str_it, double
            (), Skipper> netWaterDemand_rule;
    qi::rule<Str_it, double
            (), Skipper> greywaterContrib_rule;
    qi::rule<Str_it, int
            (), Skipper> numAllotments_rule;
    qi::rule<Str_it, double
            (), Skipper> energyCost_rule;
    qi::rule<Str_it, int
            (), Skipper> planPeriod_rule;
    qi::rule<Str_it, double
            (), Skipper> peakPumpEfficiency_rule;
    qi::rule<Str_it, double
            (), Skipper> avgPumpEfficiency_rule;
    qi::rule<Str_it, int
            (), Skipper> pumpElectricsLife_rule;
    qi::rule<Str_it, int
            (), Skipper> pumpMechanicsLife_rule;
    qi::rule<Str_it, int
            (), Skipper> pumpCivilLife_rule;
    qi::rule<Str_it, int
            (), Skipper> treatmentLife_rule;
    qi::rule<Str_it, double
            (), Skipper> dailyPumpOpHours_rule;
    qi::rule<Str_it, double
            (), Skipper> dayPeak_hourPeakRatio_rule;

    qi::rule<Str_it, std::string
            (), Skipper> iuwm_topupNde_rule;

    qi::rule<Str_it, std::string
            (), Skipper> en_pat_id_rule;

    qi::rule<Str_it, double
            (), Skipper> treatLogReduction_rule;

    qi::rule <Str_it, Skipper> extWaterEnergyRule;
    qi::rule <Str_it, Skipper> extTreatEnergyRule;
    qi::rule <Str_it, Skipper> extTreatCostRule;
    qi::rule<Str_it, TarifPair(), Skipper> tarifPairRule;
    qi::rule <Str_it, Skipper> extWaterCostRule;


    //rules that define the pressure constrain part of the input file
    qi::rule <Str_it, Skipper> pressures_header;
    qi::rule<Str_it, std::string
            (), Skipper> node_id;
    qi::rule<Str_it, double
            (), Skipper> min_pressure;
    qi::rule<Str_it, double
            (), Skipper> max_pressure;
    qi::rule<Str_it, std::pair<double, double>
            (), Skipper> mm_pressure_constraint;
    qi::rule<Str_it,
            std::pair<std::string, std::pair<double, double> >
                    (), Skipper> pressure_constraint;
    qi::rule<Str_it, NodeConstraintsT
            (), Skipper> pressure_constraints_r;

    //rules that define the heads constrain part of the input file
    qi::rule <Str_it, Skipper> heads_header;
    qi::rule<Str_it, double
            (), Skipper> min_head;
    qi::rule<Str_it, double
            (), Skipper> max_head;
    qi::rule<Str_it, std::pair<double, double>
            (), Skipper> mm_head_constraint;
    qi::rule<Str_it,
            std::pair<std::string, std::pair<double, double> >
                    (), Skipper> head_constraint;
    qi::rule<Str_it, NodeConstraintsT
            (), Skipper> head_constraints_r;

    //rules that define the velocities part of the input file
    qi::rule <Str_it, Skipper> velocities_header;
    qi::rule<Str_it, std::string
            (), Skipper> pipe_id;
    qi::rule<Str_it, double
            (), Skipper> max_velocity;
    qi::rule<Str_it, std::pair<std::string, double>
            (), Skipper> velocity_constraint;
    qi::rule<Str_it, VelocityConstraintsT
            (), Skipper> velocity_constraints_r;

    No_eol_skip customSkipper;

    boost::shared_ptr <OptData> optData;

private:
    void
    init();

};

typedef boost::shared_ptr <OptFileParser> OptParser_SPtr;

std::ostream &
operator<<(std::ostream &os, const OptData &optdata);

void
printOptToFile(const OptData &optdata, std::string fileName);

void
printOptToFile_CStr(const OptData &optdata, const char *fileName);


//Function for getting the
OptData_SPtr
getObjectiveInput(boost::filesystem::path optFile);

#endif	/* _OPTPARSER_H */

