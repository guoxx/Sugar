/***************************************************************************
# Copyright (c) 2015, NVIDIA CORPORATION. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#  * Neither the name of NVIDIA CORPORATION nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/

#ifndef _FALCOR_LTC_H_
#define _FALCOR_LTC_H_

// Make sure we get the macros like `_fn` and `_ref`
// TODO: just eliminate these since we know this is pure Slang.
#include "HostDeviceData.h"

__import SugarLights;
__import Helpers;

const float LUT_SIZE  = 64.0;
const float LUT_SCALE = (LUT_SIZE - 1.0)/LUT_SIZE;
const float LUT_BIAS  = 0.5/LUT_SIZE;

struct LtcLut
{
    Texture2D ltcMat;
    Texture2D ltcAmp;
    SamplerState ltcSamp;
};

struct Point5
{
    float3 points[5];
};

struct Point4
{
    float3 points[4];
};

#define ARRAY_TO_POINTS(N, array, pt)       \
for (int i = 0; i < N; ++i)                 \
{                                           \
    pt.points[i] = array[i];                \
}

#define POINTS_TO_ARRAY(N, pt, array)       \
for (int i = 0; i < N; ++i)                 \
{                                           \
    array[i] = pt.points[i];                \
}

int clipQuadToHorizon(_ref(Point5) pt5) {
    float3 L[5];
    POINTS_TO_ARRAY(5, pt5, L);

    /* Detect clipping config */
    int config = 0;
    if(L[0].z > 0.0) config += 1;
    if(L[1].z > 0.0) config += 2;
    if(L[2].z > 0.0) config += 4;
    if(L[3].z > 0.0) config += 8;

    int n = 0;

    if(config == 0) {
        // clip all
    } else if(config == 1) { // V1 clip V2 V3 V4
        n = 3;
        L[1] = -L[1].z * L[0] + L[0].z * L[1];
        L[2] = -L[3].z * L[0] + L[0].z * L[3];
    } else if(config == 2) { // V2 clip V1 V3 V4
        n = 3;
        L[0] = -L[0].z * L[1] + L[1].z * L[0];
        L[2] = -L[2].z * L[1] + L[1].z * L[2];
    } else if(config == 3) { // V1 V2 clip V3 V4
        n = 4;
        L[2] = -L[2].z * L[1] + L[1].z * L[2];
        L[3] = -L[3].z * L[0] + L[0].z * L[3];
    } else if(config == 4) { // V3 clip V1 V2 V4
        n = 3;
        L[0] = -L[3].z * L[2] + L[2].z * L[3];
        L[1] = -L[1].z * L[2] + L[2].z * L[1];
    } else if(config == 5) { // V1 V3 clip V2 V4, impossible
        n = 0;
    } else if(config == 6) { // V2 V3 clip V1 V4
        n = 4;
        L[0] = -L[0].z * L[1] + L[1].z * L[0];
        L[3] = -L[3].z * L[2] + L[2].z * L[3];
    } else if(config == 7) { // V1 V2 V3 clip V4
        n = 5;
        L[4] = -L[3].z * L[0] + L[0].z * L[3];
        L[3] = -L[3].z * L[2] + L[2].z * L[3];
    } else if(config == 8) { // V4 clip V1 V2 V3
        n = 3;
        L[0] = -L[0].z * L[3] + L[3].z * L[0];
        L[1] = -L[2].z * L[3] + L[3].z * L[2];
        L[2] =  L[3];
    } else if(config == 9) { // V1 V4 clip V2 V3
        n = 4;
        L[1] = -L[1].z * L[0] + L[0].z * L[1];
        L[2] = -L[2].z * L[3] + L[3].z * L[2];
    } else if(config == 10) { // V2 V4 clip V1 V3, impossible
        n = 0;
    } else if(config == 11) { // V1 V2 V4 clip V3
        n = 5;
        L[4] = L[3];
        L[3] = -L[2].z * L[3] + L[3].z * L[2];
        L[2] = -L[2].z * L[1] + L[1].z * L[2];
    } else if(config == 12) { // V3 V4 clip V1 V2
        n = 4;
        L[1] = -L[1].z * L[2] + L[2].z * L[1];
        L[0] = -L[0].z * L[3] + L[3].z * L[0];
    } else if(config == 13) { // V1 V3 V4 clip V2
        n = 5;
        L[4] = L[3];
        L[3] = L[2];
        L[2] = -L[1].z * L[2] + L[2].z * L[1];
        L[1] = -L[1].z * L[0] + L[0].z * L[1];
    } else if(config == 14) { // V2 V3 V4 clip V1
        n = 5;
        L[4] = -L[0].z * L[3] + L[3].z * L[0];
        L[0] = -L[0].z * L[1] + L[1].z * L[0];
    } else if(config == 15) { // V1 V2 V3 V4
        n = 4;
    }

    if(n == 3)
        L[3] = L[0];
    if(n == 4)
        L[4] = L[0];

    ARRAY_TO_POINTS(5, L, pt5);
    return n;
}

float2 ltcCoords(float roughness, float3 N, float3 V)
{
    float theta = acos(dot(N, V));
    float2 uv = float2(roughness, theta / (0.5 * M_PIf));
    uv = uv * LUT_SCALE + LUT_BIAS;
    return uv;
}

float3x3 ltcMatrix(LtcLut lut, float2 coord)
{
    const float4 t = lut.ltcMat.Sample(lut.ltcSamp, coord);
    float3x3 invM = float3x3(
        float3(  1,   0, t.w),
        float3(  0, t.z,   0),
        float3(t.y,   0, t.x)
    );

    return invM;
}

/* Integrate between two edges on a clamped cosine distribution */
float integrateEdge(float3 v1, float3 v2)
{
    float cosTheta = dot(v1, v2);
    cosTheta = clamp(cosTheta, -0.9999, 0.9999);

    const float theta = acos(cosTheta);
    /* For theta <= 0.001 `theta/sin(theta)` is approximated as 1.0 */
    const float res = cross(v1, v2).z*((theta > 0.001) ? theta/sin(theta) : 1.0);
    return res;
}

float ltcEvaluate(float3 N, float3 V, float3 P, float3x3 invM, Point4 pt4, bool twoSided)
{
    float3 points[4];
    POINTS_TO_ARRAY(4, pt4, points);

    /* Construct orthonormal basis around N */
    float3x3 basis;
    basis[0] = normalize(V - N * dot(N, V));
    basis[1] = normalize(cross(N, basis[0]));
    basis[2] = N;

    // rotate area light in local basis
    invM = mul(transpose(basis), invM);

    /* Allocate 5 vertices for polygon (one additional which may result from
     * clipping) */
    float3 L[5];
    L[0] = mul(points[0] - P, invM);
    L[1] = mul(points[1] - P, invM);
    L[2] = mul(points[2] - P, invM);
    L[3] = mul(points[3] - P, invM);

    /* Clip light quad so that the part behind the surface does not affect the
     * lighting of the point */

    Point5 pt5;
    L[4] = 0.0;
    ARRAY_TO_POINTS(5, L, pt5);
    int n = clipQuadToHorizon(pt5);
    POINTS_TO_ARRAY(5, pt5, L);

    if(n == 0)
        return 0.0;

    // project onto sphere
    L[0] = normalize(L[0]);
    L[1] = normalize(L[1]);
    L[2] = normalize(L[2]);
    L[3] = normalize(L[3]);
    L[4] = normalize(L[4]);

    /* Integrate over the clamped cosine distribution in the domain of the
     * transformed light polygon */
    float sum = integrateEdge(L[0], L[1])
              + integrateEdge(L[1], L[2])
              + integrateEdge(L[2], L[3]);
    if(n >= 4)
        sum += integrateEdge(L[3], L[4]);
    if(n == 5)
        sum += integrateEdge(L[4], L[0]);

    /* Negated due to winding order */
    sum = twoSided ? abs(sum) : max(0.0, -sum);

    sum /= 2*M_PIf;

    return sum;
}

float ltcEvalSpecularGGX(LtcLut lut, float3 N, float3 V, float roughness)
{
    //float2 uv = ltcCoords();
    return 0.0f;
}

float ltcEvalLambert(ShadingAttribs shAttr, LightAttribs lAttr)
{
    float3x3 identity = float3x3(
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 0.0, 1.0,
    );

    Point4 pt4;
    ARRAY_TO_POINTS(4, lAttr.points, pt4);
    return ltcEvaluate(shAttr.N, shAttr.E, shAttr.P, identity, pt4, false);
}

#endif	// _FALCOR_LTC_H_
