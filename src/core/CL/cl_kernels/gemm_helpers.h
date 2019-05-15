/*
 * Copyright (c) 2019 ARM Limited.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "helpers.h"

#define LOAD_ROW_1(N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z) \
    VEC_DATA_TYPE(DATA_TYPE, N0)                                      \
    BASENAME##0 = VLOAD(N0)(0, (__global DATA_TYPE *)(PTR + OFFSET + 0 * STRIDE_Y + Z##0));

#define LOAD_ROW_2(N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z) \
    LOAD_ROW_1(N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z)     \
    VEC_DATA_TYPE(DATA_TYPE, N0)                                      \
    BASENAME##1 = VLOAD(N0)(0, (__global DATA_TYPE *)(PTR + OFFSET + 1 * STRIDE_Y + Z##1));

#define LOAD_ROW_3(N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z) \
    LOAD_ROW_2(N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z)     \
    VEC_DATA_TYPE(DATA_TYPE, N0)                                      \
    BASENAME##2 = VLOAD(N0)(0, (__global DATA_TYPE *)(PTR + OFFSET + 2 * STRIDE_Y + Z##2));

#define LOAD_ROW_4(N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z) \
    LOAD_ROW_3(N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z)     \
    VEC_DATA_TYPE(DATA_TYPE, N0)                                      \
    BASENAME##3 = VLOAD(N0)(0, (__global DATA_TYPE *)(PTR + OFFSET + 3 * STRIDE_Y + Z##3));

#define LOAD_ROW_5(N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z) \
    LOAD_ROW_4(N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z)     \
    VEC_DATA_TYPE(DATA_TYPE, N0)                                      \
    BASENAME##4 = VLOAD(N0)(0, (__global DATA_TYPE *)(PTR + OFFSET + 4 * STRIDE_Y + Z##4));

#define LOAD_ROW_6(N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z) \
    LOAD_ROW_5(N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z)     \
    VEC_DATA_TYPE(DATA_TYPE, N0)                                      \
    BASENAME##5 = VLOAD(N0)(0, (__global DATA_TYPE *)(PTR + OFFSET + 5 * STRIDE_Y + Z##5));

#define LOAD_ROW_7(N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z) \
    LOAD_ROW_6(N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z)     \
    VEC_DATA_TYPE(DATA_TYPE, N0)                                      \
    BASENAME##6 = VLOAD(N0)(0, (__global DATA_TYPE *)(PTR + OFFSET + 6 * STRIDE_Y + Z##6));

#define LOAD_ROW_8(N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z) \
    LOAD_ROW_7(N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z)     \
    VEC_DATA_TYPE(DATA_TYPE, N0)                                      \
    BASENAME##7 = VLOAD(N0)(0, (__global DATA_TYPE *)(PTR + OFFSET + 7 * STRIDE_Y + Z##7));

#define LOAD_ROW_9(N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z) \
    LOAD_ROW_8(N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z)     \
    VEC_DATA_TYPE(DATA_TYPE, N0)                                      \
    BASENAME##8 = VLOAD(N0)(0, (__global DATA_TYPE *)(PTR + OFFSET + 8 * STRIDE_Y + Z##8));

#define LOAD_ROW_10(N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z) \
    LOAD_ROW_9(N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z)      \
    VEC_DATA_TYPE(DATA_TYPE, N0)                                       \
    BASENAME##9 = VLOAD(N0)(0, (__global DATA_TYPE *)(PTR + OFFSET + 9 * STRIDE_Y + Z##9));

#define LOAD_ROW_11(N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z) \
    LOAD_ROW_10(N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z)     \
    VEC_DATA_TYPE(DATA_TYPE, N0)                                       \
    BASENAME##A = VLOAD(N0)(0, (__global DATA_TYPE *)(PTR + OFFSET + 10 * STRIDE_Y + Z##A));

#define LOAD_ROW_12(N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z) \
    LOAD_ROW_11(N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z)     \
    VEC_DATA_TYPE(DATA_TYPE, N0)                                       \
    BASENAME##B = VLOAD(N0)(0, (__global DATA_TYPE *)(PTR + OFFSET + 11 * STRIDE_Y + Z##B));

#define LOAD_ROW_13(N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z) \
    LOAD_ROW_12(N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z)     \
    VEC_DATA_TYPE(DATA_TYPE, N0)                                       \
    BASENAME##C = VLOAD(N0)(0, (__global DATA_TYPE *)(PTR + OFFSET + 12 * STRIDE_Y + Z##C));

#define LOAD_ROW_14(N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z) \
    LOAD_ROW_13(N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z)     \
    VEC_DATA_TYPE(DATA_TYPE, N0)                                       \
    BASENAME##D = VLOAD(N0)(0, (__global DATA_TYPE *)(PTR + OFFSET + 13 * STRIDE_Y + Z##D));

#define LOAD_ROW_15(N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z) \
    LOAD_ROW_14(N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z)     \
    VEC_DATA_TYPE(DATA_TYPE, N0)                                       \
    BASENAME##E = VLOAD(N0)(0, (__global DATA_TYPE *)(PTR + OFFSET + 14 * STRIDE_Y + Z##E));

#define LOAD_ROW_16(N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z) \
    LOAD_ROW_15(N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z)     \
    VEC_DATA_TYPE(DATA_TYPE, N0)                                       \
    BASENAME##F = VLOAD(N0)(0, (__global DATA_TYPE *)(PTR + OFFSET + 15 * STRIDE_Y + Z##F));

// LOAD_ROW_n loads the rows 0..n-1 in variables BASENAME##0 to BASENAME##(n-1)
#define LOAD_BLOCK_STR(M0, N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z) LOAD_ROW_##M0(N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z)
/** Load Blocks of M0 consecutive rows and N0 consecutive columns when using Z offset as well
 * Supported cases M0=1,2,3..16. N0=1,2,3,4,8,16, for variables BASENAME[0..M0]
 * The data to load is expected to have consecutive names for each row, For e.g. For M0=3, and basename=c, the expected data is c0, c1 and c2.
 * The Z offset is expected to have consecutive names For e.g. For M0=3, and Z=zin, the expected z offsets are zin0, zin1 and zin2.
 */
#define LOAD_BLOCK(M0, N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z) LOAD_BLOCK_STR(M0, N0, DATA_TYPE, BASENAME, PTR, OFFSET, STRIDE_Y, Z)

#define CALCULATE_Z_OFFSET_1(M0, DATA_TYPE, Z, Y, HEIGHT_GEMM3D, DEPTH_GEMM3D, CROSS_PLANE_PAD, STRIDE_Y) \
    Z##0 = (0 + (DATA_TYPE)(Y * (DATA_TYPE)M0)) / (DATA_TYPE)HEIGHT_GEMM3D;                              \
    Z##0 = min((DATA_TYPE)(DEPTH_GEMM3D - 1), Z##0);                                                     \
    Z##0 *= (CROSS_PLANE_PAD * STRIDE_Y);

#define CALCULATE_Z_OFFSET_2(M0, DATA_TYPE, Z, Y, HEIGHT_GEMM3D, DEPTH_GEMM3D, CROSS_PLANE_PAD, STRIDE_Y) \
    CALCULATE_Z_OFFSET_1(M0, DATA_TYPE, Z, Y, HEIGHT_GEMM3D, DEPTH_GEMM3D, CROSS_PLANE_PAD, STRIDE_Y)     \
    Z##1 = (1 + (DATA_TYPE)(Y * (DATA_TYPE)M0)) / (DATA_TYPE)HEIGHT_GEMM3D;                              \
    Z##1 = min((DATA_TYPE)(DEPTH_GEMM3D - 1), Z##1);                                                     \
    Z##1 *= (CROSS_PLANE_PAD * STRIDE_Y);

#define CALCULATE_Z_OFFSET_3(M0, DATA_TYPE, Z, Y, HEIGHT_GEMM3D, DEPTH_GEMM3D, CROSS_PLANE_PAD, STRIDE_Y) \
    CALCULATE_Z_OFFSET_2(M0, DATA_TYPE, Z, Y, HEIGHT_GEMM3D, DEPTH_GEMM3D, CROSS_PLANE_PAD, STRIDE_Y)     \
    Z##2 = (2 + (DATA_TYPE)(Y * (DATA_TYPE)M0)) / (DATA_TYPE)HEIGHT_GEMM3D;                              \
    Z##2 = min((DATA_TYPE)(DEPTH_GEMM3D - 1), Z##2);                                                     \
    Z##2 *= (CROSS_PLANE_PAD * STRIDE_Y);

#define CALCULATE_Z_OFFSET_4(M0, DATA_TYPE, Z, Y, HEIGHT_GEMM3D, DEPTH_GEMM3D, CROSS_PLANE_PAD, STRIDE_Y) \
    CALCULATE_Z_OFFSET_3(M0, DATA_TYPE, Z, Y, HEIGHT_GEMM3D, DEPTH_GEMM3D, CROSS_PLANE_PAD, STRIDE_Y)     \
    Z##3 = (3 + (DATA_TYPE)(Y * (DATA_TYPE)M0)) / (DATA_TYPE)HEIGHT_GEMM3D;                              \
    Z##3 = min((DATA_TYPE)(DEPTH_GEMM3D - 1), Z##3);                                                     \
    Z##3 *= (CROSS_PLANE_PAD * STRIDE_Y);

#define CALCULATE_Z_OFFSET_5(M0, DATA_TYPE, Z, Y, HEIGHT_GEMM3D, DEPTH_GEMM3D, CROSS_PLANE_PAD, STRIDE_Y) \
    CALCULATE_Z_OFFSET_4(M0, DATA_TYPE, Z, Y, HEIGHT_GEMM3D, DEPTH_GEMM3D, CROSS_PLANE_PAD, STRIDE_Y)     \
    Z##4 = (4 + (DATA_TYPE)(Y * (DATA_TYPE)M0)) / (DATA_TYPE)HEIGHT_GEMM3D;                              \
    Z##4 = min((DATA_TYPE)(DEPTH_GEMM3D - 1), Z##4);                                                     \
    Z##4 *= (CROSS_PLANE_PAD * STRIDE_Y);

#define CALCULATE_Z_OFFSET_6(M0, DATA_TYPE, Z, Y, HEIGHT_GEMM3D, DEPTH_GEMM3D, CROSS_PLANE_PAD, STRIDE_Y) \
    CALCULATE_Z_OFFSET_5(M0, DATA_TYPE, Z, Y, HEIGHT_GEMM3D, DEPTH_GEMM3D, CROSS_PLANE_PAD, STRIDE_Y)     \
    Z##5 = (5 + (DATA_TYPE)(Y * (DATA_TYPE)M0)) / (DATA_TYPE)HEIGHT_GEMM3D;                              \
    Z##5 = min((DATA_TYPE)(DEPTH_GEMM3D - 1), Z##5);                                                     \
    Z##5 *= (CROSS_PLANE_PAD * STRIDE_Y);

#define CALCULATE_Z_OFFSET_7(M0, DATA_TYPE, Z, Y, HEIGHT_GEMM3D, DEPTH_GEMM3D, CROSS_PLANE_PAD, STRIDE_Y) \
    CALCULATE_Z_OFFSET_6(M0, DATA_TYPE, Z, Y, HEIGHT_GEMM3D, DEPTH_GEMM3D, CROSS_PLANE_PAD, STRIDE_Y)     \
    Z##6 = (6 + (DATA_TYPE)(Y * (DATA_TYPE)M0)) / (DATA_TYPE)HEIGHT_GEMM3D;                              \
    Z##6 = min((DATA_TYPE)(DEPTH_GEMM3D - 1), Z##6);                                                     \
    Z##6 *= (CROSS_PLANE_PAD * STRIDE_Y);

#define CALCULATE_Z_OFFSET_8(M0, DATA_TYPE, Z, Y, HEIGHT_GEMM3D, DEPTH_GEMM3D, CROSS_PLANE_PAD, STRIDE_Y) \
    CALCULATE_Z_OFFSET_7(M0, DATA_TYPE, Z, Y, HEIGHT_GEMM3D, DEPTH_GEMM3D, CROSS_PLANE_PAD, STRIDE_Y)     \
    Z##7 = (1 + (DATA_TYPE)(Y * (DATA_TYPE)M0)) / (DATA_TYPE)HEIGHT_GEMM3D;                              \
    Z##7 = min((DATA_TYPE)(DEPTH_GEMM3D - 1), Z##7);                                                     \
    Z##7 *= (CROSS_PLANE_PAD * STRIDE_Y);

// CALCULATE_Z_OFFSET_n calculates Z for Z##0 to Z##(n-1)
#define CALCULATE_Z_OFFSET_STR(M0, DATA_TYPE, Z, Y, HEIGHT_GEMM3D, DEPTH_GEMM3D, CROSS_PLANE_PAD, STRIDE_Y) CALCULATE_Z_OFFSET_##M0(M0, DATA_TYPE, Z, Y, HEIGHT_GEMM3D, DEPTH_GEMM3D, CROSS_PLANE_PAD, STRIDE_Y)
/** The Z offsets are expected to have consecutive names, For e.g. For M0=3, and Z=zin, the expected Z offsets are zin1, zin2, zin3.
 * Note for the REINTERPRET_INPUT_AS_3D case
 * Since we load a 2D input tile from a 3D tensor, we need to check when the plane changes across the z dimension
 * in order to take into account the presence of possible cross plane paddings
 *
 *  |                  |
 *  |      plane0      |
 *  |                  |
 *  |__________________|
 *  |******************|
 *  |  cross_plane_pad |
 *  |******************|
 *  |                  |
 *  |      plane1      |
 *  |                  |
 *  |__________________|
 */
#define CALCULATE_Z_OFFSET(M0, DATA_TYPE, Z, Y, HEIGHT_GEMM3D, DEPTH_GEMM3D, CROSS_PLANE_PAD, STRIDE_Y) CALCULATE_Z_OFFSET_STR(M0, DATA_TYPE, Z, Y, HEIGHT_GEMM3D, DEPTH_GEMM3D, CROSS_PLANE_PAD, STRIDE_Y)

#define STORE_ROW_1(N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z) \
    VSTORE(N0)                                                 \
    (BASENAME##0, 0, (__global DATA_TYPE *)(PTR + 0 * STRIDE_Y + Z##0));

#define STORE_ROW_2(N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z) \
    STORE_ROW_1(N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z)     \
    VSTORE(N0)                                                 \
    (BASENAME##1, 0, (__global DATA_TYPE *)(PTR + 1 * STRIDE_Y + Z##1));

#define STORE_ROW_3(N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z) \
    STORE_ROW_2(N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z)     \
    VSTORE(N0)                                                 \
    (BASENAME##2, 0, (__global DATA_TYPE *)(PTR + 2 * STRIDE_Y + Z##2));

#define STORE_ROW_4(N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z) \
    STORE_ROW_3(N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z)     \
    VSTORE(N0)                                                 \
    (BASENAME##3, 0, (__global DATA_TYPE *)(PTR + 3 * STRIDE_Y + Z##3));

#define STORE_ROW_5(N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z) \
    STORE_ROW_4(N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z)     \
    VSTORE(N0)                                                 \
    (BASENAME##4, 0, (__global DATA_TYPE *)(PTR + 4 * STRIDE_Y + Z##4));

#define STORE_ROW_6(N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z) \
    STORE_ROW_5(N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z)     \
    VSTORE(N0)                                                 \
    (BASENAME##5, 0, (__global DATA_TYPE *)(PTR + 5 * STRIDE_Y + Z##5));

#define STORE_ROW_7(N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z) \
    STORE_ROW_6(N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z)     \
    VSTORE(N0)                                                 \
    (BASENAME##6, 0, (__global DATA_TYPE *)(PTR + 6 * STRIDE_Y + Z##6));

#define STORE_ROW_8(N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z) \
    STORE_ROW_7(N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z)     \
    VSTORE(N0)                                                 \
    (BASENAME##7, 0, (__global DATA_TYPE *)(PTR + 7 * STRIDE_Y + Z##7));

#define STORE_ROW_9(N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z) \
    STORE_ROW_8(N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z)     \
    VSTORE(N0)                                                 \
    (BASENAME##8, 0, (__global DATA_TYPE *)(PTR + 8 * STRIDE_Y + Z##8));

#define STORE_ROW_10(N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z) \
    STORE_ROW_9(N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z)      \
    VSTORE(N0)                                                  \
    (BASENAME##9, 0, (__global DATA_TYPE *)(PTR + 9 * STRIDE_Y + Z##9));

#define STORE_ROW_11(N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z) \
    STORE_ROW_10(N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z)     \
    VSTORE(N0)                                                  \
    (BASENAME##A, 0, (__global DATA_TYPE *)(PTR + 10 * STRIDE_Y + Z##A));

#define STORE_ROW_12(N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z) \
    STORE_ROW_11(N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z)     \
    VSTORE(N0)                                                  \
    (BASENAME##B, 0, (__global DATA_TYPE *)(PTR + 11 * STRIDE_Y + Z##B));

#define STORE_ROW_13(N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z) \
    STORE_ROW_12(N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z)     \
    VSTORE(N0)                                                  \
    (BASENAME##C, 0, (__global DATA_TYPE *)(PTR + 12 * STRIDE_Y + Z##C));

#define STORE_ROW_14(N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z) \
    STORE_ROW_13(N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z)     \
    VSTORE(N0)                                                  \
    (BASENAME##D, 0, (__global DATA_TYPE *)(PTR + 13 * STRIDE_Y + Z##D));

#define STORE_ROW_15(N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z) \
    STORE_ROW_14(N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z)     \
    VSTORE(N0)                                                  \
    (BASENAME##E, 0, (__global DATA_TYPE *)(PTR + 14 * STRIDE_Y + Z##E));

#define STORE_ROW_16(N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z) \
    STORE_ROW_15(N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z)     \
    VSTORE(N0)                                                  \
    (BASENAME##F, 0, (__global DATA_TYPE *)(PTR + 15 * STRIDE_Y + Z##F));

// STORE_ROW_n stores the rows 0..n-1 from variables BASENAME##0 to BASENAME##(n-1)
#define STORE_BLOCK_STR(M0, N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z) STORE_ROW_##M0(N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z)
/** Store Blocks of M0 consecutive rows and N0 consecutive columns when using Z offset as well
* Supported cases M0=1,2,3..16. N0=2,3,4,8,16, for variables BASENAME[0..M]
 * The data to store is expected to have consecutive names for each row, For e.g. For M0=3, and basename=c, the expected data is c0, c1 and c2.
 * The Z offset is expected to have consecutive names For e.g. For M0=3, and Z=zin, the expected z offsets are zin0, zin1 and zin2.
 */
#define STORE_BLOCK(M0, N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z) STORE_BLOCK_STR(M0, N0, DATA_TYPE, BASENAME, PTR, STRIDE_Y, Z)

#define SCALE_ROW_1(DATA_TYPE, BASENAME, SCALE) \
    BASENAME##0 = BASENAME##0 * (DATA_TYPE)SCALE;

#define SCALE_ROW_2(DATA_TYPE, BASENAME, SCALE) \
    SCALE_ROW_1(DATA_TYPE, BASENAME, SCALE)     \
    BASENAME##1 = BASENAME##1 * (DATA_TYPE)SCALE;

#define SCALE_ROW_3(DATA_TYPE, BASENAME, SCALE) \
    SCALE_ROW_2(DATA_TYPE, BASENAME, SCALE)     \
    BASENAME##2 = BASENAME##2 * (DATA_TYPE)SCALE;

#define SCALE_ROW_4(DATA_TYPE, BASENAME, SCALE) \
    SCALE_ROW_3(DATA_TYPE, BASENAME, SCALE)     \
    BASENAME##3 = BASENAME##3 * (DATA_TYPE)SCALE;

#define SCALE_ROW_5(DATA_TYPE, BASENAME, SCALE) \
    SCALE_ROW_4(DATA_TYPE, BASENAME, SCALE)     \
    BASENAME##4 = BASENAME##4 * (DATA_TYPE)SCALE;

#define SCALE_ROW_6(DATA_TYPE, BASENAME, SCALE) \
    SCALE_ROW_5(DATA_TYPE, BASENAME, SCALE)     \
    BASENAME##5 = BASENAME##5 * (DATA_TYPE)SCALE;

#define SCALE_ROW_7(DATA_TYPE, BASENAME, SCALE) \
    SCALE_ROW_6(DATA_TYPE, BASENAME, SCALE)     \
    BASENAME##6 = BASENAME##6 * (DATA_TYPE)SCALE;

#define SCALE_ROW_8(DATA_TYPE, BASENAME, SCALE) \
    SCALE_ROW_7(DATA_TYPE, BASENAME, SCALE)     \
    BASENAME##7 = BASENAME##7 * (DATA_TYPE)SCALE;

#define SCALE_ROW_9(DATA_TYPE, BASENAME, SCALE) \
    SCALE_ROW_8(DATA_TYPE, BASENAME, SCALE)     \
    BASENAME##8 = BASENAME##8 * (DATA_TYPE)SCALE;

#define SCALE_ROW_10(DATA_TYPE, BASENAME, SCALE) \
    SCALE_ROW_9(DATA_TYPE, BASENAME, SCALE)      \
    BASENAME##9 = BASENAME##9 * (DATA_TYPE)SCALE;

#define SCALE_ROW_11(DATA_TYPE, BASENAME, SCALE) \
    SCALE_ROW_10(DATA_TYPE, BASENAME, SCALE)     \
    BASENAME##A = BASENAME##A * (DATA_TYPE)SCALE;

#define SCALE_ROW_12(DATA_TYPE, BASENAME, SCALE) \
    SCALE_ROW_11(DATA_TYPE, BASENAME, SCALE)     \
    BASENAME##B = BASENAME##B * (DATA_TYPE)SCALE;

#define SCALE_ROW_13(DATA_TYPE, BASENAME, SCALE) \
    SCALE_ROW_12(DATA_TYPE, BASENAME, SCALE)     \
    BASENAME##C = BASENAME##C * (DATA_TYPE)SCALE;

#define SCALE_ROW_14(DATA_TYPE, BASENAME, SCALE) \
    SCALE_ROW_13(DATA_TYPE, BASENAME, SCALE)     \
    BASENAME##D = BASENAME##D * (DATA_TYPE)SCALE;

#define SCALE_ROW_15(DATA_TYPE, BASENAME, SCALE) \
    SCALE_ROW_14(DATA_TYPE, BASENAME, SCALE)     \
    BASENAME##E = BASENAME##E * (DATA_TYPE)SCALE;

#define SCALE_ROW_16(DATA_TYPE, BASENAME, SCALE) \
    SCALE_ROW_15(DATA_TYPE, BASENAME, SCALE)     \
    BASENAME##F = BASENAME##F * (DATA_TYPE)SCALE;

// SCALE_ROW_n scales the variables BASENAME##0 to BASENAME##(n-1) by SCALE
#define SCALE_BLOCK_STR(N, DATA_TYPE, BASENAME, SCALE) SCALE_ROW_##N(DATA_TYPE, BASENAME, SCALE)
/** Scale elements stored in variables BASENAME##0 to BASENAME##(N-1) by SCALE
 * Supported cases N=1,2,3..16, for variables BASENAME[0..N]
 */
#define SCALE_BLOCK(N, DATA_TYPE, BASENAME, SCALE) SCALE_BLOCK_STR(N, DATA_TYPE, BASENAME, SCALE)