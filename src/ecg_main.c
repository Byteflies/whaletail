/**
 * Copyright (c) 2026 Byteflies
 *
 * whalepulse: drive a WhaleTeq SECG 4.0 single channel ECG system.
 */

#include <stdio.h>
#include <strings.h>

#include "WhaleTeqSECG_SDK.h"

#include "argparse.h"
#include "run.h"

static const enum_name wave_shapes[] = {
    { "off",             Output_Off },
    { "sine",            Output_Sine },
    { "triangle",        Output_Triangle },
    { "square",          Output_Square },
    { "rectanglepulse",  Output_RectanglePulse },
    { "rectangle_pulse", Output_RectanglePulse },
    { "trianglepulse",   Output_TrianglePulse },
    { "triangle_pulse",  Output_TrianglePulse },
    { "exponential",     Output_Exponential },
    { "ecg2_27",         Output_ECG2_27 },
    { "iec227w",         Output_IEC227W },
    { "iec251w",         Output_IEC251W },
    { "jjg1041",         Output_JJG1041 },
    { "jjg1041_hr",      Output_JJG1041_HR },
    { "jjg_hysteresis",  Output_JJG_Hysteresis },
    { "ecg_file",        Output_ECG_File },
    ENUM_NAME_END
};

static void usage(const char* prog_name)
{
    fprintf(stderr, "Usage: %s <wave_shape> <frequency> <amplitude> [pace <bpm> <amplitude> <duration>]\n", prog_name);
    fprintf(stderr, "Example: %s sine 1.0 1.0 pace 60 1.0 0.5\n", prog_name);
    fprintf(stderr, "Supported wave shapes: ");
    print_enum_names(stderr, wave_shapes);
    fprintf(stderr, "\n");
    fprintf(stderr, "Frequency unit: Hz, Amplitude unit: mV, Pacing BPM: beats per minute, Pacing Amplitude: mV, Pacing Duration: ms\n");
}

static void shutdown_device(void)
{
    CloseSECG();
}

int main(int argc, char* argv[])
{
    int wave_shape_value;
    OutputFunction_E wave_shape;
    double frequency;
    double amplitude;
    /* optional variables */
    bool pace_on = false;
    unsigned pace_bpm = DefaultPacingRate;
    double pace_amp = 0.0;
    double pace_duration = DefaultPacingDuration;

    if (argc < 4) {
        usage(argv[0]);
        return 1;
    }

    if (!parse_enum(wave_shapes, argv[1], &wave_shape_value)) {
        fprintf(stderr, "Invalid wave shape: %s\n", argv[1]);
        usage(argv[0]);
        return 1;
    }
    wave_shape = (OutputFunction_E)wave_shape_value;

    if (!parse_double(argv[2], &frequency)) {
        fprintf(stderr, "Invalid frequency: %s\n", argv[2]);
        usage(argv[0]);
        return 1;
    }

    if (!parse_double(argv[3], &amplitude)) {
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

        if (!parse_unsigned(argv[5], &pace_bpm)) {
            fprintf(stderr, "Invalid pacing BPM: %s\n", argv[5]);
            usage(argv[0]);
            return 1;
        }

        if (!parse_double(argv[6], &pace_amp)) {
            fprintf(stderr, "Invalid pacing amplitude: %s\n", argv[6]);
            usage(argv[0]);
            return 1;
        }

        if (!parse_double(argv[7], &pace_duration)) {
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

    if (pace_on) {
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
    run_until_interrupted(shutdown_device);

    return 0;
}
