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

#include "OpenEphysFFTWBatch.h"

#include <fftw3.h>

#include <limits>
#include <mutex>
#include <stdexcept>

namespace
{
std::mutex plannerMutex;

std::size_t checkedElementCount (int elementsPerTransform, int transformCount)
{
    if (elementsPerTransform <= 0 || transformCount <= 0)
        throw std::invalid_argument ("FFTW batch dimensions must be positive");

    const auto elements = static_cast<std::size_t> (elementsPerTransform);
    const auto transforms = static_cast<std::size_t> (transformCount);
    if (transforms > std::numeric_limits<std::size_t>::max() / elements)
        throw std::length_error ("FFTW batch allocation is too large");
    return elements * transforms;
}

bool validTransformIndex (int transformIndex, int transformCount)
{
    return transformIndex >= 0 && transformIndex < transformCount;
}
} // namespace

struct FFTWRealToComplexBatchFloat::Impl
{
    Impl (int length, int count, unsigned int flags)
        : transformLength (length), transformCount (count), binCount (length > 0 ? length / 2 + 1 : 0), input (fftwf_alloc_real (checkedElementCount (length, count))), output (fftwf_alloc_complex (checkedElementCount (binCount, count)))
    {
        if (input == nullptr || output == nullptr)
        {
            fftwf_free (output);
            fftwf_free (input);
            throw std::bad_alloc();
        }

        const int dimensions[] { transformLength };
        {
            const std::lock_guard<std::mutex> lock (plannerMutex);
            plan = fftwf_plan_many_dft_r2c (1, dimensions, transformCount, input, nullptr, 1, transformLength, output, nullptr, 1, binCount, flags);
        }
        if (plan == nullptr)
        {
            fftwf_free (output);
            fftwf_free (input);
            throw std::runtime_error (
                "Unable to create FFTW single-precision batch plan");
        }
    }

    ~Impl()
    {
        if (plan != nullptr)
        {
            const std::lock_guard<std::mutex> lock (plannerMutex);
            fftwf_destroy_plan (plan);
        }
        fftwf_free (output);
        fftwf_free (input);
    }

    const int transformLength;
    const int transformCount;
    const int binCount;
    float* input = nullptr;
    fftwf_complex* output = nullptr;
    fftwf_plan plan = nullptr;
};

FFTWRealToComplexBatchFloat::FFTWRealToComplexBatchFloat (int transformLength,
                                                          int transformCount,
                                                          unsigned int flags)
    : impl (new Impl (transformLength, transformCount, flags)) {}

FFTWRealToComplexBatchFloat::~FFTWRealToComplexBatchFloat() = default;

float* FFTWRealToComplexBatchFloat::getInputPointer (int transformIndex)
{
    return validTransformIndex (transformIndex, impl->transformCount)
               ? impl->input + transformIndex * impl->transformLength
               : nullptr;
}

const float*
    FFTWRealToComplexBatchFloat::getInputPointer (int transformIndex) const
{
    return validTransformIndex (transformIndex, impl->transformCount)
               ? impl->input + transformIndex * impl->transformLength
               : nullptr;
}

std::complex<float>*
    FFTWRealToComplexBatchFloat::getOutputPointer (int transformIndex)
{
    return validTransformIndex (transformIndex, impl->transformCount)
               ? reinterpret_cast<std::complex<float>*> (impl->output) + transformIndex * impl->binCount
               : nullptr;
}

const std::complex<float>*
    FFTWRealToComplexBatchFloat::getOutputPointer (int transformIndex) const
{
    return validTransformIndex (transformIndex, impl->transformCount)
               ? reinterpret_cast<const std::complex<float>*> (impl->output) + transformIndex * impl->binCount
               : nullptr;
}

int FFTWRealToComplexBatchFloat::getTransformLength() const noexcept
{
    return impl->transformLength;
}
int FFTWRealToComplexBatchFloat::getTransformCount() const noexcept
{
    return impl->transformCount;
}
int FFTWRealToComplexBatchFloat::getBinCount() const noexcept
{
    return impl->binCount;
}

void FFTWRealToComplexBatchFloat::execute() { fftwf_execute (impl->plan); }

struct FFTWRealToComplexBatchDouble::Impl
{
    Impl (int length, int count, unsigned int flags)
        : transformLength (length), transformCount (count), binCount (length > 0 ? length / 2 + 1 : 0), input (fftw_alloc_real (checkedElementCount (length, count))), output (fftw_alloc_complex (checkedElementCount (binCount, count)))
    {
        if (input == nullptr || output == nullptr)
        {
            fftw_free (output);
            fftw_free (input);
            throw std::bad_alloc();
        }

        const int dimensions[] { transformLength };
        {
            const std::lock_guard<std::mutex> lock (plannerMutex);
            plan = fftw_plan_many_dft_r2c (1, dimensions, transformCount, input, nullptr, 1, transformLength, output, nullptr, 1, binCount, flags);
        }
        if (plan == nullptr)
        {
            fftw_free (output);
            fftw_free (input);
            throw std::runtime_error (
                "Unable to create FFTW double-precision batch plan");
        }
    }

    ~Impl()
    {
        if (plan != nullptr)
        {
            const std::lock_guard<std::mutex> lock (plannerMutex);
            fftw_destroy_plan (plan);
        }
        fftw_free (output);
        fftw_free (input);
    }

    const int transformLength;
    const int transformCount;
    const int binCount;
    double* input = nullptr;
    fftw_complex* output = nullptr;
    fftw_plan plan = nullptr;
};

FFTWRealToComplexBatchDouble::FFTWRealToComplexBatchDouble (int transformLength,
                                                            int transformCount,
                                                            unsigned int flags)
    : impl (new Impl (transformLength, transformCount, flags)) {}

FFTWRealToComplexBatchDouble::~FFTWRealToComplexBatchDouble() = default;

double* FFTWRealToComplexBatchDouble::getInputPointer (int transformIndex)
{
    return validTransformIndex (transformIndex, impl->transformCount)
               ? impl->input + transformIndex * impl->transformLength
               : nullptr;
}

const double*
    FFTWRealToComplexBatchDouble::getInputPointer (int transformIndex) const
{
    return validTransformIndex (transformIndex, impl->transformCount)
               ? impl->input + transformIndex * impl->transformLength
               : nullptr;
}

std::complex<double>*
    FFTWRealToComplexBatchDouble::getOutputPointer (int transformIndex)
{
    return validTransformIndex (transformIndex, impl->transformCount)
               ? reinterpret_cast<std::complex<double>*> (impl->output) + transformIndex * impl->binCount
               : nullptr;
}

const std::complex<double>*
    FFTWRealToComplexBatchDouble::getOutputPointer (int transformIndex) const
{
    return validTransformIndex (transformIndex, impl->transformCount)
               ? reinterpret_cast<const std::complex<double>*> (impl->output) + transformIndex * impl->binCount
               : nullptr;
}

int FFTWRealToComplexBatchDouble::getTransformLength() const noexcept
{
    return impl->transformLength;
}
int FFTWRealToComplexBatchDouble::getTransformCount() const noexcept
{
    return impl->transformCount;
}
int FFTWRealToComplexBatchDouble::getBinCount() const noexcept
{
    return impl->binCount;
}

void FFTWRealToComplexBatchDouble::execute() { fftw_execute (impl->plan); }

struct FFTWComplexToRealBatchFloat::Impl
{
    Impl (int length, int count, unsigned int flags)
        : transformLength (length), transformCount (count), binCount (length > 0 ? length / 2 + 1 : 0), input (fftwf_alloc_complex (checkedElementCount (binCount, count))), output (fftwf_alloc_real (checkedElementCount (length, count)))
    {
        if (input == nullptr || output == nullptr)
        {
            fftwf_free (output);
            fftwf_free (input);
            throw std::bad_alloc();
        }

        const int dimensions[] { transformLength };
        {
            const std::lock_guard<std::mutex> lock (plannerMutex);
            plan = fftwf_plan_many_dft_c2r (1, dimensions, transformCount, input, nullptr, 1, binCount, output, nullptr, 1, transformLength, flags);
        }
        if (plan == nullptr)
        {
            fftwf_free (output);
            fftwf_free (input);
            throw std::runtime_error (
                "Unable to create FFTW single-precision inverse batch plan");
        }
    }

    ~Impl()
    {
        if (plan != nullptr)
        {
            const std::lock_guard<std::mutex> lock (plannerMutex);
            fftwf_destroy_plan (plan);
        }
        fftwf_free (output);
        fftwf_free (input);
    }

    const int transformLength;
    const int transformCount;
    const int binCount;
    fftwf_complex* input = nullptr;
    float* output = nullptr;
    fftwf_plan plan = nullptr;
};

FFTWComplexToRealBatchFloat::FFTWComplexToRealBatchFloat (int transformLength,
                                                          int transformCount,
                                                          unsigned int flags)
    : impl (new Impl (transformLength, transformCount, flags)) {}

FFTWComplexToRealBatchFloat::~FFTWComplexToRealBatchFloat() = default;

std::complex<float>* FFTWComplexToRealBatchFloat::getInputPointer (int transformIndex)
{
    return validTransformIndex (transformIndex, impl->transformCount)
               ? reinterpret_cast<std::complex<float>*> (impl->input) + transformIndex * impl->binCount
               : nullptr;
}

const std::complex<float>*
    FFTWComplexToRealBatchFloat::getInputPointer (int transformIndex) const
{
    return validTransformIndex (transformIndex, impl->transformCount)
               ? reinterpret_cast<const std::complex<float>*> (impl->input) + transformIndex * impl->binCount
               : nullptr;
}

float* FFTWComplexToRealBatchFloat::getOutputPointer (int transformIndex)
{
    return validTransformIndex (transformIndex, impl->transformCount)
               ? impl->output + transformIndex * impl->transformLength
               : nullptr;
}

const float* FFTWComplexToRealBatchFloat::getOutputPointer (int transformIndex) const
{
    return validTransformIndex (transformIndex, impl->transformCount)
               ? impl->output + transformIndex * impl->transformLength
               : nullptr;
}

int FFTWComplexToRealBatchFloat::getTransformLength() const noexcept { return impl->transformLength; }
int FFTWComplexToRealBatchFloat::getTransformCount() const noexcept { return impl->transformCount; }
int FFTWComplexToRealBatchFloat::getBinCount() const noexcept { return impl->binCount; }

void FFTWComplexToRealBatchFloat::execute() { fftwf_execute (impl->plan); }

struct FFTWComplexToRealBatchDouble::Impl
{
    Impl (int length, int count, unsigned int flags)
        : transformLength (length), transformCount (count), binCount (length > 0 ? length / 2 + 1 : 0), input (fftw_alloc_complex (checkedElementCount (binCount, count))), output (fftw_alloc_real (checkedElementCount (length, count)))
    {
        if (input == nullptr || output == nullptr)
        {
            fftw_free (output);
            fftw_free (input);
            throw std::bad_alloc();
        }

        const int dimensions[] { transformLength };
        {
            const std::lock_guard<std::mutex> lock (plannerMutex);
            plan = fftw_plan_many_dft_c2r (1, dimensions, transformCount, input, nullptr, 1, binCount, output, nullptr, 1, transformLength, flags);
        }
        if (plan == nullptr)
        {
            fftw_free (output);
            fftw_free (input);
            throw std::runtime_error (
                "Unable to create FFTW double-precision inverse batch plan");
        }
    }

    ~Impl()
    {
        if (plan != nullptr)
        {
            const std::lock_guard<std::mutex> lock (plannerMutex);
            fftw_destroy_plan (plan);
        }
        fftw_free (output);
        fftw_free (input);
    }

    const int transformLength;
    const int transformCount;
    const int binCount;
    fftw_complex* input = nullptr;
    double* output = nullptr;
    fftw_plan plan = nullptr;
};

FFTWComplexToRealBatchDouble::FFTWComplexToRealBatchDouble (int transformLength,
                                                            int transformCount,
                                                            unsigned int flags)
    : impl (new Impl (transformLength, transformCount, flags)) {}

FFTWComplexToRealBatchDouble::~FFTWComplexToRealBatchDouble() = default;

std::complex<double>* FFTWComplexToRealBatchDouble::getInputPointer (int transformIndex)
{
    return validTransformIndex (transformIndex, impl->transformCount)
               ? reinterpret_cast<std::complex<double>*> (impl->input) + transformIndex * impl->binCount
               : nullptr;
}

const std::complex<double>*
    FFTWComplexToRealBatchDouble::getInputPointer (int transformIndex) const
{
    return validTransformIndex (transformIndex, impl->transformCount)
               ? reinterpret_cast<const std::complex<double>*> (impl->input) + transformIndex * impl->binCount
               : nullptr;
}

double* FFTWComplexToRealBatchDouble::getOutputPointer (int transformIndex)
{
    return validTransformIndex (transformIndex, impl->transformCount)
               ? impl->output + transformIndex * impl->transformLength
               : nullptr;
}

const double* FFTWComplexToRealBatchDouble::getOutputPointer (int transformIndex) const
{
    return validTransformIndex (transformIndex, impl->transformCount)
               ? impl->output + transformIndex * impl->transformLength
               : nullptr;
}

int FFTWComplexToRealBatchDouble::getTransformLength() const noexcept { return impl->transformLength; }
int FFTWComplexToRealBatchDouble::getTransformCount() const noexcept { return impl->transformCount; }
int FFTWComplexToRealBatchDouble::getBinCount() const noexcept { return impl->binCount; }

void FFTWComplexToRealBatchDouble::execute() { fftw_execute (impl->plan); }
