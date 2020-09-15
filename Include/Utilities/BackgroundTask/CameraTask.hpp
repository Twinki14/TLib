#ifndef CAMERATASK_HPP_INCLUDED
#define CAMERATASK_HPP_INCLUDED

#include <Core/Types/Tile.hpp>
#include "BackgroundTask.hpp"

class CameraTask
{
public:
    typedef struct CameraTaskData
    {
        std::int32_t Angle = -1;
        std::int32_t Pitch = -1;
        Tile T = Tile();
    } CameraTaskData;

    static void Start(const CameraTaskData& Data);
    static CameraTaskData Data;
    static BackgroundTask* Task;
    static void TerminateTask();

private:
    static void RunTask(void* Data);
};

#endif // CAMERATASK_HPP_INCLUDED