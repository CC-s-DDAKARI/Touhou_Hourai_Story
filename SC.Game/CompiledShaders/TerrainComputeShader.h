#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 10.1
//
//
// Buffer Definitions: 
//
// cbuffer gConstants
// {
//
//   struct Constants
//   {
//       
//       uint GridX;                    // Offset:    0
//       uint GridY;                    // Offset:    4
//
//   } gConstants;                      // Offset:    0 Size:     8
//
// }
//
// Resource bind info for gVertexBufferBase
// {
//
//   struct Vertex
//   {
//       
//       float3 Pos;                    // Offset:    0
//       float4 Color;                  // Offset:   12
//       float2 Tex;                    // Offset:   28
//       float3 Normal;                 // Offset:   36
//       float3 Tangent;                // Offset:   48
//
//   } $Element;                        // Offset:    0 Size:    60
//
// }
//
// Resource bind info for gVertexBuffer
// {
//
//   struct Vertex
//   {
//       
//       float3 Pos;                    // Offset:    0
//       float4 Color;                  // Offset:   12
//       float2 Tex;                    // Offset:   28
//       float3 Normal;                 // Offset:   36
//       float3 Tangent;                // Offset:   48
//
//   } $Element;                        // Offset:    0 Size:    60
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim      ID      HLSL Bind  Count
// ------------------------------ ---------- ------- ----------- ------- -------------- ------
// gSampler                          sampler      NA          NA      S0             s0      1 
// gHeightMap                        texture  float4          2d      T0             t0      1 
// gVertexBufferBase                 texture  struct         r/o      T1             t1      1 
// gVertexBuffer                         UAV  struct         r/w      U0             u0      1 
// gConstants                        cbuffer      NA          NA     CB0            cb0      1 
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// no Input
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// no Output
cs_5_1
dcl_globalFlags refactoringAllowed
dcl_constantbuffer CB0[0:0][1], immediateIndexed, space=0
dcl_sampler S0[0:0], mode_default, space=0
dcl_resource_texture2d (float,float,float,float) T0[0:0], space=0
dcl_resource_structured T1[1:1], 60, space=0
dcl_uav_structured U0[0:0], 60, space=0
dcl_input vThreadID.xy
dcl_temps 6
dcl_thread_group 32, 32, 1
iadd r0.xy, CB0[0][0].xyxx, l(1, 1, 0, 0)
ult r0.zw, vThreadID.xxxy, r0.xxxy
and r0.z, r0.w, r0.z
if_nz r0.z
  utof r0.z, CB0[0][0].x
  div r0.z, l(2.000000), r0.z
  iadd r1.xyzw, vThreadID.xyyx, l(-1, -1, 0, 0)
  imax r1.xy, r1.xyxx, l(0, 0, 0, 0)
  imin r1.xyzw, r0.xyxy, r1.xzwy
  imad r1.xy, r1.ywyy, r0.xxxx, r1.xzxx
  ld_structured r1.xz, r1.x, l(28), T1[1].xxyx
  sample_l r0.w, r1.xzxx, T0[0].yzwx, S0[0], l(0.000000)
  ld_structured r1.xy, r1.y, l(28), T1[1].xyxx
  sample_l r1.x, r1.xyxx, T0[0].xyzw, S0[0], l(0.000000)
  iadd r2.xyzw, vThreadID.xyxy, l(1, 0, 0, 1)
  imin r2.xyzw, r0.xyxy, r2.xyzw
  imad r1.yz, r2.yywy, r0.xxxx, r2.xxzx
  ld_structured r1.yw, r1.y, l(28), T1[1].xxxy
  sample_l r1.y, r1.ywyy, T0[0].yxzw, S0[0], l(0.000000)
  ld_structured r1.zw, r1.z, l(28), T1[1].xxxy
  sample_l r1.z, r1.zwzz, T0[0].yzxw, S0[0], l(0.000000)
  imin r2.xy, r0.xyxx, vThreadID.xyxx
  imad r0.x, r2.y, r0.x, r2.x
  ld_structured r2.x, r0.x, l(0), T1[1].xxxx
  ld_structured r3.xyzw, r0.x, l(8), T1[1].zwxy
  ld_structured r4.xyz, r0.x, l(24), T1[1].xyzx
  sample_l r2.y, r4.yzyy, T0[0].yxzw, S0[0], l(0.000000)
  add r5.w, -r0.w, r1.y
  mov r5.x, -r5.w
  add r5.y, r0.z, r0.z
  add r5.z, -r1.x, r1.z
  dp3 r0.y, r5.xyzx, r5.xyzx
  rsq r0.y, r0.y
  mul r1.yzw, r0.yyyy, r5.xxyz
  dp2 r0.y, r5.ywyy, r5.ywyy
  rsq r0.y, r0.y
  mul r5.xy, r0.yyyy, r5.ywyy
  mov r2.zw, r3.zzzw
  store_structured U0[0].xyzw, r0.x, l(0), r2.xyzw
  mov r3.zw, r4.xxxy
  store_structured U0[0].xyzw, r0.x, l(16), r3.xyzw
  mov r1.x, r4.z
  store_structured U0[0].xyzw, r0.x, l(32), r1.xyzw
  mov r5.z, l(0)
  store_structured U0[0].xyz, r0.x, l(48), r5.xyzx
endif 
ret 
// Approximately 47 instruction slots used
#endif

const BYTE pTerrainComputeShader[] =
{
     68,  88,  66,  67,  92, 161, 
     46,  53,  94, 119,  14, 169, 
     13,  99, 151, 184,  65, 204, 
     79, 199,   1,   0,   0,   0, 
    104,  11,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
    252,   3,   0,   0,  12,   4, 
      0,   0,  28,   4,   0,   0, 
    204,  10,   0,   0,  82,  68, 
     69,  70, 192,   3,   0,   0, 
      3,   0,   0,   0,  68,   1, 
      0,   0,   5,   0,   0,   0, 
     60,   0,   0,   0,   1,   5, 
     83,  67,   0,   5,   0,   0, 
    152,   3,   0,   0,  19,  19, 
     68,  37,  60,   0,   0,   0, 
     24,   0,   0,   0,  40,   0, 
      0,   0,  40,   0,   0,   0, 
     36,   0,   0,   0,  12,   0, 
      0,   0,   0,   0,   0,   0, 
      4,   1,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  13,   1, 
      0,   0,   2,   0,   0,   0, 
      5,   0,   0,   0,   4,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0,   1,   0, 
      0,   0,  12,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  24,   1,   0,   0, 
      5,   0,   0,   0,   6,   0, 
      0,   0,   1,   0,   0,   0, 
     60,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
     42,   1,   0,   0,   6,   0, 
      0,   0,   6,   0,   0,   0, 
      1,   0,   0,   0,  60,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  56,   1, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 103,  83,  97, 109, 
    112, 108, 101, 114,   0, 103, 
     72, 101, 105, 103, 104, 116, 
     77,  97, 112,   0, 103,  86, 
    101, 114, 116, 101, 120,  66, 
    117, 102, 102, 101, 114,  66, 
     97, 115, 101,   0, 103,  86, 
    101, 114, 116, 101, 120,  66, 
    117, 102, 102, 101, 114,   0, 
    103,  67, 111, 110, 115, 116, 
     97, 110, 116, 115,   0, 171, 
     56,   1,   0,   0,   1,   0, 
      0,   0, 140,   1,   0,   0, 
     16,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     24,   1,   0,   0,   1,   0, 
      0,   0,  52,   2,   0,   0, 
     60,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
     42,   1,   0,   0,   1,   0, 
      0,   0, 112,   3,   0,   0, 
     60,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
     56,   1,   0,   0,   0,   0, 
      0,   0,   8,   0,   0,   0, 
      2,   0,   0,   0,  16,   2, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0,  67, 111, 
    110, 115, 116,  97, 110, 116, 
    115,   0,  71, 114, 105, 100, 
     88,   0, 100, 119, 111, 114, 
    100,   0, 171, 171,   0,   0, 
     19,   0,   1,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    196,   1,   0,   0,  71, 114, 
    105, 100,  89,   0, 171, 171, 
    190,   1,   0,   0, 204,   1, 
      0,   0,   0,   0,   0,   0, 
    240,   1,   0,   0, 204,   1, 
      0,   0,   4,   0,   0,   0, 
      5,   0,   0,   0,   1,   0, 
      2,   0,   0,   0,   2,   0, 
    248,   1,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 180,   1,   0,   0, 
     92,   2,   0,   0,   0,   0, 
      0,   0,  60,   0,   0,   0, 
      2,   0,   0,   0,  76,   3, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0,  36,  69, 
    108, 101, 109, 101, 110, 116, 
      0,  86, 101, 114, 116, 101, 
    120,   0,  80, 111, 115,   0, 
    102, 108, 111,  97, 116,  51, 
      0, 171,   1,   0,   3,   0, 
      1,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 112,   2, 
      0,   0,  67, 111, 108, 111, 
    114,   0, 102, 108, 111,  97, 
    116,  52,   0, 171, 171, 171, 
      1,   0,   3,   0,   1,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 162,   2,   0,   0, 
     84, 101, 120,   0, 102, 108, 
    111,  97, 116,  50,   0, 171, 
      1,   0,   3,   0,   1,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 212,   2,   0,   0, 
     78, 111, 114, 109,  97, 108, 
      0,  84,  97, 110, 103, 101, 
    110, 116,   0, 171, 108,   2, 
      0,   0, 120,   2,   0,   0, 
      0,   0,   0,   0, 156,   2, 
      0,   0, 172,   2,   0,   0, 
     12,   0,   0,   0, 208,   2, 
      0,   0, 220,   2,   0,   0, 
     28,   0,   0,   0,   0,   3, 
      0,   0, 120,   2,   0,   0, 
     36,   0,   0,   0,   7,   3, 
      0,   0, 120,   2,   0,   0, 
     48,   0,   0,   0,   5,   0, 
      0,   0,   1,   0,  15,   0, 
      0,   0,   5,   0,  16,   3, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    101,   2,   0,   0,  92,   2, 
      0,   0,   0,   0,   0,   0, 
     60,   0,   0,   0,   2,   0, 
      0,   0,  76,   3,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,  77, 105,  99, 114, 
    111, 115, 111, 102, 116,  32, 
     40,  82,  41,  32,  72,  76, 
     83,  76,  32,  83, 104,  97, 
    100, 101, 114,  32,  67, 111, 
    109, 112, 105, 108, 101, 114, 
     32,  49,  48,  46,  49,   0, 
     73,  83,  71,  78,   8,   0, 
      0,   0,   0,   0,   0,   0, 
      8,   0,   0,   0,  79,  83, 
     71,  78,   8,   0,   0,   0, 
      0,   0,   0,   0,   8,   0, 
      0,   0,  83,  72,  69,  88, 
    168,   6,   0,   0,  81,   0, 
      5,   0, 170,   1,   0,   0, 
    106,   8,   0,   1,  89,   0, 
      0,   7,  70, 142,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
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
    162,   0,   0,   7,  70, 126, 
     48,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,  60,   0,   0,   0, 
      0,   0,   0,   0, 158,   0, 
      0,   7,  70, 238,  49,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     60,   0,   0,   0,   0,   0, 
      0,   0,  95,   0,   0,   2, 
     50,   0,   2,   0, 104,   0, 
      0,   2,   6,   0,   0,   0, 
    155,   0,   0,   4,  32,   0, 
      0,   0,  32,   0,   0,   0, 
      1,   0,   0,   0,  30,   0, 
      0,  12,  50,   0,  16,   0, 
      0,   0,   0,   0,  70, 128, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   2,  64,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  79,   0, 
      0,   6, 194,   0,  16,   0, 
      0,   0,   0,   0,   6,   4, 
      2,   0,   6,   4,  16,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   7,  66,   0,  16,   0, 
      0,   0,   0,   0,  58,   0, 
     16,   0,   0,   0,   0,   0, 
     42,   0,  16,   0,   0,   0, 
      0,   0,  31,   0,   4,   3, 
     42,   0,  16,   0,   0,   0, 
      0,   0,  86,   0,   0,   7, 
     66,   0,  16,   0,   0,   0, 
      0,   0,  10, 128,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     14,   0,   0,   7,  66,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,  64,  42,   0,  16,   0, 
      0,   0,   0,   0,  30,   0, 
      0,   9, 242,   0,  16,   0, 
      1,   0,   0,   0,  70,   1, 
      2,   0,   2,  64,   0,   0, 
    255, 255, 255, 255, 255, 255, 
    255, 255,   0,   0,   0,   0, 
      0,   0,   0,   0,  36,   0, 
      0,  10,  50,   0,  16,   0, 
      1,   0,   0,   0,  70,   0, 
     16,   0,   1,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  37,   0,   0,   7, 
    242,   0,  16,   0,   1,   0, 
      0,   0,  70,   4,  16,   0, 
      0,   0,   0,   0, 134,   7, 
     16,   0,   1,   0,   0,   0, 
     35,   0,   0,   9,  50,   0, 
     16,   0,   1,   0,   0,   0, 
    214,   5,  16,   0,   1,   0, 
      0,   0,   6,   0,  16,   0, 
      0,   0,   0,   0, 134,   0, 
     16,   0,   1,   0,   0,   0, 
    167,   0,   0,  10,  82,   0, 
     16,   0,   1,   0,   0,   0, 
     10,   0,  16,   0,   1,   0, 
      0,   0,   1,  64,   0,   0, 
     28,   0,   0,   0,   6, 113, 
     32,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,  72,   0, 
      0,  13, 130,   0,  16,   0, 
      0,   0,   0,   0, 134,   0, 
     16,   0,   1,   0,   0,   0, 
    150, 115,  32,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,  96,  32,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0, 167,   0,   0,  10, 
     50,   0,  16,   0,   1,   0, 
      0,   0,  26,   0,  16,   0, 
      1,   0,   0,   0,   1,  64, 
      0,   0,  28,   0,   0,   0, 
     70, 112,  32,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
     72,   0,   0,  13,  18,   0, 
     16,   0,   1,   0,   0,   0, 
     70,   0,  16,   0,   1,   0, 
      0,   0,  70, 126,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,  96,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,  30,   0, 
      0,   9, 242,   0,  16,   0, 
      2,   0,   0,   0,  70,   4, 
      2,   0,   2,  64,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,  37,   0, 
      0,   7, 242,   0,  16,   0, 
      2,   0,   0,   0,  70,   4, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   2,   0, 
      0,   0,  35,   0,   0,   9, 
     98,   0,  16,   0,   1,   0, 
      0,   0,  86,   7,  16,   0, 
      2,   0,   0,   0,   6,   0, 
     16,   0,   0,   0,   0,   0, 
      6,   2,  16,   0,   2,   0, 
      0,   0, 167,   0,   0,  10, 
    162,   0,  16,   0,   1,   0, 
      0,   0,  26,   0,  16,   0, 
      1,   0,   0,   0,   1,  64, 
      0,   0,  28,   0,   0,   0, 
      6, 116,  32,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
     72,   0,   0,  13,  34,   0, 
     16,   0,   1,   0,   0,   0, 
    214,   5,  16,   0,   1,   0, 
      0,   0,  22, 126,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,  96,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0, 167,   0, 
      0,  10, 194,   0,  16,   0, 
      1,   0,   0,   0,  42,   0, 
     16,   0,   1,   0,   0,   0, 
      1,  64,   0,   0,  28,   0, 
      0,   0,   6, 116,  32,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,  72,   0,   0,  13, 
     66,   0,  16,   0,   1,   0, 
      0,   0, 230,  10,  16,   0, 
      1,   0,   0,   0, 150, 124, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,  96, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
     37,   0,   0,   6,  50,   0, 
     16,   0,   2,   0,   0,   0, 
     70,   0,  16,   0,   0,   0, 
      0,   0,  70,   0,   2,   0, 
     35,   0,   0,   9,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     26,   0,  16,   0,   2,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   2,   0,   0,   0, 
    167,   0,   0,  10,  18,   0, 
     16,   0,   2,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,   6, 112, 
     32,   0,   1,   0,   0,   0, 
      1,   0,   0,   0, 167,   0, 
      0,  10, 242,   0,  16,   0, 
      3,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   8,   0, 
      0,   0, 230, 116,  32,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0, 167,   0,   0,  10, 
    114,   0,  16,   0,   4,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,  24,   0,   0,   0, 
     70, 114,  32,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
     72,   0,   0,  13,  34,   0, 
     16,   0,   2,   0,   0,   0, 
    150,   5,  16,   0,   4,   0, 
      0,   0,  22, 126,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,  96,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   8, 130,   0,  16,   0, 
      5,   0,   0,   0,  58,   0, 
     16, 128,  65,   0,   0,   0, 
      0,   0,   0,   0,  26,   0, 
     16,   0,   1,   0,   0,   0, 
     54,   0,   0,   6,  18,   0, 
     16,   0,   5,   0,   0,   0, 
     58,   0,  16, 128,  65,   0, 
      0,   0,   5,   0,   0,   0, 
      0,   0,   0,   7,  34,   0, 
     16,   0,   5,   0,   0,   0, 
     42,   0,  16,   0,   0,   0, 
      0,   0,  42,   0,  16,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   8,  66,   0,  16,   0, 
      5,   0,   0,   0,  10,   0, 
     16, 128,  65,   0,   0,   0, 
      1,   0,   0,   0,  42,   0, 
     16,   0,   1,   0,   0,   0, 
     16,   0,   0,   7,  34,   0, 
     16,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   5,   0, 
      0,   0,  70,   2,  16,   0, 
      5,   0,   0,   0,  68,   0, 
      0,   5,  34,   0,  16,   0, 
      0,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     56,   0,   0,   7, 226,   0, 
     16,   0,   1,   0,   0,   0, 
     86,   5,  16,   0,   0,   0, 
      0,   0,   6,   9,  16,   0, 
      5,   0,   0,   0,  15,   0, 
      0,   7,  34,   0,  16,   0, 
      0,   0,   0,   0, 214,   5, 
     16,   0,   5,   0,   0,   0, 
    214,   5,  16,   0,   5,   0, 
      0,   0,  68,   0,   0,   5, 
     34,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,  56,   0, 
      0,   7,  50,   0,  16,   0, 
      5,   0,   0,   0,  86,   5, 
     16,   0,   0,   0,   0,   0, 
    214,   5,  16,   0,   5,   0, 
      0,   0,  54,   0,   0,   5, 
    194,   0,  16,   0,   2,   0, 
      0,   0, 166,  14,  16,   0, 
      3,   0,   0,   0, 168,   0, 
      0,  10, 242, 224,  33,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   2,   0, 
      0,   0,  54,   0,   0,   5, 
    194,   0,  16,   0,   3,   0, 
      0,   0,   6,   4,  16,   0, 
      4,   0,   0,   0, 168,   0, 
      0,  10, 242, 224,  33,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,  16,   0,   0,   0, 
     70,  14,  16,   0,   3,   0, 
      0,   0,  54,   0,   0,   5, 
     18,   0,  16,   0,   1,   0, 
      0,   0,  42,   0,  16,   0, 
      4,   0,   0,   0, 168,   0, 
      0,  10, 242, 224,  33,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,  32,   0,   0,   0, 
     70,  14,  16,   0,   1,   0, 
      0,   0,  54,   0,   0,   5, 
     66,   0,  16,   0,   5,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0, 168,   0, 
      0,  10, 114, 224,  33,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,  48,   0,   0,   0, 
     70,   2,  16,   0,   5,   0, 
      0,   0,  21,   0,   0,   1, 
     62,   0,   0,   1,  83,  84, 
     65,  84, 148,   0,   0,   0, 
     47,   0,   0,   0,   6,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,  11,   0, 
      0,   0,  10,   0,   0,   0, 
      2,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     12,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      4,   0,   0,   0
};
