#ifndef BACKGROUNDTASK_HPP_INCLUDED
#define BACKGROUNDTASK_HPP_INCLUDED

#include <future>

class BackgroundTask
{
    private:
        void (*Func)(void*);
        void* Data = nullptr;
        std::future<void> Task;
        bool TaskStarted = false;
    public:
        BackgroundTask(void (*Func)(void*), void* Data);
        void Start();
        void Wait();
        bool Started() const;
        bool Running() const;
        bool Finished() const;
        operator bool() const;
        virtual ~BackgroundTask();
};

#endif // BACKGROUNDTASK_HPP_INCLUDED
