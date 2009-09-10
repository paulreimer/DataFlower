/*
 * Copyright 1993-2009 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */

// Defines for Sobel filter kernel
//*****************************************************************************
#define BLOCKDIMX 16
#define BLOCKDIMY 16

// RGB Sobel gradient intensity filter kernel
// Uses 32 bit GMEM reads into a block of LMEM padded for apron of radius = 1 (3x3 neighbor op)
// Gradient intensity is from RSS combination of H and V gradient components
// R, G and B gradient intensities are treated separately then combined with linear weighting
//
// Implementation below is equivalent to linear 2D convolutions for H and V compoonents with:
//	    Convo Coefs for Horizontal component {1,0,-1,   2,0,-2,  1,0,-1}
//	    Convo Coefs for Vertical component   {-1,-2,-1,  0,0,0,  1,2,1};
//*****************************************************************************
__kernel void ckSobel(__global uchar4* uc4Source, __global uchar* uiDest,
                      __local uchar4 uc4LocalData[(BLOCKDIMX + 2) * (BLOCKDIMY + 2)],
                      unsigned int uiWidth, unsigned int uiHeight, float fThresh)
{
    // Get parent image x and y pixel coordinates from global ID, and compute offset into parent GMEM data
    int globalPosX = get_global_id(0);
    int globalPosY = (int)get_global_id(1) - 1;  // shifted up 1 for Sobel radius
    int iGlobalOffset = globalPosY * get_global_size(0) + globalPosX;

    // Compute LMEM Block pitch and offset of current pixel within work group LMEM block
    int iLocalPitch = get_local_size(0) + 2;
    int iLocalOffset = (get_local_id(1) * iLocalPitch) + get_local_id(0) + 1;

    // Main read of GMEM data into LMEM
    if((globalPosY > -1) && (globalPosY < uiHeight) && (globalPosX < uiWidth))
    {
        uc4LocalData[iLocalOffset] = uc4Source[iGlobalOffset];
    }
    else
    {
        uc4LocalData[iLocalOffset] = (uchar4)0;
    }

    // If in the bottom 2 rows of workgroup LMEM block
    if (get_local_id(1) < 2)
    {
        // Increase local offset by 1 workgroup LMEM block height
        // to read in top rows from the next block region down
        iLocalOffset += (get_local_size(1) * iLocalPitch);

        // If in the image boundaries
        if (((globalPosY + get_local_size(1)) < uiHeight) && (globalPosX < uiWidth))
        {
            // Read in top rows from the next block region down
            uc4LocalData[iLocalOffset] = uc4Source[iGlobalOffset + (get_local_size(1) * get_global_size(0))];
        }
        else
        {
            uc4LocalData[iLocalOffset] = (uchar4)0;
        }
    }

    // If in the rightmost column of the workgroup,
    if (get_local_id(0) == (get_local_size(0) - 1))
    {
        // set local offset to read data from the next region over
        iLocalOffset = get_local_id(1) * iLocalPitch;

        // If in the image and not at the leftmost workgroup
        if ((globalPosY > -1) && (globalPosY < uiHeight) && (get_group_id(0) > 0))
        {
            // Read data into the LMEM apron from the GMEM at the left edge of the next block region over
            uc4LocalData[iLocalOffset] = uc4Source[globalPosY * get_global_size(0) + get_group_id(0) * get_local_size(0) - 1];
        }
        else
        {
            uc4LocalData[iLocalOffset] = (uchar4)0;
        }

        // If in the bottom 2 rows of workgroup LMEM block
        if (get_local_id(1) < 2)
        {
            // Increase local offset by 1 workgroup LMEM block height
            // to read in top rows from the next block region down
            iLocalOffset += (get_local_size(1) * iLocalPitch);

            // If the next block down isn't off the image and not at the leftmost workgroup
            if (((globalPosY + get_local_size(1)) < uiHeight) && (get_group_id(0) > 0))
            {
                // read in from GMEM (reaching down 1 workgroup LMEM block height and left 1 pixel)
                uc4LocalData[iLocalOffset] = uc4Source[(globalPosY + get_local_size(1)) * get_global_size(0) + get_group_id(0) * get_local_size(0) - 1];
            }
            else
            {
                uc4LocalData[iLocalOffset] = (uchar4)0;
            }
        }
    }

    // If in the leftmost column of the workgroup
    if (get_local_id(0) == 0)
    {
        // set local offset
        iLocalOffset = ((get_local_id(1) + 1) * iLocalPitch) - 1;

        if ((globalPosY > -1) && (globalPosY < uiHeight) && (((get_group_id(0) + 1) * get_local_size(0)) < uiWidth))
        {
            // read in from GMEM (reaching left 1 pixel)
            uc4LocalData[iLocalOffset] = uc4Source[globalPosY * get_global_size(0) + (get_group_id(0) + 1) * get_local_size(0)];
        }
        else
        {
            uc4LocalData[iLocalOffset] = (uchar4)0;
        }

        // Read bottom 2 rows of workgroup LMEM block
        if (get_local_id(1) < 2)
        {
            // increase local offset by 1 workgroup LMEM block height
            iLocalOffset += (get_local_size(1) * iLocalPitch);

            if (((globalPosY + get_local_size(1)) < uiHeight) && (((get_group_id(0) + 1) * get_local_size(0)) < uiWidth))
            {
                // read in from GMEM (reaching down 1 workgroup LMEM block height and left 1 pixel)
                uc4LocalData[iLocalOffset] = uc4Source[(globalPosY + get_local_size(1)) * get_global_size(0) + (get_group_id(0) + 1) * get_local_size(0)];
            }
            else
            {
                uc4LocalData[iLocalOffset] = (uchar4)0;
            }
        }
    }

    // Synchronize the read into LMEM
    barrier(CLK_LOCAL_MEM_FENCE);

    // Compute and write out to GMEM (if pixel plus apron is within bounds)
    if((globalPosX < uiWidth) && (globalPosY > -1) && (globalPosY < uiHeight))
    {
        // Init summation registers to zero
        float fTemp = 0.0f;
        float4 fHSum = (float4)0.0f;
        float4 fVSum = (float4)0.0f;

        // set local offset and kernel offset
        iLocalOffset = (get_local_id(1) * iLocalPitch) + get_local_id(0);

        // NW
		fHSum.x += (float)uc4LocalData[iLocalOffset].x;    // horizontal gradient of Red
		fHSum.y += (float)uc4LocalData[iLocalOffset].y;    // horizontal gradient of Green
		fHSum.z += (float)uc4LocalData[iLocalOffset].z;    // horizontal gradient of Blue
        fVSum.x -= (float)uc4LocalData[iLocalOffset].x;    // vertical gradient of Red
		fVSum.y -= (float)uc4LocalData[iLocalOffset].y;    // vertical gradient of Green
		fVSum.z -= (float)uc4LocalData[iLocalOffset++].z;  // vertical gradient of Blue

        // N
		fVSum.x -= (float)(uc4LocalData[iLocalOffset].x << 1);  // vertical gradient of Red
		fVSum.y -= (float)(uc4LocalData[iLocalOffset].y << 1);  // vertical gradient of Green
		fVSum.z -= (float)(uc4LocalData[iLocalOffset++].z << 1);// vertical gradient of Blue

        // NE
		fHSum.x -= (float)uc4LocalData[iLocalOffset].x;    // horizontal gradient of Red
		fHSum.y -= (float)uc4LocalData[iLocalOffset].y;    // horizontal gradient of Green
		fHSum.z -= (float)uc4LocalData[iLocalOffset].z;    // horizontal gradient of Blue
		fVSum.x -= (float)uc4LocalData[iLocalOffset].x;    // vertical gradient of Red
		fVSum.y -= (float)uc4LocalData[iLocalOffset].y;    // vertical gradient of Green
		fVSum.z -= (float)uc4LocalData[iLocalOffset].z;    // vertical gradient of Blue

        // increment LMEM block to next row, and unwind increments
        iLocalOffset += (iLocalPitch - 2);

        // W
		fHSum.x += (float)(uc4LocalData[iLocalOffset].x << 1);  // vertical gradient of Red
		fHSum.y += (float)(uc4LocalData[iLocalOffset].y << 1);  // vertical gradient of Green
		fHSum.z += (float)(uc4LocalData[iLocalOffset++].z << 1);// vertical gradient of Blue

        // C
        iLocalOffset++;

        // E
		fHSum.x -= (float)(uc4LocalData[iLocalOffset].x << 1);  // vertical gradient of Red
		fHSum.y -= (float)(uc4LocalData[iLocalOffset].y << 1);  // vertical gradient of Green
		fHSum.z -= (float)(uc4LocalData[iLocalOffset].z << 1);  // vertical gradient of Blue

        // increment LMEM block to next row, and unwind increments
        iLocalOffset += (iLocalPitch - 2);

        // SW
		fHSum.x += (float)uc4LocalData[iLocalOffset].x;    // horizontal gradient of Red
		fHSum.y += (float)uc4LocalData[iLocalOffset].y;    // horizontal gradient of Green
		fHSum.z += (float)uc4LocalData[iLocalOffset].z;    // horizontal gradient of Blue
		fVSum.x += (float)uc4LocalData[iLocalOffset].x;    // vertical gradient of Red
		fVSum.y += (float)uc4LocalData[iLocalOffset].y;    // vertical gradient of Green
		fVSum.z += (float)uc4LocalData[iLocalOffset++].z;  // vertical gradient of Blue

        // S
		fVSum.x += (float)(uc4LocalData[iLocalOffset].x << 1);  // vertical gradient of Red
		fVSum.y += (float)(uc4LocalData[iLocalOffset].y << 1);  // vertical gradient of Green
		fVSum.z += (float)(uc4LocalData[iLocalOffset++].z << 1);// vertical gradient of Blue

        // SE
		fHSum.x -= (float)uc4LocalData[iLocalOffset].x;    // horizontal gradient of Red
		fHSum.y -= (float)uc4LocalData[iLocalOffset].y;    // horizontal gradient of Green
		fHSum.z -= (float)uc4LocalData[iLocalOffset].z;    // horizontal gradient of Blue
		fVSum.x += (float)uc4LocalData[iLocalOffset].x;    // vertical gradient of Red
		fVSum.y += (float)uc4LocalData[iLocalOffset].y;    // vertical gradient of Green
		fVSum.z += (float)uc4LocalData[iLocalOffset++].z;  // vertical gradient of Blue

		// Weighted combination of Root-Sum-Square per-color-band H & V gradients for each of RGB
		fTemp =  0.30f * sqrt((fHSum.x * fHSum.x) + (fVSum.x * fVSum.x));
		fTemp += 0.55f * sqrt((fHSum.y * fHSum.y) + (fVSum.y * fVSum.y));
		fTemp += 0.15f * sqrt((fHSum.z * fHSum.z) + (fVSum.z * fVSum.z));

        // threshold and clamp
        if (fTemp < fThresh)
        {
            fTemp = 0.0f;
        }
        else if (fTemp > 255.0f)
        {
            fTemp = 255.0f;
        }
/*
        // pack into a monochrome uint
        unsigned int uiPackedPix = 0x000000FF & (unsigned int)fTemp;
        uiPackedPix |= 0x0000FF00 & (((unsigned int)fTemp) << 8);
        uiPackedPix |= 0x00FF0000 & (((unsigned int)fTemp) << 16);
*/
        // Write back out to GMEM (shift offset back down a row also)
        uiDest[iGlobalOffset + get_global_size(0)] = (uchar)fTemp;
    }
}
