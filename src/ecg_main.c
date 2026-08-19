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

/* One generator feeds every enabled lead through a relay matrix, so selecting
   several of them puts the identical signal on each. */
#define LeadCount 9

static const enum_name leads[] = {
    { "ra", Lead_RA },
    { "r",  Lead_RA },
    { "la", Lead_LA },
    { "l",  Lead_LA },
    { "ll", Lead_LL },
    { "f",  Lead_LL },
    { "v1", Lead_V1 },
    { "v2", Lead_V2 },
    { "v3", Lead_V3 },
    { "v4", Lead_V4 },
    { "v5", Lead_V5 },
    { "v6", Lead_V6 },
    ENUM_NAME_END
};

/* Canonical name per lead value, for reporting the selection back. */
static const char* const lead_names[LeadCount] = {
    "ra", "la", "ll", "v1", "v2", "v3", "v4", "v5", "v6"
};

static void usage(const char* prog_name)
{
    fprintf(stderr, "Usage: %s <wave_shape> <frequency> <amplitude>"
                    " [pace <bpm> <amplitude> <duration>]"
                    " [leads <lead>[,<lead>...]]\n", prog_name);
    fprintf(stderr, "Example: %s sine 1.0 1.0 pace 60 1.0 0.5\n", prog_name);
    fprintf(stderr, "Supported wave shapes: ");
    print_enum_names(stderr, wave_shapes);
    fprintf(stderr, "\n");
    fprintf(stderr, "Supported leads: ");
    print_enum_names(stderr, leads);
    fprintf(stderr, " (default la,v3)\n");
    fprintf(stderr, "Frequency unit: Hz, Amplitude unit: mV, Pacing BPM: beats per minute, Pacing Amplitude: mV, Pacing Duration: ms\n");
}

/** Print the enabled leads as a comma separated list, without a newline. */
static void print_enabled_leads(FILE* stream, const int* lead_on)
{
    int i;
    int printed = 0;

    for (i = 0; i < LeadCount; i++) {
        if (lead_on[i]) {
            fprintf(stream, "%s%s", printed == 0 ? "" : ", ", lead_names[i]);
            printed++;
        }
    }
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
    /* RA, LL and V1 to V6 off; LA and V3 on */
    int lead_on[LeadCount] = { 0, 1, 0, 0, 0, 1, 0, 0, 0 };
    int i;

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

    /* Trailing optional groups, in any order: "pace" takes three values and
       "leads" takes one. */
    i = 4;
    while (i < argc) {
        if (strcasecmp(argv[i], "pace") == 0) {
            if (argc - i < 4) {
                fprintf(stderr, "Missing values for %s\n", argv[i]);
                usage(argv[0]);
                return 1;
            }

            pace_on = true;

            if (!parse_unsigned(argv[i + 1], &pace_bpm)) {
                fprintf(stderr, "Invalid pacing BPM: %s\n", argv[i + 1]);
                usage(argv[0]);
                return 1;
            }

            if (!parse_double(argv[i + 2], &pace_amp)) {
                fprintf(stderr, "Invalid pacing amplitude: %s\n", argv[i + 2]);
                usage(argv[0]);
                return 1;
            }

            if (!parse_double(argv[i + 3], &pace_duration)) {
                fprintf(stderr, "Invalid pacing duration: %s\n", argv[i + 3]);
                usage(argv[0]);
                return 1;
            }

            i += 4;
        } else if (strcasecmp(argv[i], "leads") == 0) {
            char bad_token[64];

            if (argc - i < 2) {
                fprintf(stderr, "Missing values for %s\n", argv[i]);
                usage(argv[0]);
                return 1;
            }

            if (parse_enum_set(leads, argv[i + 1], lead_on, LeadCount,
                               bad_token, sizeof(bad_token)) < 0) {
                fprintf(stderr, "Invalid lead: %s\n", bad_token);
                usage(argv[0]);
                return 1;
            }

            i += 2;
        } else {
            fprintf(stderr, "Invalid argument: %s\n", argv[i]);
            usage(argv[0]);
            return 1;
        }
    }

    if (!InitSECG()) {
        fprintf(stderr, "InitSECG failed\n");
        return 1;
    }

    ResetSECG();

    /* Set every lead explicitly rather than relying on the reset default, so
       the selection is the same however the device was last left. */
    for (i = 0; i < LeadCount; i++) {
        if (SetOutputLead((OutputLead_E)i, lead_on[i] != 0) != 0) {
            fprintf(stderr, "SetOutputLead failed for lead %s\n", lead_names[i]);
            CloseSECG();
            return 1;
        }
    }

    if (SetOutputFunc(Output_Off) != 0) {
        fprintf(stderr, "SetOutputFunc failed\n");
        CloseSECG();
        return 1;
    }

    if (SetFrequency(frequency) != 0) {
        fprintf(stderr, "SetFrequency failed for value: %.2f Hz\n", frequency);
        CloseSECG();
        return 1;
    }

    if (SetAmplitude(amplitude) != 0) {
        fprintf(stderr, "SetAmplitude failed for value: %.2f mV\n", amplitude);
        CloseSECG();
        return 1;
    }

    if (pace_on) {
        if (SetPacingRate(pace_bpm) != 0) {
            fprintf(stderr, "SetPacingRate failed for value: %u bpm\n", pace_bpm);
            CloseSECG();
            return 1;
        }

        if (SetPacingAmplitude(pace_amp) != 0) {
            fprintf(stderr, "SetPacingAmplitude failed for value: %.2f mV\n", pace_amp);
            CloseSECG();
            return 1;
        }

        if (SetPacingDuration(pace_duration) != 0) {
            fprintf(stderr, "SetPacingDuration failed for value: %.2f ms\n", pace_duration);
            CloseSECG();
            return 1;
        }
    }

    if (SetOutputFunc(wave_shape) != 0) {
        fprintf(stderr, "SetOutputFunc failed\n");
        CloseSECG();
        return 1;
    }

    printf("Outputting %s wave at %.2f Hz and %.2f mV amplitude on ", argv[1], frequency, amplitude);
    print_enabled_leads(stdout, lead_on);
    printf(". Press Ctrl+C to stop.\n");

    run_until_interrupted(shutdown_device);

    return 0;
}
