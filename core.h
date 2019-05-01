//
//  core.h
//  Oemfoeland
//
//  Created by Jef Geskens on 24/05/14.
//  Copyright (c) 2014 Jef Geskens. All rights reserved.
//

#ifndef Oemfoeland_core_h
#define Oemfoeland_core_h

#include "terrain.h"

namespace Oemfoeland
{
    const double PIDIV180 = 3.14159265359 / 180.0;
    
    typedef struct vertex_s
    {
        GLfloat x;
        GLfloat y;
        GLfloat z;
    } vertex_t;
    
    typedef struct color_s
    {
        GLubyte r;
        GLubyte g;
        GLubyte b;
    } color_t;
    
    typedef struct uv_s
    {
        GLfloat u;
        GLfloat v;
    } uv_t;
    
    typedef struct completevertex_s
    {
        vertex_t vertex;
        color_t color;
        vertex_t normal;
        uv_t texcoord;
    } completevertex_t;
    
    typedef struct buffer_s
    {
        GLuint buffer_id;
        GLuint elem_count;
    } buffer_t;
    
    typedef struct chunk_s
    {
        vector3_t offset;
        uint8_t * data;
        buffer_t buffer;
        struct chunk_s * neighbors[8];
    } chunk_t;
    
    typedef struct chunkelem_s
    {
        chunk_t chunk;
        struct chunkelem_s * next;
    } chunkelem_t;
};

#endif
