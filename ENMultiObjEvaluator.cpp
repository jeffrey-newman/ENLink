//
// Created by a1091793 on 2/3/17.
//

#include <dlfcn.h>
#include "ENMultiObjEvaluator.h"
#include <boost/lexical_cast.hpp>
#include <iomanip>

int EN_ELEVATION;
int EN_BASEDEMAND;
int EN_PATTERN;
int EN_EMITTER;
int EN_INITQUAL;
int EN_SOURCEQUAL;
int EN_SOURCEPAT;
int EN_SOURCETYPE;
int EN_TANKLEVEL;
int EN_DEMAND;
int EN_HEAD;
int EN_PRESSURE;
int EN_QUALITY;
int EN_SOURCEMASS;
int EN_INITVOLUME;
int EN_MIXMODEL;
int EN_MIXZONEVOL;

int EN_TANKDIAM;
int EN_MINVOLUME;
int EN_VOLCURVE;
int EN_MINLEVEL;
int EN_MAXLEVEL;
int EN_MIXFRACTION;
int EN_TANK_KBULK;
int EN_TANKVOLUME;
int EN_MAXVOLUME;

int EN_DIAMETER;
int EN_LENGTH;
int EN_ROUGHNESS;
int EN_MINORLOSS;
int EN_INITSTATUS;
int EN_INITSETTING;
int EN_KBULK;
int EN_KWALL;
int EN_FLOW;
int EN_VELOCITY;
int EN_HEADLOSS;
int EN_STATUS;
int EN_SETTING;
int EN_ENERGY;
int EN_LINKQUAL;
int EN_LINKPATTERN;

int EN_DURATION;
int EN_HYDSTEP;
int EN_QUALSTEP;
int EN_PATTERNSTEP;
int EN_PATTERNSTART;
int EN_REPORTSTEP;
int EN_REPORTSTART;
int EN_RULESTEP;
int EN_STATISTIC;
int EN_PERIODS;
int EN_STARTTIME;
int         EN_HTIME;
int EN_QTIME;
int         EN_HALTFLAG;
int EN_NEXTEVENT;

int EN_ITERATIONS;
int         EN_RELATIVEERROR;

int EN_NODECOUNT;
int EN_TANKCOUNT;
int EN_LINKCOUNT;
int EN_PATCOUNT;
int EN_CURVECOUNT;
int EN_CONTROLCOUNT;

int EN_JUNCTION;
int EN_RESERVOIR;
int EN_TANK;

int EN_CVPIPE;
int EN_PIPE;
int EN_PUMP;
int EN_PRV;
int EN_PSV;
int EN_PBV;
int EN_FCV;
int EN_TCV;
int EN_GPV;

int EN_NONE;
int EN_CHEM;
int EN_AGE;
int EN_TRACE;

int EN_CONCEN;
int EN_MASS;
int EN_SETPOINT;
int EN_FLOWPACED;

int EN_CFS;
int EN_GPM;
int EN_MGD;
int EN_IMGD;
int EN_AFD;
int EN_LPS;
int EN_LPM;
int EN_MLD;
int EN_CMH;
int EN_CMD;

int EN_TRIALS;
int EN_ACCURACY;
int EN_TOLERANCE;
int EN_EMITEXPON;
int EN_DEMANDMULT;

int EN_LOWLEVEL;
int EN_HILEVEL;
int EN_TIMER;
int EN_TIMEOFDAY;

int EN_AVERAGE;
int EN_MINIMUM;
int EN_MAXIMUM ;
int EN_RANGE ;

int EN_MIX1;
int EN_MIX2 ;
int EN_FIFO ;
int EN_LIFO ;

int EN_NOSAVE ;
int EN_SAVE;
int EN_INITFLOW ;
int EN_SAVE_AND_INIT;

int EN_CONST_HP;
int         EN_POWER_FUNC;
int EN_CUSTOM;

//Additional variables in Ibanez's version of EN2
int EN_VOLUME;
int EN_UPATTERN;
int EN_INITVOL;
int EN_SCHEDULE;
int EN_CLOCKSTART;
int EN_PUMPCOUNT;
int EN_RESERVCOUNT;
int EN_JUNCSCOUNT;

//Additional variables in EN3
//int EN_BASEDEMAND;
int EN_EMITTERFLOW	;
int EN_FULLDEMAND	;
int EN_ACTUALDEMAND	;
int EN_OUTFLOW		;
int EN_LEAKCOEFF1	;
int EN_LEAKCOEFF2	;
int EN_LEAKAGE		;
int EN_STARTDATE	;
int EN_RULECOUNT	;
int EN_RESVCOUNT	;
int EN_QUALTOL		;
int EN_HYDTOL		;
int EN_MINPRESSURE	;
int EN_MAXPRESSURE	;
int EN_PRESSEXPON	;
int EN_NETLEAKCOEFF1;
int EN_NETLEAKCOEFF2;
int EN_BASEPATTERN;
int EN_NOINITFLOW;

int evaluator_id = 0;



#include "ENFunctFlt2Dbl.h"
#include "InitialiseIbanezEN2.h"
#include "InitialiseOwaEN2.h"
#include "InitialiseEN3.h"
// --- Define the EPANET toolkit constants



typedef struct _ENsimulation_t * ENsimulation_t;

ENMultiObjEvaluator::ENMultiObjEvaluator()
: logging(boost::none)
{

}

ENMultiObjEvaluator::~ENMultiObjEvaluator()
{
    if (this->params->en_lib_version == OWA_EN2)
    {
        this->close_OWA_EN2();
    }
    else if(this->params->en_lib_version == OWA_EN3)
    {
        this->close_OWA_EN3();
    }
    else if(this->params->en_lib_version == Ibanev_EN2)
    {
        this->close_Ibanez_EN2();
    }
    else
    {
        std::cout << "Error: Epanet Library Version unspecified. Library path also probably not specified\n";
    }
    // Close Ratings library
    if (dlclose(en_lib_handle) != 0) {
        printf("[%s] Problem closing library: %s", __FILE__, dlerror());
    }
    if (do_remove_work_dir_on_exit)
    {
        boost::filesystem::remove_all(working_dir);
    }
}

void
ENMultiObjEvaluator::initialise(boost::filesystem::path opt_cfg_path, boost::filesystem::path _working_dir, bool _do_remove_work_dir_on_exit)
{
    namespace fs = boost::filesystem;

    do_remove_work_dir_on_exit = _do_remove_work_dir_on_exit;
//    if (initParams)
//    {
        //copy the params to the working directory.
        // Transfer a copy of the opt cfg file to the working directory...
//        fs::path working_opt_path = this->working_dir
//                                    / orig_opt_file_path.filename();
//        if (!fs::exists(working_opt_path))
//        {
//            fs::copy_file(orig_opt_file_path, working_opt_path);
//        }
        //open the params
        params = getObjectiveInput(opt_cfg_path);

    // Copy project directory into working directory
    std::string temp_dir_template = "WDSOpt_worker" + std::to_string(evaluator_id++) + "_%%%%-%%%%";
    working_dir = boost::filesystem::unique_path(_working_dir / temp_dir_template);
    if (!fs::exists(this->working_dir))
    {
        if (!fs::create_directories(this->working_dir))
        {
            std::string err = "Unable to create EN working directory: "
                              + this->working_dir.string();
            throw std::runtime_error(err);
        }
    }
    boost::filesystem::path epanet_file_path(params->epanetFile);
    if (!fs::exists(epanet_file_path))
    {
        std::string err = "Epanet input file does not exist: "
                          + en_inp_path.string();
        throw std::runtime_error(err);
    }
    en_inp_path = working_dir / epanet_file_path.filename();
    boost::filesystem::copy_file(epanet_file_path, en_inp_path);

    epanet_dylib_path = params->epanet_dylib_loc;
    if (!fs::exists(epanet_dylib_path))
    {
        std::string err = "Epanet library does not exist at: "
                          + epanet_dylib_path.string();
        throw std::runtime_error(err);
    }


    ++dlOpenCount;

    //Work out the location for the report file....
    std::string tmpReportName = reportFileName
                                + boost::lexical_cast< std::string >(dlOpenCount) + ".rpt";
    en_report_path = this->working_dir / tmpReportName;

    //create the report and binary files...
    reportFile_cstr.reset(new char[en_report_path.string().size() + 1]);
	strcpy(reportFile_cstr.get(), en_report_path.string().c_str());

    //Woprk out the location for the binary file...
    std::string tmpBinName = binaryFileName
                             + boost::lexical_cast< std::string >(dlOpenCount) + ".bin";
    en_bin_path = this->working_dir / tmpBinName;

    binaryFile_cstr.reset(new char[en_bin_path.string().size() + 1]);
	strcpy(binaryFile_cstr.get(), en_bin_path.string().c_str());


    if (this->params->en_lib_version == OWA_EN2)
    {
        this->initialise_OWA_EN2();
        this->open_OWA_EN2();
    }
    else if(this->params->en_lib_version == OWA_EN3)
    {
        this->initialise_OWA_EN3();
        this->open_OWA_EN3();
    }
    else if(this->params->en_lib_version == Ibanev_EN2)
    {
        this->initialise_Ibanez_EN2();
        this->open_Ibanez_EN2();
    }
    else
    {
        std::cout << "Error: Epanet Library Version unspecified. Library path also probably not specified\n";
    }

    // Determine the pipe indices for the links we are optimising choice of pipe for.
    this->setLinkIndices();
    this->setNodeIndices();
    this->getENInfo();

}

bool
ENMultiObjEvaluator::log()
{
    if (!logging)
    {
        boost::filesystem::path log_path = working_dir.parent_path() / "enLink_calcs.log";
        logging = boost::shared_ptr<std::ofstream>(new std::ofstream(log_path.string().c_str()));
        if(!(logging.value()->is_open()))
        {
            logging = boost::none;
            return (false);
        }
//        this->do_remove_work_dir_on_exit = false; /// NOw saved in directory above working dir.

    }
    return (true);

}

void
ENMultiObjEvaluator::getENInfo()
{

    //GET infromation of network from EN.
    errors(ENgetcount_f(EN_NODECOUNT, &(this->node_count)));
    errors(ENgetcount_f(EN_LINKCOUNT, &(this->link_count)));

    nodes.resize(node_count+1);
    links.resize(link_count+1);

    char id_buffer[16] = "";
    int type_buffer = 0;
    int j, end_j, k, end_k;
    if (this->params->en_lib_version == OWA_EN2)
    {
        j = 1;
        end_j = this->node_count;
        k = 1;
        end_k = this->link_count;
    }
    else if(this->params->en_lib_version == OWA_EN3)
    {
        j = 0;
        end_j = this->node_count - 1;
        k = 0;
        end_k = this->link_count - 1;
    }
    else if(this->params->en_lib_version == Ibanev_EN2)
    {
        j = 1;
        end_j = this->node_count;
        k = 1;
        end_k = this->link_count;
    }
    else
    {
        std::cout << "Error: Epanet Library Version unspecified. Library path also probably not specified\n";
    }

    for (; j <= end_j; ++j)
    {
        NodeInfo& node = nodes[j];
        node.index = j;
        errors(ENgetnodeid_f(j,id_buffer));
        node.id = std::string(id_buffer);
        errors(ENgetnodetype_f(j, &type_buffer));
        switch (type_buffer)
        {
            case 0 :
                node.type = ENLNK_JUNCTION;
                break;
            case 1 :
                node.type = ENLNK_RESERVOIR;
                break;
            case 2:
                node.type = ENLNK_TANK;
                break;
        }

    }

    for (; k <= end_k ; ++k)
    {
        LinkInfo& link = links[k];
        link.index = k;
        errors(ENgetlinkid_f(k,id_buffer));
        link.id = std::string(id_buffer);
        errors(ENgetlinktype_f(k,&type_buffer));
        switch (type_buffer)
        {
            case 0:
                link.type = ENLNK_CVPIPE;
                break;
            case 1:
                link.type = ENLNK_PIPE;
                break;
            case 2:
                link.type = ENLNK_PUMP;
                break;
            case 3:
                link.type = ENLNK_PRV;
                break;
            case 4:
                link.type = ENLNK_PSV;
                break;
            case 5:
                link.type = ENLNK_PBV;
                break;
            case 6:
                link.type = ENLNK_FCV;
                break;
            case 7:
                link.type = ENLNK_TCV;
                break;
            case 8:
                link.type = ENLNK_GPV;
                break;
        }
        errors(ENgetlinknodes_f(k, &(link.fromNode), &(link.toNode)));
        nodes[link.fromNode].connectedLinkIndices.push_back(k);
        nodes[link.toNode].connectedLinkIndices.push_back(k);

    }
}

void
ENMultiObjEvaluator::setLinkIndices()
{

    linkIndices.clear();
    int enIndex;

    //Work out how many different types of pipe can be chosen for each group of epanet links.
    LinkGroupsType::iterator pipeGroup =
            params->link_groups_data.begin();
    LinkGroupsType::iterator end = params->link_groups_data.end();

    for (; pipeGroup != end; ++pipeGroup)
    {
        LinkGroups::iterator enLinkId =
                LinkGroups(pipeGroup).epanetLinkIds.begin();
        LinkGroups::iterator endId =
                LinkGroups(pipeGroup).epanetLinkIds.end();
        //std::cout << "\"" << PipeGroups(pipeGroup).id << "\"" << std::endl;

        //Open the toolkit and the input file in epanet
        //			std::vector<char> writableLinkID;
        for (; enLinkId != endId; ++enLinkId)
        {
            //				writableLinkID.clear();
            //				writableLinkID.resize(enLinkId->size());
            //				std::copy(enLinkId->begin(), enLinkId->end(), writableLinkID.begin());
            //				writableLinkID.push_back('\0');

            //delete[] linkID;
            linkID.reset(new char[enLinkId->size() + 1]);strcpy(linkID.get(),enLinkId->c_str());
            //std::cout << "\"" << linkID << "\"";
            //char * trial = "P_0.0.0_1.0.0";
            //int n;
            //ENgetlinkindex(trial, &n);
            //std::cout << " for " << trial << " index = " << n << std::endl;
            errors(ENgetlinkindex_f(linkID.get(), &enIndex), *enLinkId);
            //debug << *enLinkId << ": ";
            //debug << " for " << linkID.get() << " index = " << enIndex << std::endl;

            //errors(ENgetlinkindex(&writableLinkID[0], &enIndex);
            linkIndices.insert(std::pair< std::string, int >(*enLinkId,
                                                             enIndex));
        }
    }

    //Also check that the pipes in the velocitties constraint list are present in the map
    VelocityConstraintsT::iterator vConstraintPipe =
            params->velocity_constraints.begin();
    VelocityConstraintsT::iterator end_vConst =
            params->velocity_constraints.end();

    for (; vConstraintPipe != end_vConst; ++vConstraintPipe)
    {
        //delete[] linkID;

        linkID.reset(
                new char[Velocities(vConstraintPipe).id.size() + 1]);strcpy(linkID.get(), Velocities(vConstraintPipe).id.c_str());
        errors(ENgetlinkindex_f(linkID.get(), &enIndex), Velocities(
                vConstraintPipe).id);
        linkIndices.insert(std::pair< std::string, int >(Velocities(
                vConstraintPipe).id, enIndex));
    }
}


void
ENMultiObjEvaluator::setNodeIndices()
{

    nodeIndices.clear();

    int enIndex;
    //		std::string nodeId;
    //		std::vector<char> writableNodeID;    //char for en funtion call;

    //Work out how many different types of pipe can be chosen for each group of epanet links.
    NodeConstraintsT::iterator enNode =
            params->pressure_constraints.begin();
    NodeConstraintsT::iterator end =
            params->pressure_constraints.end();

    for (; enNode != end; ++enNode)
    {
        //			nodeId = Pressures(enNode).id;

        //			writableNodeID.clear();
        //			writableNodeID.resize(nodeId.size());
        //			std::copy(nodeId.begin(), nodeId.end(), writableNodeID.begin());
        //			writableNodeID.push_back('\0');

        //delete[] nodeID;

        nodeID.reset(new char[Pressures(enNode).id.size() + 1]);strcpy(nodeID.get(),Pressures(enNode).id.c_str());
        //debug << "\"" << nodeID << "\"";


        errors(ENgetnodeindex_f(nodeID.get(), &enIndex), Pressures(enNode).id);
        nodeIndices.insert(std::pair< std::string, int >(nodeID.get(),
                                                         enIndex));
        //debug << " for " << nodeID.get() << " index = " << enIndex << std::endl;
    }


    float head = 0;

    NodeConstraintsT::iterator hConstraint =
            params->head_constraints.begin();
    NodeConstraintsT::iterator endH =
            params->head_constraints.end();

    for (; hConstraint != endH; ++hConstraint)
    {
        nodeID.reset(new char[Heads(hConstraint).id.size() + 1]);strcpy(nodeID.get(),Heads(hConstraint).id.c_str());
        //debug << "\"" << nodeID << "\"";


        errors(ENgetnodeindex_f(nodeID.get(), &enIndex),
               Heads(hConstraint).id);
        nodeIndices.insert(std::pair< std::string, int >(nodeID.get(),
                                                         enIndex));
    }

}


bool
ENMultiObjEvaluator::errors(int err, std::string what)
{
    if (err > 100)
    {

        std::cerr << "\n EPANET2 ERROR CODE: " << err << std::endl;
        err_out << "\n EPANET2 ERROR CODE: " << err << std::endl;
        int nchar = 160;

        //char errorMsg[nchar + 1];
		char errorMsg[161];
		this->errors(ENgeterror_f(err, errorMsg, nchar));

        std::cerr << "\n\t MEANING: " << errorMsg << std::endl;
        err_out << "\n EPANET2 ERROR CODE: " << err << std::endl;
        std::cerr << "RELATED TO: " << what << std::endl;
        err_out << "RELATED TO: " << what << std::endl;

    }
    bool returnVal = true;
    if (err != 0)
        returnVal = false;
    return (returnVal);
}


/**
	 * @brief Calculates the net present value of a series
	 * @param annualValue
	 * @param numYears
	 * @param rate
	 * @return
	 */
inline double
net_present_value_series(double annualValue, int numYears, double rate)
{
    return ((1.0 - std::pow(1.0 + rate, -1.0 * numYears)) / rate) * annualValue;
};

/**
 * @brief Calculates the present value of an amount paid at some point in the future
 * @param amount The amount paid in year numYears
 * @param numYears The time in the future where the amount is paid
 * @param rate The discount rate
 */
inline double
net_present_value(double amount, int numYears, double rate)
{
    return amount / std::pow(1.0 + rate, numYears);
};


boost::shared_ptr<std::vector<int> >
ENMultiObjEvaluator::getPipeDVBounds()
{
    int numberDVs = params->pipe_availOpt_data.size();
    boost::shared_ptr<std::vector<int> > dv_bounds(new std::vector<int>);
    typedef std::pair<const std::string, std::vector<std::string> > PipeAvailOptionsPair;
    BOOST_FOREACH(PipeAvailOptionsPair & pipe_options, params->pipe_availOpt_data)
                {
                    dv_bounds->push_back(pipe_options.second.size());
                }
    return (dv_bounds);
};

void
ENMultiObjEvaluator::setPipeChoices(const int* dvs)
{


    PipeAvailOptionsType::iterator pipeGroup =
            params->pipe_availOpt_data.begin();
    PipeAvailOptionsType::iterator endGroup =
            params->pipe_availOpt_data.end();

    std::string * pipeChoiceID;
    PipeDataProperties * pipeChoiceData;
    const std::string * pipeGroupID;

    double pipelength = 0;
    double totalLength = 0;
    double lengthAvgedDiameter = 0;

    if (logging)
    {
        **logging << "Pipe option choices:\n";
        **logging << std::setw(20) << std::right << "LinkID" << std::setw(20) << std::right << "dv#"
                  << std::setw(20) << std::right << "group#" << std::setw(20) << std::right << "choice#"
                  << std::setw(20) << std::right << "Status" << std::setw(20) << std::right << "Diameter"
                  << std::setw(20) << std::right << "Length" << std::setw(20) << std::right << "roughnesscost\n";
    }

    int dv_number = 0;
    while (pipeGroup != endGroup)
    {
        int dv_val = dvs[dv_number];
        pipeChoiceID =
                &(PipeAvailOptions(pipeGroup).pipeDataIds[dv_val]);
        pipeGroupID = &(PipeAvailOptions(pipeGroup).groupId);
        pipeChoiceData = &(params->pipe_data_map[*pipeChoiceID]);

        //Get pointers to the links/pipes in the group
        //std::cout << "SIZE: " << params->pipe_groups_data[pipeGroupID].size() << std::endl;
        LinkGroups::iterator enLinkId =
                params->link_groups_data[*pipeGroupID].begin();
        LinkGroups::iterator endId =
                params->link_groups_data[*pipeGroupID].end();

        //Iterate through the epanet links in this group.
        for (; enLinkId != endId; ++enLinkId)
        {
            //set diameter
            //std::cout << " link indice: " << linkIndices[*enPipe] << std::endl;
            if (pipeChoiceData->diameter > 0)
            {
                this->errors(
                        ENsetlinkvalue_f(linkIndices[*enLinkId], EN_INITSTATUS,
                                       1), *enLinkId + ": Setting link to open"); // LINK IS OPEN

                this->errors(
                        ENsetlinkvalue_f(linkIndices[*enLinkId], EN_DIAMETER,
                                       pipeChoiceData->diameter),
                        *enLinkId + ": Setting link diameter to "
                        + boost::lexical_cast< std::string >(
                                pipeChoiceData->diameter));

                this->errors(
                        ENgetlinkvalue_f(linkIndices[*enLinkId], EN_LENGTH,
                                       &pipelength),
                        *enLinkId + ": Getting link length ");

                totalLength += pipelength;
                lengthAvgedDiameter += pipelength
                                       * pipeChoiceData->diameter;

                this->errors(
                        ENsetlinkvalue_f(linkIndices[*enLinkId], EN_ROUGHNESS,
                                       pipeChoiceData->roughnessHeight),
                        *enLinkId + ": Setting link roughness to "
                        + boost::lexical_cast< std::string >(
                                pipeChoiceData->roughnessHeight));
                if(logging)
                {
                    **logging << *enLinkId << std::setw(20) << std::right <<  dv_number
                              << std::setw(20) << std::right <<  pipeGroup->first
                              << std::setw(20) << std::right <<  dv_val
                              << std::setw(20) << std::right <<  "open"
                              << std::setw(20) << std::right <<  pipeChoiceData->diameter
                              << std::setw(20) << std::right <<  pipelength
                              << std::setw(20) << std::right <<  pipeChoiceData->roughnessHeight
                              << std::setw(20) << std::right <<  pipeChoiceData->cost << "\n";
                }
            }
            else
            {
                this->errors(
                        ENsetlinkvalue_f(linkIndices[*enLinkId], EN_INITSTATUS,
                                       0), *enLinkId); // LINK IS CLOSED
                if(logging)
                {
                    **logging << *enLinkId << "\t" << "closed" << "\n";
                }
            }
        }
        ++pipeGroup;
        ++dv_number;
    }

    lengthAvgedDiameter /= totalLength;
    results.lengthAvgDiameter = lengthAvgedDiameter;

    if(logging)
    {
        **logging << "\n\n\n";
    }
}

void
ENMultiObjEvaluator::evalPipes(const int* dvs)
{
    results.pipeCapitalCost = 0;
    results.pipeRepairCost = 0;
    results.pipeEmbodiedEnergy = 0;

    double length;
    std::string * pipeChoiceID;
    const std::string * pipeGroupID;
    PipeDataProperties * pipeChoiceData;

    PipeAvailOptionsType::iterator pipeGroup =
            params->pipe_availOpt_data.begin();
    PipeAvailOptionsType::iterator endGroup =
            params->pipe_availOpt_data.end();

    double t_pipeCapitalCost = 0;
    double t_pipeRepairCost = 0;
    double t_pipeRepairEE = 0;
    double t_pipeEmbodiedEnergy = 0;
    double t_trenchingCost = 0;
    double t_trenchEE = 0;

    int dv_number = 0;

    if (logging)
    {
        **logging << "Pipe costings:\n";
        **logging << "PipeID" << std::setw(20) << std::right << "CapitalCost"
                  << std::setw(20) << std::right << "RepairCost"<< std::setw(20) << std::right << "RepairEE"
                  << std::setw(20) << std::right << "EmbodiedEnergy"<< std::setw(20) << std::right << "trenchCost"
                  << std::setw(20) << std::right << "trenchEE\n";
    }

    while (pipeGroup != endGroup)
    {
        int dv_val = dvs[dv_number];
        pipeChoiceID =
                &(PipeAvailOptions(pipeGroup).pipeDataIds[dv_val]);
        pipeGroupID = &(PipeAvailOptions(pipeGroup).groupId);
        pipeChoiceData = &(params->pipe_data_map[*pipeChoiceID]);

        //Get pointers to the links/pipes in the group
        //std::cout << "SIZE: " << params->pipe_groups_data[pipeGroupID].size() << std::endl;
        LinkGroups::iterator enLinkId =
                params->link_groups_data[*pipeGroupID].begin();
        LinkGroups::iterator endId =
                params->link_groups_data[*pipeGroupID].end();



        //Iterate through the epanet links in this group.
        for (; enLinkId != endId; ++enLinkId)
        {
            //std::cout << " link indice: " << linkIndices[*enPipe] << std::endl;

            //find length
            this->errors(ENgetlinkvalue_f(linkIndices[*enLinkId], EN_LENGTH, &length), *enLinkId);

            //calculate cost
            if (params->isCostObj)
            {
                t_pipeCapitalCost += ((params->FittingsCost > 0) ? (1  + params->FittingsCost) : 1)
                                           * (length * pipeChoiceData->cost); //The pipe cost
                results.pipeCapitalCost += t_pipeCapitalCost;
                //calculate repair cost... for this pipe in one year
                t_pipeRepairCost += (length * pipeChoiceData->repairCost); //The pipe cost
                results.pipeRepairCost += t_pipeRepairCost;
                t_pipeRepairEE += (length * pipeChoiceData->repairEE);
                results.pipeRepairEE += t_pipeRepairEE;

            }
            if (params->isEnergyObj)
            {
                t_pipeEmbodiedEnergy = ((params->FittingsEE > 0) ? (1 + params->FittingsEE) : 1)
                                       * (length * pipeChoiceData->embodiedEnergy); //The pipe cost
                results.pipeEmbodiedEnergy += t_pipeEmbodiedEnergy;
            }

            if (params->TrenchingCost > 0)
            {
                double trenchVolume = ((pipeChoiceData->diameter+ 2 * pipeChoiceData->sideCover) / 1000) //trenching width - divide by 1000 to convert from mm to m
                                      * ((pipeChoiceData->diameter + pipeChoiceData->topCover + pipeChoiceData->bottomCover) / 1000) //trenching depth - divide by 1000 to convert from mm to m
                                      * length;
                if (params->isCostObj)
                {
                    t_trenchingCost = trenchVolume * params->TrenchingCost;
                    results.pipeCapitalCost += t_trenchingCost;
                } // The trenching cost
                if (params->isEnergyObj)
                {
                    t_trenchEE = trenchVolume * params->trenchEE;
                    results.pipeEmbodiedEnergy += t_trenchEE;
                }
            }
            if (logging)
            {
                **logging << *enLinkId << std::setw(20) << std::right <<  t_pipeCapitalCost
                          << std::setw(20) << std::right <<  t_pipeRepairCost
                          << std::setw(20) << std::right <<  t_pipeRepairEE
                          << std::setw(20) << std::right <<  t_pipeEmbodiedEnergy
                          << std::setw(20) << std::right <<  t_trenchingCost
                          << std::setw(20) << std::right <<  t_trenchEE << "\n" ;
            }


        }
        ++pipeGroup;
        ++dv_number;
    }

    // Calculate net present value for this series
    if (params->drateCost > 0 && params->planHorizon > 0)
    {
        if (params->isCostObj && params->isDistSystem) results.pipeRepairCost =
                                                               net_present_value_series(results.pipeRepairCost,
                                                                                        params->planHorizon, params->drateCost);
        if (params->isEnergyObj && params->isDistSystem) results.pipeRepairEE =
                                                                 net_present_value_series(results.pipeRepairEE,
                                                                                          params->planHorizon, params->drateEE);
    }

    if(logging)
    {
        **logging << "\n\n\n";
    }
}

int
ENMultiObjEvaluator::runEN(const int* dvs)
{
    // Set varaibles based on choice vector
    this->setPipeChoices(dvs);

    // Run simulations
        this->evalHydraulicConstraints();
        results.net_costPumpBreakdown.pumpCivilCost = 0;
        results.net_costPumpBreakdown.pumpElectricalCost = 0;
        results.net_costPumpBreakdown.pumpMechanicalCost = 0;
        results.net_costPumpBreakdown.pumpRunningCost = 0;
        results.net_costPumpBreakdown.pumpRunningEnergy = 0;
        results.costPumps = 0;

    if (params->isCostObj || params->isEnergyObj) this->evalPipes(
                dvs);

    return 0;

}



void
ENMultiObjEvaluator::evalPressurePenalty()
{



    NodeConstraintsT::iterator pConstraint =
            params->pressure_constraints.begin();
    NodeConstraintsT::iterator end =
            params->pressure_constraints.end();

    double pressure = 0;
    double deviationLow = 0;
    double deviationHigh = 0;
    double maxDeviationHigh = 0;
    double maxDeviationLow = 0;
    double sumDeviationHigh = 0;
    double sumDeviationLow = 0;
    double constraintLow = 0;
    double constraintHigh = 0;

    std::string nodeID;

    if (logging)
    {
        **logging << "Pressure constraint evaluation\n";
        **logging << "NodeID" << std::setw(20) << std::right << "Pressure" << std::setw(20) << std::right << "constraintLow" << std::setw(20) << std::right << "deviationLow" << std::setw(20) << std::right << "constraintHigh" << std::setw(20) << std::right << "deviationHigh\n";
    }

    for (; pConstraint != end; ++pConstraint)
    {
        //std::cout << "Junction: \"" << Pressures(pConstraint).id << "\"" << std::endl;
        //std::cout << "ID: \"" << nodeIndices[Pressures(pConstraint).id] << "\"" << std::endl;

        deviationLow = 0;
        deviationHigh = 0;
        nodeID = Pressures(pConstraint).id;

        this->errors(
                ENgetnodevalue_f(nodeIndices[nodeID],
                               EN_PRESSURE, &pressure), nodeID);

        constraintLow = Pressures(pConstraint).minPressure;
        if ((pressure) < constraintLow)
        {
            deviationLow = (constraintLow - pressure);
            sumDeviationLow += deviationLow;
            if (deviationLow < maxDeviationLow)
                maxDeviationLow = deviationLow;

//            results.minPressureTooLow +=
//                    (Pressures(pConstraint).minPressure - pressure);

        }

        constraintHigh = Pressures(pConstraint).maxPressure;
        if ((pressure) > constraintHigh)
        {
            deviationHigh = pressure - constraintHigh;
            sumDeviationHigh += deviationHigh;
            if (deviationHigh > maxDeviationHigh)
                maxDeviationHigh = deviationHigh;
//            results.maxPessureTooHigh += (pressure
//                                               - Pressures(pConstraint).maxPressure);

        }

        if (logging)
        {
            **logging << nodeID   << std::setw(20) << std::right <<  pressure  << std::setw(20) << std::right <<  constraintLow  << std::setw(20) << std::right <<  deviationLow  << std::setw(20) << std::right <<  constraintHigh  << std::setw(20) << std::right <<  deviationHigh << "\n";
        }
    }

    results.minPressureTooLow = maxDeviationLow;
    results.maxPressureTooHigh = maxDeviationHigh;
    results.sumPressureTooLow = sumDeviationLow;
    results.sumPressureTooHigh = sumDeviationHigh;

    if(logging)
    {
        **logging << "\n\n\n";
    }
}


void
ENMultiObjEvaluator::evalHeadPenalty()
{

    NodeConstraintsT::iterator hConstraint =
            params->head_constraints.begin();
    NodeConstraintsT::iterator end = params->head_constraints.end();

    double head = 0;
    double deviationLow = 0;
    double deviationHigh = 0;
    double maxDeviationHigh = 0;
    double maxDeviationLow = 0;
    double sumDeviationHigh = 0;
    double sumDeviationLow = 0;
    double constraintLow = 0;
    double constraintHigh = 0;

    std::string nodeID;

    if (logging)
    {
        **logging << "Head constraint evaluation:\n";
        **logging << "NodeID" << std::setw(20) << std::right << "Head" << std::setw(20) << std::right << "constraintLow" << std::setw(20) << std::right << "deviationLow" << std::setw(20) << std::right << "constraintHigh" << std::setw(20) << std::right << "deviationHigh\n";
    }

    for (; hConstraint != end; ++hConstraint)
    {
        //std::cout << "Junction: \"" << Pressures(pConstraint).id << "\"" << std::endl;
        //std::cout << "ID: \"" << nodeIndices[Pressures(pConstraint).id] << "\"" << std::endl;

        deviationLow = 0;
        deviationHigh = 0;
        nodeID = Heads(hConstraint).id;

        this->errors(
                ENgetnodevalue_f(nodeIndices[nodeID], EN_HEAD,
                               &head), nodeID);

//        if (this->writeHeadViolation)
//        {
////                std::cout << "node: " << Heads(hConstraint).id
////                    << "; required head: " << Heads(hConstraint).minHead
////                    << "; actual head: " << head << std::endl;
//        }

        constraintLow = Heads(hConstraint).minHead;
        if (head < constraintLow)
        {
            deviationLow = constraintLow - head;
            sumDeviationLow += deviationLow;
            if (deviationLow > maxDeviationLow)
                maxDeviationLow = deviationLow;

        }

        constraintHigh = Heads(hConstraint).maxHead;
        if (head > constraintHigh)
        {
            deviationHigh = head - constraintHigh;
            sumDeviationHigh += deviationHigh;
            if (deviationHigh > maxDeviationHigh)
                maxDeviationHigh = deviationHigh;
        }

        if (logging)
        {
            **logging << nodeID   << std::setw(20) << std::right <<  head  << std::setw(20) << std::right <<  constraintLow  << std::setw(20) << std::right <<  deviationLow  << std::setw(20) << std::right <<  constraintHigh  << std::setw(20) << std::right <<  deviationHigh << "\n";
        }
    }

    results.minHeadTooLow = maxDeviationLow;
    results.maxHeadTooHigh = maxDeviationHigh;
    results.sumHeadTooLow = sumDeviationLow;
    results.sumHeadTooHigh = sumDeviationHigh;

    if(logging)
    {
        **logging << "\n\n\n";
    }
}


void
ENMultiObjEvaluator::evalVelocityPenalty()
{
    VelocityConstraintsT::iterator vConstraint =
            params->velocity_constraints.begin();
    VelocityConstraintsT::iterator end =
            params->velocity_constraints.end();

    double velocity = 0;
    double maxDeviationHigh = 0;
    double sumDeviationHigh = 0;
    double deviationHigh = 0;
    double constraintHigh = 0;
    std::string linkID;

    if (logging)
    {
        **logging << "Velocity constraint evaluation:\n";
        **logging << "LinkID" << std::setw(20) << std::right << "velocity" << std::setw(20) << std::right << "constraintHigh" << std::setw(20) << std::right << "deviationHigh\n";
    }


    for (; vConstraint != end; ++vConstraint)
    {
        //std::cout << "Link: \"" << Velocities(vConstraint).id << "\"" << std::endl;
        //std::cout << "ID: \"" << linkIndices[Velocities(vConstraint).id] << "\"" << std::endl;

        deviationHigh = 0;
        linkID = Velocities(vConstraint).id;

        errors(
                ENgetlinkvalue_f(linkIndices[linkID],
                               EN_VELOCITY, &velocity), linkID);
        constraintHigh = Velocities(vConstraint).maxVelocity;
        if (velocity > constraintHigh)
        {
            deviationHigh = velocity  - constraintHigh;
            sumDeviationHigh += deviationHigh;
            if (deviationHigh > maxDeviationHigh)
                maxDeviationHigh = deviationHigh;
//            results.maxVelocityTooHigh += (velocity
//                                        - Velocities(vConstraint).maxVelocity);
            //* params->VelocityPenalty;
        }

        if (logging)
        {
            **logging << linkID  << std::setw(20) << std::right <<  velocity  << std::setw(20) << std::right <<  constraintHigh  << std::setw(20) << std::right <<  deviationHigh << "\n";
        }
    }
    results.sumVelocityTooHigh = sumDeviationHigh;
    results.maxVelocityTooHigh = maxDeviationHigh;

    if(logging)
    {
        **logging << "\n\n\n";
    }
}
;

void
ENMultiObjEvaluator::evalHydraulicConstraints()
{
    //std::cout << "evaluating..." << std::endl;

    results.sumPressureTooLow = 0;
    results.sumPressureTooHigh = 0;
    results.maxPressureTooHigh = 0;
    results.minPressureTooLow = 0;
    results.sumHeadTooHigh = 0;
    results.sumHeadTooLow = 0;
    results.maxHeadTooHigh = 0;
    results.minHeadTooLow = 0;
    results.maxVelocityTooHigh = 0;
    results.sumVelocityTooHigh = 0;
    long time;
    long timeStep;

    //std::clog << "Initialising the Hydraulic engine" << std::endl;
    if (!this->errors(ENinitH_f(), "Init hydraulic engine 1"))
        throw std::runtime_error("unable to init H eng.");
    // Initialises the hydraulic engine. 1 indicates that flows are reinitialised abut no results are saved to file
    do
    {
        //std::clog << "Running the Hydraulic engine" << std::endl;
        this->errors(ENrunH_f(&time), "running hydraulic engine");
        //std::clog << "Evaluating penalties" << std::endl;
        if (params->isPressureConstraint)
            this->evalPressurePenalty();
        if (params->isHeadConstraint)
            this->evalHeadPenalty();
        if (params->isVelocityConstraint)
            this->evalVelocityPenalty();
        if (params->isNetworkResiliencyObj)
            this->evalResiliency();
        //std::clog << "Progressing to next time step" << std::endl;
        this->errors(ENnextH_f(&timeStep), "solving time step"); //Runs the hydraulic simulation. Only one period used at the moment
    }
    while (timeStep > 0);

}



void
ENMultiObjEvaluator::evalResiliency()
{

    int nPipe;
    double maxDiameter;
    double sumDiameter;
    double head_actual;
    double head_required;
    double demand;
    double retrievedData;
    double Cj;
    double X = 0;
    double Xmax = 0;


    NodeConstraintsT::iterator hConstraint =
            params->head_constraints.begin();
    NodeConstraintsT::iterator end = params->head_constraints.end();


    if (logging)
    {
        **logging << "Resilience calc: Demand nodes\n";
        **logging << "nodeID" << std::setw(20) << std::right << "Cj" << std::setw(20) << std::right << "X'" << std::setw(20) << std::right << "Xmax'\n";
    }

    std::string nodeID;
    double X_ = 0;
    double Xmax_ = 0;

    for (; hConstraint != end; ++hConstraint)
    {
        //std::cout << "Junction: \"" << Pressures(pConstraint).id << "\"" << std::endl;
        //std::cout << "ID: \"" << nodeIndices[Pressures(pConstraint).id] << "\"" << std::endl;

        nodeID = Heads(hConstraint).id;
        int node_index = nodeIndices[nodeID];
        NodeInfo &node = this->nodes[node_index];
        nPipe = 0;
        maxDiameter = 0;
        sumDiameter = 0;

        if (node.type == ENLNK_JUNCTION)
        {

            BOOST_FOREACH(int link_index, node.connectedLinkIndices)
                        {
                            ENgetlinkvalue_f(link_index, EN_DIAMETER, &retrievedData);
                            if (retrievedData > 0.01)
                            {
                                nPipe += 1;
                                sumDiameter += retrievedData;
                                if (retrievedData > maxDiameter) maxDiameter = retrievedData;
                            }
                        }
            Cj = sumDiameter / (nPipe * maxDiameter);

            errors(ENgetnodevalue_f(node_index, EN_HEAD, &head_actual));
            head_required = Heads(hConstraint).minHead;
            errors(ENgetnodevalue_f(node_index, EN_DEMAND, &demand));
            X_ = Cj * demand * (head_actual - head_required);
            X += X_;
            Xmax_ = demand * head_required;
            Xmax -= Xmax_;

            if (logging)
            {
                **logging <<  nodeID  << std::setw(20) << std::right <<  Cj   << std::setw(20) << std::right <<  X_  << std::setw(20) << std::right <<  Xmax_ << "\n";
            }
        }
    }

    double reservoir_discharge;
    double reservoir_head;
    if (logging)
    {
        **logging << "\nResilience calc: Reservoirs\n";
        **logging << "nodeID" << std::setw(20) << std::right << "Xmax'\n";
    }
    for (int i=1; i<=this->node_count; i++)
    {
        NodeInfo & node = this->nodes[i];
        if (node.type == ENLNK_RESERVOIR)
        {
            errors(ENgetnodevalue_f(node.index, EN_HEAD, &reservoir_head));
            errors(ENgetnodevalue_f(node.index, EN_DEMAND, &reservoir_discharge));
            Xmax_ = reservoir_discharge * reservoir_head;
            Xmax -= Xmax_;

            if(logging)
            {
                **logging << node.id  << std::setw(20) << std::right <<  Xmax_ << "\n";
            }
        }
    }

    this->results.networkResilience = X / Xmax;

    if(logging)
    {
        **logging << "\n\n\n";
    }

}

double ENMultiObjEvaluator::getPipeCapitalCost()
{
    if (logging)
    {
        **logging << "\nPipe capital cost: " << this->results.pipeCapitalCost << "\n";
    }
    return this->results.pipeCapitalCost;
}

double ENMultiObjEvaluator::getNetworkResilience()
{
    if (logging)
    {
        **logging << "\nNetwork Resilience: " << this->results.networkResilience << "\n";
    }
    return this->results.networkResilience;
}

double ENMultiObjEvaluator::getSumPressureTooHigh()
{
    if (logging)
    {
        **logging << "\nSum Pressure too high: " << this->results.sumPressureTooHigh << "\n";
    }
    return this->results.sumPressureTooHigh;
}

double ENMultiObjEvaluator::getMaxPressureTooHigh()
{
    if (logging)
    {
        **logging << "\nMaximum Pressure too high: " << this->results.maxPressureTooHigh << "\n";
    }
    return this->results.maxPressureTooHigh;
}

double ENMultiObjEvaluator::getSumPressureTooLow()
{
    if (logging)
    {
        **logging << "\nSum Pressure too low: " << this->results.sumPressureTooLow << "\n";
    }
    return this->results.sumPressureTooLow;
}

double ENMultiObjEvaluator::getMinPressureTooLow()
{
    if (logging)
    {
        **logging << "\nMinimum pressure too low: " << this->results.minPressureTooLow << "\n";
    }
    return this->results.minPressureTooLow;
}

double ENMultiObjEvaluator::getSumHeadTooHigh()
{
    if (logging)
    {
        **logging << "\nSum head too high: " << this->results.sumHeadTooHigh << "\n";
    }
    return this->results.sumHeadTooHigh;
}

double ENMultiObjEvaluator::getMaxHeadTooHigh()
{
    if (logging)
    {
        **logging << "\nMaximum head too high: " << this->results.maxHeadTooHigh << "\n";
    }
    return this->results.maxHeadTooHigh;
}

double ENMultiObjEvaluator::getSumHeadTooLow()
{
    if (logging)
    {
        **logging << "\nSum head too low: " << this->results.sumHeadTooLow << "\n";
    }
    return this->results.sumHeadTooLow;
}

double ENMultiObjEvaluator::getMinHeadTooLow()
{
    if (logging)
    {
        **logging << "\nMinimum head too low: " << this->results.minHeadTooLow << "\n";
    }
    return this->results.minHeadTooLow;
}

double ENMultiObjEvaluator::getSumVelocityTooHigh()
{
    if (logging)
    {
        **logging << "\nSum velocity too high: " << this->results.sumVelocityTooHigh << "\n";
    }
    return this->results.sumVelocityTooHigh;
}

double ENMultiObjEvaluator::getMaxVelocityTooHigh()
{
    if (logging)
    {
        **logging << "\nMaximum velocity too high: " << this->results.maxVelocityTooHigh << "\n";
    }
    return this->results.maxVelocityTooHigh;
}


bool ENMultiObjEvaluator::isPipeCapitalCostCalculated()
{
    return this->params->isCostObj;
}

bool ENMultiObjEvaluator::isPressureViolationCalculated()
{
    return this->params->isPressureConstraint;
}

bool ENMultiObjEvaluator::isHeadViolationCalculated()
{
    return this->params->isHeadConstraint;
}

bool ENMultiObjEvaluator::isNetworkResilienceCalculated()
{
    return this->params->isNetworkResiliencyObj;
}



int
ENMultiObjEvaluator::saveAnalysis(int analysisID, boost::filesystem::path save_path,
                                  bool doSaveReport, bool doUseInpSpecs)
{
    if (params->en_lib_version == OWA_EN3 or params->en_lib_version == Ibanev_EN2)
    {
        std::cout << "Saving analysis not yet supported for Epanet 3 or Ibanev's Epanet 2\n";
        return -1;
    }
    boost::filesystem::path save_dir;
    boost::filesystem::path save_inp_path;
    boost::filesystem::path save_rpt_path;

    if (save_path == "")
    {
        save_path = this->working_dir.parent_path();
    }

    if(boost::filesystem::is_empty(save_path.parent_path()))
    {
        save_path = this->working_dir / save_path;
    }

    if (save_path.extension() == ".inp" or save_path.extension() == ".rpt")
    {
        save_dir = save_path.parent_path();
        save_inp_path = save_path;
        save_rpt_path = save_path.parent_path() / (save_path.stem().string() + ".rpt");
    }
    else if (boost::filesystem::exists(save_path.parent_path()) && !boost::filesystem::exists(save_path))
    {
        // Then filename is the name of the files minus the extension to save.
        save_dir = save_path.parent_path();
        save_inp_path = save_path.parent_path() / (save_path.filename().string() + ".inp");
        save_rpt_path = save_path.parent_path() / (save_path.filename().string() + ".rpt");
    }
    else // No filename is givem.
    {
        save_dir = save_path;
        save_inp_path = save_path / ("save_analysisID_" + std::to_string(analysisID) + ".inp");
        save_rpt_path = save_path / ("save_analysisID_" + std::to_string(analysisID) + ".rpt");
    }

    if (!boost::filesystem::exists(save_dir))
    {
        if (!boost::filesystem::create_directory(save_dir))
        {
            std::string err =
                    "Unable to create output direction in baseEval::fileSaver: "
                    + save_dir.string();
            throw std::runtime_error(err);
        }
    }


    //Get c-array for file name of inp file to save.
    boost::scoped_array< char > inpFileName(
            new char[save_inp_path.string().size() + 1]); //
    strcpy            (inpFileName.get(), save_inp_path.c_str());

    this->errors(ENsaveinpfile_f(inpFileName.get()), "saving inp file");
    if (doSaveReport)
    {
        this->errors(ENcloseH_f(), "closing hydraulic simulation");
        this->errors(ENsaveH_f(), "Saving hydraulics");

        if (!doUseInpSpecs)
        {
            this->errors(ENresetreport_f(),
                         "resetting report formating options");
            char pagesizep[] = "PAGESIZE 0";
            this->errors(
                    ENsetreport_f(pagesizep),
                    "Setting pagesize to 0; meaning that no line limit per page is in effect");
            if (save_rpt_path != "")
            {
                boost::scoped_array< char > creportname(
                        new char[save_rpt_path.string().size() + 1]); //
                strcpy                     (creportname.get(), save_rpt_path.string().c_str());
                this->errors(ENsetreport_f(creportname.get()),
                             "name of file which will be written");
            }
            char status[] = "STATUS YES";
            this->errors(ENsetreport_f(status),
                         "hydraulic status report should be generated");
            char summary[] = "SUMMARY YES";
            this->errors(ENsetreport_f(summary),
                         "Summary tables will be generated");
            char energy[] = "ENERGY YES";
            this->errors(ENsetreport_f(energy),
                         "Energy usage and cost for pumps generated");
            char nodes[] = "NODES ALL";
            this->errors(ENsetreport_f(nodes),
                         "All nodes will be reported on");
            char links[] = "LINKS ALL";
            this->errors(ENsetreport_f(links),
                         "All links will be reported on");
            char elevation[]= "ELEVATION YES";
            this->errors(ENsetreport_f(elevation),
                         "ELEVATION will be reported on");
            char demand[] = "DEMAND YES";
            this->errors(ENsetreport_f(demand),
                         "DEMAND will be reported on");
            char head[] = "HEAD YES";
            this->errors(ENsetreport_f(head),
                         "HEAD will be reported on");
            char pressure[] = "PRESSURE YES";
            this->errors(ENsetreport_f(pressure),
                         "PRESSURE will be reported on");
            char quality[]= "QUALITY YES";
            this->errors(ENsetreport_f(quality),
                         "QUALITY will be reported on");
            char length[] = "LENGTH YES";
            this->errors(ENsetreport_f(length),
                         "LENGTH will be reported on");
            char diameter[] = "DIAMETER YES";
            this->errors(ENsetreport_f(diameter),
                         "DIAMETER will be reported on");
            char flow[] = "FLOW YES";
            this->errors(ENsetreport_f(flow),
                         "FLOW will be reported on");
            char velocity[] = "VELOCITY YES";
            this->errors(ENsetreport_f(velocity),
                         "VELOCITY will be reported on");
            char headloss[] = "HEADLOSS YES";
            this->errors(ENsetreport_f(headloss),
                         "HEADLOSS will be reported on");
            //            this->errors(ENsetreport("POSITION YES"),
            //                "POSITION will be reported on");
            char setting[] = "SETTING YES";
            this->errors(ENsetreport_f(setting),
                         "SETTING will be reported on");
            char ffactor[] = "F-FACTOR YES";
            this->errors(ENsetreport_f(ffactor),
                         "F-FACTOR will be reported on");
        }
        this->errors(ENreport_f(), "generating report");

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // OPen and close as ENsaveH frees needed memory in the hydraulic solver... (I think...)            //
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////

        if (this->params->en_lib_version == OWA_EN2)
        {
            this->open_OWA_EN2();
        }
        else if(this->params->en_lib_version == OWA_EN3)
        {
            this->open_OWA_EN3();
        }
        else if(this->params->en_lib_version == Ibanev_EN2)
        {
            this->open_Ibanez_EN2();
        }
    }
    return 0;


}
//
//void
//baseEval::fileSaver(std::string filename, std::string _outDirPath,
//                    bool doSaveReport, bool doUseInpSpecs, std::string reportname)
//{
//    fs::path outDirPath(_outDirPath);
//    this->fileSaver(filename, outDirPath, doSaveReport, doUseInpSpecs,
//                    reportname);
//}
//
//void
//baseEval::fileSaver(std::string filename, bool doSaveReport,
//                    bool doUseInpSpecs, std::string reportname)
//{
//    this->fileSaver(filename, outputDir, doSaveReport, doUseInpSpecs,
//                    reportname);
//}




