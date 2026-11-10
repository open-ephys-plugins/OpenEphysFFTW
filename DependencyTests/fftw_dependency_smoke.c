#include <fftw3.h>

#include <math.h>
#include <stddef.h>

int main(void)
{
    double double_input[8] = { 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
    fftw_complex double_output[5];
    fftw_plan double_plan = fftw_plan_dft_r2c_1d(8, double_input, double_output,
                                                  FFTW_ESTIMATE);
    if (double_plan == NULL)
        return 1;
    fftw_execute(double_plan);
    fftw_destroy_plan(double_plan);

    float float_input[8] = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    fftwf_complex float_output[5];
    fftwf_plan float_plan = fftwf_plan_dft_r2c_1d(8, float_input, float_output,
                                                   FFTW_ESTIMATE);
    if (float_plan == NULL)
        return 2;
    fftwf_execute(float_plan);
    fftwf_destroy_plan(float_plan);

    for (size_t index = 0; index < 5; ++index)
    {
        if (fabs(double_output[index][0] - 1.0) > 1.0e-12
            || fabs(double_output[index][1]) > 1.0e-12
            || fabsf(float_output[index][0] - 1.0f) > 1.0e-6f
            || fabsf(float_output[index][1]) > 1.0e-6f)
            return 3;
    }
    return 0;
}
