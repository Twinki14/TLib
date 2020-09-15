#ifndef AUTHENTICATETASK_HPP_INCLUDED
#define AUTHENTICATETASK_HPP_INCLUDED

#include "../../Tools/WinnHttpComm.hpp"
#include "BackgroundTask.hpp"
#include <atomic>

class AuthenticateTask
{
public:
    static void Start();
    static bool GetSucceeded();
    static WinnHttpComm::Response GetResponse();
    static BackgroundTask* Task;
    static void TerminateTask();

    inline static std::string Host;
    inline static std::string Token;
    inline static std::string Header;
    inline static std::uint32_t PluginID;
private:
    static bool Succeeded;
    static WinnHttpComm::Response Response;
    static void RunTask(void* Data);
};


#endif // AUTHENTICATETASK_HPP_INCLUDED
