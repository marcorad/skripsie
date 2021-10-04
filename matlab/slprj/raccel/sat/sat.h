#include "__cf_sat.h"
#ifndef RTW_HEADER_sat_h_
#define RTW_HEADER_sat_h_
#include <stddef.h>
#include <string.h>
#include "rtw_modelmap.h"
#ifndef sat_COMMON_INCLUDES_
#define sat_COMMON_INCLUDES_
#include <stdlib.h>
#include "rtwtypes.h"
#include "simtarget/slSimTgtSigstreamRTW.h"
#include "simtarget/slSimTgtSlioCoreRTW.h"
#include "simtarget/slSimTgtSlioClientsRTW.h"
#include "simtarget/slSimTgtSlioSdiRTW.h"
#include "sigstream_rtw.h"
#include "simstruc.h"
#include "fixedpoint.h"
#include "raccel.h"
#include "slsv_diagnostic_codegen_c_api.h"
#include "rt_logging.h"
#include "dt_info.h"
#include "ext_work.h"
#endif
#include "sat_types.h"
#include "multiword_types.h"
#include "mwmathutil.h"
#include "rt_defines.h"
#include "rtGetInf.h"
#include "rt_nonfinite.h"
#define MODEL_NAME sat
#define NSAMPLE_TIMES (2) 
#define NINPUTS (0)       
#define NOUTPUTS (0)     
#define NBLOCKIO (1) 
#define NUM_ZC_EVENTS (0) 
#ifndef NCSTATES
#define NCSTATES (0)   
#elif NCSTATES != 0
#error Invalid specification of NCSTATES defined in compiler command
#endif
#ifndef rtmGetDataMapInfo
#define rtmGetDataMapInfo(rtm) (*rt_dataMapInfoPtr)
#endif
#ifndef rtmSetDataMapInfo
#define rtmSetDataMapInfo(rtm, val) (rt_dataMapInfoPtr = &val)
#endif
#ifndef IN_RACCEL_MAIN
#endif
typedef struct { real_T d4fs3hqoqg [ 8096 ] ; real_T m1bczq4ndq_mbvzarwird [
8096 ] ; } B ; typedef struct { real_T mayzukdd3b [ 2 ] ; real_T a4wq5skkro ;
real_T fvkkquez2d ; real_T pr0s52djpl ; real_T kllq51ttqc ; real_T gl401ntxmo
; real_T kew5inqcku ; struct { void * LoggedData ; } hvieabyy45 ; boolean_T
ir3llpuyi3 ; } DW ; typedef struct { rtwCAPI_ModelMappingInfo mmi ; }
DataMapInfo ; struct P_ { real_T G ; real_T T ; real_T Chirp_f0 ; real_T
Chirp_f1 ; real_T Chirp_phase ; real_T DiscreteFilter_NumCoef [ 3 ] ; real_T
DiscreteFilter_DenCoef [ 3 ] ; real_T DiscreteFilter_InitialStates ; real_T
Gain1_Gain ; } ; extern const char * RT_MEMORY_ALLOCATION_ERROR ; extern B
rtB ; extern DW rtDW ; extern P rtP ; extern const
rtwCAPI_ModelMappingStaticInfo * sat_GetCAPIStaticMap ( void ) ; extern
SimStruct * const rtS ; extern const int_T gblNumToFiles ; extern const int_T
gblNumFrFiles ; extern const int_T gblNumFrWksBlocks ; extern rtInportTUtable
* gblInportTUtables ; extern const char * gblInportFileName ; extern const
int_T gblNumRootInportBlks ; extern const int_T gblNumModelInputs ; extern
const int_T gblInportDataTypeIdx [ ] ; extern const int_T gblInportDims [ ] ;
extern const int_T gblInportComplex [ ] ; extern const int_T
gblInportInterpoFlag [ ] ; extern const int_T gblInportContinuous [ ] ;
extern const int_T gblParameterTuningTid ; extern DataMapInfo *
rt_dataMapInfoPtr ; extern rtwCAPI_ModelMappingInfo * rt_modelMapInfoPtr ;
void MdlOutputs ( int_T tid ) ; void MdlOutputsParameterSampleTime ( int_T
tid ) ; void MdlUpdate ( int_T tid ) ; void MdlTerminate ( void ) ; void
MdlInitializeSizes ( void ) ; void MdlInitializeSampleTimes ( void ) ;
SimStruct * raccel_register_model ( void ) ;
#endif
