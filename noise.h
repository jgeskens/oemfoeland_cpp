//
//  noise.h
//  terraingen
//
//  Created by Jef Geskens on 16/05/14.
//  Copyright (c) 2014 Jef Geskens. All rights reserved.
//

#ifndef terraingen_noise_h
#define terraingen_noise_h

extern "C"
{
    
float fastabs(float x);
float noise(float xin, float yin, float zin);
float octaves(int octaves, float x, float y, float z, int ridged);
float triLerp(float x, float y, float z, float q000,
              float q001, float q010, float q011, float q100, float q101,
              float q110, float q111, float x1, float x2, float y1, float y2,
              float z1, float z2);
};

#endif
