#include "Log.hpp"


vfc::Log* vfc::Log::m_Instance{nullptr};
std::mutex vfc::Log::m_log_mutex;

vfc::Log::Log()
{
    m_File.open("../log/LogFile.log", std::ofstream::out | std::ofstream::app);

    m_File << "-------------------------------------------------------------------------------------------------------------\n";
    m_File << "                                                NEW LOG                                                      \n";
    m_File << "-------------------------------------------------------------------------------------------------------------\n";

    configure();
}

vfc::Log::~Log()
{
   m_File.close();
}

vfc::Log* vfc::Log::getInstance()
{
   if (m_Instance == nullptr) 
   {
      m_Instance = new Log();
   }
   return m_Instance;
}

void vfc::Log::lock(std::string const& data)
{
   // std::scoped_lock lock(m_log_mutex);
   std::lock_guard<std::mutex> lock(m_log_mutex);
   if(m_LogType == FILE)
      logIntoFile(data);
   else if(m_LogType == CONSOLE)
      logOnConsole(data);
   else if(m_LogType == ALL)
   {
      logOnConsole(data);
      logIntoFile(data);
   }
}

void vfc::Log::logIntoFile(std::string const& data)
{
   m_File << "[" << m_FileLogNumber << "]" << " " << "[" << getCurrentTime() << "]"<< " " << data << std::endl;
   m_FileLogNumber++;
}

void vfc::Log::logOnConsole(std::string const& data)
{
   std::cout << "[" << m_ConsoleLogNumber << "]" << " " << "[" << getCurrentTime() << "]" << " " << data << std::endl;
   m_ConsoleLogNumber++;
}

std::string vfc::Log::getCurrentTime()
{
   std::string currTime;
   //Current date/time based on current time
   time_t now = time(0); 
   // Convert current time to string
   currTime.assign(ctime(&now));
   // Last charactor of currentTime is "\n", so remove it
   std::string currentTime = currTime.substr(0, currTime.size()-1);
   return currentTime;
}

void vfc::Log::configure(LogMode log_mode, LogType log_type, LogLevel log_level)
{
   m_LogMode = log_mode;
   m_LogLevel = log_level;
   m_LogType = log_type;
}
void vfc::Log::configure(std::string log_mode, std::string log_type, std::string log_level)
{
   if(log_mode == "ENABLE" || log_mode == "enable" )
      m_LogMode = LOG_MODE::ENABLE;
   else if(log_mode == "DISABLE" || log_mode == "disable")
      m_LogMode = LOG_MODE::DISABLE;

   if(log_type == "FILE" || log_type == "file" )
      m_LogType = LOG_TYPE::FILE;
   else if(log_type == "CONSOLE" || log_type == "console")
      m_LogType = LOG_TYPE::CONSOLE;
   else if(log_type == "ALL" || log_type == "all")
      m_LogType = LOG_TYPE::ALL;

   if(log_level == "INFO" || log_level == "info" )
      m_LogLevel = LOG_LEVEL::INFO;
   else if(log_level == "WARNING" || log_level == "warning")
      m_LogLevel = LOG_LEVEL::WARNING;
   else if(log_level == "ERROR" || log_level == "error")
      m_LogLevel = LOG_LEVEL::ERROR;
   else if(log_level == "BUFFER" || log_level == "buffer")
      m_LogLevel = LOG_LEVEL::BUFFER;
   else if(log_level == "TRACE" || log_level == "trace")
      m_LogLevel = LOG_LEVEL::TRACE;
   else if(log_level == "DEBUG" || log_level == "debug")
      m_LogLevel = LOG_LEVEL::DEBUG;
}

void vfc::Log::logMode(LogMode log_mode)
{
   m_LogMode = log_mode; 
}

void vfc::Log::logLevel(LogLevel logLevel)
{
   m_LogLevel = logLevel;
}

void vfc::Log::logType(LogType LogType)
{
   m_LogType = LogType;
}