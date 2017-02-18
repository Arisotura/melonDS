/*
    Copyright 2016-2017 StapleButter

    This file is part of melonDS.

    melonDS is free software: you can redistribute it and/or modify it under
    the terms of the GNU General Public License as published by the Free
    Software Foundation, either version 3 of the License, or (at your option)
    any later version.

    melonDS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with melonDS. If not, see http://www.gnu.org/licenses/.
*/

#ifndef GPU3D_H
#define GPU3D_H

namespace GPU3D
{

typedef struct
{
    s32 Position[4];
    u8 Color[3];

    bool Clipped;

    // final vertex attributes.
    // allows them to be reused in polygon strips.

    s32 FinalPosition[4];
    s32 FinalColor[3];

    bool ViewportTransformDone;

} Vertex;

typedef struct
{
    Vertex* Vertices[10];
    u32 NumVertices;

    u32 Attr;

    bool FacingView;

} Polygon;

extern s32 Viewport[4];

bool Init();
void DeInit();
void Reset();

void ExecuteCommand();

void Run(s32 cycles);
void CheckFIFOIRQ();
void CheckFIFODMA();

void VBlank();
u8* GetLine(int line);

u8 Read8(u32 addr);
u16 Read16(u32 addr);
u32 Read32(u32 addr);
void Write8(u32 addr, u8 val);
void Write16(u32 addr, u16 val);
void Write32(u32 addr, u32 val);

namespace SoftRenderer
{

bool Init();
void DeInit();
void Reset();

void RenderFrame(Vertex* vertices, Polygon* polygons, int npolys);
u8* GetLine(int line);

}

}

#endif
