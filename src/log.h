#pragma once

typedef enum {
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
} log_level_t;

typedef enum {
    LOG_SUBSYS_CPU,
    LOG_SUBSYS_MEMORY,
    LOG_SUBSYS_DISPLAY,
    LOG_SUBSYS_TIMER,
    LOG_SUBSYS_STACK,
    LOG_SUBSYS_SYSTEM,
    LOG_SUBSYS_UNKNOWN,
} log_subsystem_t;

#ifdef ENABLE_LOGS

#define LOG_ERROR(subsystem, fmt, ...) \
    log_message(LOG_LEVEL_ERROR, subsystem, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARN(subsystem, fmt, ...) \
    log_message(LOG_LEVEL_WARN, subsystem, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_INFO(subsystem, fmt, ...) \
    log_message(LOG_LEVEL_INFO, subsystem, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(subsystem, fmt, ...) \
    log_message(LOG_LEVEL_DEBUG, subsystem, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#else

#define LOG_ERROR(fmt, ...) ((void)0)
#define LOG_WARN(fmt, ...)  ((void)0)
#define LOG_INFO(fmt, ...)  ((void)0)
#define LOG_DEBUG(fmt, ...) ((void)0)

#endif

/**
 * Logs a message to stderr.
 *
 * This is the core logging function used by the LOG_* macros. When ENABLE_LOGS
 * is defined, this will write the formatted message with the following format:
 *
 *     [LEVEL:SUBSYSTEM] file:line: message
 *
 * @param level - The severity level of the message
 * @param subsystem - The subsystem signaling the log
 * @param file - The source file where the log originated
 * @param line - The line number in the source file
 * @param fmt - The printf-style format string.
 * @param ... - Variable arguments for the format string.
 */
void log_message(
    log_level_t     level,
    log_subsystem_t subsystem,
    const char     *file,
    int             line,
    const char     *fmt,
    ...
);

/**
 * Decodes a log level into a string.
 *
 * @param level - The log level to decode
 * @return A constant string representation of the log level
 */
const char *log_level_name(log_level_t level);

/**
 * Decodes a subsystem into a string.
 *
 * @param subsystem - The subsystem to decode
 * @return A constant string representation of the subsystem
 */
const char *log_subsystem_name(log_subsystem_t subsystem);
