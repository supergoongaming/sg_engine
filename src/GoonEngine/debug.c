#include <GoonEngine/gnpch.h>
#include <GoonEngine/debug.h>

#define MAX_LOG_SIZE 200
#ifdef GN_BENCHMARKING_ENABLED
uint64_t gGN_TIMER_TIC = 0;
uint64_t gGN_TIMER_TOC = 0;
#endif

/**
 * @brief The file that will be written to when logs are put.
 *
 */
static FILE *openDebugFile = NULL;
/**
 * @brief The internal logging function that the others will end up calling.  Probably don't call it manually
 *
 * @param level The log level to log this as.
 * @param data_to_write The data to write to the log.
 */
static void Log(LogLevel level, const char *data_to_write);
/**
 * @brief The log level to log at, this should be sent in via settings.
 */
static LogLevel logLevel = Log_LInfo;
static const char *logFileName = "errors.log";

int InitializeDebugLogFile()
{
    LogDebug("Opening log file at %s", logFileName);
    openDebugFile = fopen(logFileName, "a");
    if (openDebugFile)
        return 1;
    LogError("Could not open file for logging!");
    return 0;
}

int CloseDebugLogFile()
{
    if (!openDebugFile)
        return 1;
    int result = fclose(openDebugFile);
    if (result)
        LogError("Couldn't close logging file.");
    return !result;
}
static void Log(LogLevel level, const char *thing_to_write)
{
    time_t current_time;
    time(&current_time);
    struct tm *gm_time = gmtime(&current_time);
    char buf[30];
    strftime(buf, sizeof(buf), "%m-%d-%H:%M-%S", gm_time);
    FILE *outStream = level == Log_LError ? stderr : stdout;
    fprintf(outStream, "%s: %s end\n", buf, thing_to_write);
    if (level == Log_LError && openDebugFile)
    {
        fprintf(openDebugFile, "%s: %s\n", buf, thing_to_write);
    }
}
static void LogSetup(LogLevel level, const char *fmt, va_list args)
{
    char buf[MAX_LOG_SIZE];
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    Log(level, buf);
}

static int ShouldLog(LogLevel level)
{
#ifdef GN_RELEASE_BUILD
    return false;
#endif
    return logLevel <= level;
}

void LogDebug(const char *fmt, ...)
{
    if (!ShouldLog(Log_LDebug))
        return;
    va_list args;
    va_start(args, fmt);
    LogSetup(Log_LDebug, fmt, args);
}

void LogInfo(const char *fmt, ...)
{
    if (!ShouldLog(Log_LInfo))
        return;
    va_list args;
    va_start(args, fmt);
    LogSetup(Log_LInfo, fmt, args);
}

void LogWarn(const char *fmt, ...)
{
    if (!ShouldLog(Log_LWarn))
        return;
    va_list args;
    va_start(args, fmt);
    LogSetup(Log_LWarn, fmt, args);
}

void LogError(const char *fmt, ...)
{
    if (!ShouldLog(Log_LError))
        return;
    va_list args;
    va_start(args, fmt);
    LogSetup(Log_LError, fmt, args);
}

void LogCritical(const char *fmt, ...)
{
    if (!ShouldLog(Log_LCritical))
        return;
    va_list args;
    va_start(args, fmt);
    LogSetup(Log_LCritical, fmt, args);
    fprintf(stderr, "Critical error, exiting!\n");
    exit(1);
}

void SetLogLevel(int newLevel)
{
    logLevel = newLevel;
}
