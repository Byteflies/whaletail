/**
 * Copyright (c) 2026 Byteflies
 *
 * Idle loop and shutdown. Device agnostic: must not include an SDK header.
 */

#ifndef RUN_H
#define RUN_H

/**
 * Idle until the process is interrupted (SIGINT, e.g. Ctrl+C) or terminated
 * (SIGTERM), then call shutdown and return. The device keeps generating its
 * configured output for the whole time this is blocked.
 *
 * @param[in] shutdown  device teardown, called once before returning
 */
void run_until_interrupted(void (*shutdown)(void));

#endif /* RUN_H */
