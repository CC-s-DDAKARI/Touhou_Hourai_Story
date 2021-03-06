#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 10.1
//
//
// Buffer Definitions: 
//
// cbuffer gCamera
// {
//
//   struct Camera
//   {
//       
//       row_major float4x4 ViewProj;   // Offset:    0
//       row_major float4x4 ViewInv;    // Offset:   64
//       row_major float4x4 ProjInv;    // Offset:  128
//       float3 Pos;                    // Offset:  192
//
//   } gCamera;                         // Offset:    0 Size:   204
//
// }
//
// cbuffer gTransform
// {
//
//   struct Transform
//   {
//       
//       row_major float4x4 World;      // Offset:    0
//       row_major float4x4 WorldInvTranspose;// Offset:   64
//
//   } gTransform;                      // Offset:    0 Size:   128
//
// }
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
//       row_major float4x4 TexWorld;   // Offset:   16
//
//   } gMaterial;                       // Offset:    0 Size:    80
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim      ID      HLSL Bind  Count
// ------------------------------ ---------- ------- ----------- ------- -------------- ------
// gCamera                           cbuffer      NA          NA     CB0            cb0      1 
// gTransform                        cbuffer      NA          NA     CB1            cb1      1 
// gMaterial                         cbuffer      NA          NA     CB2            cb3      1 
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// POSITION                 0   xyz         0     NONE   float   xyz 
// COLOR                    0   xyzw        1     NONE   float   xyzw
// TEXCOORD                 0   xy          2     NONE   float   xy  
// NORMAL                   0   xyz         3     NONE   float   xyz 
// TANGENT                  0   xyz         4     NONE   float   xyz 
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_Position              0   xyzw        0      POS   float   xyzw
// COLOR                    0   xyzw        1     NONE   float   xyzw
// TEXCOORD                 0   xy          2     NONE   float   xy  
// NORMAL                   0   xyz         3     NONE   float   xyz 
// TANGENT                  0   xyz         4     NONE   float   xyz 
//
vs_5_1
dcl_globalFlags refactoringAllowed
dcl_constantbuffer CB0[0:0][4], immediateIndexed, space=0
dcl_constantbuffer CB1[1:1][7], immediateIndexed, space=0
dcl_constantbuffer CB2[3:3][5], immediateIndexed, space=0
dcl_input v0.xyz
dcl_input v1.xyzw
dcl_input v2.xy
dcl_input v3.xyz
dcl_input v4.xyz
dcl_output_siv o0.xyzw, position
dcl_output o1.xyzw
dcl_output o2.xy
dcl_output o3.xyz
dcl_output o4.xyz
dcl_temps 4
mul r0.xy, v2.yyyy, CB2[3][2].xyxx
mad r0.xy, v2.xxxx, CB2[3][1].xyxx, r0.xyxx
add o2.xy, r0.xyxx, CB2[3][4].xyxx
mul r0.xyzw, CB0[0][1].xyzw, CB1[1][0].yyyy
mad r0.xyzw, CB1[1][0].xxxx, CB0[0][0].xyzw, r0.xyzw
mad r0.xyzw, CB1[1][0].zzzz, CB0[0][2].xyzw, r0.xyzw
mad r0.xyzw, CB1[1][0].wwww, CB0[0][3].xyzw, r0.xyzw
mul r1.xyzw, CB0[0][1].xyzw, CB1[1][1].yyyy
mad r1.xyzw, CB1[1][1].xxxx, CB0[0][0].xyzw, r1.xyzw
mad r1.xyzw, CB1[1][1].zzzz, CB0[0][2].xyzw, r1.xyzw
mad r1.xyzw, CB1[1][1].wwww, CB0[0][3].xyzw, r1.xyzw
mul r2.xyzw, CB0[0][1].xyzw, CB1[1][2].yyyy
mad r2.xyzw, CB1[1][2].xxxx, CB0[0][0].xyzw, r2.xyzw
mad r2.xyzw, CB1[1][2].zzzz, CB0[0][2].xyzw, r2.xyzw
mad r2.xyzw, CB1[1][2].wwww, CB0[0][3].xyzw, r2.xyzw
mul r3.xyzw, CB0[0][1].xyzw, CB1[1][3].yyyy
mad r3.xyzw, CB1[1][3].xxxx, CB0[0][0].xyzw, r3.xyzw
mad r3.xyzw, CB1[1][3].zzzz, CB0[0][2].xyzw, r3.xyzw
mad r3.xyzw, CB1[1][3].wwww, CB0[0][3].xyzw, r3.xyzw
mul r1.xyzw, r1.xyzw, v0.yyyy
mad r0.xyzw, v0.xxxx, r0.xyzw, r1.xyzw
mad r0.xyzw, v0.zzzz, r2.xyzw, r0.xyzw
add o0.xyzw, r3.xyzw, r0.xyzw
mul r0.xyz, v3.yyyy, CB1[1][5].xyzx
mad r0.xyz, v3.xxxx, CB1[1][4].xyzx, r0.xyzx
mad o3.xyz, v3.zzzz, CB1[1][6].xyzx, r0.xyzx
mul r0.xyz, v4.yyyy, CB1[1][1].xyzx
mad r0.xyz, v4.xxxx, CB1[1][0].xyzx, r0.xyzx
mad o4.xyz, v4.zzzz, CB1[1][2].xyzx, r0.xyzx
mov o1.xyzw, v1.xyzw
ret 
// Approximately 31 instruction slots used
#endif

const BYTE pColorVertexShader[] =
{
     68,  88,  66,  67,  14, 246, 
    133,  53, 173,  25, 137,  75, 
     23, 210,  44,  21,   9, 181, 
    166, 229,   1,   0,   0,   0, 
      8,  12,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
    248,   3,   0,   0, 168,   4, 
      0,   0,  92,   5,   0,   0, 
    108,  11,   0,   0,  82,  68, 
     69,  70, 188,   3,   0,   0, 
      3,   0,   0,   0, 212,   0, 
      0,   0,   3,   0,   0,   0, 
     60,   0,   0,   0,   1,   5, 
    254, 255,   0,   5,   0,   0, 
    148,   3,   0,   0,  19,  19, 
     68,  37,  60,   0,   0,   0, 
     24,   0,   0,   0,  40,   0, 
      0,   0,  40,   0,   0,   0, 
     36,   0,   0,   0,  12,   0, 
      0,   0,   0,   0,   0,   0, 
    180,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 188,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0, 199,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   2,   0,   0,   0, 
    103,  67,  97, 109, 101, 114, 
     97,   0, 103,  84, 114,  97, 
    110, 115, 102, 111, 114, 109, 
      0, 103,  77,  97, 116, 101, 
    114, 105,  97, 108,   0, 171, 
    171, 171, 180,   0,   0,   0, 
      1,   0,   0,   0,  28,   1, 
      0,   0, 208,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 188,   0,   0,   0, 
      1,   0,   0,   0,  24,   2, 
      0,   0, 128,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 199,   0,   0,   0, 
      1,   0,   0,   0, 160,   2, 
      0,   0,  80,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 180,   0,   0,   0, 
      0,   0,   0,   0, 204,   0, 
      0,   0,   2,   0,   0,   0, 
    244,   1,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
     67,  97, 109, 101, 114,  97, 
      0,  86, 105, 101, 119,  80, 
    114, 111, 106,   0, 102, 108, 
    111,  97, 116,  52, 120,  52, 
      0, 171, 171, 171,   2,   0, 
      3,   0,   4,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     84,   1,   0,   0,  86, 105, 
    101, 119,  73, 110, 118,   0, 
     80, 114, 111, 106,  73, 110, 
    118,   0,  80, 111, 115,   0, 
    102, 108, 111,  97, 116,  51, 
      0, 171,   1,   0,   3,   0, 
      1,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 152,   1, 
      0,   0,  75,   1,   0,   0, 
     96,   1,   0,   0,   0,   0, 
      0,   0, 132,   1,   0,   0, 
     96,   1,   0,   0,  64,   0, 
      0,   0, 140,   1,   0,   0, 
     96,   1,   0,   0, 128,   0, 
      0,   0, 148,   1,   0,   0, 
    160,   1,   0,   0, 192,   0, 
      0,   0,   5,   0,   0,   0, 
      1,   0,  51,   0,   0,   0, 
      4,   0, 196,   1,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  68,   1, 
      0,   0, 188,   0,   0,   0, 
      0,   0,   0,   0, 128,   0, 
      0,   0,   2,   0,   0,   0, 
    124,   2,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
     84, 114,  97, 110, 115, 102, 
    111, 114, 109,   0,  87, 111, 
    114, 108, 100,   0,  87, 111, 
    114, 108, 100,  73, 110, 118, 
     84, 114,  97, 110, 115, 112, 
    111, 115, 101,   0, 171, 171, 
     74,   2,   0,   0,  96,   1, 
      0,   0,   0,   0,   0,   0, 
     80,   2,   0,   0,  96,   1, 
      0,   0,  64,   0,   0,   0, 
      5,   0,   0,   0,   1,   0, 
     32,   0,   0,   0,   2,   0, 
    100,   2,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  64,   2,   0,   0, 
    199,   0,   0,   0,   0,   0, 
      0,   0,  80,   0,   0,   0, 
      2,   0,   0,   0, 112,   3, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0,  77,  97, 
    116, 101, 114, 105,  97, 108, 
     95,  67, 111, 110, 115, 116, 
     97, 110, 116, 115,   0,  73, 
    110, 100, 101, 120,   0, 105, 
    110, 116,   0, 171, 171, 171, 
      0,   0,   2,   0,   1,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 225,   2,   0,   0, 
     68, 105, 102, 102, 117, 115, 
    101,  77,  97, 112,   0,  78, 
    111, 114, 109,  97, 108,  77, 
     97, 112,   0,  65, 108, 112, 
    104,  97,  67, 108, 105, 112, 
      0,  84, 101, 120,  87, 111, 
    114, 108, 100,   0, 219,   2, 
      0,   0, 232,   2,   0,   0, 
      0,   0,   0,   0,  12,   3, 
      0,   0, 232,   2,   0,   0, 
      4,   0,   0,   0,  23,   3, 
      0,   0, 232,   2,   0,   0, 
      8,   0,   0,   0,  33,   3, 
      0,   0, 232,   2,   0,   0, 
     12,   0,   0,   0,  43,   3, 
      0,   0,  96,   1,   0,   0, 
     16,   0,   0,   0,   5,   0, 
      0,   0,   1,   0,  20,   0, 
      0,   0,   5,   0,  52,   3, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    200,   2,   0,   0,  77, 105, 
     99, 114, 111, 115, 111, 102, 
    116,  32,  40,  82,  41,  32, 
     72,  76,  83,  76,  32,  83, 
    104,  97, 100, 101, 114,  32, 
     67, 111, 109, 112, 105, 108, 
    101, 114,  32,  49,  48,  46, 
     49,   0,  73,  83,  71,  78, 
    168,   0,   0,   0,   5,   0, 
      0,   0,   8,   0,   0,   0, 
    128,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,   7,   7,   0,   0, 
    137,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   1,   0, 
      0,   0,  15,  15,   0,   0, 
    143,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   2,   0, 
      0,   0,   3,   3,   0,   0, 
    152,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   3,   0, 
      0,   0,   7,   7,   0,   0, 
    159,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   4,   0, 
      0,   0,   7,   7,   0,   0, 
     80,  79,  83,  73,  84,  73, 
     79,  78,   0,  67,  79,  76, 
     79,  82,   0,  84,  69,  88, 
     67,  79,  79,  82,  68,   0, 
     78,  79,  82,  77,  65,  76, 
      0,  84,  65,  78,  71,  69, 
     78,  84,   0, 171,  79,  83, 
     71,  78, 172,   0,   0,   0, 
      5,   0,   0,   0,   8,   0, 
      0,   0, 128,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,  15,   0, 
      0,   0, 140,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      1,   0,   0,   0,  15,   0, 
      0,   0, 146,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      2,   0,   0,   0,   3,  12, 
      0,   0, 155,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      3,   0,   0,   0,   7,   8, 
      0,   0, 162,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      4,   0,   0,   0,   7,   8, 
      0,   0,  83,  86,  95,  80, 
    111, 115, 105, 116, 105, 111, 
    110,   0,  67,  79,  76,  79, 
     82,   0,  84,  69,  88,  67, 
     79,  79,  82,  68,   0,  78, 
     79,  82,  77,  65,  76,   0, 
     84,  65,  78,  71,  69,  78, 
     84,   0, 171, 171,  83,  72, 
     69,  88,   8,   6,   0,   0, 
     81,   0,   1,   0, 130,   1, 
      0,   0, 106,   8,   0,   1, 
     89,   0,   0,   7,  70, 142, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,  89,   0, 
      0,   7,  70, 142,  48,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
      7,   0,   0,   0,   0,   0, 
      0,   0,  89,   0,   0,   7, 
     70, 142,  48,   0,   2,   0, 
      0,   0,   3,   0,   0,   0, 
      3,   0,   0,   0,   5,   0, 
      0,   0,   0,   0,   0,   0, 
     95,   0,   0,   3, 114,  16, 
     16,   0,   0,   0,   0,   0, 
     95,   0,   0,   3, 242,  16, 
     16,   0,   1,   0,   0,   0, 
     95,   0,   0,   3,  50,  16, 
     16,   0,   2,   0,   0,   0, 
     95,   0,   0,   3, 114,  16, 
     16,   0,   3,   0,   0,   0, 
     95,   0,   0,   3, 114,  16, 
     16,   0,   4,   0,   0,   0, 
    103,   0,   0,   4, 242,  32, 
     16,   0,   0,   0,   0,   0, 
      1,   0,   0,   0, 101,   0, 
      0,   3, 242,  32,  16,   0, 
      1,   0,   0,   0, 101,   0, 
      0,   3,  50,  32,  16,   0, 
      2,   0,   0,   0, 101,   0, 
      0,   3, 114,  32,  16,   0, 
      3,   0,   0,   0, 101,   0, 
      0,   3, 114,  32,  16,   0, 
      4,   0,   0,   0, 104,   0, 
      0,   2,   4,   0,   0,   0, 
     56,   0,   0,   9,  50,   0, 
     16,   0,   0,   0,   0,   0, 
     86,  21,  16,   0,   2,   0, 
      0,   0,  70, 128,  48,   0, 
      2,   0,   0,   0,   3,   0, 
      0,   0,   2,   0,   0,   0, 
     50,   0,   0,  11,  50,   0, 
     16,   0,   0,   0,   0,   0, 
      6,  16,  16,   0,   2,   0, 
      0,   0,  70, 128,  48,   0, 
      2,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
     70,   0,  16,   0,   0,   0, 
      0,   0,   0,   0,   0,   9, 
     50,  32,  16,   0,   2,   0, 
      0,   0,  70,   0,  16,   0, 
      0,   0,   0,   0,  70, 128, 
     48,   0,   2,   0,   0,   0, 
      3,   0,   0,   0,   4,   0, 
      0,   0,  56,   0,   0,  11, 
    242,   0,  16,   0,   0,   0, 
      0,   0,  70, 142,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
     86, 133,  48,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,  50,   0, 
      0,  13, 242,   0,  16,   0, 
      0,   0,   0,   0,   6, 128, 
     48,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,  70, 142,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   0,   0, 
      0,   0,  50,   0,   0,  13, 
    242,   0,  16,   0,   0,   0, 
      0,   0, 166, 138,  48,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
     70, 142,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      2,   0,   0,   0,  70,  14, 
     16,   0,   0,   0,   0,   0, 
     50,   0,   0,  13, 242,   0, 
     16,   0,   0,   0,   0,   0, 
    246, 143,  48,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,  70, 142, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,  70,  14,  16,   0, 
      0,   0,   0,   0,  56,   0, 
      0,  11, 242,   0,  16,   0, 
      1,   0,   0,   0,  70, 142, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,  86, 133,  48,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
     50,   0,   0,  13, 242,   0, 
     16,   0,   1,   0,   0,   0, 
      6, 128,  48,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,  70, 142, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0,  50,   0, 
      0,  13, 242,   0,  16,   0, 
      1,   0,   0,   0, 166, 138, 
     48,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,  70, 142,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   2,   0,   0,   0, 
     70,  14,  16,   0,   1,   0, 
      0,   0,  50,   0,   0,  13, 
    242,   0,  16,   0,   1,   0, 
      0,   0, 246, 143,  48,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
     70, 142,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,  70,  14, 
     16,   0,   1,   0,   0,   0, 
     56,   0,   0,  11, 242,   0, 
     16,   0,   2,   0,   0,   0, 
     70, 142,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,  86, 133, 
     48,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,   2,   0, 
      0,   0,  50,   0,   0,  13, 
    242,   0,  16,   0,   2,   0, 
      0,   0,   6, 128,  48,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,   2,   0,   0,   0, 
     70, 142,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   2,   0,   0,   0, 
     50,   0,   0,  13, 242,   0, 
     16,   0,   2,   0,   0,   0, 
    166, 138,  48,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
      2,   0,   0,   0,  70, 142, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,  70,  14,  16,   0, 
      2,   0,   0,   0,  50,   0, 
      0,  13, 242,   0,  16,   0, 
      2,   0,   0,   0, 246, 143, 
     48,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,   2,   0, 
      0,   0,  70, 142,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
     70,  14,  16,   0,   2,   0, 
      0,   0,  56,   0,   0,  11, 
    242,   0,  16,   0,   3,   0, 
      0,   0,  70, 142,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
     86, 133,  48,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
      3,   0,   0,   0,  50,   0, 
      0,  13, 242,   0,  16,   0, 
      3,   0,   0,   0,   6, 128, 
     48,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,   3,   0, 
      0,   0,  70, 142,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   3,   0, 
      0,   0,  50,   0,   0,  13, 
    242,   0,  16,   0,   3,   0, 
      0,   0, 166, 138,  48,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,   3,   0,   0,   0, 
     70, 142,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      2,   0,   0,   0,  70,  14, 
     16,   0,   3,   0,   0,   0, 
     50,   0,   0,  13, 242,   0, 
     16,   0,   3,   0,   0,   0, 
    246, 143,  48,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
      3,   0,   0,   0,  70, 142, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,  70,  14,  16,   0, 
      3,   0,   0,   0,  56,   0, 
      0,   7, 242,   0,  16,   0, 
      1,   0,   0,   0,  70,  14, 
     16,   0,   1,   0,   0,   0, 
     86,  21,  16,   0,   0,   0, 
      0,   0,  50,   0,   0,   9, 
    242,   0,  16,   0,   0,   0, 
      0,   0,   6,  16,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   1,   0, 
      0,   0,  50,   0,   0,   9, 
    242,   0,  16,   0,   0,   0, 
      0,   0, 166,  26,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   2,   0,   0,   0, 
     70,  14,  16,   0,   0,   0, 
      0,   0,   0,   0,   0,   7, 
    242,  32,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      3,   0,   0,   0,  70,  14, 
     16,   0,   0,   0,   0,   0, 
     56,   0,   0,   9, 114,   0, 
     16,   0,   0,   0,   0,   0, 
     86,  21,  16,   0,   3,   0, 
      0,   0,  70, 130,  48,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,   5,   0,   0,   0, 
     50,   0,   0,  11, 114,   0, 
     16,   0,   0,   0,   0,   0, 
      6,  16,  16,   0,   3,   0, 
      0,   0,  70, 130,  48,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,   4,   0,   0,   0, 
     70,   2,  16,   0,   0,   0, 
      0,   0,  50,   0,   0,  11, 
    114,  32,  16,   0,   3,   0, 
      0,   0, 166,  26,  16,   0, 
      3,   0,   0,   0,  70, 130, 
     48,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,   6,   0, 
      0,   0,  70,   2,  16,   0, 
      0,   0,   0,   0,  56,   0, 
      0,   9, 114,   0,  16,   0, 
      0,   0,   0,   0,  86,  21, 
     16,   0,   4,   0,   0,   0, 
     70, 130,  48,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,  50,   0, 
      0,  11, 114,   0,  16,   0, 
      0,   0,   0,   0,   6,  16, 
     16,   0,   4,   0,   0,   0, 
     70, 130,  48,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   0,   0,   0,   0, 
     50,   0,   0,  11, 114,  32, 
     16,   0,   4,   0,   0,   0, 
    166,  26,  16,   0,   4,   0, 
      0,   0,  70, 130,  48,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,   2,   0,   0,   0, 
     70,   2,  16,   0,   0,   0, 
      0,   0,  54,   0,   0,   5, 
    242,  32,  16,   0,   1,   0, 
      0,   0,  70,  30,  16,   0, 
      1,   0,   0,   0,  62,   0, 
      0,   1,  83,  84,  65,  84, 
    148,   0,   0,   0,  31,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,  10,   0, 
      0,   0,  29,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
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
      0,   0,   0,   0,   0,   0, 
      0,   0
};
