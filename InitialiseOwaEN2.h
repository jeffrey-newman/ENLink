//
// Created by a1091793 on 29/3/17.
//


#include "ENMultiObjEvaluator.h"
#include <dlfcn.h>

class ENfunctFlt2Dbl
{
private:
    boost::function<int (int, int, float*) >  en_flt_api_funct;
    float val_f;
    double val_d;
public:

    ENfunctFlt2Dbl(boost::function<int (int, int, float*) > & _en_flt_api_funct)
            : en_flt_api_funct(_en_flt_api_funct)
    {

    }

    int
    operator()(int arg1, int arg2, double * arg3)
    {
        int ret = en_flt_api_funct(arg1, arg2, &val_f);
        *arg3 = double(val_f);
        return (ret);
    }
};



void
ENMultiObjEvaluator::initialise_OWA_EN2()
{


    namespace fs = boost::filesystem;

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
    EN_INITVOLUME   = 14;
    EN_MIXMODEL     = 15;
    EN_MIXZONEVOL   = 16;
    EN_TANKDIAM     = 17;
    EN_MINVOLUME    = 18;
    EN_VOLCURVE     = 19;
    EN_MINLEVEL     = 20;
    EN_MAXLEVEL     = 21;
    EN_MIXFRACTION  = 22;
    EN_TANK_KBULK   = 23;
    EN_TANKVOLUME   = 24;
    EN_MAXVOLUME    = 25;

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
    EN_LINKQUAL     = 14;
    EN_LINKPATTERN  = 15;

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
    EN_STARTTIME    = 10;
    EN_HTIME        = 11;
    EN_QTIME        = 12;
    EN_HALTFLAG     = 13;
    EN_NEXTEVENT    = 14;

    EN_ITERATIONS    = 0;
    EN_RELATIVEERROR = 1;

    EN_NODECOUNT    = 0;
    EN_TANKCOUNT    = 1;
    EN_LINKCOUNT    = 2;
    EN_PATCOUNT     = 3;
    EN_CURVECOUNT   = 4;
    EN_CONTROLCOUNT = 5;

    EN_JUNCTION    = 0;
    EN_RESERVOIR   = 1;
    EN_TANK        = 2;

    EN_CVPIPE       = 0;
    EN_PIPE         = 1;
    EN_PUMP         = 2;
    EN_PRV          = 3;
    EN_PSV          = 4;
    EN_PBV          = 5;
    EN_FCV          = 6;
    EN_TCV          = 7;
    EN_GPV          = 8;

    EN_NONE        = 0;
    EN_CHEM        = 1;
    EN_AGE         = 2;
    EN_TRACE       = 3;

    EN_CONCEN      = 0;
    EN_MASS        = 1;
    EN_SETPOINT    = 2;
    EN_FLOWPACED   = 3;

    EN_CFS         = 0;
    EN_GPM         = 1;
    EN_MGD         = 2;
    EN_IMGD        = 3;
    EN_AFD         = 4;
    EN_LPS         = 5;
    EN_LPM         = 6;
    EN_MLD         = 7;
    EN_CMH         = 8;
    EN_CMD         = 9;

    EN_TRIALS       = 0;
    EN_ACCURACY     = 1;
    EN_TOLERANCE    = 2;
    EN_EMITEXPON    = 3;
    EN_DEMANDMULT   = 4;

    EN_LOWLEVEL    = 0;
    EN_HILEVEL     = 1;
    EN_TIMER       = 2;
    EN_TIMEOFDAY   = 3;

    EN_AVERAGE     = 1;
    EN_MINIMUM     = 2;
    EN_MAXIMUM     = 3;
    EN_RANGE       = 4;


    EN_MIX1        = 0;
    EN_MIX2        = 1;
    EN_FIFO        = 2;
    EN_LIFO        = 3;

    EN_NOSAVE        = 0;
    EN_SAVE          = 1;
    EN_INITFLOW      = 10;
    EN_SAVE_AND_INIT = 11;

    EN_CONST_HP    = 0;
    EN_POWER_FUNC  = 1;
    EN_CUSTOM      = 2;



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

    int (*ENopen)(char *, char *, char *);
    ENopen = (int (*)(char *, char *, char *)) dlsym(en_lib_handle, "ENopen"); //
    if(!ENopen)
    {
        std::string err = "Unable to find ENopen function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENopen_owa_en2_f = ENopen;

    int (*ENopenH)(void);
    ENopenH = (int (*)(void)) dlsym(en_lib_handle, "ENopenH"); //
    if(!ENopenH)
    {

        std::string err = "Unable to find ENOpenH function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENopenH_owa_en2_f = ENopenH;

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

    int (*ENgetlinkvalue)(int, int, float *);
    ENgetlinkvalue = (int (*)(int, int, float *)) dlsym(en_lib_handle, "ENgetlinkvalue");
    if(!ENgetlinkvalue)
    {
        std::string err = "Unable to find ENgetlinkvalue function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    boost::function<int (int, int, float*)> ENgetlinkvalue_owa2_flt_f(ENgetlinkvalue);
    ENfunctFlt2Dbl ENgetlinkvalue_owa2_fo(ENgetlinkvalue_owa2_flt_f);
//    ENgetlinkvalue_owa2_flt_f = boost::function<int (int, int, float*)>(ENgetlinkvalue);
//    ENgetlinkvalue_owa2_fo = ENfunctFlt2Dbl(ENgetlinkvalue_owa2_flt_f);
    ENgetlinkvalue_f = ENgetlinkvalue_owa2_fo;
//    ENgetlinkvalue_f = ENgetlinkvalue;

    int (*ENsetlinkvalue)(int, int, float);
    ENsetlinkvalue = (int(*)(int, int, float)) dlsym(en_lib_handle, "ENsetlinkvalue");
    if (!ENsetlinkvalue)
    {
        std::string err = "Unable to find ENsetlinkvalue function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENsetlinkvalue_f = ENsetlinkvalue;

    int (*ENgetnodeindex)(char *, int *);
    ENgetnodeindex = (int (*)(char *, int*)) dlsym(en_lib_handle, "ENgetnodeindex");
    if (!ENgetnodeindex)
    {
        std::string err ="Unable to find ENgetnodeindex function in epanet libary";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENgetnodeindex_f = ENgetnodeindex;

    int (*ENgetnodevalue)(int, int, float *);
    ENgetnodevalue = (int (*)(int, int, float *)) dlsym(en_lib_handle, "ENgetnodevalue");
    if(!ENgetnodevalue)
    {
        std::string err = "Unable to find ENgetnodevalue function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    boost::function<int (int, int, float*)> ENgetnodevalue_owa2_flt_f(ENgetnodevalue);
    ENfunctFlt2Dbl ENgetnodevalue_owa2_fo(ENgetnodevalue_owa2_flt_f);
    ENgetnodevalue_f = ENgetnodevalue_owa2_fo;
//    ENgetnodevalue_f = ENgetnodevalue;

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

    int (*ENsetnodevalue)(int, int, float);
    ENsetnodevalue = (int (*)(int, int, float)) dlsym(en_lib_handle, "ENsetnodevalue");
    if (!ENsetnodevalue)
    {
        std::string err = "Unable to find ENsetnodevalue function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENsetnodevalue_f = ENsetnodevalue;

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

    int (*ENsetpattern)(int, float *, int);
    ENsetpattern = (int (*)(int, float*, int)) dlsym(en_lib_handle, "ENsetpattern");
    if (!ENsetpattern)
    {
        std::string err = "Unable to find ENsetpattern function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENsetpattern_f = ENsetpattern;

    int (*ENsetpatternvalue)(int, int, float);
    ENsetpatternvalue = (int(*)(int, int, float)) dlsym(en_lib_handle, "ENsetpatternvalue");
    if(!ENsetpatternvalue)
    {
        std::string err = "Unable to find ENsetpatternvalue function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENsetpatternvalue_f = ENsetpatternvalue;

    int (*ENinitH)(int);
    ENinitH = (int (*)(int)) dlsym(en_lib_handle, "ENinitH");
    if(!ENinitH)
    {
        std::string err = "Unable to find ENinitH function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENinitH_f = boost::bind(ENinitH, 01);

    int (*ENrunH)(long *);
    ENrunH = (int (*)(long *)) dlsym(en_lib_handle, "ENrunH");
    if ( !ENrunH)
    {
        std::string err = "Unable to find ENrunH function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENrunH_f = ENrunH;

    int (*ENnextH)(long *);
    ENnextH = (int (*)(long *)) dlsym(en_lib_handle, "ENnextH");
    if (!ENnextH)
    {
        std::string err = "Unable to find ENnextH function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENnextH_f = ENnextH;

    int (*ENsaveH)(void);
    ENsaveH = (int (*)(void)) dlsym(en_lib_handle, "ENsaveH");
    if (!ENsaveH)
    {
        std::string err = "Unable to find ENsaveH function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENsaveH_f = ENsaveH;

    int (*ENresetreport)(void);
    ENresetreport = (int (*)(void)) dlsym(en_lib_handle, "ENresetreport");
    if (!ENresetreport)
    {
        std::string err = "Unable to find ENresetreport function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENresetreport_f = ENresetreport;

    int (*ENsetreport)(char *);
    ENsetreport = (int (*)(char *)) dlsym(en_lib_handle, "ENsetreport");
    if (!ENsetreport)
    {
        std::string err = "Unable to find ENsetreport function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENsetreport_f = ENsetreport;

    int (*ENreport)(void);
    ENreport = (int (*)(void)) dlsym(en_lib_handle, "ENreport");
    if (!ENreport)
    {
        std::string err = "Unable to find ENreport function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENreport_f = ENreport;

    int (*ENsaveinpfile)(char *);
    ENsaveinpfile = (int (*)(char *)) dlsym(en_lib_handle, "ENsaveinpfile");
    if  (!ENsaveinpfile)
    {
        std::string err = "Unable to find ENsaveinpfile function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENsaveinpfile_f = ENsaveinpfile;

    int (*ENgeterror)(int, char *, int);
    ENgeterror = (int (*)(int, char*, int)) dlsym(en_lib_handle, "ENgeterror");
    if (!ENgeterror)
    {
        std::string err = "Unable to find ENgeterror function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENgeterror_f = ENgeterror;

    int (*ENcloseH)(void);
    ENcloseH = (int (*)(void)) dlsym(en_lib_handle, "ENcloseH");
    if(!ENcloseH)
    {
        std::string err = "Unable to find ENcloseH function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENcloseH_f = ENcloseH;

    int (*ENclose)(void);
    ENclose = (int (*)(void)) dlsym(en_lib_handle, "ENclose");
    if (!ENclose)
    {
        std::string err = "Unable to find ENclose function in epanet library";
        std::cerr << err << std::endl;
        throw std::runtime_error(err);
    }
    ENclose_f = ENclose;




}

void
ENMultiObjEvaluator::open_OWA_EN2()
{
    //Open the toolkit and the input file in epanet
    ENFile_cstr.reset(
            new char[en_inp_path.string().size() + 1]);strcpy
            (ENFile_cstr.get(), en_inp_path.c_str());

    //std::cout << ENFile_cstr << std::endl;
    errors(
            ENopen_owa_en2_f(ENFile_cstr.get(), reportFile_cstr.get(),
                     binaryFile_cstr.get()),
            "opening EN inp file " + en_inp_path.string() + " and report file "
            + binaryFileName);
    // OPen the hydraulic solution
    errors(ENopenH_owa_en2_f(), "opening hydraulic solution");
}

void
ENMultiObjEvaluator::close_OWA_EN2()
{
    this->errors(ENcloseH_f(), "closing hydraulic simulation");
    this->errors(ENclose_f(), "closing OWA EN2 lib");
//        this->errors(ENsaveH_f(), "Saving hydraulics");
}
