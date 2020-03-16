#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 10.1
//
//
// Buffer Definitions: 
//
// cbuffer gMaterial
// {
//
//   struct Material_Constants
//   {
//       
//       int Index;                     // Offset:    0
//       int DiffuseMap;                // Offset:    4
//       int NormalMap;                 // Offset:    8
//       int AlphaClip;                 // Offset:   12
//
//   } gMaterial;                       // Offset:    0 Size:    16
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim      ID      HLSL Bind  Count
// ------------------------------ ---------- ------- ----------- ------- -------------- ------
// gSampler                          sampler      NA          NA      S0             s0      1 
// gDiffuseMap0                      texture  float4          2d      T0             t0      1 
// gMaterial                         cbuffer      NA          NA     CB0            cb3      1 
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_Position              0   xyzw        0      POS   float       
// COLOR                    0   xyzw        1     NONE   float      w
// TEXCOORD                 0   xy          2     NONE   float   xy  
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// no Output
ps_5_1
dcl_globalFlags refactoringAllowed
dcl_constantbuffer CB0[3:3][1], immediateIndexed, space=0
dcl_sampler S0[0:0], mode_default, space=0
dcl_resource_texture2d (float,float,float,float) T0[0:0], space=0
dcl_input_ps linear v1.w
dcl_input_ps linear v2.xy
dcl_temps 1
sample r0.x, v2.xyxx, T0[0].wxyz, S0[0]
imin r0.y, CB0[3][0].y, l(1)
itof r0.y, r0.y
add r0.x, r0.x, -v1.w
mad r0.x, r0.y, r0.x, v1.w
add r0.x, r0.x, l(-0.900000)
lt r0.x, r0.x, l(0.000000)
discard_nz r0.x
ret 
// Approximately 9 instruction slots used
#endif

const BYTE pShadowCastPixelShader[] =
{
     68,  88,  66,  67, 230, 243, 
     70, 155, 182, 128, 109, 216, 
     61, 228, 105, 250, 255,  90, 
    252, 124,   1,   0,   0,   0, 
    196,   4,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
     48,   2,   0,   0, 164,   2, 
      0,   0, 180,   2,   0,   0, 
     40,   4,   0,   0,  82,  68, 
     69,  70, 244,   1,   0,   0, 
      1,   0,   0,   0, 212,   0, 
      0,   0,   3,   0,   0,   0, 
     60,   0,   0,   0,   1,   5, 
    255, 255,   0,   5,   0,   0, 
    204,   1,   0,   0,  19,  19, 
     68,  37,  60,   0,   0,   0, 
     24,   0,   0,   0,  40,   0, 
      0,   0,  40,   0,   0,   0, 
     36,   0,   0,   0,  12,   0, 
      0,   0,   0,   0,   0,   0, 
    180,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 189,   0, 
      0,   0,   2,   0,   0,   0, 
      5,   0,   0,   0,   4,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0,   1,   0, 
      0,   0,  12,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 202,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    103,  83,  97, 109, 112, 108, 
    101, 114,   0, 103,  68, 105, 
    102, 102, 117, 115, 101,  77, 
     97, 112,  48,   0, 103,  77, 
     97, 116, 101, 114, 105,  97, 
    108,   0, 202,   0,   0,   0, 
      1,   0,   0,   0, 236,   0, 
      0,   0,  16,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 202,   0,   0,   0, 
      0,   0,   0,   0,  16,   0, 
      0,   0,   2,   0,   0,   0, 
    168,   1,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
     77,  97, 116, 101, 114, 105, 
     97, 108,  95,  67, 111, 110, 
    115, 116,  97, 110, 116, 115, 
      0,  73, 110, 100, 101, 120, 
      0, 105, 110, 116,   0, 171, 
    171, 171,   0,   0,   2,   0, 
      1,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  45,   1, 
      0,   0,  68, 105, 102, 102, 
    117, 115, 101,  77,  97, 112, 
      0,  78, 111, 114, 109,  97, 
    108,  77,  97, 112,   0,  65, 
    108, 112, 104,  97,  67, 108, 
    105, 112,   0, 171,  39,   1, 
      0,   0,  52,   1,   0,   0, 
      0,   0,   0,   0,  88,   1, 
      0,   0,  52,   1,   0,   0, 
      4,   0,   0,   0,  99,   1, 
      0,   0,  52,   1,   0,   0, 
      8,   0,   0,   0, 109,   1, 
      0,   0,  52,   1,   0,   0, 
     12,   0,   0,   0,   5,   0, 
      0,   0,   1,   0,   4,   0, 
      0,   0,   4,   0, 120,   1, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     20,   1,   0,   0,  77, 105, 
     99, 114, 111, 115, 111, 102, 
    116,  32,  40,  82,  41,  32, 
     72,  76,  83,  76,  32,  83, 
    104,  97, 100, 101, 114,  32, 
     67, 111, 109, 112, 105, 108, 
    101, 114,  32,  49,  48,  46, 
     49,   0,  73,  83,  71,  78, 
    108,   0,   0,   0,   3,   0, 
      0,   0,   8,   0,   0,   0, 
     80,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  15,   0,   0,   0, 
     92,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   1,   0, 
      0,   0,  15,   8,   0,   0, 
     98,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   2,   0, 
      0,   0,   3,   3,   0,   0, 
     83,  86,  95,  80, 111, 115, 
    105, 116, 105, 111, 110,   0, 
     67,  79,  76,  79,  82,   0, 
     84,  69,  88,  67,  79,  79, 
     82,  68,   0, 171,  79,  83, 
     71,  78,   8,   0,   0,   0, 
      0,   0,   0,   0,   8,   0, 
      0,   0,  83,  72,  69,  88, 
    108,   1,   0,   0,  81,   0, 
      0,   0,  91,   0,   0,   0, 
    106,   8,   0,   1,  89,   0, 
      0,   7,  70, 142,  48,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   3,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,  90,   0,   0,   6, 
     70, 110,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  88,  24,   0,   7, 
     70, 126,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  85,  85, 
      0,   0,   0,   0,   0,   0, 
     98,  16,   0,   3, 130,  16, 
     16,   0,   1,   0,   0,   0, 
     98,  16,   0,   3,  50,  16, 
     16,   0,   2,   0,   0,   0, 
    104,   0,   0,   2,   1,   0, 
      0,   0,  69,   0,   0,  11, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  70,  16,  16,   0, 
      2,   0,   0,   0,  54, 121, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,  96, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  37,   0, 
      0,   9,  34,   0,  16,   0, 
      0,   0,   0,   0,  26, 128, 
     48,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      1,   0,   0,   0,  43,   0, 
      0,   5,  34,   0,  16,   0, 
      0,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
      0,   0,   0,   8,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  58,  16,  16, 128, 
     65,   0,   0,   0,   1,   0, 
      0,   0,  50,   0,   0,   9, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
     58,  16,  16,   0,   1,   0, 
      0,   0,   0,   0,   0,   7, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0, 102, 102, 102, 191, 
     49,   0,   0,   7,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,  13,   0, 
      4,   3,  10,   0,  16,   0, 
      0,   0,   0,   0,  62,   0, 
      0,   1,  83,  84,  65,  84, 
    148,   0,   0,   0,   9,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,   4,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0
};
