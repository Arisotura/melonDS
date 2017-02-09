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

#include <stdio.h>
#include <string.h>
#include "NDS.h"
#include "GPU.h"
#include "FIFO.h"


namespace GPU3D
{

const u32 CmdNumParams[256] =
{
    // 0x00
    0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // 0x10
    1, 0, 1, 1, 1, 0, 16, 12, 16, 12, 9, 3, 3,
    0, 0, 0,
    // 0x20
    1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0,
    // 0x30
    1, 1, 1, 1, 32,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // 0x40
    1, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // 0x50
    1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // 0x60
    1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // 0x70
    3, 2, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // 0x80+
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const s32 CmdNumCycles[256] =
{
    // 0x00
    0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // 0x10
    1, 17, 36, 17, 36, 19, 34, 30, 35, 31, 28, 22, 22,
    0, 0, 0,
    // 0x20
    1, 9, 1, 9, 8, 8, 8, 8, 8, 1, 1, 1,
    0, 0, 0, 0,
    // 0x30
    4, 4, 6, 1, 32,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // 0x40
    1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // 0x50
    392,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // 0x60
    1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // 0x70
    103, 9, 5,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // 0x80+
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

typedef struct
{
    u8 Command;
    u32 Param;

} CmdFIFOEntry;

FIFO<CmdFIFOEntry>* CmdFIFO;
FIFO<CmdFIFOEntry>* CmdPIPE;

u32 NumCommands, CurCommand, ParamCount, TotalParams;

u32 GXStat;

u32 ExecParams[32];
u32 ExecParamCount;
s32 CycleCount;


u32 MatrixMode;

s32 ProjMatrix[16];
s32 PosMatrix[16];
s32 VecMatrix[16];
s32 TexMatrix[16];

s32 ProjMatrixStack[16];
s32 PosMatrixStack[31][16];
s32 ProjMatrixStackPointer;
s32 PosMatrixStackPointer;


void MatrixLoadIdentity(s32* m)
{
    m[0] = 0x1000; m[1] = 0;      m[2] = 0;       m[3] = 0;
    m[4] = 0;      m[5] = 0x1000; m[6] = 0;       m[7] = 0;
    m[8] = 0;      m[9] = 0;      m[10] = 0x1000; m[11] = 0;
    m[12] = 0;     m[13] = 0;     m[14] = 0;      m[15] = 0x1000;
}

void MatrixLoad4x4(s32* m, s32* s)
{
    memcpy(m, s, 16*4);
}

void MatrixLoad4x3(s32* m, s32* s)
{
    m[0] = s[0];  m[1] = s[1];  m[2] = s[2];    m[3] = 0;
    m[4] = s[3];  m[5] = s[4];  m[6] = s[5];    m[7] = 0;
    m[8] = s[6];  m[9] = s[7];  m[10] = s[8];   m[11] = 0;
    m[12] = s[9]; m[13] = s[10]; m[14] = s[11]; m[15] = 0x1000;
}

void MatrixMult4x4(s32* m, s32* s)
{
    s32 tmp[16];
    memcpy(tmp, m, 16*4);

    // m = s*m
    m[0] = (s[0]*tmp[0] + s[1]*tmp[4] + s[2]*tmp[8] + s[3]*tmp[12]) >> 12;
    m[1] = (s[0]*tmp[1] + s[1]*tmp[5] + s[2]*tmp[9] + s[3]*tmp[13]) >> 12;
    m[2] = (s[0]*tmp[2] + s[1]*tmp[6] + s[2]*tmp[10] + s[3]*tmp[14]) >> 12;
    m[3] = (s[0]*tmp[3] + s[1]*tmp[7] + s[2]*tmp[11] + s[3]*tmp[15]) >> 12;

    m[4] = (s[4]*tmp[0] + s[5]*tmp[4] + s[6]*tmp[8] + s[7]*tmp[12]) >> 12;
    m[5] = (s[4]*tmp[1] + s[5]*tmp[5] + s[6]*tmp[9] + s[7]*tmp[13]) >> 12;
    m[6] = (s[4]*tmp[2] + s[5]*tmp[6] + s[6]*tmp[10] + s[7]*tmp[14]) >> 12;
    m[7] = (s[4]*tmp[3] + s[5]*tmp[7] + s[6]*tmp[11] + s[7]*tmp[15]) >> 12;

    m[8] = (s[8]*tmp[0] + s[9]*tmp[4] + s[10]*tmp[8] + s[11]*tmp[12]) >> 12;
    m[9] = (s[8]*tmp[1] + s[9]*tmp[5] + s[10]*tmp[9] + s[11]*tmp[13]) >> 12;
    m[10] = (s[8]*tmp[2] + s[9]*tmp[6] + s[10]*tmp[10] + s[11]*tmp[14]) >> 12;
    m[11] = (s[8]*tmp[3] + s[9]*tmp[7] + s[10]*tmp[11] + s[11]*tmp[15]) >> 12;

    m[12] = (s[12]*tmp[0] + s[13]*tmp[4] + s[14]*tmp[8] + s[15]*tmp[12]) >> 12;
    m[13] = (s[12]*tmp[1] + s[13]*tmp[5] + s[14]*tmp[9] + s[15]*tmp[13]) >> 12;
    m[14] = (s[12]*tmp[2] + s[13]*tmp[6] + s[14]*tmp[10] + s[15]*tmp[14]) >> 12;
    m[15] = (s[12]*tmp[3] + s[13]*tmp[7] + s[14]*tmp[11] + s[15]*tmp[15]) >> 12;
}

void MatrixMult4x3(s32* m, s32* s)
{
    s32 tmp[16];
    memcpy(tmp, m, 16*4);

    // m = s*m
    m[0] = (s[0]*tmp[0] + s[1]*tmp[4] + s[2]*tmp[8]) >> 12;
    m[1] = (s[0]*tmp[1] + s[1]*tmp[5] + s[2]*tmp[9]) >> 12;
    m[2] = (s[0]*tmp[2] + s[1]*tmp[6] + s[2]*tmp[10]) >> 12;
    m[3] = (s[0]*tmp[3] + s[1]*tmp[7] + s[2]*tmp[11]) >> 12;

    m[4] = (s[3]*tmp[0] + s[4]*tmp[4] + s[5]*tmp[8]) >> 12;
    m[5] = (s[3]*tmp[1] + s[4]*tmp[5] + s[5]*tmp[9]) >> 12;
    m[6] = (s[3]*tmp[2] + s[4]*tmp[6] + s[5]*tmp[10]) >> 12;
    m[7] = (s[3]*tmp[3] + s[4]*tmp[7] + s[5]*tmp[11]) >> 12;

    m[8] = (s[6]*tmp[0] + s[7]*tmp[4] + s[8]*tmp[8]) >> 12;
    m[9] = (s[6]*tmp[1] + s[7]*tmp[5] + s[8]*tmp[9]) >> 12;
    m[10] = (s[6]*tmp[2] + s[7]*tmp[6] + s[8]*tmp[10]) >> 12;
    m[11] = (s[6]*tmp[3] + s[7]*tmp[7] + s[8]*tmp[11]) >> 12;

    m[12] = (s[9]*tmp[0] + s[10]*tmp[4] + s[11]*tmp[8] + 0x1000*tmp[12]) >> 12;
    m[13] = (s[9]*tmp[1] + s[10]*tmp[5] + s[11]*tmp[9] + 0x1000*tmp[13]) >> 12;
    m[14] = (s[9]*tmp[2] + s[10]*tmp[6] + s[11]*tmp[10] + 0x1000*tmp[14]) >> 12;
    m[15] = (s[9]*tmp[3] + s[10]*tmp[7] + s[11]*tmp[11] + 0x1000*tmp[15]) >> 12;
}

void MatrixMult3x3(s32* m, s32* s)
{
    s32 tmp[12];
    memcpy(tmp, m, 12*4);

    // m = s*m
    m[0] = (s[0]*tmp[0] + s[1]*tmp[4] + s[2]*tmp[8]) >> 12;
    m[1] = (s[0]*tmp[1] + s[1]*tmp[5] + s[2]*tmp[9]) >> 12;
    m[2] = (s[0]*tmp[2] + s[1]*tmp[6] + s[2]*tmp[10]) >> 12;
    m[3] = (s[0]*tmp[3] + s[1]*tmp[7] + s[2]*tmp[11]) >> 12;

    m[4] = (s[3]*tmp[0] + s[4]*tmp[4] + s[5]*tmp[8]) >> 12;
    m[5] = (s[3]*tmp[1] + s[4]*tmp[5] + s[5]*tmp[9]) >> 12;
    m[6] = (s[3]*tmp[2] + s[4]*tmp[6] + s[5]*tmp[10]) >> 12;
    m[7] = (s[3]*tmp[3] + s[4]*tmp[7] + s[5]*tmp[11]) >> 12;

    m[8] = (s[6]*tmp[0] + s[7]*tmp[4] + s[8]*tmp[8]) >> 12;
    m[9] = (s[6]*tmp[1] + s[7]*tmp[5] + s[8]*tmp[9]) >> 12;
    m[10] = (s[6]*tmp[2] + s[7]*tmp[6] + s[8]*tmp[10]) >> 12;
    m[11] = (s[6]*tmp[3] + s[7]*tmp[7] + s[8]*tmp[11]) >> 12;
}

void MatrixScale(s32* m, s32* s)
{
    m[0] = (s[0]*m[0]) >> 12;
    m[1] = (s[0]*m[1]) >> 12;
    m[2] = (s[0]*m[2]) >> 12;
    m[3] = (s[0]*m[3]) >> 12;

    m[4] = (s[1]*m[4]) >> 12;
    m[5] = (s[1]*m[5]) >> 12;
    m[6] = (s[1]*m[6]) >> 12;
    m[7] = (s[1]*m[7]) >> 12;

    m[8] = (s[2]*m[8]) >> 12;
    m[9] = (s[2]*m[9]) >> 12;
    m[10] = (s[2]*m[10]) >> 12;
    m[11] = (s[2]*m[11]) >> 12;
}

void MatrixTranslate(s32* m, s32* s)
{
    m[12] += (s[0]*m[0] + s[1]*m[4] + s[2]*m[8]) >> 12;
    m[13] += (s[0]*m[1] + s[1]*m[5] + s[2]*m[9]) >> 12;
    m[14] += (s[0]*m[2] + s[1]*m[6] + s[2]*m[10]) >> 12;
}


bool Init()
{
    CmdFIFO = new FIFO<CmdFIFOEntry>(256);
    CmdPIPE = new FIFO<CmdFIFOEntry>(4);

    return true;
}

void DeInit()
{
    delete CmdFIFO;
    delete CmdPIPE;
}

void Reset()
{
    CmdFIFO->Clear();
    CmdPIPE->Clear();

    NumCommands = 0;
    CurCommand = 0;
    ParamCount = 0;
    TotalParams = 0;

    GXStat = 0;

    memset(ExecParams, 0, 32*4);
    ExecParamCount = 0;
    CycleCount = 0;


    MatrixMode = 0;

    MatrixLoadIdentity(ProjMatrix);
    MatrixLoadIdentity(PosMatrix);
    MatrixLoadIdentity(VecMatrix);
    MatrixLoadIdentity(TexMatrix);

    memset(ProjMatrixStack, 0, 16*4);
    memset(PosMatrixStack, 0, 31 * 16*4);
    ProjMatrixStackPointer = 0;
    PosMatrixStackPointer = 0;
}


void CmdFIFOWrite(CmdFIFOEntry& entry)
{
    if (CmdFIFO->IsEmpty() && !CmdPIPE->IsFull())
    {
        CmdPIPE->Write(entry);
    }
    else
    {
        if (CmdFIFO->IsFull())
        {
            printf("!!! GX FIFO FULL\n");
            return;
        }

        CmdFIFO->Write(entry);
    }
}

CmdFIFOEntry CmdFIFORead()
{
    CmdFIFOEntry ret = CmdPIPE->Read();

    if (CmdPIPE->Level() <= 2)
    {
        if (!CmdFIFO->IsEmpty())
            CmdPIPE->Write(CmdFIFO->Read());
        if (!CmdFIFO->IsEmpty())
            CmdPIPE->Write(CmdFIFO->Read());

        CheckFIFODMA();
        CheckFIFOIRQ();
    }

    return ret;
}




void ExecuteCommand()
{
    CmdFIFOEntry entry = CmdFIFORead();

    ExecParams[ExecParamCount] = entry.Param;
    ExecParamCount++;

    if (ExecParamCount >= CmdNumParams[entry.Command])
    {
        CycleCount += CmdNumCycles[entry.Command];
        ExecParamCount = 0;

        GXStat &= ~(1<<14);

        //printf("3D CMD %02X\n", entry.Command);

        switch (entry.Command)
        {
        case 0x10: // matrix mode
            MatrixMode = ExecParams[0] & 0x3;
            break;

        case 0x11: // push matrix
            if (MatrixMode == 0)
            {
                if (ProjMatrixStackPointer > 0)
                {
                    printf("!! PROJ MATRIX STACK OVERFLOW\n");
                    GXStat |= (1<<15);
                    break;
                }

                memcpy(ProjMatrixStack, ProjMatrix, 16*4);
                ProjMatrixStackPointer++;
                GXStat |= (1<<14);
            }
            else if (MatrixMode == 3)
            {
                printf("!! CAN'T PUSH TEXTURE MATRIX\n");
                GXStat |= (1<<15); // CHECKME
            }
            else
            {
                if (PosMatrixStackPointer > 30)
                {
                    printf("!! POS MATRIX STACK OVERFLOW\n");
                    GXStat |= (1<<15);
                    break;
                }

                memcpy(PosMatrixStack[PosMatrixStackPointer], PosMatrix, 16*4);
                PosMatrixStackPointer++;
                GXStat |= (1<<14);
            }
            break;

        case 0x12: // pop matrix
            if (MatrixMode == 0)
            {
                if (ProjMatrixStackPointer <= 0)
                {
                    printf("!! PROJ MATRIX STACK UNDERFLOW\n");
                    GXStat |= (1<<15);
                    break;
                }

                ProjMatrixStackPointer--;
                memcpy(ProjMatrix, ProjMatrixStack, 16*4);
                GXStat |= (1<<14);
            }
            else if (MatrixMode == 3)
            {
                printf("!! CAN'T POP TEXTURE MATRIX\n");
                GXStat |= (1<<15); // CHECKME
            }
            else
            {
                s32 offset = (s32)(ExecParams[0] << 26) >> 26;
                PosMatrixStackPointer -= offset;

                if (PosMatrixStackPointer < 0 || PosMatrixStackPointer > 30)
                {
                    printf("!! POS MATRIX STACK UNDER/OVERFLOW %d\n", PosMatrixStackPointer);
                    PosMatrixStackPointer += offset;
                    GXStat |= (1<<15);
                    break;
                }

                memcpy(PosMatrix, PosMatrixStack[PosMatrixStackPointer], 16*4);
                GXStat |= (1<<14);
            }
            break;

        case 0x13: // store matrix
            if (MatrixMode == 0)
            {
                memcpy(ProjMatrixStack, ProjMatrix, 16*4);
            }
            else if (MatrixMode == 3)
            {
                printf("!! CAN'T STORE TEXTURE MATRIX\n");
                GXStat |= (1<<15); // CHECKME
            }
            else
            {
                u32 addr = ExecParams[0] & 0x1F;
                if (addr > 30)
                {
                    printf("!! POS MATRIX STORE ADDR 31\n");
                    GXStat |= (1<<15);
                    break;
                }

                memcpy(PosMatrixStack[addr], PosMatrix, 16*4);
            }
            break;

        case 0x14: // restore matrix
            if (MatrixMode == 0)
            {
                memcpy(ProjMatrix, ProjMatrixStack, 16*4);
            }
            else if (MatrixMode == 3)
            {
                printf("!! CAN'T RESTORE TEXTURE MATRIX\n");
                GXStat |= (1<<15); // CHECKME
            }
            else
            {
                u32 addr = ExecParams[0] & 0x1F;
                if (addr > 30)
                {
                    printf("!! POS MATRIX STORE ADDR 31\n");
                    GXStat |= (1<<15);
                    break;
                }

                memcpy(PosMatrix, PosMatrixStack[addr], 16*4);
            }
            break;

        case 0x15: // identity
            if (MatrixMode == 0)
                MatrixLoadIdentity(ProjMatrix);
            else if (MatrixMode == 3)
                MatrixLoadIdentity(TexMatrix);
            else
            {
                MatrixLoadIdentity(PosMatrix);
                if (MatrixMode == 2)
                    MatrixLoadIdentity(VecMatrix);
            }
            break;

        case 0x16: // load 4x4
            if (MatrixMode == 0)
                MatrixLoad4x4(ProjMatrix, (s32*)ExecParams);
            else if (MatrixMode == 3)
                MatrixLoad4x4(TexMatrix, (s32*)ExecParams);
            else
            {
                MatrixLoad4x4(PosMatrix, (s32*)ExecParams);
                if (MatrixMode == 2)
                    MatrixLoad4x4(VecMatrix, (s32*)ExecParams);
            }
            break;

        case 0x17: // load 4x3
            if (MatrixMode == 0)
                MatrixLoad4x3(ProjMatrix, (s32*)ExecParams);
            else if (MatrixMode == 3)
                MatrixLoad4x3(TexMatrix, (s32*)ExecParams);
            else
            {
                MatrixLoad4x3(PosMatrix, (s32*)ExecParams);
                if (MatrixMode == 2)
                    MatrixLoad4x3(VecMatrix, (s32*)ExecParams);
            }
            break;

        case 0x18: // mult 4x4
            if (MatrixMode == 0)
                MatrixMult4x4(ProjMatrix, (s32*)ExecParams);
            else if (MatrixMode == 3)
                MatrixMult4x4(TexMatrix, (s32*)ExecParams);
            else
            {
                MatrixMult4x4(PosMatrix, (s32*)ExecParams);
                if (MatrixMode == 2)
                {
                    MatrixMult4x4(VecMatrix, (s32*)ExecParams);
                    CycleCount += 30;
                }
            }
            break;

        case 0x19: // mult 4x3
            if (MatrixMode == 0)
                MatrixMult4x3(ProjMatrix, (s32*)ExecParams);
            else if (MatrixMode == 3)
                MatrixMult4x3(TexMatrix, (s32*)ExecParams);
            else
            {
                MatrixMult4x3(PosMatrix, (s32*)ExecParams);
                if (MatrixMode == 2)
                {
                    MatrixMult4x3(VecMatrix, (s32*)ExecParams);
                    CycleCount += 30;
                }
            }
            break;

        case 0x1A: // mult 3x3
            if (MatrixMode == 0)
                MatrixMult3x3(ProjMatrix, (s32*)ExecParams);
            else if (MatrixMode == 3)
                MatrixMult3x3(TexMatrix, (s32*)ExecParams);
            else
            {
                MatrixMult3x3(PosMatrix, (s32*)ExecParams);
                if (MatrixMode == 2)
                {
                    MatrixMult3x3(VecMatrix, (s32*)ExecParams);
                    CycleCount += 30;
                }
            }
            break;

        case 0x1B: // scale
            if (MatrixMode == 0)
                MatrixScale(ProjMatrix, (s32*)ExecParams);
            else if (MatrixMode == 3)
                MatrixScale(TexMatrix, (s32*)ExecParams);
            else
                MatrixScale(PosMatrix, (s32*)ExecParams);
            break;

        case 0x1C: // translate
            if (MatrixMode == 0)
                MatrixTranslate(ProjMatrix, (s32*)ExecParams);
            else if (MatrixMode == 3)
                MatrixTranslate(TexMatrix, (s32*)ExecParams);
            else
            {
                MatrixTranslate(PosMatrix, (s32*)ExecParams);
                if (MatrixMode == 2)
                    MatrixTranslate(VecMatrix, (s32*)ExecParams);
            }
            break;

        case 0x21:
            // TODO: more cycles if lights are enabled
            break;

        case 0x50:
            // TODO: make it happen upon VBlank, not right now
            break;
        }
    }
}

void Run(s32 cycles)
{
    if (CycleCount <= 0)
    {
        while (CycleCount <= 0 && !CmdPIPE->IsEmpty())
            ExecuteCommand();

        if (CmdPIPE->IsEmpty())
            CycleCount = 0;
    }
    else
        CycleCount -= cycles;
}


void CheckFIFOIRQ()
{
    bool irq = false;
    switch (GXStat >> 30)
    {
    case 1: irq = (CmdFIFO->Level() < 128); break;
    case 2: irq = CmdFIFO->IsEmpty(); break;
    }

    if (irq) NDS::TriggerIRQ(0, NDS::IRQ_GXFIFO);
}

void CheckFIFODMA()
{
    if (CmdFIFO->Level() < 128)
        NDS::CheckDMAs(0, 0x07);
}


u8 Read8(u32 addr)
{
    return 0;
}

u16 Read16(u32 addr)
{
    return 0;
}

u32 Read32(u32 addr)
{
    switch (addr)
    {
    case 0x04000320:
        return 46; // TODO, eventually

    case 0x04000600:
        {
            u32 fifolevel = CmdFIFO->Level();

            return GXStat |
                   ((PosMatrixStackPointer & 0x1F) << 8) |
                   ((ProjMatrixStackPointer & 0x1) << 13) |
                   (fifolevel << 16) |
                   (fifolevel < 128 ? (1<<25) : 0) |
                   (fifolevel == 0  ? (1<<26) : 0) |
                   (CycleCount > 0  ? (1<<27) : 0);
        }
    }

    if (addr >= 0x04000640 && addr < 0x04000680)
    {
        printf("!! CLIPMTX READ\n");
        return 0;
    }
    if (addr >= 0x04000680 && addr < 0x040006A4)
    {
        printf("!! VECMTX READ\n");
        return 0;
    }

    return 0;
}

void Write8(u32 addr, u8 val)
{
    //
}

void Write16(u32 addr, u16 val)
{
    //
}

void Write32(u32 addr, u32 val)
{
    switch (addr)
    {
    case 0x04000600:
        if (val & 0x8000) GXStat &= ~0x8000;
        val &= 0xC0000000;
        GXStat &= 0x3FFFFFFF;
        GXStat |= val;
        return;
    }

    if (addr >= 0x04000400 && addr < 0x04000440)
    {
        if (NumCommands == 0)
        {
            NumCommands = 4;
            CurCommand = val;
            ParamCount = 0;
            TotalParams = CmdNumParams[CurCommand & 0xFF];
        }
        else
            ParamCount++;

        while (ParamCount == TotalParams)
        {
            CmdFIFOEntry entry;
            entry.Command = CurCommand & 0xFF;
            entry.Param = val;
            CmdFIFOWrite(entry);

            CurCommand >>= 8;
            NumCommands--;
            if (NumCommands == 0) break;

            ParamCount = 0;
            TotalParams = CmdNumParams[CurCommand & 0xFF];
        }

        return;
    }

    if (addr >= 0x04000440 && addr < 0x040005CC)
    {
        CmdFIFOEntry entry;
        entry.Command = (addr & 0x1FC) >> 2;
        entry.Param = val;
        CmdFIFOWrite(entry);
        return;
    }
}

}

