//
// Created by a1091793 on 2/3/17.
//

#include "ENMultiObjEvaluator.h"
#include <dlfcn.h>

#include <boost/lexical_cast.hpp>

// --- Define the EPANET toolkit constants

#define EN_ELEVATION    0    /* Node parameters */
#define EN_BASEDEMAND   1
#define EN_PATTERN      2
#define EN_EMITTER      3
#define EN_INITQUAL     4
#define EN_SOURCEQUAL   5
#define EN_SOURCEPAT    6
#define EN_SOURCETYPE   7
#define EN_TANKLEVEL    8
#define EN_DEMAND       9
#define EN_HEAD         10
#define EN_PRESSURE     11
#define EN_QUALITY      12
#define EN_SOURCEMASS   13
#define EN_INITVOLUME   14
#define EN_MIXMODEL     15
#define EN_MIXZONEVOL   16

#define EN_TANKDIAM     17
#define EN_MINVOLUME    18
#define EN_VOLCURVE     19
#define EN_MINLEVEL     20
#define EN_MAXLEVEL     21
#define EN_MIXFRACTION  22
#define EN_TANK_KBULK   23

#define EN_DIAMETER     0    /* Link parameters */
#define EN_LENGTH       1
#define EN_ROUGHNESS    2
#define EN_MINORLOSS    3
#define EN_INITSTATUS   4
#define EN_INITSETTING  5
#define EN_KBULK        6
#define EN_KWALL        7
#define EN_FLOW         8
#define EN_VELOCITY     9
#define EN_HEADLOSS     10
#define EN_STATUS       11
#define EN_SETTING      12
#define EN_ENERGY       13

#define EN_DURATION     0    /* Time parameters */
#define EN_HYDSTEP      1
#define EN_QUALSTEP     2
#define EN_PATTERNSTEP  3
#define EN_PATTERNSTART 4
#define EN_REPORTSTEP   5
#define EN_REPORTSTART  6
#define EN_RULESTEP     7
#define EN_STATISTIC    8
#define EN_PERIODS      9

#define EN_NODECOUNT    0    /* Component counts */
#define EN_TANKCOUNT    1
#define EN_LINKCOUNT    2
#define EN_PATCOUNT     3
#define EN_CURVECOUNT   4
#define EN_CONTROLCOUNT 5

#define EN_JUNCTION     0    /* Node types */
#define EN_RESERVOIR    1
#define EN_TANK         2

#define EN_CVPIPE       0    /* Link types */
#define EN_PIPE         1
#define EN_PUMP         2
#define EN_PRV          3
#define EN_PSV          4
#define EN_PBV          5
#define EN_FCV          6
#define EN_TCV          7
#define EN_GPV          8

#define EN_NONE         0    /* Quality analysis types */
#define EN_CHEM         1
#define EN_AGE          2
#define EN_TRACE        3

#define EN_CONCEN       0    /* Source quality types */
#define EN_MASS         1
#define EN_SETPOINT     2
#define EN_FLOWPACED    3

#define EN_CFS          0    /* Flow units types */
#define EN_GPM          1
#define EN_MGD          2
#define EN_IMGD         3
#define EN_AFD          4
#define EN_LPS          5
#define EN_LPM          6
#define EN_MLD          7
#define EN_CMH          8
#define EN_CMD          9

#define EN_TRIALS       0   /* Misc. options */
#define EN_ACCURACY     1
#define EN_TOLERANCE    2
#define EN_EMITEXPON    3
#define EN_DEMANDMULT   4

#define EN_LOWLEVEL     0   /* Control types */
#define EN_HILEVEL      1
#define EN_TIMER        2
#define EN_TIMEOFDAY    3

#define EN_AVERAGE      1   /* Time statistic types.    */
#define EN_MINIMUM      2
#define EN_MAXIMUM      3
#define EN_RANGE        4

#define EN_MIX1         0   /* Tank mixing models */
#define EN_MIX2         1
#define EN_FIFO         2
#define EN_LIFO         3

#define EN_NOSAVE       0   /* Save-results-to-file flag */
#define EN_SAVE         1
#define EN_INITFLOW     10  /* Re-initialize flow flag   */


ENMultiObjEvaluator::ENMultiObjEvaluator()
{

}

ENMultiObjEvaluator::~ENMultiObjEvaluator()
{
    // Close Ratings library
    if (dlclose(en_lib_handle) != 0) {
        printf("[%s] Problem closing library: %s", __FILE__, dlerror());
    }
}

void
ENMultiObjEvaluator::initialise(boost::filesystem::path opt_cfg_path)
{
    namespace fs = boost::filesystem;

//    if (initParams)
//    {
        //copy the params to the working directory.
        // Transfer a copy of the opt cfg file to the working directory...
//        fs::path working_opt_path = this->workingDir
//                                    / orig_opt_file_path.filename();
//        if (!fs::exists(working_opt_path))
//        {
//            fs::copy_file(orig_opt_file_path, working_opt_path);
//        }
        //open the params
        params = getObjectiveInput(opt_cfg_path);
        ENFile = params->epanetFile;
        epanet_dylib_loc = params->epanet_dylib_loc;

//    }



//    if (!fs::exists(this->outputDir))
//    {
//        if (!fs::create_directories(this->outputDir))
//        {
//            std::string err = "Unable to create EN working directory: "
//                              + this->outputDir.string();
//            throw std::runtime_error(err);
//        }
//    }
//
//    if (!fs::exists(this->workingDir))
//    {
//        if (!fs::create_directories(this->workingDir))
//        {
//            std::string err = "Unable to create EN working directory: "
//                              + this->workingDir.string();
//            throw std::runtime_error(err);
//        }
//    }



    //Work out the location for the error log.
//    fs::path en_err_logLoc = this->workingDir / epanet_error_log_name;

    err_out.open("en2_err.log",
                 std::ios_base::out | std::ios_base::app);

    if (!err_out.is_open())
    {
        std::string err = "Unable to open epanet error log file."
                                  " Check location";
        std::cerr << err << std::endl;
        std::cout << dlerror() << std::endl;
        throw std::runtime_error(err);

    }

    //Work out the location of the epanet library.

    fs::path en_lib_path(epanet_dylib_loc);

    if (!fs::exists(en_lib_path))
    {
        std::string err = "Unable to locate epanet library at: "
                          + en_lib_path.string();
        throw std::runtime_error(err);
    }

    en_lib_handle = dlopen(en_lib_path.c_str(), RTLD_LAZY);
    if (!en_lib_handle)
    {
        std::string err = "Unable to load epanet library."
                                  " Check location: " + en_lib_path.string();
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }

    ENopen = (int
    (*)(char *, char *, char *)) dlsym(en_lib_handle, "ENopen"); //
    if(            !ENopen)
    {
        std::string err = "Unable to find ENopen function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }

    ENopenH = (int
    (*)(void)) dlsym(en_lib_handle, "ENopenH"); //
    if(            !ENopenH)
    {

        std::string err = "Unable to find ENOpenH function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }

    ENgetlinkindex = (int
    (*)(char *, int*)) dlsym(en_lib_handle, "ENgetlinkindex");if
            (            !ENgetlinkindex)
    {
        std::string err = "Unable to find ENgetlinkindex function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }

    ENgetnodeindex = (int
    (*)(char *, int*)) dlsym(en_lib_handle, "ENgetnodeindex");if
            (            !ENgetnodeindex)
    {
        std::string err ="Unable to find ENgetnodeindex function in epanet libary";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }

    ENsetlinkvalue = (int
    (*)(int, int, float)) dlsym(en_lib_handle, "ENsetlinkvalue");if
            (            !ENsetlinkvalue)
    {
        std::string err = "Unable to find ENsetlinkvalue function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }

    ENgetlinkvalue = (int
    (*)(int, int, float *)) dlsym(en_lib_handle, "ENgetlinkvalue");if
            (            !ENgetlinkvalue)
    {
        std::string err = "Unable to find ENgetlinkvalue function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }

    ENgetnodevalue = (int
    (*)(int, int, float *)) dlsym(en_lib_handle, "ENgetnodevalue");if
            (            !ENgetnodevalue)
    {
        std::string err = "Unable to find ENgetnodevalue function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }

    ENgetlinknodes = (int
    (*)(int, int*, int*)) dlsym(en_lib_handle, "ENgetlinknodes");if
            (            !ENgetlinknodes)
    {
        std::string err = "Unable to find ENgetlinknodes function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }

    ENinitH = (int
    (*)(int)) dlsym(en_lib_handle, "ENinitH");if
            (            !ENinitH)
    {
        std::string err = "Unable to find ENinitH function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }

    ENrunH = (int
    (*)(long *)) dlsym(en_lib_handle, "ENrunH");if
            (            !ENrunH)
    {
        std::string err = "Unable to find ENrunH function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }

    ENnextH = (int
    (*)(long *)) dlsym(en_lib_handle, "ENnextH");if
            (            !ENnextH)
    {
        std::string err = "Unable to find ENnextH function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }

    ENclose = (int
    (*)(void)) dlsym(en_lib_handle, "ENclose");if
            (            !ENclose)
    {
        std::string err = "Unable to find ENclose function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }

    ENsaveinpfile = (int
    (*)(char *)) dlsym(en_lib_handle, "ENsaveinpfile");if
            (            !ENsaveinpfile)
    {
        std::string err = "Unable to find ENsaveinpfile function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }

    ENcloseH = (int
    (*)(void)) dlsym(en_lib_handle, "ENcloseH");if
            (            !ENcloseH)
    {
        std::string err = "Unable to find ENcloseH function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }

    ENsaveH = (int
    (*)(void)) dlsym(en_lib_handle, "ENsaveH");if
            (            !ENsaveH)
    {
        std::string err = "Unable to find ENsaveH function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }

    ENresetreport = (int
    (*)(void)) dlsym(en_lib_handle, "ENresetreport");if
            (            !ENresetreport)
    {
        std::string err = "Unable to find ENresetreport function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }

    ENsetreport = (int
    (*)(char *)) dlsym(en_lib_handle, "ENsetreport");if
            (            !ENsetreport)
    {
        std::string err = "Unable to find ENsetreport function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }

    ENreport = (int
    (*)(void)) dlsym(en_lib_handle, "ENreport");if
            (            !ENreport)
    {
        std::string err = "Unable to find ENreport function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }

    ENgetnodetype = (int
    (*)(int, int *)) dlsym(en_lib_handle, "ENgetnodetype");if
            (            !ENgetnodetype)
    {
        std::string err = "Unable to find ENgetnodetype function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }

    ENsetnodevalue = (int
    (*)(int, int, float)) dlsym(en_lib_handle, "ENsetnodevalue");if
            (            !ENsetnodevalue)
    {
        std::string err = "Unable to find ENsetnodevalue function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }

    ENgetcount = (int
    (*)(int, int *)) dlsym(en_lib_handle, "ENgetcount");if
            (            !ENgetcount)
    {
        std::string err = "Unable to find ENgetcount function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }

    ENgeterror = (int
    (*)(int, char*, int)) dlsym(en_lib_handle, "ENgeterror");if
            (            !ENgeterror)
    {
        std::string err = "Unable to find ENgeterror function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }

    ENgetpatternindex = (int
    (*)(char*, int*)) dlsym(en_lib_handle, "ENgetpatternindex");if
            (            !ENgetpatternindex)
    {
        std::string err = "Unable to find ENgetpatternindex function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }

    ENsetpattern = (int
    (*)(int, float*, int)) dlsym(en_lib_handle, "ENsetpattern");if
            (            !ENsetpattern)
    {
        std::string err = "Unable to find ENsetpattern function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }

    ENsetpatternvalue = (int
    (*)(int, int, float)) dlsym(en_lib_handle, "ENsetpatternvalue");if
            (            !ENsetpatternvalue)
    {
        std::string err = "Unable to find ENsetpatternvalue function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }

    //Check whether the input file exists...
    fs::path en_inp_path(this->ENFile);
    if (!fs::exists(en_inp_path))
    {
        std::string err = "Epanet input file does not exist: "
                          + en_inp_path.string();
        throw std::runtime_error(err);
    }

    ++dlOpenCount;

    // Transfer a copy of the epanet input file to the working directory...
    fs::path working_en_inp_path = this->workingDir
                                   / en_inp_path.filename();
    if (!fs::exists(working_en_inp_path))
    {
        fs::copy_file(en_inp_path, working_en_inp_path);
    }

    //Open the toolkit and the input file in epanet
    ENFile_cstr.reset(
            new char[working_en_inp_path.string().size() + 1]);strcpy
            (ENFile_cstr.get(), working_en_inp_path.c_str());

    //Work out the location for the report file....
    std::string tmpReportName = reportFileName
                                + boost::lexical_cast< std::string >(dlOpenCount) + ".rpt";
    fs::path en_report_path = this->workingDir / tmpReportName;

    //create the report and binary files...
    reportFile_cstr.reset(new char[en_report_path.string().size() + 1]);strcpy
            (reportFile_cstr.get(), en_report_path.c_str());

    //Woprk out the location for the binary file...
    std::string tmpBinName = binaryFileName
                             + boost::lexical_cast< std::string >(dlOpenCount) + ".bin";
    fs::path en_bin_path = this->workingDir / tmpBinName;

    binaryFile_cstr.reset(new char[en_bin_path.string().size() + 1]);strcpy
            (binaryFile_cstr.get(), en_bin_path.c_str());

    //std::cout << ENFile_cstr << std::endl;
    errors(
            ENopen(ENFile_cstr.get(), reportFile_cstr.get(),
                   binaryFile_cstr.get()),
            "opening EN inp file " + ENFile + " and report file "
            + binaryFileName);
    // OPen the hydraulic solution
    errors(ENopenH(), "opening hydraulic solution");
    // Determine the pipe indices for the links we are optimising choice of pipe for.
    this->setLinkIndices();
    this->setNodeIndices();

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
            errors(ENgetlinkindex(linkID.get(), &enIndex), *enLinkId);
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
        errors(ENgetlinkindex(linkID.get(), &enIndex), Velocities(
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
    PressureConstraintsT::iterator enNode =
            params->pressure_constraints.begin();
    PressureConstraintsT::iterator end =
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


        errors(ENgetnodeindex(nodeID.get(), &enIndex), Pressures(enNode).id);
        nodeIndices.insert(std::pair< std::string, int >(nodeID.get(),
                                                         enIndex));
        //debug << " for " << nodeID.get() << " index = " << enIndex << std::endl;
    }


    float head = 0;

    PressureConstraintsT::iterator hConstraint =
            params->head_constraints.begin();
    PressureConstraintsT::iterator endH =
            params->head_constraints.end();

    for (; hConstraint != endH; ++hConstraint)
    {
        nodeID.reset(new char[Heads(hConstraint).id.size() + 1]);strcpy(nodeID.get(),Heads(hConstraint).id.c_str());
        //debug << "\"" << nodeID << "\"";


        errors(ENgetnodeindex(nodeID.get(), &enIndex),
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
        char errorMsg[nchar + 1];this
                ->errors(ENgeterror(err, errorMsg, nchar));
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

    float pipelength = 0;
    double totalLength = 0;
    double lengthAvgedDiameter = 0;

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
                        ENsetlinkvalue(linkIndices[*enLinkId], EN_INITSTATUS,
                                       1), *enLinkId + ": Setting link to open"); // LINK IS OPEN

                this->errors(
                        ENsetlinkvalue(linkIndices[*enLinkId], EN_DIAMETER,
                                       pipeChoiceData->diameter),
                        *enLinkId + ": Setting link diameter to "
                        + boost::lexical_cast< std::string >(
                                pipeChoiceData->diameter));

                this->errors(
                        ENgetlinkvalue(linkIndices[*enLinkId], EN_LENGTH,
                                       &pipelength),
                        *enLinkId + ": Getting link length ");

                totalLength += pipelength;
                lengthAvgedDiameter += pipelength
                                       * pipeChoiceData->diameter;

                this->errors(
                        ENsetlinkvalue(linkIndices[*enLinkId], EN_ROUGHNESS,
                                       pipeChoiceData->roughnessHeight),
                        *enLinkId + ": Setting link roughness to "
                        + boost::lexical_cast< std::string >(
                                pipeChoiceData->roughnessHeight));
            }
            else
            {
                this->errors(
                        ENsetlinkvalue(linkIndices[*enLinkId], EN_INITSTATUS,
                                       0), *enLinkId); // LINK IS CLOSED
            }
        }
        ++pipeGroup;
        ++dv_number;
    }

    lengthAvgedDiameter /= totalLength;
    results.lengthAvgDiameter = lengthAvgedDiameter;

}

void
ENMultiObjEvaluator::evalPipes(const int* dvs)
{
    results.pipeCapitalCost = 0;
    results.pipeRepairCost = 0;
    results.pipeEmbodiedEnergy = 0;

    float length;
    std::string * pipeChoiceID;
    const std::string * pipeGroupID;
    PipeDataProperties * pipeChoiceData;

    PipeAvailOptionsType::iterator pipeGroup =
            params->pipe_availOpt_data.begin();
    PipeAvailOptionsType::iterator endGroup =
            params->pipe_availOpt_data.end();

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
            //std::cout << " link indice: " << linkIndices[*enPipe] << std::endl;

            //find length
            this->errors(
                    ENgetlinkvalue(linkIndices[*enLinkId], EN_LENGTH,
                                   &length), *enLinkId);

            //calculate cost
            if (params->isCostObj)
            {
                results.pipeCapitalCost += (
                                                   (params->FittingsCost > 0) ? (1
                                                                                 + params->FittingsCost) :
                                                   1) * (length * pipeChoiceData->cost); //The pipe cost
                //calculate repair cost... for this pipe in one year
                results.pipeRepairCost += (length
                                           * pipeChoiceData->repairCost); //The pipe cost
                results.pipeRepairEE +=
                        (length * pipeChoiceData->repairEE);
            }
            if (params->isEnergyObj)
            {
                results.pipeEmbodiedEnergy += (
                                                      (params->FittingsEE > 0) ? (1 + params->FittingsEE) :
                                                      1) * (length * pipeChoiceData->embodiedEnergy); //The pipe cost
            }

            if (params->TrenchingCost > 0)
            {
                double trenchVolume = ((pipeChoiceData->diameter
                                        + 2 * pipeChoiceData->sideCover) / 1000) //trenching width - divide by 1000 to convert from mm to m
                                      * ((pipeChoiceData->diameter
                                          + pipeChoiceData->topCover
                                          + pipeChoiceData->bottomCover) / 1000) //trenching depth - divide by 1000 to convert from mm to m
                                      * length;
                if (params->isCostObj) results.pipeCapitalCost +=
                                               trenchVolume * params->TrenchingCost; // The trenching cost
                if (params->isEnergyObj) results.pipeEmbodiedEnergy +=
                                                 trenchVolume * params->trenchEE;
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

    float pressure = 0;

    PressureConstraintsT::iterator pConstraint =
            params->pressure_constraints.begin();
    PressureConstraintsT::iterator end =
            params->pressure_constraints.end();

    double maxDeviationHigh = 0;
    double maxDeviationLow = 0;
    double sumDeviationHigh = 0;
    double sumDeviationLow = 0;

    for (; pConstraint != end; ++pConstraint)
    {
        //std::cout << "Junction: \"" << Pressures(pConstraint).id << "\"" << std::endl;
        //std::cout << "ID: \"" << nodeIndices[Pressures(pConstraint).id] << "\"" << std::endl;

        this->errors(
                ENgetnodevalue(nodeIndices[Pressures(pConstraint).id],
                               EN_PRESSURE, &pressure), Pressures(pConstraint).id);
        if ((pressure) < Pressures(pConstraint).minPressure)
        {
            double deviationLow = (Pressures(pConstraint).minPressure - pressure);
            sumDeviationLow += deviationLow;
            if (deviationLow < maxDeviationLow)
                maxDeviationLow = deviationLow;

//            results.minPressureTooLow +=
//                    (Pressures(pConstraint).minPressure - pressure);

        }
        if ((pressure) > Pressures(pConstraint).maxPressure)
        {
            double deviationHigh = pressure - Pressures(pConstraint).maxPressure;
            sumDeviationHigh += deviationHigh;
            if (deviationHigh > maxDeviationHigh)
                maxDeviationHigh = deviationHigh;
//            results.maxPessureTooHigh += (pressure
//                                               - Pressures(pConstraint).maxPressure);

        }
    }

    results.minPressureTooLow = maxDeviationLow;
    results.maxPressureTooHigh = maxDeviationHigh;
    results.sumPressureTooLow = sumDeviationLow;
    results.sumPressureTooHigh = sumDeviationHigh;
}


void
ENMultiObjEvaluator::evalHeadPenalty()
{

    float head = 0;

    PressureConstraintsT::iterator hConstraint =
            params->head_constraints.begin();
    PressureConstraintsT::iterator end = params->head_constraints.end();

    double maxDeviationHigh = 0;
    double maxDeviationLow = 0;
    double sumDeviationHigh = 0;
    double sumDeviationLow = 0;

    for (; hConstraint != end; ++hConstraint)
    {
        //std::cout << "Junction: \"" << Pressures(pConstraint).id << "\"" << std::endl;
        //std::cout << "ID: \"" << nodeIndices[Pressures(pConstraint).id] << "\"" << std::endl;

        this->errors(
                ENgetnodevalue(nodeIndices[Heads(hConstraint).id], EN_HEAD,
                               &head), Heads(hConstraint).id);

//        if (this->writeHeadViolation)
//        {
////                std::cout << "node: " << Heads(hConstraint).id
////                    << "; required head: " << Heads(hConstraint).minHead
////                    << "; actual head: " << head << std::endl;
//        }

        if (head < Heads(hConstraint).minHead)
        {
            double deviationLow = Heads(hConstraint).minHead - head;
            sumDeviationLow += deviationLow;
            if (deviationLow > maxDeviationLow)
                maxDeviationLow = deviationLow;

        }
        if (head > Heads(hConstraint).maxHead)
        {
            double deviationHigh = head - Heads(hConstraint).maxHead;
            sumDeviationHigh += deviationHigh;
            if (deviationHigh > maxDeviationHigh)
                maxDeviationHigh = deviationHigh;
        }
    }

    results.minHeadTooLow = maxDeviationLow;
    results.maxHeadTooHigh = maxDeviationHigh;
    results.sumHeadTooLow = sumDeviationLow;
    results.sumHeadTooHigh = sumDeviationHigh;
}


void
ENMultiObjEvaluator::evalVelocityPenalty()
{


    float velocity = 0;

    VelocityConstraintsT::iterator vConstraint =
            params->velocity_constraints.begin();
    VelocityConstraintsT::iterator end =
            params->velocity_constraints.end();

    double maxDeviationHigh = 0;
    double sumDeviationHigh = 0;

    for (; vConstraint != end; ++vConstraint)
    {
        //std::cout << "Link: \"" << Velocities(vConstraint).id << "\"" << std::endl;
        //std::cout << "ID: \"" << linkIndices[Velocities(vConstraint).id] << "\"" << std::endl;

        errors(
                ENgetlinkvalue(linkIndices[Velocities(vConstraint).id],
                               EN_VELOCITY, &velocity), Velocities(vConstraint).id);
        if (velocity > Velocities(vConstraint).maxVelocity)
        {
            double deviationHigh = velocity
                                   - Velocities(vConstraint).maxVelocity;
            sumDeviationHigh += deviationHigh;
            if (deviationHigh > maxDeviationHigh)
                maxDeviationHigh = deviationHigh;
//            results.maxVelocityTooHigh += (velocity
//                                        - Velocities(vConstraint).maxVelocity);
            //* params->VelocityPenalty;
        }
    }
    results.sumVelocityTooHigh = sumDeviationHigh;
    results.maxVelocityTooHigh = maxDeviationHigh;
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
    if (!this->errors(ENinitH(01), "Init hydraulic engine 1"))
        throw std::runtime_error("unable to init H eng.");
    // Initialises the hydraulic engine. 1 indicates that flows are reinitialised abut no results are saved to file
    do
    {
        //std::clog << "Running the Hydraulic engine" << std::endl;
        this->errors(ENrunH(&time), "running hydraulic engine");
        //std::clog << "Evaluating penalties" << std::endl;
        if (params->isPressureConstraint)
            this->evalPressurePenalty();
        if (params->isHeadConstraint)
            this->evalHeadPenalty();
        if (params->isVelocityConstraint)
            this->evalVelocityPenalty();
        //std::clog << "Progressing to next time step" << std::endl;
        this->errors(ENnextH(&timeStep), "solving time step"); //Runs the hydraulic simulation. Only one period used at the moment
    }
    while (timeStep > 0);

}

double ENMultiObjEvaluator::getPipeCapitalCost()
{
    return this->results.pipeCapitalCost;
}

double ENMultiObjEvaluator::getSumPressureTooHigh()
{
    return this->results.sumPressureTooHigh;
}

double ENMultiObjEvaluator::getMaxPressureTooHigh()
{
    return this->results.maxPressureTooHigh;
}

double ENMultiObjEvaluator::getSumPressureTooLow()
{
    return this->results.sumPressureTooLow;
}

double ENMultiObjEvaluator::getMinPressureTooLow()
{
    return this->results.minPressureTooLow;
}

double ENMultiObjEvaluator::getSumHeadTooHigh()
{
    return this->results.sumHeadTooHigh;
}

double ENMultiObjEvaluator::getMaxHeadTooHigh()
{
    return this->results.maxHeadTooHigh;
}

double ENMultiObjEvaluator::getSumHeadTooLow()
{
    return this->results.sumHeadTooLow;
}

double ENMultiObjEvaluator::getMinHeadTooLow()
{
    return this->results.minHeadTooLow;
}

double ENMultiObjEvaluator::getSumVelocityTooHigh()
{
    return this->results.sumVelocityTooHigh;
}

double ENMultiObjEvaluator::getMaxVelocityTooHigh()
{
    return this->results.maxVelocityTooHigh;
}



///**
// * hydraulic solver must be run before doing this....
// * @param filename
// * @param _outDirPath
// * @param doSaveReport
// * @param doUseInpSpecs
// * @param reportname
// */
//void
//ENMultiObjEvaluator::fileSaver(std::string filename, fs::path & _outDirPath,
//                    bool doSaveReport, bool doUseInpSpecs, std::string reportname)
//{
//    if (!fs::exists(_outDirPath))
//    {
//        if (!fs::create_directory(_outDirPath))
//        {
//            std::string err =
//                    "Unable to create output direction in baseEval::fileSaver: "
//                    + _outDirPath.string();
//            throw std::runtime_error(err);
//        }
//    }
//
//    filename = filename + ".inp";
//    fs::path outEN_inpDir = _outDirPath / filename;
//
//    boost::scoped_array< char > inpFileName(
//            new char[outEN_inpDir.string().size() + 1]); //
//    strcpy            (inpFileName.get(), outEN_inpDir.c_str());
//
//    this->errors(ENsaveinpfile(inpFileName.get()), "saving inp file");
//    if (doSaveReport)
//    {
//        this->errors(ENcloseH(), "closing hydraulic simulation");
//        this->errors(ENsaveH(), "Saving hydraulics");
//
//        if (!doUseInpSpecs)
//        {
//            this->errors(ENresetreport(),
//                         "resetting report formating options");
//            this->errors(
//                    ENsetreport("PAGESIZE 0"),
//                    "Setting pagesize to 0; meaning that no line limit per page is in effect");
//            if (reportname != "")
//            {
//                reportname = reportname + ".rpt";
//                fs::path outEN_rptDir = _outDirPath / reportname;
//                std::string outEN_rptStr = "FILE " + outEN_rptDir.string();
//                boost::scoped_array< char > creportname(
//                        new char[outEN_rptStr.size() + 1]); //
//                strcpy                     (creportname.get(), outEN_rptStr.c_str());
//                this->errors(ENsetreport(creportname.get()),
//                             "name of file which will be written");
//            }
//            this->errors(ENsetreport("STATUS YES"),
//                         "hydraulic status report should be generated");
//            this->errors(ENsetreport("SUMMARY YES"),
//                         "Summary tables will be generated");
//            this->errors(ENsetreport("ENERGY YES"),
//                         "Energy usage and cost for pumps generated");
//            this->errors(ENsetreport("NODES ALL"),
//                         "All nodes will be reported on");
//            this->errors(ENsetreport("LINKS ALL"),
//                         "All links will be reported on");
//            this->errors(ENsetreport("ELEVATION YES"),
//                         "ELEVATION will be reported on");
//            this->errors(ENsetreport("DEMAND YES"),
//                         "DEMAND will be reported on");
//            this->errors(ENsetreport("HEAD YES"),
//                         "HEAD will be reported on");
//            this->errors(ENsetreport("PRESSURE YES"),
//                         "PRESSURE will be reported on");
//            this->errors(ENsetreport("QUALITY YES"),
//                         "QUALITY will be reported on");
//            this->errors(ENsetreport("LENGTH YES"),
//                         "LENGTH will be reported on");
//            this->errors(ENsetreport("DIAMETER YES"),
//                         "DIAMETER will be reported on");
//            this->errors(ENsetreport("FLOW YES"),
//                         "FLOW will be reported on");
//            this->errors(ENsetreport("VELOCITY YES"),
//                         "VELOCITY will be reported on");
//            this->errors(ENsetreport("HEADLOSS YES"),
//                         "HEADLOSS will be reported on");
//            //            this->errors(ENsetreport("POSITION YES"),
//            //                "POSITION will be reported on");
//            this->errors(ENsetreport("SETTING YES"),
//                         "SETTING will be reported on");
//            this->errors(ENsetreport("F-FACTOR YES"),
//                         "F-FACTOR will be reported on");
//        }
//        this->errors(ENreport(), "generating report");
//
//        this->close();
//        this->initialiseEPANET();
////                     this->errors(ENclose(), "closing epanet down");
////                     errors(
////                           ENopen(ENFile_cstr.get(), reportFile_cstr.get(), binaryFile_cstr.get()),
////                           "opening EN inp file " + ENFile + " and report file "
////                           + binaryFile);
////                     // OPen the hydraulic solution
////                     this->errors(ENopenH(), "opening hydraulic solution");
//        //        this->evalHydraulicConstraints(results);
//    }
//    // OPen and close as ENsaveH frees needed memory in the hydraulic solver... (I think...)
//
//}
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




