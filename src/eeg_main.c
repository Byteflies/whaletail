/**
 * Copyright (c) 2026 Byteflies
 *
 * whalewave: drive a WhaleTeq SEEG 100 / 100E EEG system.
 */

#include <stdio.h>
#include <strings.h>

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

/* Ch1 to Ch9 are the only terminals that can output a waveform; Ch10 to Ch22
   are tied to ground. One generator feeds every enabled channel, so selecting
   several of them puts the identical signal on each. */
#define ChannelCount 9

static const enum_name channels[] = {
    { "ch1", Lead_Ch1 },
    { "ch2", Lead_Ch2 },
    { "ch3", Lead_Ch3 },
    { "ch4", Lead_Ch4 },
    { "ch5", Lead_Ch5 },
    { "ch6", Lead_Ch6 },
    { "ch7", Lead_Ch7 },
    { "ch8", Lead_Ch8 },
    { "ch9", Lead_Ch9 },
    ENUM_NAME_END
};

static void usage(const char* prog_name)
{
    fprintf(stderr, "Usage: %s <wave_shape> <frequency> <amplitude> [channels <ch>[,<ch>...]]\n", prog_name);
    fprintf(stderr, "Example: %s sine 10.0 100.0 channels ch1,ch2\n", prog_name);
    fprintf(stderr, "Supported wave shapes: ");
    print_enum_names(stderr, wave_shapes);
    fprintf(stderr, "\n");
    fprintf(stderr, "Supported channels: ");
    print_enum_names(stderr, channels);
    fprintf(stderr, " (default ch1)\n");
    fprintf(stderr, "Frequency unit: Hz, Amplitude unit: uV\n");
}

/** Print the enabled channels as a comma separated list, without a newline. */
static void print_enabled_channels(FILE* stream, const int* channel_on)
{
    int i;
    int printed = 0;

    for (i = 0; i < ChannelCount; i++) {
        if (channel_on[i]) {
            fprintf(stream, "%sch%d", printed == 0 ? "" : ", ", i + 1);
            printed++;
        }
    }
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
    /* optional variables; the SDK default is Ch1 alone */
    int channel_on[ChannelCount] = { 1, 0, 0, 0, 0, 0, 0, 0, 0 };
    int i;

    if (argc != 4 && argc != 6) {
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

    if (argc == 6) {
        char bad_token[64];

        if (strcasecmp(argv[4], "channels") != 0) {
            fprintf(stderr, "Invalid argument: %s\n", argv[4]);
            usage(argv[0]);
            return 1;
        }

        if (parse_enum_set(channels, argv[5], channel_on, ChannelCount,
                           bad_token, sizeof(bad_token)) < 0) {
            fprintf(stderr, "Invalid channel: %s\n", bad_token);
            usage(argv[0]);
            return 1;
        }
    }

    if (!Connect(AutoSelectPort, ConnectTimeoutMs)) {
        fprintf(stderr, "Connect failed\n");
        return 1;
    }

    ResetSEEG();

    /* Set every channel explicitly rather than relying on the reset default,
       so the selection is the same however the device was last left. */
    for (i = 0; i < ChannelCount; i++) {
        if (SetOutputLead((OutputLead_E)i, channel_on[i] != 0) != 0) {
            fprintf(stderr, "SetOutputLead failed for channel ch%d\n", i + 1);
            Close();
            return 1;
        }
    }

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

    printf("Outputting %s wave at %.2f Hz and %.2f uV amplitude on ", argv[1], frequency, amplitude);
    print_enabled_channels(stdout, channel_on);
    printf(". Press Ctrl+C to stop.\n");

    run_until_interrupted(shutdown_device);

    return 0;
}
