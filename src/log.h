#ifndef __SYLAR_LOG_H__
#define __SYLAR_LOG_H__

#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <sstream>
#include <fstream>

namespace sylar {

//日志事件
class LogEvent {
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent();
private:
    const char *m_file = nullptr;   //文件名
    int32_t m_line = 0;             //行号
    uint32_t m_elapser = 0;         //程序启动开始到现在的毫秒数
    uint32_t m_threadid = 0;        //线程号
    uint32_t m_fiberid = 0;         //协程号
    uint64_t time = 0;              //时间戳
    std::string m_content;          //消息
};

//日志输出地
class LogAppender {
public:
    typedef std::shared_ptr<LogAppender> ptr;

    //纯虚函数
    virtual void log(LogLevel::Level level, LogEvent::ptr event) = 0;

    //日志输出的目的地有很多，所以用虚函数来写;析构函数设为虚函数，便于顺利释放
    virtual ~LogAppender() {};

    //设置日志格式
    void setFormatter(LogFormatter::ptr val) { m_formatter = val;}

    //获取日志格式
    LogFormatter::ptr getFormatter() const { return m_formatter;}
protected:
    //针对特定日志来定义日志级别(子类中要用到 m_level 所以为了继承使用(比如StdoutLogAppender和FileLogAppender)，设为protected)
    LogLevel::Level m_level;
    LogFormatter::ptr m_formatter;
};

//日志级别
class LogLevel {
public:
    enum Level {
        DEBUG = 1,  //最低级别
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };
};

//日志格式
class LogFormatter {
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    LogFormatter(const std::string & pattern);
    std::string format(LogEvent::ptr event);
private:
    class FormatItem {
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        virtual ~FormatItem() {}
        virtual void format(std::ostream &os,LogEvent::ptr event) = 0;

    };

    //用于pattern的解析
    void init();
private:
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items;
};

//日志器
class Logger {
public:
    typedef std::shared_ptr<Logger> ptr;
    Logger(const std::string& name = "root");
    void log(LogLevel::Level level, LogEvent::ptr event);
    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    //添加日志级别
    void addAppender(LogAppender::ptr appender);

    //删除日志级别
    void delAppender(LogAppender::ptr appender);

    //获取日志级别(const成员函数)
    LogLevel::Level getLevel() const { return m_level;}

    //设置日志级别
    void setLevel(LogLevel::Level val) { m_level = val;}
private:
    std::string m_name;                         //日志名称
    LogLevel::Level m_level;                    //日志器的日志级别，满足了级别的才会被输出
    std::list<LogAppender::ptr> m_appenders;    //Appender集合
};

//输出到控制台的Appender
class StdoutLogAppender
:public LogAppender {
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    void log(LogLevel::Level level, LogEvent::ptr event) override;
private:
};

//输出到文件的Appender
class FileLogAppender
:public LogAppender {
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string & filename);   //文件名
    void log(LogLevel::Level level, LogEvent::ptr event) override;
    bool reopen();//重新打开文件
private:
    std::string m_filename;
    std::ofstream m_filestream;
};


};

#endif