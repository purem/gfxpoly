/* xrow.c

   Representation of point positions in scanlines

   Copyright (c) 2009 Matthias Kramm <kramm@quiss.org>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "xrow.h"
#include "poly.h"

xrow_t* xrow_new()
{
    xrow_t*r = calloc(1,sizeof(xrow_t));
    r->size = 16;
    r->x = malloc(sizeof(r->x[0])*r->size);
    return r;
}

void xrow_add(xrow_t*r, int32_t x)
{
    if(r->num && r->lastx==x)
        return;
    r->lastx = x;
    if(r->num >= r->size) {
        r->size *= 2;
        r->x = realloc(r->x, sizeof(r->x[0])*r->size);
    }
    r->x[r->num++]=x;
}

int compare_int32(const void*_i1,const void*_i2)
{
    int32_t*i1 = (int32_t*)_i1;
    int32_t*i2 = (int32_t*)_i2;
    return *i1-*i2;
}

void xrow_sort(xrow_t*r)
{
    if(!r->num)
        return;
    qsort(r->x, r->num, sizeof(r->x[0]), compare_int32);
    int t;
    int pos = 1;
    int32_t lastx=r->x[0];
    for(t=1;t<r->num;t++) {
        if(r->x[t]!=lastx) {
            r->x[pos++] = lastx = r->x[t];
        }
    }
    r->num = pos;
}

int xrow_find(xrow_t*r, int32_t x)
{
    int min, max, i, l;

    for(min=0, max=r->num, i=r->num/2, l=r->num; i != l; l=i, i=(min+max)/2) {
        if(x < r->x[i]) max=i;
        else min=i;
    }

#ifdef CHECKS
    int t;
    for(t=0;t<r->num;t++) {
        if(x < r->x[t]) 
            break;
    }
    assert(max == t);
#endif

    return max;
}

char xrow_contains(xrow_t*r, int32_t x)
{
    int pos = xrow_find(r,x) - 1;
    return (pos>=0 && r->x[pos]==x);
}

void xrow_reset(xrow_t*r)
{
    r->num = 0;
}

void xrow_dump(xrow_t*xrow, double gridsize)
{
    fprintf(stderr, "x: ");
    int t;
    for(t=0;t<xrow->num;t++) {
        if(t)
            fprintf(stderr, ", ");
        fprintf(stderr, "%.2f", xrow->x[t] * gridsize);
    }
    fprintf(stderr, "\n");
}

void xrow_destroy(xrow_t*r)
{
    if(r->x) {
        free(r->x);r->x = 0;
    }
    free(r);
}
