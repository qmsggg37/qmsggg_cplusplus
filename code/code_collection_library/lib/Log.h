// Copyright (C) 2018 qmsggg

#ifndef LOG_H
#define LOG_H
 
#include "Singleton.h"
#include <string>

class Log : public Singleton<Log>
{
public:
    enum Level
    {
        Emergency,
        Alert,
        Critical,
        Error,
        Warning,
        Notice,
        Info,
        Debug
    };
    
    Log();
    
    virtual ~Log();

    Level getMinimumLogLevel();

    void setMinimumLogLevel(Level level);

    void append(const char* str);

    void setIdent(const char* ident);

    const char* getIdent();

protected:
    Level m_minimumLogLevel;

    const char* m_ident;

    string m_outputBuffer;
};


#endif /* LOG_H */
