#include "log.h"

#ifdef ENABLE_LOGS

#include <stdarg.h>
#include <stdio.h>

static const char *LOG_LEVEL_NAMES[] = {
    "ERROR",
    "WARN",
    "INFO",
    "DEBUG"
};

static const char *LOG_SUBSYSTEM_NAMES[] = {
    "CPU",
    "MEMORY",
    "DISPLAY",
    "TIMER",
    "STACK",
    "SYSTEM",
    "UNKNOWN"
};

void log_message(
    log_level_t     level,
    log_subsystem_t subsystem,
    const char     *file,
    int             line,
    const char     *fmt,
    ...
) {
    fprintf(
        stderr,
        "[%s:%s] %s:%d: ",
        log_level_name(level),
        log_subsystem_name(subsystem),
        file,
        line
    );

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, "\n");
}

const char *log_level_name(log_level_t level) {
    return LOG_LEVEL_NAMES[level];
}

const char *log_subsystem_name(log_subsystem_t subsystem) {
    return LOG_SUBSYSTEM_NAMES[subsystem];
}

#else

void log_message(
    log_level_t     level,
    log_subsystem_t subsystem,
    const char     *file,
    int             line,
    const char     *fmt,
    ...
) {
    (void)level;
    (void)subsystem;
    (void)file;
    (void)line;
    (void)fmt;
}

const char *log_level_name(log_level_t level) {
    (void)level;
    return "DISABLED";
}

const char *log_subsystem_name(log_subsystem_t subsystem) {
    (void)subsystem;
    return "DISABLED";
}

#endif
