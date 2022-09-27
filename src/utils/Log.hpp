#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <thread>
#include <mutex>
#include <ctime>

namespace vfc
{
   // Direct Interface for logging into log file or console using MACRO(s)
   #define LOG_INFO(x)           vfc::Log::getInstance()->info(x)
   #define LOG_WARNING(x)        vfc::Log::getInstance()->warning(x)
   #define LOG_ERROR(x)          vfc::Log::getInstance()->error(x)
   #define LOG_BUFFER(x)         vfc::Log::getInstance()->buffer(x)
   #define LOG_TRACE(x,y)        vfc::Log::getInstance()->trace(x,y)
   #define LOG_DEBUG(x,y)        vfc::Log::getInstance()->debug(x,y)

   #define LOG_UPDATE_MODE(x)           vfc::Log::getInstance()->logMode(x)
   #define LOG_UPDATE_TYPE(x)           vfc::Log::getInstance()->logType(x)
   #define LOG_UPDATE_LEVEL(x)          vfc::Log::getInstance()->logLevel(x)

   #define LOG_UPDATE_CONFIGURE(x,y,z)  vfc::Log::getInstance()->configure(x,y,z)


   typedef enum LOG_LEVEL
   {
       INFO	       = 1,
       WARNING     = 2,
       ERROR	    = 3,
       BUFFER      = 4,
       TRACE       = 5,
       DEBUG       = 6,
   } LogLevel;

   typedef enum LOG_TYPE
   {
       CONSOLE     = 1,
       FILE        = 2,
       ALL         = 3,
   }LogType;

   typedef enum LOG_MODE
   {
       ENABLE      = 1,
       DISABLE     = 2,
   }LogMode;

   class Log
   {
      private:
         static std::mutex       m_log_mutex;
         static Log*             m_Instance;
         std::ofstream           m_File;
         unsigned int            m_FileLogNumber=1;
         unsigned int            m_ConsoleLogNumber=1;
         LogLevel                m_LogLevel;
         LogType                 m_LogType;
         LogMode                 m_LogMode;

      public:
         static Log* getInstance();

         template<typename S> 
         void info(S text)  
         {
            if(m_LogMode==ENABLE)
            {               
               if(m_LogLevel >= INFO)
                  print(text, "[INFO]:    ");
            }
         }

         template<typename S> 
         void warning(S text)  
         {
            if(m_LogMode==ENABLE)
            {               
               if(m_LogLevel >= WARNING)
                  print(text, "[WARNING]: ");
            }
         }

         template<typename S> 
         void error(S text)  
         {
            if(m_LogMode==ENABLE)
            {               
               if(m_LogLevel >= ERROR)
                  print(text, "[ERROR]:   ");
            }
         }

         template<typename S> 
         void buffer(S text)  
         {
            if(m_LogMode==ENABLE)
            {               
               if(m_LogLevel >= BUFFER)
                  print(text, "[BUFFER]:  ");
            }
         }

         template<typename S, typename T> 
         void trace(S text, T value)  
         {
            if(m_LogMode==ENABLE)
            {               
               if(m_LogLevel >= TRACE)
                  print(text, "[TRACE]:   ", value);
            }
         }

         template<typename S, typename T> 
         void debug(S text, T value)  
         {
            if(m_LogMode==ENABLE)
            {               
               if(m_LogLevel >= DEBUG)
                  print(text, "[DEBUG]:   ", value);
            }
         }

         void logMode(LogMode log_mode);
         
         void logLevel(LogLevel logLevel);
         
         void logType(LogType LogType);

         void configure(LogMode log_mode=ENABLE, LogType log_type=FILE, LogLevel log_level=ERROR);
         void configure(std::string log_mode, std::string log_type, std::string log_level);

      private:
         template<typename S, typename T>
         void print(S text, T type)
         {
            std::string data;
            data.append(type);
            data.append("[\"");
            data.append(toString(text));
            data.append("\"]");
            lock(data);
         }

         template<typename STRING, typename VALUE>
         void print(STRING text, std::string const& type, VALUE value)
         {
            std::string data;
            data.append(type);
            data.append("[\"");
            data.append(text);
            data.append(toString(value));
            data.append("\"]");
            lock(data);
         }

         template<class T>
         std::string toString(const T& x) 
         {
            std::ostringstream ss;
               ss << x;
            return ss.str();
         }

         void lock(std::string const& data);

         void logIntoFile(std::string const& data);

         void logOnConsole(std::string const& data);


      protected:
         Log();

         ~Log();

         Log(const Log& obj)= delete;

         void operator=(const Log& obj)= delete;

         std::string getCurrentTime();  
   };


static vfc::Log* logger;

} // End of namespace
