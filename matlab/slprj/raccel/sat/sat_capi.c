#include "__cf_sat.h"
#include "rtw_capi.h"
#ifdef HOST_CAPI_BUILD
#include "sat_capi_host.h"
#define sizeof(s) ((size_t)(0xFFFF))
#undef rt_offsetof
#define rt_offsetof(s,el) ((uint16_T)(0xFFFF))
#define TARGET_CONST
#define TARGET_STRING(s) (s)    
#else
#include "builtin_typeid_types.h"
#include "sat.h"
#include "sat_capi.h"
#include "sat_private.h"
#ifdef LIGHT_WEIGHT_CAPI
#define TARGET_CONST                  
#define TARGET_STRING(s)               (NULL)                    
#else
#define TARGET_CONST                   const
#define TARGET_STRING(s)               (s)
#endif
#endif
static const rtwCAPI_Signals rtBlockSignals [ ] = { { 0 , 0 , TARGET_STRING (
"sat/Sum" ) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 0 } , { 0 , 0 , ( NULL )
, ( NULL ) , 0 , 0 , 0 , 0 , 0 } } ; static const rtwCAPI_BlockParameters
rtBlockParameters [ ] = { { 1 , TARGET_STRING ( "sat/Gain1" ) , TARGET_STRING
( "Gain" ) , 0 , 1 , 0 } , { 2 , TARGET_STRING ( "sat/Chirp" ) ,
TARGET_STRING ( "f0" ) , 0 , 1 , 0 } , { 3 , TARGET_STRING ( "sat/Chirp" ) ,
TARGET_STRING ( "f1" ) , 0 , 1 , 0 } , { 4 , TARGET_STRING ( "sat/Chirp" ) ,
TARGET_STRING ( "phase" ) , 0 , 1 , 0 } , { 5 , TARGET_STRING (
"sat/Discrete Filter" ) , TARGET_STRING ( "Numerator" ) , 0 , 2 , 0 } , { 6 ,
TARGET_STRING ( "sat/Discrete Filter" ) , TARGET_STRING ( "Denominator" ) , 0
, 2 , 0 } , { 7 , TARGET_STRING ( "sat/Discrete Filter" ) , TARGET_STRING (
"InitialStates" ) , 0 , 1 , 0 } , { 0 , ( NULL ) , ( NULL ) , 0 , 0 , 0 } } ;
static const rtwCAPI_ModelParameters rtModelParameters [ ] = { { 8 ,
TARGET_STRING ( "G" ) , 0 , 1 , 0 } , { 9 , TARGET_STRING ( "T" ) , 0 , 1 , 0
} , { 0 , ( NULL ) , 0 , 0 , 0 } } ;
#ifndef HOST_CAPI_BUILD
static void * rtDataAddrMap [ ] = { & rtB . d4fs3hqoqg [ 0 ] , & rtP .
Gain1_Gain , & rtP . Chirp_f0 , & rtP . Chirp_f1 , & rtP . Chirp_phase , &
rtP . DiscreteFilter_NumCoef [ 0 ] , & rtP . DiscreteFilter_DenCoef [ 0 ] , &
rtP . DiscreteFilter_InitialStates , & rtP . G , & rtP . T , } ; static
int32_T * rtVarDimsAddrMap [ ] = { ( NULL ) } ;
#endif
static TARGET_CONST rtwCAPI_DataTypeMap rtDataTypeMap [ ] = { { "double" ,
"real_T" , 0 , 0 , sizeof ( real_T ) , SS_DOUBLE , 0 , 0 , 0 } } ;
#ifdef HOST_CAPI_BUILD
#undef sizeof
#endif
static TARGET_CONST rtwCAPI_ElementMap rtElementMap [ ] = { { ( NULL ) , 0 ,
0 , 0 , 0 } , } ; static const rtwCAPI_DimensionMap rtDimensionMap [ ] = { {
rtwCAPI_MATRIX_COL_MAJOR , 0 , 2 , 0 } , { rtwCAPI_SCALAR , 2 , 2 , 0 } , {
rtwCAPI_VECTOR , 4 , 2 , 0 } } ; static const uint_T rtDimensionArray [ ] = {
8096 , 1 , 1 , 1 , 1 , 3 } ; static const real_T rtcapiStoredFloats [ ] = {
0.18358276643990931 , 0.0 } ; static const rtwCAPI_FixPtMap rtFixPtMap [ ] =
{ { ( NULL ) , ( NULL ) , rtwCAPI_FIX_RESERVED , 0 , 0 , 0 } , } ; static
const rtwCAPI_SampleTimeMap rtSampleTimeMap [ ] = { { ( const void * ) &
rtcapiStoredFloats [ 0 ] , ( const void * ) & rtcapiStoredFloats [ 1 ] , 1 ,
0 } } ; static rtwCAPI_ModelMappingStaticInfo mmiStatic = { { rtBlockSignals
, 1 , ( NULL ) , 0 , ( NULL ) , 0 } , { rtBlockParameters , 7 ,
rtModelParameters , 2 } , { ( NULL ) , 0 } , { rtDataTypeMap , rtDimensionMap
, rtFixPtMap , rtElementMap , rtSampleTimeMap , rtDimensionArray } , "float"
, { 1071215569U , 1974552370U , 303149440U , 1465377467U } , ( NULL ) , 0 , 0
} ; const rtwCAPI_ModelMappingStaticInfo * sat_GetCAPIStaticMap ( void ) {
return & mmiStatic ; }
#ifndef HOST_CAPI_BUILD
void sat_InitializeDataMapInfo ( void ) { rtwCAPI_SetVersion ( ( *
rt_dataMapInfoPtr ) . mmi , 1 ) ; rtwCAPI_SetStaticMap ( ( *
rt_dataMapInfoPtr ) . mmi , & mmiStatic ) ; rtwCAPI_SetLoggingStaticMap ( ( *
rt_dataMapInfoPtr ) . mmi , ( NULL ) ) ; rtwCAPI_SetDataAddressMap ( ( *
rt_dataMapInfoPtr ) . mmi , rtDataAddrMap ) ; rtwCAPI_SetVarDimsAddressMap (
( * rt_dataMapInfoPtr ) . mmi , rtVarDimsAddrMap ) ;
rtwCAPI_SetInstanceLoggingInfo ( ( * rt_dataMapInfoPtr ) . mmi , ( NULL ) ) ;
rtwCAPI_SetChildMMIArray ( ( * rt_dataMapInfoPtr ) . mmi , ( NULL ) ) ;
rtwCAPI_SetChildMMIArrayLen ( ( * rt_dataMapInfoPtr ) . mmi , 0 ) ; }
#else
#ifdef __cplusplus
extern "C" {
#endif
void sat_host_InitializeDataMapInfo ( sat_host_DataMapInfo_T * dataMap ,
const char * path ) { rtwCAPI_SetVersion ( dataMap -> mmi , 1 ) ;
rtwCAPI_SetStaticMap ( dataMap -> mmi , & mmiStatic ) ;
rtwCAPI_SetDataAddressMap ( dataMap -> mmi , NULL ) ;
rtwCAPI_SetVarDimsAddressMap ( dataMap -> mmi , NULL ) ; rtwCAPI_SetPath (
dataMap -> mmi , path ) ; rtwCAPI_SetFullPath ( dataMap -> mmi , NULL ) ;
rtwCAPI_SetChildMMIArray ( dataMap -> mmi , ( NULL ) ) ;
rtwCAPI_SetChildMMIArrayLen ( dataMap -> mmi , 0 ) ; }
#ifdef __cplusplus
}
#endif
#endif
