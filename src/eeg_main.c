/**
 * Copyright (c) 2026 Byteflies
 *
 * whalewave: drive a WhaleTeq SEEG 100 / 100E EEG system.
 */

#include <stdio.h>

#include "SEEG100.h"

#include "argparse.h"
#include "run.h"

/* Connect() takes an unsigned port number, where -1 selects a port
   automatically. Only one device is expected to be attached at a time. */
#define AutoSelectPort ((unsigned)-1)
#define ConnectTimeoutMs 5000

static const enum_name wave_shapes[] = {
    { "off",             EEG_Output_Off },
    { "sine",            EEG_Output_Sine },
    { "triangle",        EEG_Output_Triangle },
    { "square",          EEG_Output_Square },
    { "rectanglepulse",  EEG_Output_RectanglePulse },
    { "rectangle_pulse", EEG_Output_RectanglePulse },
    { "trianglepulse",   EEG_Output_TrianglePulse },
    { "triangle_pulse",  EEG_Output_TrianglePulse },
    { "hysteresis",      EEG_Output_Hysteresis },
    { "playwaveform",    EEG_Output_PlayWaveform },
    ENUM_NAME_END
};

static void usage(const char* prog_name)
{
    fprintf(stderr, "Usage: %s <wave_shape> <frequency> <amplitude>\n", prog_name);
    fprintf(stderr, "Example: %s sine 10.0 100.0\n", prog_name);
    fprintf(stderr, "Supported wave shapes: ");
    print_enum_names(stderr, wave_shapes);
    fprintf(stderr, "\n");
    fprintf(stderr, "Frequency unit: Hz, Amplitude unit: uV\n");
}

static void shutdown_device(void)
{
    Close();
}

int main(int argc, char* argv[])
{
    int wave_shape_value;
    OutputFunctionEEG_E wave_shape;
    double frequency;
    double amplitude;

    if (argc != 4) {
        usage(argv[0]);
        return 1;
    }

    if (!parse_enum(wave_shapes, argv[1], &wave_shape_value)) {
        fprintf(stderr, "Invalid wave shape: %s\n", argv[1]);
        usage(argv[0]);
        return 1;
    }
    wave_shape = (OutputFunctionEEG_E)wave_shape_value;

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

    if (!Connect(AutoSelectPort, ConnectTimeoutMs)) {
        fprintf(stderr, "Connect failed\n");
        return 1;
    }

    /* Leaves the SDK default output lead selection in place: Lead_Ch1 on, all
       other channels off. */
    ResetSEEG();

    if (SetOutputFunc(EEG_Output_Off) != 0) {
        fprintf(stderr, "SetOutputFunc failed\n");
        Close();
        return 1;
    }

    if (SetFrequency(frequency) != 0) {
        fprintf(stderr, "SetFrequency failed for value: %s\n", argv[2]);
        Close();
        return 1;
    }

    if (SetAmplitude(amplitude) != 0) {
        fprintf(stderr, "SetAmplitude failed for value: %s\n", argv[3]);
        Close();
        return 1;
    }

    if (SetOutputFunc(wave_shape) != 0) {
        fprintf(stderr, "SetOutputFunc failed\n");
        Close();
        return 1;
    }

    printf("Outputting %s wave at %.2f Hz and %.2f uV amplitude. Press Ctrl+C to stop.\n", argv[1], frequency, amplitude);
    run_until_interrupted(shutdown_device);

    return 0;
}
