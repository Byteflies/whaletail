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

void usage(const char* prog_name) {
    fprintf(stderr, "Usage: %s <wave_shape> <frequency> <amplitude>\n", prog_name);
    fprintf(stderr, "Example: %s sine 1.0 1.0\n", prog_name);
    fprintf(stderr, "Supported wave shapes: off, sine, triangle, square, rectanglepulse, trianglepulse, exponential, ecg2_27, iec227w, iec251w, jjg1041, jjg1041_hr, jjg_hysteresis, ecg_file\n");
}

int main(int argc, char* argv[])
{
    OutputFunction_E wave_shape;
    double frequency;
    double amplitude;
    /* optional variables */
    bool pace_on = false;
    unsigned pace_bpm = DefaultPacingRate;
    double pace_amp = 0.0;
    double pace_duration = DefaultPacingDuration;


    char* endptr = NULL;


    if (argc < 4) {
        usage(argv[0]);
        return 1;
    }

    if (!parse_wave_shape(argv[1], &wave_shape)) {
        fprintf(stderr, "Invalid wave shape: %s\n", argv[1]);
        usage(argv[0]);
        return 1;
    }

    endptr = NULL;
    frequency = strtod(argv[2], &endptr);
    if (argv[2][0] == '\0' || endptr == argv[2] || *endptr != '\0') {
        fprintf(stderr, "Invalid frequency: %s\n", argv[2]);
        usage(argv[0]);
        return 1;
    }

    endptr = NULL;
    amplitude = strtod(argv[3], &endptr);
    if (argv[3][0] == '\0' || endptr == argv[3] || *endptr != '\0') {
        fprintf(stderr, "Invalid amplitude: %s\n", argv[3]);
        usage(argv[0]);
        return 1;
    }

    /* if argc > 4, it has to be 8 and argv[4] must be "pace" */
    if (argc > 4 && argc != 8) {
        usage(argv[0]);
        return 1;
    }

    if (argc == 8) {
        if (strcasecmp(argv[4], "pace") != 0) {
            fprintf(stderr, "Invalid argument: %s\n", argv[4]);
            usage(argv[0]);
            return 1;
        }

        pace_on = true;


        endptr = NULL;
        pace_bpm = strtoul(argv[5], &endptr, 10);
        if (argv[5][0] == '\0' || endptr == argv[5] || *endptr != '\0') {
            fprintf(stderr, "Invalid pacing BPM: %s\n", argv[5]);
            usage(argv[0]);
            return 1;
        }

        endptr = NULL;
        pace_amp = strtod(argv[6], &endptr);
        if (argv[6][0] == '\0' || endptr == argv[6] || *endptr != '\0') {
            fprintf(stderr, "Invalid pacing amplitude: %s\n", argv[6]);
            usage(argv[0]);
            return 1;
        }


        endptr = NULL;
        pace_duration = strtod(argv[7], &endptr);
        if (argv[7][0] == '\0' || endptr == argv[7] || *endptr != '\0') {
            fprintf(stderr, "Invalid pacing duration: %s\n", argv[7]);
            usage(argv[0]);
            return 1;
        }
    }


    if (!InitSECG()) {
        fprintf(stderr, "InitSECG failed\n");
        return 1;
    }

    ResetSECG();
    SetOutputLead(Lead_RA, false);
    SetOutputLead(Lead_LA, true);
    SetOutputLead(Lead_V3, true);


    if (SetOutputFunc(Output_Off) != 0) {
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

    if(pace_on)
    {
        if (SetPacingRate(pace_bpm) != 0) {
            fprintf(stderr, "SetPacingRate failed for value: %s\n", argv[5]);
            CloseSECG();
            return 1;
        }

        if (SetPacingAmplitude(pace_amp) != 0) {
            fprintf(stderr, "SetPacingAmplitude failed for value: %s\n", argv[6]);
            CloseSECG();
            return 1;
        }

        if (SetPacingDuration(pace_duration) != 0) {
            fprintf(stderr, "SetPacingDuration failed for value: %s\n", argv[7]);
            CloseSECG();
            return 1;
        }
    }

    if (SetOutputFunc(wave_shape) != 0) {
        fprintf(stderr, "SetOutputFunc failed\n");
        CloseSECG();
        return 1;
    }

    printf("Outputting %s wave at %.2f Hz and %.2f mV amplitude. Press Ctrl+C to stop.\n", argv[1], frequency, amplitude);
    while(1) {
        sleep(1);
    }

    CloseSECG();
    return 0;
}
