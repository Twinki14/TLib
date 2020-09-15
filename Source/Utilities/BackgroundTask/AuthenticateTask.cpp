#include "../../../Include/Utilities/BackgroundTask/AuthenticateTask.hpp"
#include "TGotify.hpp"

BackgroundTask* AuthenticateTask::Task = nullptr;
bool AuthenticateTask::Succeeded = false;
WinnHttpComm::Response AuthenticateTask::Response;

void AuthenticateTask::RunTask(void *Data)
{
    const auto AuthPlugin = TGotify::Plugin(Host, Header, PluginID);
    const auto Auth = TGotify::Authenticator(AuthPlugin, Header, Script::GetName(), Token);
    AuthenticateTask::Succeeded = Auth.Authenticate(&AuthenticateTask::Response);
}

void AuthenticateTask::Start()
{
    if (!AuthenticateTask::Task)
    {
        AuthenticateTask::Task = new BackgroundTask(AuthenticateTask::RunTask, nullptr);
        AuthenticateTask::Task->Start();
    } else if (AuthenticateTask::Task->Finished())
    {
        delete AuthenticateTask::Task;
        AuthenticateTask::Task = nullptr;
        AuthenticateTask::Task = new BackgroundTask(AuthenticateTask::RunTask, nullptr);
        AuthenticateTask::Task->Start();
    }
}

bool AuthenticateTask::GetSucceeded()
{
    return AuthenticateTask::Succeeded;
}

WinnHttpComm::Response AuthenticateTask::GetResponse()
{
    return AuthenticateTask::Response;
}

void AuthenticateTask::TerminateTask()
{
    if (AuthenticateTask::Task)
    {
        AuthenticateTask::Task->Wait();
        delete AuthenticateTask::Task;
        AuthenticateTask::Task = nullptr;
    }
}
