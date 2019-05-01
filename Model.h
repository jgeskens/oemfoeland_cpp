//
//  Model.h
//  Oemfoeland
//
//  Created by Jef Geskens on 30/11/13.
//  Copyright (c) 2013 Jef Geskens. All rights reserved.
//

#ifndef __Oemfoeland__Model__
#define __Oemfoeland__Model__

namespace Oemfoeland
{
    class Vector3f {
    public:
        double x, y, z;
        Vector3f(const Vector3f &copy){this->x = copy.x; this->y = copy.y; this->z = copy.z;};
        Vector3f(double x, double y, double z){this->x = x; this->y = y; this->z = z;};
        Vector3f(){this->x = 0; this->y = 0; this->z = 0;};
        virtual ~Vector3f(){};
        
        Vector3f& operator=(const Vector3f &copy){this->x = copy.x; this->y = copy.y; this->z = copy.z; return *this;};
        void set(const Vector3f &copy){this->x = copy.x; this->y = copy.y; this->z = copy.z;};
    };
    
    class Model
    {
    public:
        static const int XMIN = 1;
        static const int XMAX = 2;
        static const int YMIN = 4;
        static const int YMAX = 8;
        static const int ZMIN = 16;
        static const int ZMAX = 32;
        
    private:
        int mode;
        int model_id;
        
    };
}

#endif /* defined(__Oemfoeland__Model__) */
