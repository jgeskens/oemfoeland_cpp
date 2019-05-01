//
//  MatrixMath.h
//  Oemfoeland
//
//  Created by Jef Geskens on 26/11/13.
//  Copyright (c) 2013 Jef Geskens. All rights reserved.
//

#ifndef __Oemfoeland__MatrixMath__
#define __Oemfoeland__MatrixMath__

#include <math.h>
#include "compat.h"
#include <stdlib.h>
#include <string.h>

namespace Oemfoeland {
    class MatrixMath
    {
    public:
        static void
        sincos (double a, double * s, double * c)
        {
            *s = sin(a);
            *c = cos(a);
        };
        
        static void
        multiply(GLfloat *m, const GLfloat *n)
        {
            GLfloat tmp[16];
            const GLfloat *row, *column;
            div_t d;
            int i, j;
            
            for (i = 0; i < 16; i++) {
                tmp[i] = 0;
                d = div(i, 4);
                row = n + d.quot * 4;
                column = m + d.rem;
                for (j = 0; j < 4; j++)
                    tmp[i] += row[j] * column[j * 4];
            }
            memcpy(m, &tmp, sizeof tmp);
        };
        
        static void
        rotate(GLfloat *m, GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
        {
            double s, c;
            
            sincos(angle, &s, &c);
            GLfloat r[16] = {
                x * x * (1 - c) + c,     y * x * (1 - c) + z * s, x * z * (1 - c) - y * s, 0,
                x * y * (1 - c) - z * s, y * y * (1 - c) + c,     y * z * (1 - c) + x * s, 0,
                x * z * (1 - c) + y * s, y * z * (1 - c) - x * s, z * z * (1 - c) + c,     0,
                0, 0, 0, 1
            };
            
            multiply(m, r);
        };
        
        static void
        translate(GLfloat *m, GLfloat x, GLfloat y, GLfloat z)
        {
            GLfloat t[16] = { 1, 0, 0, 0,  0, 1, 0, 0,  0, 0, 1, 0,  x, y, z, 1 };
            
            multiply(m, t);
        };
        
        static void
        identity(GLfloat *m)
        {
            GLfloat t[16] = {
                1.0, 0.0, 0.0, 0.0,
                0.0, 1.0, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                0.0, 0.0, 0.0, 1.0,
            };
            
            memcpy(m, t, sizeof(t));
        };
        
        static void
        transpose(GLfloat *m)
        {
            GLfloat t[16] = {
                m[0], m[4], m[8],  m[12],
                m[1], m[5], m[9],  m[13],
                m[2], m[6], m[10], m[14],
                m[3], m[7], m[11], m[15]};
            
            memcpy(m, t, sizeof(t));
        };
        
        static void
        invert(GLfloat *m)
        {
            GLfloat t[16];
            identity(t);
            
            // Extract and invert the translation part 't'. The inverse of a
            // translation matrix can be calculated by negating the translation
            // coordinates.
            t[12] = -m[12]; t[13] = -m[13]; t[14] = -m[14];
            
            // Invert the rotation part 'r'. The inverse of a rotation matrix is
            // equal to its transpose.
            m[12] = m[13] = m[14] = 0;
            transpose(m);
            
            // inv(m) = inv(r) * inv(t)
            multiply(m, t);
        };
        
        void perspective(GLfloat *m, GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar)
        {
            GLfloat tmp[16];
            identity(tmp);
            
            double sine, cosine, cotangent, deltaZ;
            GLfloat radians = fovy / 2 * M_PI / 180;
            
            deltaZ = zFar - zNear;
            sincos(radians, &sine, &cosine);
            
            if ((deltaZ == 0) || (sine == 0) || (aspect == 0))
                return;
            
            cotangent = cosine / sine;
            
            tmp[0] = cotangent / aspect;
            tmp[5] = cotangent;
            tmp[10] = -(zFar + zNear) / deltaZ;
            tmp[11] = -1;
            tmp[14] = -2 * zNear * zFar / deltaZ;
            tmp[15] = 0;
            
            memcpy(m, tmp, sizeof(tmp));
        };
        
        
    };
}

#endif /* defined(__Oemfoeland__MatrixMath__) */
