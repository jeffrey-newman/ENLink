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
 * Creates a new analysis. Each analysis has its own runtime loaded EN library.
 * @param opt_cfg_file The path of the file
 * @return A unique identifier for this analysis. Calls to this library use this identifier to get the cost, etc for the analysis intended.
 */
DLLEXPORT int WINAPI
createAnalysis(const char* opt_cfg_file);

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


/**
 *
 * @param analysisID
 * @return
 */
DLLEXPORT double WINAPI
getCost(int analysisID);


/**
 *
 * @param analysisID
 * @return
 */
DLLEXPORT double WINAPI
getPressureConstraint(int analysisID);


/**
 *
 * @param analysisID
 * @return
 */
DLLEXPORT double WINAPI
getHeadConstraint(int analysisID);


/**
 *
 * @param analysisID
 * @return
 */
DLLEXPORT double WINAPI
getVelocityConstraint(int analysisID);


/**
 *
 * @param analysisID
 * @return
 */
DLLEXPORT double WINAPI
getResilience(int analysisID);


DLLEXPORT void WINAPI
handBackAnalysis(int analysisID);


/**
 *
 * @param analysisID
 * @return
 */
DLLEXPORT int WINAPI
clear(int analysisID);

/**
 *
 * @return
 */
DLLEXPORT int WINAPI
clearAll();

#endif