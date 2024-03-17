/**
 * @file debug.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-07-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once
#ifdef __cplusplus
extern "C"
{
#endif
/**
 * @brief Wraps a function, or a line (so that you can return and define a variable) and measure it and display the time it took, identifier is a descriptive name for it
 *
 */
#ifdef GN_BENCHMARKING_ENABLED
    extern uint64_t gGN_TIMER_TIC;
    extern uint64_t gGN_TIMER_TOC;
#define TIMED_BLOCK(code, identifier)     \
                                          \
    gGN_TIMER_TIC = SDL_GetTicks64();     \
    code                                  \
        gGN_TIMER_TOC = SDL_GetTicks64(); \
    LogInfo("Elapsed: %llu mseconds for id, %s", gGN_TIMER_TOC - gGN_TIMER_TIC, identifier);
#else
#define TIMED_BLOCK(code, identifier) code
#endif
    /**
     * @brief The level that we should show debug events at.
     *
     */
    typedef enum LogLevel
    {
        Log_LDefault = 0,
        Log_LDebug = 1,
        Log_LInfo = 2,
        Log_LWarn = 3,
        Log_LError = 4,
        Log_LCritical = 5,
    } LogLevel;
    /**
     * @brief Opens and/or creates a file for debug logging.
     *
     * @return 1 if successful, 0 if failed.
     */
    int InitializeDebugLogFile();
    /**
     * @brief Closes the open file for logging.
     *
     * @return
     */
    int CloseDebugLogFile();
    /**
     * @brief Log a Debug log in a printf format
     *
     * @param format The printf styped text
     * @param ... The variables for the printf
     */
    void LogDebug(const char *format, ...);
    /**
     * @brief Log a warning log in a printf format
     * @param format The printf styped text
     * @param ... The variables for the printf
     */
    void LogInfo(const char *format, ...);
    /**
     * @brief Log a warning log in a printf format
     * @param format The printf styped text
     * @param ... The variables for the printf
     */
    void LogWarn(const char *format, ...);
    /**
     * @brief Log a error log in a printf format
     * @param format The printf styped text
     * @param ... The variables for the printf
     */
    void LogError(const char *format, ...);
    /**
     * @brief Log a critical log in a printf format, and then exit
     * @param format The printf styped text
     * @param ... The variables for the printf
     */
    void LogCritical(const char *fmt, ...);

    void SetLogLevel(int newLevel);
#ifdef __cplusplus
}
#endif