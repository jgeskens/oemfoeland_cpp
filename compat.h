//
//  compat.h
//  Oemfoeland
//
//  Created by Jef Geskens on 25/05/14.
//  Copyright (c) 2014 Jef Geskens. All rights reserved.
//

#ifndef Oemfoeland_compat_h
#define Oemfoeland_compat_h

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#include <SDL/SDL.h>

#ifdef EMSCRIPTEN
#include <GL/gl.h>
#include <GL/glu.h>
#else
#ifndef __APPLE__
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif
#endif

#ifdef __linux__
#include "SDL/SDL_opengl.h"
#include "SDL/SDL_image.h"
#else
#include "SDL_opengl.h"
#include "SDL_image.h"
#endif

#endif
