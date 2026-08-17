/**
 * Copyright (c) 2026 Byteflies
 */

#include <signal.h>
#include <string.h>
#include <unistd.h>

#include "run.h"

static volatile sig_atomic_t stop_requested = 0;

static void on_stop_signal(int signum)
{
    (void)signum;
    stop_requested = 1;
}

void run_until_interrupted(void (*shutdown)(void))
{
    struct sigaction action;

    memset(&action, 0, sizeof(action));
    action.sa_handler = on_stop_signal;
    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);

    /*
     * The SDK has already started worker threads by this point, so the kernel
     * may deliver the signal to any one of them. Poll the flag once a second
     * rather than blocking in sigsuspend(), which would only wake up if this
     * thread happened to be the one that received the signal.
     */
    while (!stop_requested) {
        sleep(1);
    }

    if (shutdown != NULL) {
        shutdown();
    }
}
