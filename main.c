/**
 * Copyright (c) 2026 Byteflies
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

#include "sdk/WhaleTeqSECG_SDK.h"

static int parse_wave_shape(const char* name, OutputFunction_E* out_wave_shape)
{
    if (strcasecmp(name, "off") == 0) {
        *out_wave_shape = Output_Off;
    } else if (strcasecmp(name, "sine") == 0) {
        *out_wave_shape = Output_Sine;
    } else if (strcasecmp(name, "triangle") == 0) {
        *out_wave_shape = Output_Triangle;
    } else if (strcasecmp(name, "square") == 0) {
        *out_wave_shape = Output_Square;
    } else if (strcasecmp(name, "rectanglepulse") == 0 || strcasecmp(name, "rectangle_pulse") == 0) {
        *out_wave_shape = Output_RectanglePulse;
    } else if (strcasecmp(name, "trianglepulse") == 0 || strcasecmp(name, "triangle_pulse") == 0) {
        *out_wave_shape = Output_TrianglePulse;
    } else if (strcasecmp(name, "exponential") == 0) {
        *out_wave_shape = Output_Exponential;
    } else if (strcasecmp(name, "ecg2_27") == 0) {
        *out_wave_shape = Output_ECG2_27;
    } else if (strcasecmp(name, "iec227w") == 0) {
        *out_wave_shape = Output_IEC227W;
    } else if (strcasecmp(name, "iec251w") == 0) {
        *out_wave_shape = Output_IEC251W;
    } else if (strcasecmp(name, "jjg1041") == 0) {
        *out_wave_shape = Output_JJG1041;
    } else if (strcasecmp(name, "jjg1041_hr") == 0) {
        *out_wave_shape = Output_JJG1041_HR;
    } else if (strcasecmp(name, "jjg_hysteresis") == 0) {
        *out_wave_shape = Output_JJG_Hysteresis;
    } else if (strcasecmp(name, "ecg_file") == 0) {
        *out_wave_shape = Output_ECG_File;
    } else {
        return 0;
    }

    return 1;
}


int main(int argc, char* argv[])
{
    OutputFunction_E wave_shape;
    double frequency;
    double amplitude;
    char* endptr = NULL;

    if (argc != 4) {
        fprintf(stderr, "Usage: %s <wave_shape> <frequency> <amplitude>\n", argv[0]);
        fprintf(stderr, "Example: %s sine 1.0 1.0\n", argv[0]);
        fprintf(stderr, "Supported wave shapes: off, sine, triangle, square, rectanglepulse, trianglepulse, exponential, ecg2_27, iec227w, iec251w, jjg1041, jjg1041_hr, jjg_hysteresis, ecg_file\n");
        return 1;
    }

    if (!parse_wave_shape(argv[1], &wave_shape)) {
        fprintf(stderr, "Invalid wave shape: %s\n", argv[1]);
        return 1;
    }

    endptr = NULL;
    frequency = strtod(argv[2], &endptr);
    if (argv[2][0] == '\0' || endptr == argv[2] || *endptr != '\0') {
        fprintf(stderr, "Invalid frequency: %s\n", argv[2]);
        return 1;
    }

    endptr = NULL;
    amplitude = strtod(argv[3], &endptr);
    if (argv[3][0] == '\0' || endptr == argv[3] || *endptr != '\0') {
        fprintf(stderr, "Invalid amplitude: %s\n", argv[3]);
        return 1;
    }

    if (!InitSECG()) {
        fprintf(stderr, "InitSECG failed\n");
        return -1;
    }

    ResetSECG();
    SetOutputLead(Lead_RA, false);
    SetOutputLead(Lead_LA, true);
    SetOutputLead(Lead_V3, true);


    if (SetOutputFunc(wave_shape) != 0) {
        fprintf(stderr, "SetOutputFunc failed\n");
        CloseSECG();
        return 1;
    }

    if (SetFrequency(frequency) != 0) {
        fprintf(stderr, "SetFrequency failed for value: %s\n", argv[2]);
        CloseSECG();
        return 1;
    }

    if (SetAmplitude(amplitude) != 0) {
        fprintf(stderr, "SetAmplitude failed for value: %s\n", argv[3]);
        CloseSECG();
        return 1;
    }

    while(1) {
        sleep(1);
    }
    // Do something else...

    CloseSECG();
    return 0;
}
