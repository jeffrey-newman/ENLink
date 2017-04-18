//
// Created by a1091793 on 3/4/17.
//

//
// Created by a1091793 on 29/3/17.
//


#include "ENMultiObjEvaluator.h"
#include <dlfcn.h>
#include <boost/bind.hpp>
#include "OWA_EN3/src/EN3Errors.h"





int EN3_dummyclose()
{
    return 0;
};

class ENfunctInt2Long
{
private:
    boost::function<int (int*, EN_Project) >  en3_int_api_funct;
    EN_Project & en3_simulation;
    int val_i;
    long val_l;
public:

    ENfunctInt2Long(boost::function<int (int*, EN_Project) > & _en3_int_api_funct, EN_Project & _en3_simulation)
            : en3_int_api_funct(_en3_int_api_funct), en3_simulation(_en3_simulation)
    {

    }

    int
    operator()(long* arg1)
    {
        int ret = en3_int_api_funct(&val_i, en3_simulation);
        *arg1 = long(val_i);
        return (ret);
    }
};






void
ENMultiObjEvaluator::initialise_OWA_EN3()
{

    EN_ELEVATION	=	0	;
    EN_BASEDEMAND	=	1	;
    EN_BASEPATTERN	=	2	;
    EN_EMITTERFLOW	=	3	;
    EN_INITQUAL	=	4	;
    EN_SOURCEQUAL	=	5	;
    EN_SOURCEPAT	=	6	;
    EN_SOURCETYPE	=	7	;

    EN_TANKLEVEL	=	8	;
    EN_FULLDEMAND	=	9	;
    EN_HEAD	=	10	;
    EN_PRESSURE	=	11	;
    EN_QUALITY	=	12	;
    EN_SOURCEMASS	=	13	;

    EN_INITVOLUME	=	14	;
    EN_MIXMODEL	=	15	;
    EN_MIXZONEVOL	=	16	;
    EN_TANKDIAM	=	17	;
    EN_MINVOLUME	=	18	;
    EN_VOLCURVE	=	19	;
    EN_MINLEVEL	=	20	;
    EN_MAXLEVEL	=	21	;
    EN_MIXFRACTION	=	22	;
    EN_TANK_KBULK	=	23	;
    EN_TANKVOLUME	=	24	;

    EN_ACTUALDEMAND	=	25	;
    EN_OUTFLOW	=	26	;


    EN_DIAMETER	=	0	;
    EN_LENGTH	=	1	;
    EN_ROUGHNESS	=	2	;
    EN_MINORLOSS	=	3	;
    EN_INITSTATUS	=	4	;
    EN_INITSETTING	=	5	;
    EN_KBULK	=	6	;
    EN_KWALL	=	7	;
    EN_FLOW	=	8	;
    EN_VELOCITY	=	9	;
    EN_HEADLOSS	=	10	;
    EN_STATUS	=	11	;
    EN_SETTING	=	12	;
    EN_ENERGY	=	13	;
    EN_LINKQUAL	=	14	;
    EN_LEAKCOEFF1	=	15	;
    EN_LEAKCOEFF2	=	16	;
    EN_LEAKAGE	=	17	;


    EN_DURATION	=	0	;
    EN_HYDSTEP	=	1	;
    EN_QUALSTEP	=	2	;
    EN_PATTERNSTEP	=	3	;
    EN_PATTERNSTART	=	4	;
    EN_REPORTSTEP	=	5	;
    EN_REPORTSTART	=	6	;
    EN_RULESTEP	=	7	;
    EN_STATISTIC	=	8	;
    EN_PERIODS	=	9	;
    EN_STARTDATE	=	10	;


    EN_NODECOUNT	=	0	;
    EN_TANKCOUNT	=	1	;
    EN_LINKCOUNT	=	2	;
    EN_PATCOUNT	=	3	;
    EN_CURVECOUNT	=	4	;
    EN_CONTROLCOUNT	=	5	;
    EN_RULECOUNT	=	6	;
    EN_RESVCOUNT	=	7	;


    EN_JUNCTION	=	0	;
    EN_RESERVOIR	=	1	;
    EN_TANK	=	2	;


    EN_CVPIPE	=	0	;
    EN_PIPE	=	1	;
    EN_PUMP	=	2	;
    EN_PRV	=	3	;
    EN_PSV	=	4	;
    EN_PBV	=	5	;
    EN_FCV	=	6	;
    EN_TCV	=	7	;
    EN_GPV	=	8	;


    EN_NONE	=	0	;
    EN_CHEM	=	1	;
    EN_AGE	=	2	;
    EN_TRACE	=	3	;


    EN_CONCEN	=	0	;
    EN_MASS	=	1	;
    EN_SETPOINT	=	2	;
    EN_FLOWPACED	=	3	;


    EN_CFS	=	0	;
    EN_GPM	=	1	;
    EN_MGD	=	2	;
    EN_IMGD	=	3	;
    EN_AFD	=	4	;
    EN_LPS	=	5	;
    EN_LPM	=	6	;
    EN_MLD	=	7	;
    EN_CMH	=	8	;
    EN_CMD	=	9	;


    EN_TRIALS	=	0	;
    EN_ACCURACY	=	1	;
    EN_QUALTOL	=	2	;
    EN_EMITEXPON	=	3	;
    EN_DEMANDMULT	=	4	;
    EN_HYDTOL	=	5	;
    EN_MINPRESSURE	=	6	;
    EN_MAXPRESSURE	=	7	;
    EN_PRESSEXPON	=	8	;
    EN_NETLEAKCOEFF1	=	9	;
    EN_NETLEAKCOEFF2	=	10	;


    EN_LOWLEVEL	=	0	;
    EN_HILEVEL	=	1	;
    EN_TIMER	=	2	;
    EN_TIMEOFDAY	=	3	;


    EN_AVERAGE	=	1	;
    EN_MINIMUM	=	2	;
    EN_MAXIMUM	=	3	;
    EN_RANGE	=	4	;


    EN_MIX1	=	0	;
    EN_MIX2	=	1	;
    EN_FIFO	=	2	;
    EN_LIFO	=	3	;


    EN_NOINITFLOW	=	0	;
    EN_INITFLOW	=	1	;

    namespace fs = boost::filesystem;

    err_out.open("en3_err.log",
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


    if (!fs::exists(epanet_dylib_path))
    {
        std::string err = "Unable to locate epanet library at: "
                          + epanet_dylib_path.string();
        throw std::runtime_error(err);
    }

    en_lib_handle = dlopen(epanet_dylib_path.string().c_str(), RTLD_LAZY);
    if (!en_lib_handle)
    {
        std::string err = "Unable to load epanet library."
                                  " Check location: " + epanet_dylib_path.string();
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }

    EN_Project (*EN_createProject)(void);
    EN_createProject = (EN_Project (*)(void)) dlsym(en_lib_handle, "EN_createProject");
    if (!EN_createProject)
    {
        std::string err = "Unable to find EN_createProject function in epanet3 library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    EN_createProject_OWA_EN3_f = EN_createProject;



    int (*EN_loadProject)(const char *, EN_Project);
    EN_loadProject = (int (*)(const char *, EN_Project)) dlsym(en_lib_handle, "EN_loadProject"); //
    if(!EN_loadProject)
    {
        std::string err = "Unable to find EN_loadProject function in epanet3 library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    EN_loadProject_OWA_EN3_f = EN_loadProject;
//    ENopen_f = ENopen;

    int        (*EN_initSolver)(int , EN_Project);
     EN_initSolver = (int (*)(int , EN_Project )) dlsym(en_lib_handle, "EN_initSolver");
    if(!EN_initSolver)
    {
        std::string err = "Unable to find EN_initSolver function in epanet3 library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
//    EN_initSolver_OWA_EN3_f = EN_initSolver;
    ENinitH_f = boost::bind(EN_initSolver, EN_NOINITFLOW, boost::ref(en3_simulation));

    int        (*EN_openOutput)(const char* , EN_Project );
    EN_openOutput = (int (*)(const char* , EN_Project)) dlsym(en_lib_handle, "EN_openOutput");
    if(!EN_openOutput)
    {
        std::string err = "Unable to find EN_openOutput function in epanet3 library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    EN_openOutput_OWA_EN3_f = EN_openOutput;

    int        (*EN_openReport)(const char* , EN_Project );
    EN_openReport = (int (*)(const char* , EN_Project )) dlsym(en_lib_handle, "EN_openReport");
    if(!EN_openReport)
    {
        std::string err = "Unable to find EN_openReport function in epanet3 library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    EN_openReport_OWA_EN3_f = EN_openReport;

    int (*ENgetlinkindex)(char *, int *, EN_Project);
    ENgetlinkindex = (int (*)(char *, int*, EN_Project)) dlsym(en_lib_handle, "EN_getLinkIndex");
    if(!ENgetlinkindex)
    {
        std::string err = "Unable to find EN_getLinkIndex function in epanet3 library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENgetlinkindex_f = boost::bind(ENgetlinkindex, _1, _2, boost::ref(en3_simulation));

    int (*ENgetlinkid)(int, char*, EN_Project);
    ENgetlinkid = (int (*)(int, char*, EN_Project)) dlsym(en_lib_handle, "EN_getLinkId");
    if (!ENgetlinkid)
    {
        std::string err = "Unable to find EN_getLinkId function in epanet3 library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENgetlinkid_f = boost::bind(ENgetlinkid, _1, _2, boost::ref(en3_simulation));

    int (*ENgetlinktype)(int, int*, EN_Project);
    ENgetlinktype = (int (*)(int, int*, EN_Project)) dlsym(en_lib_handle, "EN_getLinkType");if
            (            !ENgetlinktype)
    {
        std::string err = "Unable to find EN_getLinkType function in epanet3 library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENgetlinktype_f = boost::bind(ENgetlinktype, _1, _2, boost::ref(en3_simulation));

    int (*ENgetlinknodes)(int, int*, int*, EN_Project);
    ENgetlinknodes = (int(*)(int, int*, int*, EN_Project)) dlsym(en_lib_handle, "EN_getLinkNodes");
    if (!ENgetlinknodes)
    {
        std::string err = "Unable to find EN_getLinkNodes function in epanet 3 library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENgetlinknodes_f = boost::bind(ENgetlinknodes, _1, _2, _3, boost::ref(en3_simulation));

    int (*ENgetlinkvalue)(int, int, double *, EN_Project);
    ENgetlinkvalue = (int (*)(int, int, double *, EN_Project)) dlsym(en_lib_handle, "EN_getLinkValue");
    if(!ENgetlinkvalue)
    {
        std::string err = "Unable to find EN_getLinkValue function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENgetlinkvalue_f = boost::bind(ENgetlinkvalue, _1, _2, _3, boost::ref(en3_simulation));

    int (*ENsetlinkvalue)(int, int, double, EN_Project);
    ENsetlinkvalue = (int(*)(int, int, double, EN_Project)) dlsym(en_lib_handle, "EN_setLinkValue");
    if (!ENsetlinkvalue)
    {
        std::string err = "Unable to find EN_setLinkValue function in epanet 3 library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENsetlinkvalue_f = boost::bind(ENsetlinkvalue, _1, _2, _3, boost::ref(en3_simulation));

    int (*ENgetnodeindex)(char *, int *, EN_Project);
    ENgetnodeindex = (int (*)(char *, int*, EN_Project)) dlsym(en_lib_handle, "EN_getNodeIndex");
    if (!ENgetnodeindex)
    {
        std::string err ="Unable to find EN_getNodeIndex function in epanet 3 libary";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENgetnodeindex_f = boost::bind(ENgetnodeindex, _1, _2, boost::ref(en3_simulation));

    int (*ENgetnodevalue)(int, int, double *, EN_Project);
    ENgetnodevalue = (int (*)(int, int, double *, EN_Project)) dlsym(en_lib_handle, "EN_getNodeValue");
    if(!ENgetnodevalue)
    {
        std::string err = "Unable to find EN_getNodeValue function in epanet 3 library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENgetnodevalue_f = boost::bind(ENgetnodevalue, _1, _2, _3, boost::ref(en3_simulation)) ;


    int (*ENgetnodeid)(int, char*, EN_Project);
    ENgetnodeid = (int (*) (int, char*, EN_Project)) dlsym(en_lib_handle, "EN_getNodeId");
    if (!ENgetnodeid)
    {
        std::string err = "Unable to find EN_getNodeId function in epanet3 library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENgetnodeid_f = boost::bind(ENgetnodeid, _1, _2, boost::ref(en3_simulation));

    int (*ENgetnodetype)(int, int*, EN_Project);
    ENgetnodetype = (int (*)(int, int *, EN_Project)) dlsym(en_lib_handle, "EN_getNodeType");
    if (!ENgetnodetype)
    {
        std::string err = "Unable to find EN_getNodeType function in epanet3 library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENgetnodetype_f = boost::bind(ENgetnodetype, _1, _2, boost::ref(en3_simulation));

    int (*ENsetnodevalue)(int, int, double, EN_Project);
    ENsetnodevalue = (int (*)(int, int, double, EN_Project)) dlsym(en_lib_handle, "EN_setNodeValue");
    if (!ENsetnodevalue)
    {
        std::string err = "Unable to find EN_setNodeValue function in epanet3 library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENsetnodevalue_f = boost::bind(ENsetnodevalue, _1, _2, _3, boost::ref(en3_simulation));

    int (*ENgetcount)(int, int *, EN_Project);
    ENgetcount = (int (*)(int, int *, EN_Project)) dlsym(en_lib_handle, "EN_getCount");
    if (!ENgetcount)
    {
        std::string err = "Unable to find EN_getCount function in epanet3 library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENgetcount_f = boost::bind(ENgetcount, _1, _2, boost::ref(en3_simulation));

//    int (*ENgetpatternindex)(char *, int *);
//    ENgetpatternindex = (int (*)(char*, int*)) dlsym(en_lib_handle, "ENgetpatternindex");
//    if(!ENgetpatternindex)
//    {
//        std::string err = "Unable to find ENgetpatternindex function in epanet library";
//        std::cerr << err << std::endl;
//        throw std::runtime_error(err);
//    }
//    ENgetpatternindex_f = ENgetpatternindex;
//
//    int (*ENsetpattern)(ENsimulation_t, int, float *, int);
//    ENsetpattern = (int (*)(ENsimulation_t, int, float *, int)) dlsym(en_lib_handle, "ENsetpattern");
//    if (!ENsetpattern)
//    {
//        std::string err = "Unable to find ENsetpattern function in epanet library";
//        std::cerr << err << std::endl;
//        throw std::runtime_error(err);
//    }
//    ENsetpattern_f = boost::bind(ENsetpattern, ibanez_simulation, _1, _2, _3);
//
//    int (*ENsetpatternvalue)(ENsimulation_t, int, int, float);
//    ENsetpatternvalue = (int(*)(ENsimulation_t, int, int, float)) dlsym(en_lib_handle, "ENsetpatternvalue");
//    if(!ENsetpatternvalue)
//    {
//        std::string err = "Unable to find ENsetpatternvalue function in epanet library";
//        std::cerr << err << std::endl;
//        throw std::runtime_error(err);
//    }
//    ENsetpatternvalue_f = boost::bind(ENsetpatternvalue, ibanez_simulation, _1, _2, _3);

//    int (*ENinitH)(ENsimulation_t, int);
//    ENinitH = (int (*)(ENsimulation_t, int)) dlsym(en_lib_handle, "ENinitH");
//    if(!ENinitH)
//    {
//        std::string err = "Unable to find ENinitH function in epanet library";
//        std::cerr << err << std::endl;
//        throw std::runtime_error(err);
//    }
//    ENinitH_f = boost::bind(ENinitH, ibanez_simulation, _1);

    int (*EN_runSolver)(int *, EN_Project);
    EN_runSolver = (int (*)(int *, EN_Project)) dlsym(en_lib_handle, "EN_runSolver");
    if ( !EN_runSolver)
    {
        std::string err = "Unable to find EN_runSolver function in epanet3 library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    boost::function<int (int*, EN_Project) > EN_runSolver_owa3_f(EN_runSolver);
    ENfunctInt2Long EN_runSolver_owa3_fo(EN_runSolver_owa3_f, en3_simulation);
    ENrunH_f = EN_runSolver_owa3_fo;

    int (*EN_advanceSolver)(int *, EN_Project);
    EN_advanceSolver = (int (*)(int *, EN_Project)) dlsym(en_lib_handle, "EN_advanceSolver");
    if (!EN_advanceSolver)
    {
        std::string err = "Unable to find EN_advanceSolver function in epanet3 library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    boost::function<int (int*, EN_Project) > EN_advanceSolver_owa3_f(EN_advanceSolver);
    ENfunctInt2Long EN_advanceSolver_owa3_fo(EN_advanceSolver_owa3_f, en3_simulation);
    ENnextH_f = EN_advanceSolver_owa3_fo;

//    int (*ENsaveH)(void);
//    ENsaveH = (int (*)(void)) dlsym(en_lib_handle, "ENsaveH");
//    if (!ENsaveH)
//    {
//        std::string err = "Unable to find ENsaveH function in epanet library";
//        std::cerr << err << std::endl;
//        throw std::runtime_error(err);
//    }
//    ENsaveH_f = ENsaveH;

//    int (*ENresetreport)(void);
//    ENresetreport = (int (*)(void)) dlsym(en_lib_handle, "ENresetreport");
//    if (!ENresetreport)
//    {
//        std::string err = "Unable to find ENresetreport function in epanet library";
//        std::cerr << err << std::endl;
//        throw std::runtime_error(err);
//    }
//    ENresetreport_f = ENresetreport;
//
//    int (*ENsetreport)(char *);
//    ENsetreport = (int (*)(char *)) dlsym(en_lib_handle, "ENsetreport");
//    if (!ENsetreport)
//    {
//        std::string err = "Unable to find ENsetreport function in epanet library";
//        std::cerr << err << std::endl;
//        throw std::runtime_error(err);
//    }
//    ENsetreport_f = ENsetreport;
//
//    int (*ENreport)(void);
//    ENreport = (int (*)(void)) dlsym(en_lib_handle, "ENreport");
//    if (!ENreport)
//    {
//        std::string err = "Unable to find ENreport function in epanet library";
//        std::cerr << err << std::endl;
//        throw std::runtime_error(err);
//    }
//    ENreport_f = ENreport;
//
//    int (*ENsaveinpfile)(char *);
//    ENsaveinpfile = (int (*)(char *)) dlsym(en_lib_handle, "ENsaveinpfile");
//    if  (!ENsaveinpfile)
//    {
//        std::string err = "Unable to find ENsaveinpfile function in epanet library";
//        std::cerr << err << std::endl;
//        throw std::runtime_error(err);
//    }
//    ENsaveinpfile_f = ENsaveinpfile;

//    int (*ENgeterror)(int, char *, int);
//    ENgeterror = (int (*)(int, char*, int)) dlsym(en_lib_handle, "ENgeterror");
//    if (!ENgeterror)
//    {
//        std::string err = "Unable to find ENgeterror function in epanet library";
//        std::cerr << err << std::endl;
//        throw std::runtime_error(err);
//    }
    ENgeterror_f = EN3geterror;

    int (*EN_deleteProject)(EN_Project);
    EN_deleteProject = (int (*)(EN_Project)) dlsym(en_lib_handle, "EN_deleteProject");
    if(!EN_deleteProject)
    {
        std::string err = "Unable to find ENcloseH function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENcloseH_f = boost::bind(EN_deleteProject, boost::ref(en3_simulation));


//    int (*ENclose)(void);
//    ENclose = (int (*)(void)) dlsym(en_lib_handle, "ENclose");
//    if (!ENclose)
//    {
//        std::string err = "Unable to find ENclose function in epanet library";
//        std::cerr << err << std::endl;
//        throw std::runtime_error(err);
//    }
    ENclose_f = EN3_dummyclose;

}


void
ENMultiObjEvaluator::open_OWA_EN3()
{
    //Open the toolkit and the input file in epanet
    ENFile_cstr.reset(
            new char[en_inp_path.string().size() + 1]);strcpy
            (ENFile_cstr.get(), en_inp_path.string().c_str());

    //std::cout << ENFile_cstr << std::endl;

    en3_simulation = EN_createProject_OWA_EN3_f();
    errors(EN_openReport_OWA_EN3_f(reportFile_cstr.get(), en3_simulation), "Opening report file " + reportFileName );
    errors(EN_loadProject_OWA_EN3_f(ENFile_cstr.get(), en3_simulation), "opening EN inp file " + en_inp_path.string());
    errors(EN_openOutput_OWA_EN3_f(binaryFile_cstr.get(), en3_simulation), "opening EN binary file" + binaryFileName);
//    errors(EN_initSolver_OWA_EN3_f(EN_NOINITFLOW, en3_simulation), "Initialising EN3 solver");

}


void
ENMultiObjEvaluator::close_OWA_EN3()
{
    this->errors(ENcloseH_f(), "closing hydraulic simulation");

}