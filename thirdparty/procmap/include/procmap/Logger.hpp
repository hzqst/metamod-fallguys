/**
 * Copyright 2018 João Neto
 * A simple informative logger
 **/
#ifndef UNSTICKYMEM_LOGGER_HPP_
#define UNSTICKYMEM_LOGGER_HPP_

#include <unistd.h>

#include <ctime>
#include <cstring>
#include <cinttypes>
#include <cstdio>
#include <string>

#include "better-enums/enum.h"

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : \
                                               __FILE__)
#define LFMT "\033[2m%22s:%-4d %-28s\033[m "
#define LTRACE(msg) \
  L->trace(LFMT "%s", __FILENAME__, __LINE__, __FUNCTION__, msg);
#define LDEBUG(msg) \
  L->debug(LFMT "%s", __FILENAME__, __LINE__, __FUNCTION__, msg);
#define LINFO(msg) \
  L->info(LFMT "%s", __FILENAME__, __LINE__, __FUNCTION__, msg);
#define LWARN(msg) \
  L->warn(LFMT "%s", __FILENAME__, __LINE__, __FUNCTION__, msg);
#define LERROR(msg) \
  L->error(LFMT "%s", __FILENAME__, __LINE__, __FUNCTION__, msg);
#define LFATAL(msg) \
  L->fatal(LFMT "%s", __FILENAME__, __LINE__, __FUNCTION__, msg);

#define LTRACEF(fmt, ...) \
  L->trace(LFMT fmt, __FILENAME__, __LINE__, __FUNCTION__, __VA_ARGS__);
#define LDEBUGF(fmt, ...) \
  L->debug(LFMT fmt, __FILENAME__, __LINE__, __FUNCTION__, __VA_ARGS__);
#define LINFOF(fmt, ...) \
  L->info(LFMT fmt, __FILENAME__, __LINE__, __FUNCTION__, __VA_ARGS__);
#define LWARNF(fmt, ...) \
  L->warn(LFMT fmt, __FILENAME__, __LINE__, __FUNCTION__, __VA_ARGS__);
#define LERRORF(fmt, ...) \
  L->error(LFMT fmt, __FILENAME__, __LINE__, __FUNCTION__, __VA_ARGS__);
#define LFATALF(fmt, ...) \
  L->fatal(LFMT fmt, __FILENAME__, __LINE__, __FUNCTION__, __VA_ARGS__);

#define DIE(msg) \
  LFATAL(msg);\
  perror("perror");\
  ::abort();

#define DIEIF(expr, msg) \
  do {\
    if (expr) {\
      DIE(msg);\
    }\
  } while (0)

namespace procmap {

BETTER_ENUM(LogLevel, int, TRACE, DEBUG, INFO, WARN, ERROR, FATAL, OFF)

static const char *loglevel_colors[] = {
  "\033[37m",         // trace: white
  "\033[36m",         // debug: cyan
  "\033[32m",         // info: green
  "\033[43m\033[1m",  // warn: yellow bold
  "\033[31m\033[1m",  // error: red bold
  "\033[1m\033[41m",  // fatal: bold on red
  "\033[m"            // off: none
};

class Logger {
 private:
  struct timespec startTime;

 public:
  Logger() {
    clock_gettime(CLOCK_MONOTONIC, &startTime);
    printHeaderRow();
  }

  inline void set_log_level(LogLevel level) {
    _loglevel = level;
  }

  inline void printHeaderRow() {
    printf("\033[1m\033[41m%-15s %-6s %5s %27s %-28s %-74s\033[m\n",
           "TIME", "PID", "LEVEL", "FILENAME:LINE", "FUNCTION", "MESSAGE");
  }

  inline void printHorizontalRule(std::string msg = "", int bgcolor = 1) {
    printf("\033[4%dm%-160s\033[m\n", bgcolor % 8, msg.c_str());
  }

  template <typename Arg1, typename... Args>
  inline void trace(const char *fmt, const Arg1 &arg1, const Args &... args) {
    log(LogLevel::TRACE, fmt, arg1, args...);
  }

  template <typename Arg1, typename... Args>
  inline void debug(const char *fmt, const Arg1 &arg1, const Args &... args) {
    log(LogLevel::DEBUG, fmt, arg1, args...);
  }

  template <typename Arg1, typename... Args>
  inline void info(const char *fmt, const Arg1 &arg1, const Args &... args) {
    log(LogLevel::INFO, fmt, arg1, args...);
  }

  template <typename Arg1, typename... Args>
  inline void warn(const char *fmt, const Arg1 &arg1, const Args &... args) {
    log(LogLevel::WARN, fmt, arg1, args...);
  }

  template <typename Arg1, typename... Args>
  inline void error(const char *fmt, const Arg1 &arg1, const Args &... args) {
    log(LogLevel::ERROR, fmt, arg1, args...);
  }

  template <typename Arg1, typename... Args>
  inline void fatal(const char *fmt, const Arg1 &arg1, const Args &... args) {
    log(LogLevel::FATAL, fmt, arg1, args...);
  }

  template <typename T>
  inline void trace(const T &msg) {
    log(LogLevel::TRACE, msg);
  }

  template <typename T>
  inline void debug(const T &msg) {
    log(LogLevel::DEBUG, msg);
  }

  template <typename T>
  inline void info(const T &msg) {
    log(LogLevel::INFO, msg);
  }

  template <typename T>
  inline void warn(const T &msg) {
    log(LogLevel::WARN, msg);
  }

  template <typename T>
  inline void error(const T &msg) {
    log(LogLevel::ERROR, msg);
  }

  template <typename T>
  inline void fatal(const T &msg) {
    log(LogLevel::FATAL, msg);
  }

 private:
  LogLevel _loglevel = LogLevel::TRACE;

  inline bool should_log(LogLevel lvl) {
    return lvl >= _loglevel;
  }

  inline void getElapsedTimeString(char *str, size_t len) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    struct timespec elapsed;
    elapsed.tv_sec = now.tv_sec - startTime.tv_sec;
    elapsed.tv_nsec = now.tv_nsec - startTime.tv_nsec;
    if (now.tv_nsec < startTime.tv_nsec) {
      elapsed.tv_sec -= 1;
      elapsed.tv_nsec += 1000000000;
    }
    snprintf(str, len, "%05" PRIdMAX ".%09ld", elapsed.tv_sec, elapsed.tv_nsec);
  }

  template <typename... Args>
  inline void log(LogLevel lvl, const char *fmt, const Args &... args) {
    if (!should_log(lvl)) {
      return;
    }
    char time_str[32];
    getElapsedTimeString(time_str, sizeof(time_str));
    printf("\033[2m%s \033[4%dm%6d\033[m %s%5s\033[m ",
           time_str,
           getpid() % 8, getpid(),
           loglevel_colors[lvl], lvl._to_string());
    printf(fmt, args...);
    printf("\n");
  }

  template <typename... Args>
  inline void log(LogLevel lvl, const char *msg) {
    log(lvl, "%s", msg);
  }
};

}  // namespace procmap

extern procmap::Logger* L;

#endif  // UNSTICKYMEM_LOGGER_HPP_
