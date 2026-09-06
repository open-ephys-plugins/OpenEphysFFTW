/*
------------------------------------------------------------------

This file is part of a library for the Open Ephys GUI
Copyright (C) 2026 Open Ephys

------------------------------------------------------------------

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef OEP_FFTW_BATCH_H_INCLUDED
#define OEP_FFTW_BATCH_H_INCLUDED

#include <complex>
#include <memory>

#if defined(_WIN32)
#if defined(OEPLUGIN)
#define OEP_FFTW_API __declspec (dllimport)
#else
#define OEP_FFTW_API __declspec (dllexport)
#endif
#else
#define OEP_FFTW_API __attribute__ ((visibility ("default")))
#endif

/**
    Owns aligned storage and a reusable single-precision real-to-complex batch.

    Each transform occupies one contiguous row of transformLength real inputs
    and produces transformLength / 2 + 1 contiguous complex outputs. Planning
    and allocation occur in the constructor; execute() performs no allocation.
    The internal FFTW planning strategy is not part of this API contract.
    Planning is serialized across batch instances. Concurrent execution is safe
    for distinct instances, but callers must not execute one instance twice at
    the same time because its input and output storage are shared.
*/
class OEP_FFTW_API FFTWRealToComplexBatchFloat
{
public:
    FFTWRealToComplexBatchFloat (int transformLength,
                                 int transformCount,
                                 unsigned int flags = 0U /* FFTW_MEASURE */);
    ~FFTWRealToComplexBatchFloat();

    float* getInputPointer (int transformIndex = 0);
    const float* getInputPointer (int transformIndex = 0) const;
    std::complex<float>* getOutputPointer (int transformIndex = 0);
    const std::complex<float>* getOutputPointer (int transformIndex = 0) const;

    int getTransformLength() const noexcept;
    int getTransformCount() const noexcept;
    int getBinCount() const noexcept;

    void execute();

private:
    struct Impl;
    std::unique_ptr<Impl> impl;

    FFTWRealToComplexBatchFloat (const FFTWRealToComplexBatchFloat&) = delete;
    FFTWRealToComplexBatchFloat& operator= (const FFTWRealToComplexBatchFloat&) = delete;
};

/** Double-precision counterpart to FFTWRealToComplexBatchFloat. */
class OEP_FFTW_API FFTWRealToComplexBatchDouble
{
public:
    FFTWRealToComplexBatchDouble (int transformLength,
                                  int transformCount,
                                  unsigned int flags = 0U /* FFTW_MEASURE */);
    ~FFTWRealToComplexBatchDouble();

    double* getInputPointer (int transformIndex = 0);
    const double* getInputPointer (int transformIndex = 0) const;
    std::complex<double>* getOutputPointer (int transformIndex = 0);
    const std::complex<double>* getOutputPointer (int transformIndex = 0) const;

    int getTransformLength() const noexcept;
    int getTransformCount() const noexcept;
    int getBinCount() const noexcept;

    void execute();

private:
    struct Impl;
    std::unique_ptr<Impl> impl;

    FFTWRealToComplexBatchDouble (const FFTWRealToComplexBatchDouble&) = delete;
    FFTWRealToComplexBatchDouble& operator= (const FFTWRealToComplexBatchDouble&) = delete;
};

/**
    Reusable single-precision complex-to-real batch.

    Input rows contain transformLength / 2 + 1 complex values in FFTW's
    Hermitian half-spectrum layout; output rows contain transformLength real
    values. Like FFTW itself, execute() does not normalize the inverse transform.
*/
class OEP_FFTW_API FFTWComplexToRealBatchFloat
{
public:
    FFTWComplexToRealBatchFloat (int transformLength,
                                 int transformCount,
                                 unsigned int flags = 0U /* FFTW_MEASURE */);
    ~FFTWComplexToRealBatchFloat();

    std::complex<float>* getInputPointer (int transformIndex = 0);
    const std::complex<float>* getInputPointer (int transformIndex = 0) const;
    float* getOutputPointer (int transformIndex = 0);
    const float* getOutputPointer (int transformIndex = 0) const;

    int getTransformLength() const noexcept;
    int getTransformCount() const noexcept;
    int getBinCount() const noexcept;

    void execute();

private:
    struct Impl;
    std::unique_ptr<Impl> impl;

    FFTWComplexToRealBatchFloat (const FFTWComplexToRealBatchFloat&) = delete;
    FFTWComplexToRealBatchFloat& operator= (const FFTWComplexToRealBatchFloat&) = delete;
};

/** Double-precision counterpart to FFTWComplexToRealBatchFloat. */
class OEP_FFTW_API FFTWComplexToRealBatchDouble
{
public:
    FFTWComplexToRealBatchDouble (int transformLength,
                                  int transformCount,
                                  unsigned int flags = 0U /* FFTW_MEASURE */);
    ~FFTWComplexToRealBatchDouble();

    std::complex<double>* getInputPointer (int transformIndex = 0);
    const std::complex<double>* getInputPointer (int transformIndex = 0) const;
    double* getOutputPointer (int transformIndex = 0);
    const double* getOutputPointer (int transformIndex = 0) const;

    int getTransformLength() const noexcept;
    int getTransformCount() const noexcept;
    int getBinCount() const noexcept;

    void execute();

private:
    struct Impl;
    std::unique_ptr<Impl> impl;

    FFTWComplexToRealBatchDouble (const FFTWComplexToRealBatchDouble&) = delete;
    FFTWComplexToRealBatchDouble& operator= (const FFTWComplexToRealBatchDouble&) = delete;
};

#undef OEP_FFTW_API

#endif // OEP_FFTW_BATCH_H_INCLUDED
