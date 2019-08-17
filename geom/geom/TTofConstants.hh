// -*- Mode: C++  -*-
//------------------------------------------------------------------------------
//  revision history:
//  -----------------
// *0001 Jun 29 1997 G.Velev: this version
//                             
//------------------------------------------------------------------------------
#ifndef TCmpConstants_hh
#define TCmpConstants_hh


// Some MC constants 

// float      const    CMPEFF = 1.0;   // chamber efficiency
// float      const    CMPNPB = 0.0;   // random noise probability
// float      const    CMPDVL =45.0;   // drift velocity ( microns/ns)
// float      const    CMPDSG = 0.0;   // drift distance resolution sigma
// float      const    CMPQSG = 0.0;   // harge resolution sigma (femtocoulombs)
// float      const    CMPTQM = 0.056; // least count of TDC (ns)
// float      const    CMPQQM = 3.50;  // C  least count of ADC (fc)  

// New constans for reconstructions
                                 
// float      const    W0_CMP_CHAMBERS     = 100.;
// float      const    SLOPE_TOT_VS_TOT    = 1 ;     //(TOT vs TOT slope)
// float      const    SLOPE_TOT_VS_CHARGE = 1 ;      //(TOT vs Charge slope)
// float 	   const    CORRECTION_TOT_VS_CHARGE = 0 ; //(TOT vs Charge quadratic correction)

					// this originates from CMPD_StorableBank.cc

// const signed char CMP_LOGICAL_STACK[4][4][24] = 
// { 
//     {
// // Region 0, TDC 0
//         {  0,  1,  2,  3,  4,  5,  
//            6, -1, -1,  7,  8,  9, 
//           10, 11, 12, 13, 14, 15, 
//           16, 17, 18, 19, 20, 21 },

// // Region 0, TDC 1
//         { 22, 23, 24, 25, 26, 27, 
//           28, 29, 30, 31, 32, 33, 
//           34, 35, 36, 37, 38, 39, 
//           40, 41, 42, 43, 44, 45 },

// // Region 0, TDC 2
//         { 46, 47, 48, 49, 50, 51, 
//           52, 53, 54, 55, 56, 57, 
//           58, 59, 60, 61, -1, -1,
//           -1, -1, -1, -1, -1, -1 },

// // Region 0, TDC 3
//         { -1, -1, -1, -1, -1, -1,       
//           -1, -1, -1, -1, -1, -1,
//           -1, -1, -1, -1, -1, -1,
//           -1, -1, -1, -1, -1, -1 }
//     },
//     {
// // Region 1, TDC 0
//         {  0,  1,  2,  3,  4, -1,       
//            5,  6,  7,  8,  9, 10,
//           11, 12, 13, -1, -1, -1,
//           14, 15, 16, 17, 18, 19 },

// // Region 1, TDC 1
//         { 20, 21, 22, 23, 24, 25,       
//           26, 27, 28, 29, 30, 31,
//           32, 33, 34, 35, 36, 37,
//           38, 39, 40, 41, 42, 43 },

// // Region 1, TDC 2
//         { 44, 45, 46, 47, -1, -1,       
//           48, 49, 50, 51, 52, 53,
//           54, 55, 56, 57, 58, 59,
//           60, 61, 62, 63, 64, 65 },

// // Region 1, TDC 3
//         { 66, 67, 68, 69, 70, -1,       
//           71, 72, 73, 74, 75, 76,
//           -1, -1, -1, -1, -1, -1,
//           -1, -1, -1, -1, -1, -1 }
//     },
//     {
// // Region 2, TDC 0
//         { -1, 62, 61, 60, 59, 58,       
//           57, 56, -1, -1, 55, 54,
//           53, 52, 51, 50, 49, 48,
//           47, 46, 45, 44, 43, 42 },

// // Region 2, TDC 1
//         { 41, 40, 39, 38, 37, 36,       
//           35, 34, 33, 32, 31, 30,
//           29, 28, 27, 26, 25, 24,
//           23, 22, 21, 20, 19, 18 },

// // Region 2, TDC 2
//         { 17, 16, 15, 14, 13, 12,       
//           11, 10,  9,  8,  7,  6,
//            5,  4,  3,  2,  1,  0,
//           -1, -1, -1, -1, -1, -1 },

// // Region 2, TDC 3
//         { -1, -1, -1, -1, -1, -1,       
//           -1, -1, -1, -1, -1, -1,
//           -1, -1, -1, -1, -1, -1,
//           -1, -1, -1, -1, -1, -1 }
//     },
//     {
// // Region 3, TDC 0
//         {  0,  1,  2,  3,  4,  5,       
//            6,  7,  8,  9, 10, 11,
//           12, 13, 14, 15, 16, 17,
//           18, 19, 20, 21, 22, 23 },

// // Region 3, TDC 1
//         { 24, 25, 26, 27, 28, 29,       
//           30, 31, 32, 33, 34, 35,
//           36, 37, 38, 39, 40, 41,
//           42, 43, 44, 45, 46, 47 },

// // Region 3, TDC 2
//         { 48, 49, 50, 51, 52, 53,       
//           54, 55, 56, 57, 58, 59,
//           60, 61, 62, 63, 64, -1,
//           -1, -1, -1, -1, -1, -1 },

// // Region 3, TDC 3
//         { -1, -1, -1, -1, -1, -1,       
//           -1, -1, -1, -1, -1, -1,
//           -1, -1, -1, -1, -1, -1,
//           -1, -1, -1, -1, -1, -1 }
//     }
// };
#endif
