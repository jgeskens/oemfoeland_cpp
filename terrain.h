//
//  terrain.h
//  terraingen
//
//  Created by Jef Geskens on 16/05/14.
//  Copyright (c) 2014 Jef Geskens. All rights reserved.
//

#ifndef terraingen_terrain_h
#define terraingen_terrain_h

#include <stdint.h>

extern "C"
{
    
typedef struct vector3_s
{
    int x;
    int y;
    int z;
} vector3_t;

vector3_t Vector3(int x, int y, int z);

extern const int BLOCKS;
extern const int bx;
extern const int by;
extern const int bz;

void generate(uint8_t * data, vector3_t internalOffset, float scale);
int fetchRelativeOffset(int x, int y, int z);
void fetchRelativeCoordinates(int index, vector3_t * outPosition);
void fetchCoordinates(int index, vector3_t * outPosition, vector3_t internalOffset);

    
};

#endif
