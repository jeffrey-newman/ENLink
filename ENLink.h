#ifndef SRC_LIBRARY_H
#define SRC_LIBRARY_H

#ifdef _WIN32
#ifdef __cplusplus
#define DLLEXPORT extern "C" __declspec(dllexport)
#define WINAPI
//#define WINAPI __stdcall
#else
#define DLLEXPORT __declspec(dllexport)
#define WINAPI __stdcall
#endif
#else
#ifdef __cplusplus
#define DLLEXPORT extern "C"
#define WINAPI
#else
#define DLLEXPORT
#define WINAPI
#endif
#endif


/**
 * @brief Sets the working directory
 *
 * ENLink uses this to place files in - e.g. copies of the EN .inp file, and binary output files etc. Logging places
 * logs here as well. You can ensure that the working files (apart from any log files which may be saved) are deleted
 * when the analysis is finished as a parameter in the function clear.
 *
 * @param working_dir The path to the directory where files (e.g. log, temporary EN files etc) will be saved.
 * @return an int error code. 0 means success
 */
DLLEXPORT int WINAPI
setWorkingDir(const char* working_dir);


/**
 * @brief Creates a new analysis.
 *
 * Each analysis has its own runtime loaded EN library, and each analysis is identified by an integer returned from this
 * function. Always use this integer identifier to specify what analysis to apply functions to, for each of the functions
 * specified in ENLink.
 *
 * @param opt_cfg_file The path of the file
 * @return A unique identifier for this analysis. Calls to this library use this identifier to get the cost, etc for the analysis intended.
 * If this value is negative, this means an error occured during the creation of a new analysis.
 */
DLLEXPORT int WINAPI
createAnalysis(const char* opt_cfg_file);

/**
 * @brief Turns on logging of the calculations performed
 *
 * A log file is generated in the working directory, with file name "enLink_calcs.log". This breaksdown the
 * components of the calculation (e.g. costs for each pipe is costs are assessed.
 *
 * @param analysisID The identifier
 * @return an int error code. 0 means success
 */
DLLEXPORT int WINAPI
doLog(int analysisID);

/**
 *
 * @return
 */
DLLEXPORT int WINAPI
getAnAnalysis();


/**
 *
 * @param numberDVs
 * @param lower_bounds
 * @param upper_bounds
 * @return an int error code. 0 means success
 */
DLLEXPORT int* WINAPI
getDVBounds(int analysisID, int* numberDVs);

/**
 *
 * @param analysisID The identifier of this analysis
 * @param decision_variables A integer array that contains the decision variable vales
 * @return an integer error code. 0 means success. These error codes are the same as the EN2 error codes....
 */
DLLEXPORT int WINAPI
runEN(int analysisID, const int* decision_variables);


DLLEXPORT bool WINAPI
isPipeCapitalCostCalculated(int analysisID);

/**
 *
 * @param analysisID
 * @return
 */
DLLEXPORT double WINAPI
getPipeCapitalCost(int analysisID);


DLLEXPORT bool WINAPI
isPressureViolationCalculated(int analysisID);

/**
 *
 * @param analysisID
 * @return
 */
DLLEXPORT double WINAPI
getSumPressureTooHigh(int analysisID);

/**
 *
 * @param analysisID
 * @return
 */
DLLEXPORT double WINAPI
getMaxPressureTooHigh(int analysisID);

/**
 *
 * @param analysisID
 * @return
 */
DLLEXPORT double WINAPI
getSumPressureTooLow(int analysisID);

/**
 *
 * @param analysisID
 * @return
 */
DLLEXPORT double WINAPI
getMinPressureTooLow(int analysisID);

DLLEXPORT bool WINAPI
isHeadViolationCalculated(int analysisID);

/**
 *
 * @param analysisID
 * @return
 */
DLLEXPORT double WINAPI
getSumHeadTooHigh(int analysisID);

/**
 *
 * @param analysisID
 * @return
 */
DLLEXPORT double WINAPI
getMaxHeadTooHigh(int analysisID);

/**
 *
 * @param analysisID
 * @return
 */
DLLEXPORT double WINAPI
getSumHeadTooLow(int analysisID);

/**
 *
 * @param analysisID
 * @return
 */
DLLEXPORT double WINAPI
getMinHeadTooLow(int analysisID);

//Not yet implemented.
//DLLEXPORT bool WINAPI
//isVelocityViolationCalculated(int analysisID);

/**
 *
 * @param analysisID
 * @return
 */
DLLEXPORT double WINAPI
getSumVelocityTooHigh(int analysisID);

/**
 *
 * @param analysisID
 * @return
 */
DLLEXPORT double WINAPI
getMaxVelocityTooHigh(int analysisID);

/**
 *
 */
DLLEXPORT bool WINAPI
isNetworkResilienceCalculated(int analysisID);

/**
 *
 * @param analysisID
 * @return
 */
DLLEXPORT double WINAPI
getNetworkResilience(int analysisID);

/**
 *
 */
DLLEXPORT int WINAPI
saveAnalysisF(int analysisID, const char* file_path);

/**
 *
 */
DLLEXPORT int WINAPI
saveAnalysis(int analysisID);

/**
 * @param analysisID the index for the analysis to hand back (i.e. no longer need this analyssi to test network, so other threads can use)
 * @return int code whether successful or not. 0 indicates success
 */
DLLEXPORT int WINAPI
handBackAnalysis(int analysisID);


/**
 *
 * @param analysisID
 * @return
 */
DLLEXPORT int WINAPI
clear(int analysisID, bool _delete_temp_on_exit);

/**
 *
 * @return
 */
DLLEXPORT int WINAPI
clearAll(bool _delete_temp_on_exit);

#endif