/***************************************************************************

    file                 : spline.h
    created              : Mon Apr 17 13:51:00 CET 2000
    copyright            : (C) 2000-2006 by Bernhard Wymann
    email                : berniw@bluewin.ch
    version              : $Id$

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/**
 * @file spline.h
 * @brief Cubic spline interpolation utilities (header-only).
 *
 * Provides two complementary APIs:
 *
 * **Function-based API** (berniw / lliaw style) — works on raw arrays of
 * `double` and exposes slope-computation helpers (tridiagonal solvers,
 * periodic and natural constraints, 2-D parametric variants) plus a
 * single-value evaluator.
 *
 * **Class-based API** (damned / bt style) — a compact `Spline` class whose
 * caller pre-fills `SplinePoint` arrays (including slopes) and then calls
 * `evaluate()`.
 *
 * Both APIs were historically duplicated in every AI driver module.  They
 * now live here so that all drivers share a single implementation.
 */

#ifndef _TMATH_SPLINE_H_
#define _TMATH_SPLINE_H_

#include <cmath>
#include <cstdlib>

// ---------------------------------------------------------------------------
// Function-based API  (tridiagonal solver + slope computation + evaluator)
// ---------------------------------------------------------------------------

/** Data for a row of a tridiagonal system — 1-D case. */
struct SplineEquationData {
    double a, b, c, d, h;
};

/** Data for a row of a tridiagonal system — 2-vector case. */
struct SplineEquationData2 {
    double a, b, c, d, h, x1, x2;
};


/**
 * Solve a tridiagonal n×n system in O(n) using Givens rotations.
 *
 *   [ a1 b1  0   0  … ]
 *   [ c1 a2 b2   0  … ]
 *   [  0 c2 a3  b3  … ]
 *   [         …       ]
 *
 * On entry: tmp[0..dim-1] holds the matrix coefficients and x[0..dim-1]
 * holds the right-hand side.  On exit: x[i] contains the solution.
 */
inline void tridiagonal(int dim, SplineEquationData *tmp, double *x)
{
    double cs, sn, h, t;
    int i;

    dim--;
    tmp[dim].b = 0.0;
    for (i = 0; i < dim; i++) {
        if (tmp[i].c != 0.0) {
            t = tmp[i].a / tmp[i].c;
            sn = 1.0 / std::sqrt(1.0 + t*t);
            cs = t * sn;
            tmp[i].a = tmp[i].a*cs + tmp[i].c*sn;
            h = tmp[i].b;
            tmp[i].b = h*cs + tmp[i+1].a*sn;
            tmp[i+1].a = -h*sn + tmp[i+1].a*cs;
            tmp[i].c = tmp[i+1].b*sn;
            tmp[i+1].b = tmp[i+1].b*cs;

            h = x[i];
            x[i] = h*cs + x[i+1]*sn;
            x[i+1] = -h*sn + x[i+1]*cs;
        }
    }

    x[dim] = x[dim] / tmp[dim].a;
    x[dim-1] = (x[dim-1] - tmp[dim-1].b*x[dim]) / tmp[dim-1].a;

    for (i = dim - 2; i >= 0; i--) {
        x[i] = (x[i] - tmp[i].b*x[i+1] - tmp[i].c*x[i+2]) / tmp[i].a;
    }
}


/**
 * Solve a tridiagonal n×n system for two right-hand-side vectors in O(n)
 * using Givens rotations.  Solutions are stored in tmp[i].x1 and tmp[i].x2.
 */
inline void tridiagonal2(int dim, SplineEquationData2 *tmp)
{
    double cs, sn, h, t;
    int i;

    dim--;
    tmp[dim].b = 0.0;
    for (i = 0; i < dim; i++) {
        if (tmp[i].c != 0.0) {
            t = tmp[i].a / tmp[i].c;
            sn = 1.0 / std::sqrt(1.0 + t*t);
            cs = t * sn;
            tmp[i].a = tmp[i].a*cs + tmp[i].c*sn;
            h = tmp[i].b;
            tmp[i].b = h*cs + tmp[i+1].a*sn;
            tmp[i+1].a = -h*sn + tmp[i+1].a*cs;
            tmp[i].c = tmp[i+1].b*sn;
            tmp[i+1].b = tmp[i+1].b*cs;

            h = tmp[i].x1;
            tmp[i].x1 = h*cs + tmp[i+1].x1*sn;
            tmp[i+1].x1 = -h*sn + tmp[i+1].x1*cs;

            h = tmp[i].x2;
            tmp[i].x2 = h*cs + tmp[i+1].x2*sn;
            tmp[i+1].x2 = -h*sn + tmp[i+1].x2*cs;
        }
    }

    tmp[dim].x1 = tmp[dim].x1 / tmp[dim].a;
    tmp[dim-1].x1 = (tmp[dim-1].x1 - tmp[dim-1].b*tmp[dim].x1) / tmp[dim-1].a;

    tmp[dim].x2 = tmp[dim].x2 / tmp[dim].a;
    tmp[dim-1].x2 = (tmp[dim-1].x2 - tmp[dim-1].b*tmp[dim].x2) / tmp[dim-1].a;

    for (i = dim - 2; i >= 0; i--) {
        tmp[i].x1 = (tmp[i].x1 - tmp[i].b*tmp[i+1].x1 - tmp[i].c*tmp[i+2].x1) / tmp[i].a;
        tmp[i].x2 = (tmp[i].x2 - tmp[i].b*tmp[i+1].x2 - tmp[i].c*tmp[i+2].x2) / tmp[i].a;
    }
}


/**
 * Compute the slopes of a cubic spline through (x[0..dim-1], y[0..dim-1])
 * with **periodic** endpoint constraints.
 *
 * @param[in]  dim   Number of knot points (x[dim-1] wraps back to x[0]).
 * @param[in]  x     Knot x-coordinates (dim elements).
 * @param[in]  y     Knot y-coordinates (dim elements).
 * @param[out] ys    Computed slopes (dim elements); ys[dim-1] is a copy of
 *                   ys[0] to close the loop (the function decrements dim
 *                   internally, then stores ys[dim_decremented] = ys[0]).
 */
inline void slopesp(int dim, const double *const x, const double *const y,
                    double *const ys)
{
    SplineEquationData2 *tmp = static_cast<SplineEquationData2 *>(
        std::malloc(sizeof(SplineEquationData2) * static_cast<std::size_t>(dim)));
    int i;

    dim--;
    for (i = 0; i < dim; i++) {
        tmp[i].h = x[i+1] - x[i];
        tmp[i].d = (y[i+1] - y[i]) / (tmp[i].h * tmp[i].h);
    }

    for (i = 1; i < dim; i++) {
        tmp[i].a = 2.0/tmp[i-1].h + 2.0/tmp[i].h;
        tmp[i].b = 1.0/tmp[i].h;
        tmp[i].c = tmp[i].b;
        ys[i] = 3.0 * (tmp[i].d + tmp[i-1].d);
    }

    tmp[0].b = 1.0/tmp[0].h;
    tmp[0].c = tmp[0].b;
    tmp[0].a = (2.0*tmp[0].b + 1.0/tmp[dim-1].h);
    tmp[dim-1].a = 2.0/tmp[dim-2].h + 1.0/tmp[dim-1].h;

    for (i = 1; i < dim; i++) {
        tmp[i].x1 = 0.0;
        tmp[i].x2 = 3.0 * (tmp[i].d + tmp[i-1].d);
    }

    tmp[0].x1 = 1.0;
    tmp[dim-1].x1 = 1.0;
    tmp[0].x2 = 3.0 * (tmp[0].d + tmp[dim-1].d);

    tridiagonal2(dim, tmp);

    double factor = (tmp[0].x2 + tmp[dim-1].x2) /
                    (tmp[0].x1 + tmp[dim-1].x1 + tmp[dim-1].h);
    for (i = 0; i < dim; i++) {
        ys[i] = tmp[i].x2 - factor*tmp[i].x1;
    }
    ys[dim] = ys[0];

    std::free(tmp);
}


/**
 * Compute the slopes of a cubic spline through (x[0..dim-1], y[0..dim-1])
 * with **natural** (zero second-derivative) endpoint constraints.
 *
 * @param[in]  dim   Number of knot points.
 * @param[in]  x     Knot x-coordinates (dim elements).
 * @param[in]  y     Knot y-coordinates (dim elements).
 * @param[out] ys    Computed slopes (dim elements allocated by caller).
 */
inline void slopesn(int dim, const double *const x, const double *const y,
                    double *const ys)
{
    SplineEquationData *tmp = static_cast<SplineEquationData *>(
        std::malloc(sizeof(SplineEquationData) * static_cast<std::size_t>(dim)));
    int i;

    dim--;
    for (i = 0; i < dim; i++) {
        tmp[i].h = x[i+1] - x[i];
        tmp[i].d = (y[i+1] - y[i]) / (tmp[i].h * tmp[i].h);
    }

    for (i = 1; i < dim; i++) {
        tmp[i].a = 2.0/tmp[i-1].h + 2.0/tmp[i].h;
        tmp[i].b = 1.0/tmp[i].h;
        tmp[i].c = tmp[i].b;
        ys[i] = 3.0 * (tmp[i].d + tmp[i-1].d);
    }

    tmp[0].b = 1.0/tmp[0].h;
    tmp[0].c = tmp[0].b;
    tmp[0].a = 2.0*tmp[0].b;
    tmp[dim].a = 2.0/tmp[dim-1].h;
    ys[0] = 3.0*tmp[0].d;
    ys[dim] = 3.0*tmp[dim-1].d;

    tridiagonal(dim+1, tmp, ys);

    std::free(tmp);
}


/**
 * Compute slopes for a 2-D parametric cubic spline (periodic constraints).
 * Arc-length of each segment is used as the parameter.
 *
 * @param[in]  dim   Number of points (closed curve; x[0]==x[dim] assumed).
 * @param[in]  x,y   Point coordinates (dim elements each).
 * @param[out] xs,ys Computed slope components for x and y (dim+1 elements each).
 * @param[out] s     Parameter values (dim elements; s[0] == 0).
 */
inline void parametricslopesp(int dim,
                               const double *const x, const double *const y,
                               double *const xs, double *const ys,
                               double *const s)
{
    s[0] = 0.0;
    for (int i = 1; i < dim; i++) {
        s[i] = s[i-1] + std::sqrt((x[i]-x[i-1])*(x[i]-x[i-1]) +
                                   (y[i]-y[i-1])*(y[i]-y[i-1]));
    }
    slopesp(dim, s, x, xs);
    slopesp(dim, s, y, ys);
}


/**
 * Compute slopes for a 2-D parametric cubic spline (natural constraints).
 * Arc-length of each segment is used as the parameter.
 *
 * @param[in]  dim   Number of points.
 * @param[in]  x,y   Point coordinates (dim elements each).
 * @param[out] xs,ys Computed slope components for x and y (dim elements each).
 * @param[out] s     Parameter values (dim elements; s[0] == 0).
 */
inline void parametricslopesn(int dim,
                               const double *const x, const double *const y,
                               double *const xs, double *const ys,
                               double *const s)
{
    s[0] = 0.0;
    for (int i = 1; i < dim; i++) {
        s[i] = s[i-1] + std::sqrt((x[i]-x[i-1])*(x[i]-x[i-1]) +
                                   (y[i]-y[i-1])*(y[i]-y[i-1]));
    }
    slopesn(dim, s, x, xs);
    slopesn(dim, s, y, ys);
}


/**
 * Evaluate a cubic spline at position z using binary search.
 *
 * @param[in] dim  Number of knot points.
 * @param[in] z    Query position (must be within [x[0], x[dim-1]]).
 * @param[in] x    Knot x-coordinates (dim elements).
 * @param[in] y    Knot y-coordinates (dim elements).
 * @param[in] ys   Slopes at each knot (from slopesp / slopesn).
 * @return Interpolated y value at z.
 */
inline double spline(int dim, double z,
                     const double *const x,
                     const double *const y,
                     const double *const ys)
{
    int i, a, b;
    double t, a0, a1, a2, a3, h;

    a = 0; b = dim - 1;
    do {
        i = (a + b) / 2;
        if (x[i] <= z) {
            a = i;
        } else {
            b = i;
        }
    } while ((a + 1) != b);

    i = a;
    h = x[i+1] - x[i];
    t = (z - x[i]) / h;
    a0 = y[i];
    a1 = y[i+1] - a0;
    a2 = a1 - h*ys[i];
    a3 = h * ys[i+1] - a1;
    a3 -= a2;
    return a0 + (a1 + (a2 + a3*t) * (t - 1.0))*t;
}


// ---------------------------------------------------------------------------
// Class-based API  (compact Spline class for pre-computed slopes)
// ---------------------------------------------------------------------------

/** A single knot with pre-computed slope, used by the Spline class. */
class SplinePoint {
public:
    float x;    /**< x coordinate */
    float y;    /**< y coordinate */
    float s;    /**< slope (pre-computed by the caller) */
};


/**
 * Compact cubic spline evaluator for pre-computed `SplinePoint` arrays.
 *
 * The caller is responsible for filling the `SplinePoint::s` slope field
 * (e.g. using slopesn / slopesp above, converting to float, and storing
 * the result per point).  This class only evaluates the spline; it does
 * not compute slopes.
 */
class Spline {
public:
    Spline(int dim, SplinePoint *s) : s(s), dim(dim) {}

    /** Evaluate the spline at position z using binary search. */
    float evaluate(float z) const
    {
        int i, a, b;
        float t, a0, a1, a2, a3, h;

        a = 0; b = dim - 1;
        do {
            i = (a + b) / 2;
            if (s[i].x <= z) {
                a = i;
            } else {
                b = i;
            }
        } while ((a + 1) != b);

        i = a;
        h = s[i+1].x - s[i].x;
        t = (z - s[i].x) / h;
        a0 = s[i].y;
        a1 = s[i+1].y - a0;
        a2 = a1 - h*s[i].s;
        a3 = h * s[i+1].s - a1;
        a3 -= a2;
        return a0 + (a1 + (a2 + a3*t) * (t - 1.0f))*t;
    }

private:
    SplinePoint *s;
    int dim;
};


#endif // _TMATH_SPLINE_H_
