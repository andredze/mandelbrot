#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//------------------------------------------------------------------//
// погрешность делить на корень из колва измерений

int main()
{
    const char* file_names[] = {
        "NAIVE_-O2.csv",
        "NAIVE_-O3.csv",
        "NAIVE_-Ofast.csv",
        "ARRAYS_-O2.csv",
        "ARRAYS_-O3.csv",
        "ARRAYS_-Ofast.csv",
        "AVX_-O2.csv",
        "AVX_-O3.csv",
        "AVX_-Ofast.csv"
    };

    size_t size = sizeof(file_names) / sizeof(file_names[0]);

    char full_path[256] = {};
    
    const char* dir_gcc   = "g++/";
    const char* dir_clang = "clang++/";

    const char* dir_compilers[] = {dir_gcc, dir_clang};

    FILE* file_dest = fopen("results.txt", "w");

    if (file_dest == NULL)
    {
        fprintf(stderr, "failed to open file %s", "results.txt");
        return EXIT_FAILURE;
    }

    double baseline = 0;

    fprintf(file_dest, "%-32s %15s %15s %15s %15s %12s\n",
            "test", "mean", "std", "delta", "eps, %", "coeff");

    fprintf(file_dest, "%-32s %15s %15s %15s %15s %12s\n",
            "--------------------------------",
            "---------------",
            "---------------",
            "---------------",
            "---------------",
            "------------");

    for (size_t j = 0; j < 2; j++)
    {
        for (size_t i = 0; i < size; i++)
        {
            size_t sum   = 0;
            size_t count = 0;

            snprintf(full_path, sizeof(full_path), "%s%s", dir_compilers[j], file_names[i]);

            FILE* data_file = fopen(full_path, "r");

            if (data_file == NULL)
            {
                fprintf(stderr, "failed to open file %s", full_path);
                return EXIT_FAILURE;
            }        

            char buffer[1024] = {};

            double data_numbers[10] = {};

            while (fgets(buffer, sizeof(buffer), data_file) != NULL)
            {
                double number = strtod(buffer, NULL);
                sum += number;
                data_numbers[count] = number;
                count++;
            }

            double mean = (double) sum / count;

            if (i == 0)
            {
                baseline = mean;
            }

            double std = 0.0;

            for (size_t i = 0; i < count; i++)
            {
                double diff = data_numbers[i] - mean; 
                std += diff * diff;
            }

            std = sqrt(std / count);

            double delta = std / sqrt(count);
            double eps   = 100 * delta / mean;

            fprintf(file_dest, "%-32s %15.2e %15.2e %15.2e %15.3f %12.6f\n",
                    full_path, mean, std, delta, eps, baseline / mean);

            fclose(data_file);
        }
    }

    fclose(file_dest);

    return EXIT_SUCCESS;
}

//------------------------------------------------------------------//
