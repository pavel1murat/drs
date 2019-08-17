///////////////////////////////////////////////////////////////////////////////
// place holder for PET geomentry constants
///////////////////////////////////////////////////////////////////////////////
#ifndef drs_geom_TPetGeometry_hh
#define drs_geom_TPetGeometry_hh

double   const   BOUNDARY_PRECISION   =  0.1;

//       PET detector containment volume (cm):

double   const   PET_XMAX          =    500.0;
double   const   PET_YMAX          =    50.0;
double   const   PET_ZMAX          =    20.0;
double   const   PET_DIMENSIONS[]  = { PET_XMAX, PET_YMAX, PET_ZMAX };

char     const    TOF_DETECTOR_SET_NAME[] = "TOFS";

#endif
