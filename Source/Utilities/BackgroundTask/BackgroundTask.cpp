#include "../../../Include/Utilities/BackgroundTask/BackgroundTask.hpp"

BackgroundTask::BackgroundTask(void (*Func)(void*), void* Data)
{
    this->Func = Func;
    this->Data = Data;
}

void BackgroundTask::Start()
{
    this->Task = std::async(std::launch::async, Func, Data);
    this->TaskStarted = true;
}

void BackgroundTask::Wait()
{
    if (!this->Running())
        return;
    this->Task.wait();
}

bool BackgroundTask::Started() const
{
    return this->TaskStarted;
}

bool BackgroundTask::Running() const
{
    if (this->TaskStarted)
        return (this->Task.wait_for(std::chrono::microseconds(0)) != std::future_status::ready);
    else
        return false;
}

bool BackgroundTask::Finished() const
{
    return ((this->Started()) && (!this->Running()));
}

BackgroundTask::operator bool() const
{
    return Running();
}

BackgroundTask::~BackgroundTask()
{

}
