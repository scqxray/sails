#ifndef _LOGGING_H_
#define _LOGGING_H_

#include <stdio.h>

namespace sails {
namespace common {
namespace log {

#define MAX_FILENAME_LEN 1000

class Logger {
public:
    enum LogLevel{
	DEBUG,
	INFO,
	WARN,
	ERROR
    };

    enum SAVEMODE{
	SPLIT_NONE,
	SPLIT_MONTH,
	SPLIT_DAY,
	SPLIT_HOUR
    };

    Logger(LogLevel level);
    Logger(LogLevel level, char *filename);
    Logger(LogLevel level, char *filename, SAVEMODE mode);
    
    void debug(char* format, ...);
    void info (char* format, ...);
    void warn (char* format, ...);
    void error(char* format, ...);
private:
    void output(Logger::LogLevel level, char* format, va_list ap);
    void set_msg_prefix(Logger::LogLevel level, char *msg);
    void set_filename_by_savemode(char* filename);

    LogLevel level;
    char filename[MAX_FILENAME_LEN];    
    SAVEMODE save_mode;
     
};

} // namespace log
} // namespace common
} // namespace sails

#endif /* _LOGGING_H_ */


