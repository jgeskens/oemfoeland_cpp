//
//  terrain.c
//  terraingen
//
//  Created by Jef Geskens on 16/05/14.
//  Copyright (c) 2014 Jef Geskens. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "terrain.h"
#include "noise.h"

const int bx = 16;
const int by = 128;
const int bz = 16;
const int ebx = bx / 4 + 1;
const int eby = by / 4 + 1;
const int ebz = bz / 4 + 1;
const int BLOCKS = bx * by * bz;
const int EBLOCKS = ebx * eby * ebz;

float expensiveNoise1[EBLOCKS];
float expensiveNoise2[EBLOCKS];
float expensiveNoise3[EBLOCKS];
float expensiveNoise4[EBLOCKS];
float expensiveNoise5[EBLOCKS];
float expensiveNoise6[EBLOCKS];

vector3_t Vector3(int x, int y, int z)
{
    vector3_t v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

void fetchRelativeCoordinates(int index, vector3_t * outPosition)
{
    int blockNumber = index;
    int bxy = bx * by;
    int ax = blockNumber % bx;
    int planeNumber = blockNumber / bx;
    int ay = planeNumber % by;
    int az = blockNumber / bxy;
    outPosition->x = ax;
    outPosition->y = ay;
    outPosition->z = az;
}

void fetchCoordinates(int index, vector3_t * outPosition, vector3_t internalOffset)
{
    int ox = internalOffset.x, oy = internalOffset.y, oz = internalOffset.z;
    fetchRelativeCoordinates(index, outPosition);
    outPosition->x += ox;
    outPosition->y += oy;
    outPosition->z += oz;
}

int fetchRelativeOffset(int x, int y, int z)
{
    if (x < 0 || y < 0 || z < 0 || x >= bx || y >= by || z >= bz)
        return -1;
    return x + y * bx + z * bx * by;
}

int ei(int x, int y, int z)
{
    return x + y * ebx + z * ebx * eby;
}

void generateExpensiveNoise(int num_octaves, float mulx, float muly, float mulz, float offset, float dest[], vector3_t internalOffset, int ridged)
{
    int x, y, z;
    float ix, iy, iz;
    int ox = internalOffset.x;
    int oy = internalOffset.y;
    int oz = internalOffset.z;
    
    for (x = 0; x < ebx; x++)
        for (y = 0; y < eby; y++)
            for (z = 0; z < ebz; z++)
            {
                ix = (float)(x * 4 + ox);
                iy = (float)(y * 4 + oy);
                iz = (float)(z * 4 + oz);
                dest[ei(x, y, z)] = (float)octaves(num_octaves, ix * mulx, iy * muly + offset, iz * mulz, ridged);
            }
}

float interpolateExpensiveNoise(int x, int y, int z, float expNoise[], vector3_t internalOffset)
{
    x -= internalOffset.x; y -= internalOffset.y; z -= internalOffset.z;
    int ex = x / 4;
    int ey = y / 4;
    int ez = z / 4;
    float x4 = (float)((x / 4) * 4);
    float y4 = (float)((y / 4) * 4);
    float z4 = (float)((z / 4) * 4);
    float q000 = expNoise[ei(ex, ey, ez)];
    float q010 = expNoise[ei(ex, ey, ez+1)];
    float q001 = expNoise[ei(ex, ey+1, ez)];
    float q011 = expNoise[ei(ex, ey+1, ez+1)];
    float q100 = expNoise[ei(ex+1, ey, ez)];
    float q110 = expNoise[ei(ex+1, ey, ez+1)];
    float q101 = expNoise[ei(ex+1, ey+1, ez)];
    float q111 = expNoise[ei(ex+1, ey+1, ez+1)];
    return triLerp((float)x, (float)y, (float)z,
                    q000, q001, q010, q011, q100, q101, q110, q111,
                    x4, x4 + 4.0f,
                    y4, y4 + 4.0f,
                    z4, z4 + 4.0f);
}

void generate(uint8_t * data, vector3_t internalOffset, float scale)
{
    float min = 200.0f;
    vector3_t pos = Vector3(0, 0, 0);
    int x, y, z, h, treeIndex = 0;
    float n, n2, n3, n4, n5;
    vector3_t trees[10];
    
    generateExpensiveNoise(6, 0.0025f * scale, 0.005f, 0.0025f * scale, 0, expensiveNoise1, internalOffset, 0);
    //generateExpensiveNoise(6, 0.025f * scale, 0.05f, 0.025f * scale, 0, expensiveNoise1, internalOffset, 0);
    generateExpensiveNoise(3, 0.25f * scale, 0.25f, 0.25f * scale, 0, expensiveNoise2, internalOffset, 0);
    generateExpensiveNoise(3, 0.00025f * scale, 0.0005f, 0.00025f * scale, 0, expensiveNoise3, internalOffset, 0);
    generateExpensiveNoise(1, 0.01f * scale, 0.01f, 0.01f * scale, 0, expensiveNoise4, internalOffset, 1);
    generateExpensiveNoise(1, 0.01f * scale, 0.01f, 0.01f * scale, 100, expensiveNoise6, internalOffset, 1);
    generateExpensiveNoise(1, 1.0f * scale, 0.01f, 1.0f * scale, 100, expensiveNoise5, internalOffset, 1);
    
    for (int i = 0; i < BLOCKS; i++)
    {
        fetchCoordinates(i, &pos, internalOffset);
        x = pos.x;
        y = pos.y;
        z = pos.z;
        
        if (y < 2)
        {
            data[i * 2] = 1;
            continue;
        }
        
        data[i*2] = 0;
        
        n = interpolateExpensiveNoise(x, y, z, expensiveNoise4, internalOffset) * 32.0f;
        n2 = interpolateExpensiveNoise(x, y, z, expensiveNoise6, internalOffset) * 32.0f;
        n4 = interpolateExpensiveNoise(x, y, z, expensiveNoise2, internalOffset);
        n3 = interpolateExpensiveNoise(x, 0, z, expensiveNoise1, internalOffset) * 32.0f;
        
        h = (int) (n3 + 40);
        
        float noise = interpolateExpensiveNoise(x, y, z, expensiveNoise1, internalOffset) + 1.5f;
        
        n5 = interpolateExpensiveNoise(x, 0, z, expensiveNoise3, internalOffset);
        float intensity = (n5 + 1.5f) * 24.0f;
        if (intensity > 48.0f)
            intensity = 48.0f;
        if (noise < min)
            min = noise;
        
        int features = noise >= (float)y / intensity;
        
        
        if (features)
        {
            if (y < 48 && y > 42)
                data[i*2] = 2;
            else
                data[i*2] = 1;
        }
        else
        {
            if (y < 44)
            {
                data[i*2] = 6;
            }
        }
        
        float cave_factor = (8.0f - y) * 3.5f / 8.0f;
        if (y >= 8)
            cave_factor = 0.0f;
        
        if (n + cave_factor < 3.5f + n4 * 4.0f &&
            n2 + cave_factor < 3.5f + n4 * 4.0f && noise >= (y + 8.0f) / intensity)
        {
            data[i * 2] = 0;
        }
        
//        if (n5 > 0.4f)
//            data[i * 2] = 1;
        
//        if (n < 3 && y > 30 && y > h && y < h + 4)
//            data[i * 2] = (y < 44 ? 6 : 0);
        
        if (n + cave_factor < 3.5f + n4 * 4.0f && n2 + cave_factor < 3.5f + n4 * 4.0f)
            data[i * 2] = 0;
    }
    
    vector3_t origin = Vector3(0, 0, 0);

    // Grass generator
    for (x = 0; x < bx; x++)
    {
        for (z = 0; z < bz; z++)
        {
            for (y = by-16; y >= 48; y--)
            {
                int i = fetchRelativeOffset(x, y, z);
                if (data[i*2] > 0 && data[i*2] != 6)
                {
                    data[i*2] = 3;
                    if (interpolateExpensiveNoise(x, y, z, expensiveNoise5, origin) > 0.92 && treeIndex < 10)
                    {
                        trees[treeIndex] = Vector3(x, y, z);
                        data[i*2] = 4;
                        treeIndex++;
                    }
                    break;
                }
            }
        }
    }
    
}


