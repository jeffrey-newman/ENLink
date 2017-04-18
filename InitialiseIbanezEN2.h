//
// Created by a1091793 on 29/3/17.
//


#include "ENMultiObjEvaluator.h"
#include <dlfcn.h>
#include <boost/bind.hpp>
#include <boost/ref.hpp>




class ENfunctFlt2DblIbanez
{
private:
    boost::function<int (ENsimulation_t, int, int, float*) >  en_flt_api_funct;
    float val_f;
    double val_d;
public:

    ENfunctFlt2DblIbanez(boost::function<int (ENsimulation_t, int, int, float*) > & _en_flt_api_funct)
    : en_flt_api_funct(_en_flt_api_funct)
            {

            }

    int
    operator()(ENsimulation_t ensim, int arg1, int arg2, double * arg3)
    {
        int ret = en_flt_api_funct(ensim, arg1, arg2, &val_f);
        *arg3 = double(val_f);
        return (ret);
    }
};



void
ENMultiObjEvaluator::initialise_Ibanez_EN2()
{



    EN_ELEVATION    = 0;
    EN_BASEDEMAND   = 1;
    EN_PATTERN      = 2;
    EN_EMITTER      = 3;
    EN_INITQUAL     = 4;
    EN_SOURCEQUAL   = 5;
    EN_SOURCEPAT    = 6;
    EN_SOURCETYPE   = 7;
    EN_TANKLEVEL    = 8;
    EN_DEMAND       = 9;
    EN_HEAD         = 10;
    EN_PRESSURE     = 11;
    EN_QUALITY      = 12;
    EN_SOURCEMASS   = 13;
    EN_VOLUME       = 20;
    EN_INITVOL      = 21;
    EN_MAXLEVEL     = 30;
    EN_MINLEVEL     = 31;
    EN_DIAMETER     = 0;
    EN_LENGTH       = 1;
    EN_ROUGHNESS    = 2;
    EN_MINORLOSS    = 3;
    EN_INITSTATUS   = 4;
    EN_INITSETTING  = 5;
    EN_KBULK        = 6;
    EN_KWALL        = 7;
    EN_FLOW         = 8;
    EN_VELOCITY     = 9;
    EN_HEADLOSS     = 10;
    EN_STATUS       = 11;
    EN_SETTING      = 12;
    EN_ENERGY       = 13;
    EN_UPATTERN     = 20;
    EN_SCHEDULE     = 21;
    EN_DURATION     = 0;
    EN_HYDSTEP      = 1;
    EN_QUALSTEP     = 2;
    EN_PATTERNSTEP  = 3;
    EN_PATTERNSTART = 4;
    EN_REPORTSTEP   = 5;
    EN_REPORTSTART  = 6;
    EN_RULESTEP     = 7;
    EN_STATISTIC    = 8;
    EN_PERIODS      = 9;
    EN_CLOCKSTART  = 20;
    EN_NODECOUNT    = 0;
    EN_TANKCOUNT    = 1;
    EN_LINKCOUNT    = 2;
    EN_PATCOUNT     = 3;
    EN_CURVECOUNT   = 4;
    EN_CONTROLCOUNT = 5;
    EN_PUMPCOUNT    = 6;
    EN_RESERVCOUNT  = 7;
    EN_JUNCSCOUNT   = 8;
    EN_JUNCTION     = 0;
    EN_RESERVOIR    = 1;
    EN_TANK         = 2;
    EN_CVPIPE       = 0;
    EN_PIPE         = 1;
    EN_PUMP         = 2;
    EN_PRV          = 3;
    EN_PSV          = 4;
    EN_PBV          = 5;
    EN_FCV          = 6;
    EN_TCV          = 7;
    EN_GPV          = 8;
    EN_NONE         = 0;
    EN_CHEM         = 1;
    EN_AGE          = 2;
    EN_TRACE        = 3;
    EN_CONCEN       = 0;
    EN_MASS         = 1;
    EN_SETPOINT     = 2;
    EN_FLOWPACED    = 3;
    EN_CFS          = 0;
    EN_GPM          = 1;
    EN_MGD          = 2;
    EN_IMGD         = 3;
    EN_AFD          = 4;
    EN_LPS          = 5;
    EN_LPM          = 6;
    EN_MLD          = 7;
    EN_CMH          = 8;
    EN_CMD          = 9;
    EN_TRIALS       = 0;
    EN_ACCURACY     = 1;
    EN_TOLERANCE    = 2;
    EN_EMITEXPON    = 3;
    EN_DEMANDMULT   = 4;
    EN_LOWLEVEL     = 0;
    EN_HILEVEL      = 1;
    EN_TIMER        = 2;
    EN_TIMEOFDAY    = 3;
    EN_AVERAGE      = 1;
    EN_MINIMUM      = 2;
    EN_MAXIMUM      = 3;
    EN_RANGE        = 4;
    EN_NOSAVE       = 0;
    EN_SAVE         = 1;

    EN_INITFLOW    = 10;

    namespace fs = boost::filesystem;



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

    int (*ENopen)(char *, char *, char *);
    ENopen = (int (*)(char *, char *, char *)) dlsym(en_lib_handle, "ENopen"); //
    if(!ENopen)
    {
        std::string err = "Unable to find ENopen function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENopen_Ibanev_f = ENopen;

    int (*ENopenH)(ENsimulation_t *);
    ENopenH = (int (*)(ENsimulation_t *)) dlsym(en_lib_handle, "ENopenH"); //
    if(!ENopenH)
    {

        std::string err = "Unable to find ENOpenH function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENopenH_Ibanev_f = ENopenH;

    int (*ENgetlinkindex)(char *, int *);
    ENgetlinkindex = (int (*)(char *, int*)) dlsym(en_lib_handle, "ENgetlinkindex");
    if(!ENgetlinkindex)
    {
        std::string err = "Unable to find ENgetlinkindex function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENgetlinkindex_f = ENgetlinkindex;

    int (*ENgetlinkid)(int, char*);
    ENgetlinkid = (int (*)(int, char*)) dlsym(en_lib_handle, "ENgetlinkid");
    if (!ENgetlinkid)
    {
        std::string err = "Unable to find ENgetlinkid function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENgetlinkid_f = ENgetlinkid;

    int (*ENgetlinktype)(int, int*);
    ENgetlinktype = (int (*)(int, int*)) dlsym(en_lib_handle, "ENgetlinktype");if
            (            !ENgetlinktype)
    {
        std::string err = "Unable to find ENgetlinktype function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENgetlinktype_f = ENgetlinktype;

    int (*ENgetlinknodes)(int, int*, int*);
    ENgetlinknodes = (int(*)(int, int*, int*)) dlsym(en_lib_handle, "ENgetlinknodes");
    if (!ENgetlinknodes)
    {
        std::string err = "Unable to find ENgetlinknodes function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENgetlinknodes_f = ENgetlinknodes;

    int (*ENgetlinkvalue)(ENsimulation_t, int, int, float *);
    ENgetlinkvalue = (int (*)(ENsimulation_t, int, int, float *)) dlsym(en_lib_handle, "ENgetlinkvalue");
    if(!ENgetlinkvalue)
    {
        std::string err = "Unable to find ENgetlinkvalue function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    boost::function<int (ENsimulation_t, int, int, float*)> ENgetlinkvalue_ibnz_flt_f(ENgetlinkvalue);
    ENfunctFlt2DblIbanez ENgetlinkvalue_ibnz_fo(ENgetlinkvalue_ibnz_flt_f);
    ENgetlinkvalue_f = boost::bind<int>(ENgetlinkvalue_ibnz_fo, boost::ref(ibanez_simulation), _1, _2, _3);

    int (*ENsetlinkvalue)(ENsimulation_t, int, int, float);
    ENsetlinkvalue = (int(*)(ENsimulation_t, int, int, float)) dlsym(en_lib_handle, "ENsetlinkvalue");
    if (!ENsetlinkvalue)
    {
        std::string err = "Unable to find ENsetlinkvalue function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENsetlinkvalue_f = boost::bind(ENsetlinkvalue, boost::ref(ibanez_simulation), _1, _2, _3);

    int (*ENgetnodeindex)(char *, int *);
    ENgetnodeindex = (int (*)(char *, int*)) dlsym(en_lib_handle, "ENgetnodeindex");
    if (!ENgetnodeindex)
    {
        std::string err ="Unable to find ENgetnodeindex function in epanet libary";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENgetnodeindex_f = ENgetnodeindex;

    int (*ENgetnodevalue)(ENsimulation_t, int, int, float *);
    ENgetnodevalue = (int (*)(ENsimulation_t, int, int, float *)) dlsym(en_lib_handle, "ENgetnodevalue");
    if(!ENgetnodevalue)
    {
        std::string err = "Unable to find ENgetnodevalue function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    boost::function<int (ENsimulation_t, int, int, float*)> ENgetnodevalue_flt_f(ENgetnodevalue);
    ENfunctFlt2DblIbanez ENgetnodevalue_fo(ENgetnodevalue_flt_f);
    ENgetnodevalue_f = boost::bind<int>(ENgetnodevalue_fo, boost::ref(ibanez_simulation), _1, _2, _3);


    int (*ENgetnodeid)(int, char*);
    ENgetnodeid = (int (*) (int, char*)) dlsym(en_lib_handle, "ENgetnodeid");
    if (!ENgetnodeid)
    {
        std::string err = "Unable to find ENgetnodeid function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENgetnodeid_f = ENgetnodeid;

    int (*ENgetnodetype)(int, int*);
    ENgetnodetype = (int (*)(int, int *)) dlsym(en_lib_handle, "ENgetnodetype");
    if (!ENgetnodetype)
    {
        std::string err = "Unable to find ENgetnodetype function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENgetnodetype_f = ENgetnodetype;

//    int (*ENsetnodevalue)(int, int, float);
//    ENsetnodevalue = (int (*)(int, int, float)) dlsym(en_lib_handle, "ENsetnodevalue");
//    if (!ENsetnodevalue)
//    {
//        std::string err = "Unable to find ENsetnodevalue function in epanet library";
//        std::cerr << err << std::endl;
//        throw std::runtime_error(err);
//    }
//    ENsetnodevalue_f = ENsetnodevalue;

    int (*ENgetcount)(int, int *);
    ENgetcount = (int (*)(int, int *)) dlsym(en_lib_handle, "ENgetcount");
    if (!ENgetcount)
    {
        std::string err = "Unable to find ENgetcount function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENgetcount_f = ENgetcount;

    int (*ENgetpatternindex)(char *, int *);
    ENgetpatternindex = (int (*)(char*, int*)) dlsym(en_lib_handle, "ENgetpatternindex");
    if(!ENgetpatternindex)
    {
        std::string err = "Unable to find ENgetpatternindex function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENgetpatternindex_f = ENgetpatternindex;

    int (*ENsetpattern)(ENsimulation_t, int, float *, int);
    ENsetpattern = (int (*)(ENsimulation_t, int, float *, int)) dlsym(en_lib_handle, "ENsetpattern");
    if (!ENsetpattern)
    {
        std::string err = "Unable to find ENsetpattern function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENsetpattern_f = boost::bind(ENsetpattern, boost::ref(ibanez_simulation), _1, _2, _3);

    int (*ENsetpatternvalue)(ENsimulation_t, int, int, float);
    ENsetpatternvalue = (int(*)(ENsimulation_t, int, int, float)) dlsym(en_lib_handle, "ENsetpatternvalue");
    if(!ENsetpatternvalue)
    {
        std::string err = "Unable to find ENsetpatternvalue function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENsetpatternvalue_f = boost::bind(ENsetpatternvalue, boost::ref(ibanez_simulation), _1, _2, _3);

    int (*ENinitH)(ENsimulation_t, int);
    ENinitH = (int (*)(ENsimulation_t, int)) dlsym(en_lib_handle, "ENinitH");
    if(!ENinitH)
    {
        std::string err = "Unable to find ENinitH function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENinitH_f = boost::bind(ENinitH, boost::ref(ibanez_simulation), 01);

    int (*ENrunH)(ENsimulation_t, long *);
    ENrunH = (int (*)(ENsimulation_t, long *)) dlsym(en_lib_handle, "ENrunH");
    if ( !ENrunH)
    {
        std::string err = "Unable to find ENrunH function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENrunH_f = boost::bind(ENrunH, boost::ref(ibanez_simulation), _1);

    int (*ENnextH)(ENsimulation_t, long *);
    ENnextH = (int (*)(ENsimulation_t, long *)) dlsym(en_lib_handle, "ENnextH");
    if (!ENnextH)
    {
        std::string err = "Unable to find ENnextH function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENnextH_f = boost::bind(ENnextH, boost::ref(ibanez_simulation), _1);

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

//    int (*ENsaveinpfile)(char *);
//    ENsaveinpfile = (int (*)(char *)) dlsym(en_lib_handle, "ENsaveinpfile");
//    if  (!ENsaveinpfile)
//    {
//        std::string err = "Unable to find ENsaveinpfile function in epanet library";
//        std::cerr << err << std::endl;
//        throw std::runtime_error(err);
//    }
//    ENsaveinpfile_f = ENsaveinpfile;

    int (*ENgeterror)(int, char *, int);
    ENgeterror = (int (*)(int, char*, int)) dlsym(en_lib_handle, "ENgeterror");
    if (!ENgeterror)
    {
        std::string err = "Unable to find ENgeterror function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENgeterror_f = ENgeterror;

    int (*ENcloseH)(ENsimulation_t);
    ENcloseH = (int (*)(ENsimulation_t)) dlsym(en_lib_handle, "ENcloseH");
    if(!ENcloseH)
    {
        std::string err = "Unable to find ENcloseH function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENcloseH_f = boost::bind(ENcloseH, boost::ref(ibanez_simulation));

    int (*ENclose)(void);
    ENclose = (int (*)(void)) dlsym(en_lib_handle, "ENclose");
    if (!ENclose)
    {
        std::string err = "Unable to find ENclose function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENclose_f = ENclose;

    //Open the toolkit and the input file in epanet
//    ENFile_cstr.reset(
//            new char[working_en_inp_path.string().size() + 1]);strcpy
//            (ENFile_cstr.get(), working_en_inp_path.c_str());
//
//    //std::cout << ENFile_cstr << std::endl;
//    errors(
//            ENopen_f(ENFile_cstr.get(), reportFile_cstr.get(),
//                     binaryFile_cstr.get()),
//            "opening EN inp file " + ENFile + " and report file "
//            + binaryFileName);
//    // OPen the hydraulic solution
//    errors(ENopenH_f(), "opening hydraulic solution");

}


void
ENMultiObjEvaluator::open_Ibanez_EN2()
{
    //Open the toolkit and the input file in epanet
    ENFile_cstr.reset(
            new char[en_inp_path.string().size() + 1]);strcpy
            (ENFile_cstr.get(), en_inp_path.c_str());

    //std::cout << ENFile_cstr << std::endl;
    errors(
            ENopen_Ibanev_f(ENFile_cstr.get(), reportFile_cstr.get(),
                             binaryFile_cstr.get()),
            "opening EN inp file " + en_inp_path.string() + " and report file "
            + binaryFileName);
    // OPen the hydraulic solution
    errors(ENopenH_Ibanev_f(&ibanez_simulation), "opening hydraulic solution");
}


void
ENMultiObjEvaluator::close_Ibanez_EN2()
{
    this->errors(ENcloseH_f(), "closing hydraulic simulation");
    this->errors(ENclose_f(), "closing OWA EN2 lib");
//        this->errors(ENsaveH_f(), "Saving hydraulics");
}