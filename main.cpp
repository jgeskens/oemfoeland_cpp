//
//  main.cpp
//  Oemfoeland
//
//  Created by Jef Geskens on 26/11/13.
//  Copyright (c) 2013 Jef Geskens. All rights reserved.
//

#define GL_GLEXT_PROTOTYPES

#include "compat.h"

#include <stdio.h>
#include <deque>

#include "MatrixMath.h"
#include "core.h"
#include "terrain.h"
#include "noise.h"

#include <math.h>

#define DISTANCE 11
#define SPRITE_TEX_W 1.0f/16.0f

using namespace Oemfoeland;
using namespace std;

buffer_t vbo;
GLuint terrain_texture;

double speed = 0.4;

GLuint makeTexture(const char * image_filename)
{
    GLuint texture_id;
    SDL_Surface * terrain_sf = IMG_Load(image_filename);
    
    GLuint format = GL_RGBA;
    if (terrain_sf->format->Rmask == 0x00FF0000)
        format = GL_BGRA;
    
    glGenTextures(1, &texture_id);
    
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, terrain_sf->w, terrain_sf->h, 0, format, GL_UNSIGNED_BYTE, terrain_sf->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    return texture_id;
}

buffer_t makeBuffer(completevertex_t vertices[], GLuint count)
{
    buffer_t buffer;
    buffer.elem_count = count;
    glGenBuffers(1, &buffer.buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer.buffer_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(completevertex_t) * count, vertices, GL_STATIC_READ);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return buffer;
}

void enableBufferDraw()
{
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
}

void bindBuffer(GLuint buffer_id)
{
    static completevertex_t dummy_vertex;
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    glColorPointer(3, GL_UNSIGNED_BYTE, sizeof(completevertex_t), (GLvoid*)((long)&dummy_vertex.color - (long)&dummy_vertex));
    glTexCoordPointer(2, GL_FLOAT, sizeof(completevertex_t), (GLvoid*)((long)&dummy_vertex.texcoord - (long)&dummy_vertex));
    glNormalPointer(GL_FLOAT, sizeof(completevertex_t), (GLvoid*)((long)&dummy_vertex.normal - (long)&dummy_vertex));
    glVertexPointer(3, GL_FLOAT, sizeof(completevertex_t), 0);
}

void disableBufferDraw()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

double xrot = 0.0;
double yrot = 0.0;
vertex_t pos = {0.0f, 64.0f, 10.0f};


completevertex_t buffer[36] = {
    {{0.0f, 0.0f, 0.0f}, {255, 255, 255}, {-1.0f, 0.0f, 0.0f}, {0.0f, SPRITE_TEX_W}},
    {{0.0f, 1.0f, 1.0f}, {255, 255, 255}, {-1.0f, 0.0f, 0.0f}, {SPRITE_TEX_W, 0.0f}},
    {{0.0f, 1.0f, 0.0f}, {255, 255, 255}, {-1.0f, 0.0f, 0.0f}, {SPRITE_TEX_W, SPRITE_TEX_W}},
    {{0.0f, 1.0f, 1.0f}, {255, 255, 255}, {-1.0f, 0.0f, 0.0f}, {SPRITE_TEX_W, 0.0f}},
    {{0.0f, 0.0f, 0.0f}, {255, 255, 255}, {-1.0f, 0.0f, 0.0f}, {0.0f, SPRITE_TEX_W}},
    {{0.0f, 0.0f, 1.0f}, {255, 255, 255}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    
    {{1.0f, 0.0f, 0.0f}, {255, 255, 255}, {1.0f, 0.0f, 0.0f}, {0.0f, SPRITE_TEX_W}},
    {{1.0f, 1.0f, 0.0f}, {255, 255, 255}, {1.0f, 0.0f, 0.0f}, {SPRITE_TEX_W, SPRITE_TEX_W}},
    {{1.0f, 1.0f, 1.0f}, {255, 255, 255}, {1.0f, 0.0f, 0.0f}, {SPRITE_TEX_W, 0.0f}},
    {{1.0f, 1.0f, 1.0f}, {255, 255, 255}, {1.0f, 0.0f, 0.0f}, {SPRITE_TEX_W, 0.0f}},
    {{1.0f, 0.0f, 1.0f}, {255, 255, 255}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{1.0f, 0.0f, 0.0f}, {255, 255, 255}, {1.0f, 0.0f, 0.0f}, {0.0f, SPRITE_TEX_W}},
    
    {{0.0f, 0.0f, 0.0f}, {255, 255, 255}, {0.0f, -1.0f, 0.0f}, {0.0f, SPRITE_TEX_W}},
    {{1.0f, 0.0f, 0.0f}, {255, 255, 255}, {0.0f, -1.0f, 0.0f}, {SPRITE_TEX_W, SPRITE_TEX_W}},
    {{1.0f, 0.0f, 1.0f}, {255, 255, 255}, {0.0f, -1.0f, 0.0f}, {SPRITE_TEX_W, 0.0f}},
    {{0.0f, 0.0f, 0.0f}, {255, 255, 255}, {0.0f, -1.0f, 0.0f}, {0.0f, SPRITE_TEX_W}},
    {{1.0f, 0.0f, 1.0f}, {255, 255, 255}, {0.0f, -1.0f, 0.0f}, {SPRITE_TEX_W, 0.0f}},
    {{0.0f, 0.0f, 1.0f}, {255, 255, 255}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
    
    {{0.0f, 1.0f, 0.0f}, {255, 255, 255}, {0.0f, 1.0f, 0.0f}, {0.0f, SPRITE_TEX_W}},
    {{1.0f, 1.0f, 1.0f}, {255, 255, 255}, {0.0f, 1.0f, 0.0f}, {SPRITE_TEX_W, 0.0f}},
    {{1.0f, 1.0f, 0.0f}, {255, 255, 255}, {0.0f, 1.0f, 0.0f}, {SPRITE_TEX_W, SPRITE_TEX_W}},
    {{1.0f, 1.0f, 1.0f}, {255, 255, 255}, {0.0f, 1.0f, 0.0f}, {SPRITE_TEX_W, 0.0f}},
    {{0.0f, 1.0f, 0.0f}, {255, 255, 255}, {0.0f, 1.0f, 0.0f}, {0.0f, SPRITE_TEX_W}},
    {{0.0f, 1.0f, 1.0f}, {255, 255, 255}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    
    {{0.0f, 0.0f, 0.0f}, {255, 255, 255}, {0.0f, 0.0f, -1.0f}, {0.0f, SPRITE_TEX_W}},
    {{1.0f, 1.0f, 0.0f}, {255, 255, 255}, {0.0f, 0.0f, -1.0f}, {SPRITE_TEX_W, 0.0f}},
    {{1.0f, 0.0f, 0.0f}, {255, 255, 255}, {0.0f, 0.0f, -1.0f}, {SPRITE_TEX_W, SPRITE_TEX_W}},
    {{0.0f, 1.0f, 0.0f}, {255, 255, 255}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
    {{1.0f, 1.0f, 0.0f}, {255, 255, 255}, {0.0f, 0.0f, -1.0f}, {SPRITE_TEX_W, 0.0f}},
    {{0.0f, 0.0f, 0.0f}, {255, 255, 255}, {0.0f, 0.0f, -1.0f}, {0.0f, SPRITE_TEX_W}},
    
    {{0.0f, 0.0f, 1.0f}, {255, 255, 255}, {0.0f, 0.0f, 1.0f}, {0.0f, SPRITE_TEX_W}},
    {{1.0f, 0.0f, 1.0f}, {255, 255, 255}, {0.0f, 0.0f, 1.0f}, {SPRITE_TEX_W, SPRITE_TEX_W}},
    {{1.0f, 1.0f, 1.0f}, {255, 255, 255}, {0.0f, 0.0f, 1.0f}, {SPRITE_TEX_W, 0.0f}},
    {{1.0f, 1.0f, 1.0f}, {255, 255, 255}, {0.0f, 0.0f, 1.0f}, {SPRITE_TEX_W, 0.0f}},
    {{0.0f, 1.0f, 1.0f}, {255, 255, 255}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
    {{0.0f, 0.0f, 1.0f}, {255, 255, 255}, {0.0f, 0.0f, 1.0f}, {0.0f, SPRITE_TEX_W}},
};

// For some reason the makeTerrainBuffer does not like this code
// Answer: stack overflow
completevertex_t terrainmesh[128*256*36] = {{{0,0,0},{0,0,0},{0,0,0},{0,0}}};

buffer_t buffers[DISTANCE*DISTANCE];
chunkelem_t chunks = {{{0, 0, 0}, NULL, {0, 0}, {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}}, NULL};

chunk_t makeChunk(vector3_t offset)
{
    chunk_t chunk;
    chunk.offset = offset;
    chunk.data = new uint8_t[BLOCKS * 2];
    generate(chunk.data, offset, 1.0f);
    return chunk;
}

void chunkBuildMesh(chunk_t * chunk)
{
    int count = 0;
    vector3_t p = Vector3(0, 0, 0);
    vector3_t rp = Vector3(0, 0, 0);
    vector3_t offset = chunk->offset;
    int dx = 0, dy = 0, dz = 0;
    
    uint8_t * terrain = chunk->data;
    
    for (int i = 0; i < BLOCKS; i++)
    {
        fetchCoordinates(i, &p, offset);
        fetchRelativeCoordinates(i, &rp);
        if (terrain[i*2] > 0)
        {
            for (int f = 0; f < 6; f++)
            {
                dx = f < 2 				? f * 2 - 1 : 0;
                dy = f >= 2 && f < 4 	? f * 2 - 5 : 0;
                dz = f >= 4 			? f * 2 - 9 : 0;
                // calculate neighbor index
                int ni = fetchRelativeOffset(rp.x + dx, rp.y + dy, rp.z + dz);
                if (ni >= 0 && terrain[ni*2] == 0)
                {
                    for (int j = f * 6; j < f * 6 + 6; j++)
                    {
                        completevertex_t vert = buffer[j];
                        vert.vertex.x += p.x;
                        vert.vertex.y += p.y;
                        vert.vertex.z += p.z;
                        vert.texcoord.u += (float)(terrain[i*2]) * SPRITE_TEX_W;
                        vert.texcoord.v += 1.0f/256.0f;
                        terrainmesh[count] = vert;
                        count += 1;
                    }
                }
            }
        }
    }
    chunk->buffer = makeBuffer(terrainmesh, count);
}

void loop()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    glClearColor(0.8, 0.9, 1.0, 1.0);
    
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
	gluPerspective(60.0, 800.0 / 600.0, 0.25, 1000.0);
    
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // camera
    glRotatef(yrot, 1.0, 0.0, 0.0);
    glRotatef(xrot, 0.0, 1.0, 0.0);
    glTranslatef(-pos.x, -pos.y, -pos.z);
    // end camera
    
    glPushMatrix();
    
    //glColor3d(1.0, 1.0, 1.0);
    
    glBindTexture(GL_TEXTURE_2D, terrain_texture);
    
    
    
    enableBufferDraw();
    
    for (chunkelem_t * current = chunks.next; current != NULL; current = current->next)
    {
        bindBuffer(current->chunk.buffer.buffer_id);
        glDrawArrays(GL_TRIANGLES, 0, current->chunk.buffer.elem_count);
    }
    bindBuffer(vbo.buffer_id);
    glDrawArrays(GL_TRIANGLES, 0, vbo.elem_count);
    
    
    
    disableBufferDraw();
    
    
    
    glPopMatrix();
    
    
    SDL_GL_SwapBuffers();
    
    static bool keys[512] = {false};
    
    SDL_Event event;
    bool ignoreMouseMotion = false;
    while (SDL_PollEvent(&event)){
        if (event.type == SDL_KEYDOWN){
            if (event.key.keysym.sym == SDLK_ESCAPE)
                exit(0);
            if ((GLuint)event.key.keysym.sym < 512)
                keys[event.key.keysym.sym] = true;
        }
        else if (event.type == SDL_KEYUP)
        {
            if ((GLuint)event.key.keysym.sym < 512)
                keys[event.key.keysym.sym] = false;
        }
        else if (event.type == SDL_MOUSEMOTION)
        {
            if (!ignoreMouseMotion)
            {
                xrot += event.motion.xrel * 0.2;
                yrot += event.motion.yrel * 0.2;
                SDL_WarpMouse(400, 300);
                SDL_ShowCursor(0);
                ignoreMouseMotion = true;
            }
        }
        else if (event.type == SDL_QUIT){
            exit(0);
        }
    }
    
    if (keys[SDLK_w])
    {
        pos.x += speed * sin(xrot * PIDIV180);
        pos.z += speed * -cos(xrot * PIDIV180);
    }
    
    if (keys[SDLK_s])
    {
        pos.x += speed * -sin(xrot * PIDIV180);
        pos.z += speed * cos(xrot * PIDIV180);
    }
    
    if (keys[SDLK_a])
    {
        pos.x += speed * -cos(xrot * PIDIV180);
        pos.z += speed * -sin(xrot * PIDIV180);
    }
    
    if (keys[SDLK_d])
    {
        pos.x += speed * cos(xrot * PIDIV180);
        pos.z += speed * sin(xrot * PIDIV180);
    }
    
    if (keys[SDLK_SPACE])
    {
        pos.y += speed;
    }
    
    if (keys[SDLK_c])
    {
        pos.y += -speed;
    }
}

int main(int argc, char * argv[])
{
    printf("%f\n", noise(0, 0, 0));
    
    SDL_Init(SDL_INIT_VIDEO);
    SDL_WM_SetCaption("Oemfoeland", NULL);
    
    
    SDL_Surface *screen = SDL_SetVideoMode(800, 600, 32, SDL_OPENGL);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
    terrain_texture = makeTexture("terrain.png");
    
    //glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    
#ifndef EMSCRIPTEN
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat light_pos[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat light_amb[4] = {0.25f, 0.25f, 0.25f, 1.0f};
    GLfloat light_dif[4] = {0.5f, 0.5f, 0.5f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_dif);
#endif
    
    glViewport(0, 0, 800, 600);
    
    vbo = makeBuffer(buffer, 36);
    
    SDL_WarpMouse(400, 300);
    SDL_Event event;
    while (SDL_PollEvent(&event));
    
    chunkelem_t * last = &chunks;
    
    for (int j = 0; j < DISTANCE; j++)
    {
        for (int i = 0; i < DISTANCE; i++)
        {
            last->next = new chunkelem_t();
            last->next->chunk = makeChunk(Vector3((i - DISTANCE/2) * 16, 0, (j - DISTANCE/2) * 16));
            chunkBuildMesh(&(last->next->chunk));
            last->next->next = NULL;
            last = last->next;
            //buffers[i + j * DISTANCE] = makeTerrainBuffer(Vector3((i - DISTANCE/2) * 16, 0, (j - DISTANCE/2) * 16));
        }
        printf("%d of %d\n", j+1, DISTANCE);
    }
    
    //vbo = makeTerrainBuffer();
    
    
    atexit(SDL_Quit);
    
#ifdef EMSCRIPTEN
    emscripten_set_main_loop(loop, 0, 0);
#else
    while (1)
    {
        loop();
        SDL_Delay(1);
    }
#endif
    
    return 0;
}
