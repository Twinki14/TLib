#include <Game/Tools/Camera.hpp>
#include <Core/Math.hpp>
#include "../../../Include/Utilities/BackgroundTask/CameraTask.hpp"

CameraTask::CameraTaskData  CameraTask::Data;
BackgroundTask* CameraTask::Task = nullptr;

void CameraTask::RunTask(void *Data)
{
    auto D = (CameraTask::CameraTaskData*) Data;
    if (D)
    {
        if (UniformRandom() <= 0.50)
        {
            if (D->Pitch > -1) Camera::SetPitch(D->Pitch);

            if (D->T.IsPositive())
                Camera::RotateTo(D->T, D->Angle, 50);
            else
                Camera::RotateTo(D->Angle, 50);
        } else
        {
            if (D->T.IsPositive())
                Camera::RotateTo(D->T, D->Angle, 50);
            else
                Camera::RotateTo(D->Angle);

            if (D->Pitch > -1) Camera::SetPitch(D->Pitch);
        }
    }
}

void CameraTask::Start(const CameraTask::CameraTaskData& Data)
{
    CameraTask::Data = Data;
    if (!CameraTask::Task)
    {
        CameraTask::Task = new BackgroundTask(CameraTask::RunTask, &CameraTask::Data);
        CameraTask::Task->Start();
    } else if (CameraTask::Task->Finished())
    {
        delete CameraTask::Task;
        CameraTask::Task = nullptr;
        CameraTask::Task = new BackgroundTask(CameraTask::RunTask, &CameraTask::Data);
        CameraTask::Task->Start();
    }
}

void CameraTask::TerminateTask()
{
    if (CameraTask::Task)
    {
        CameraTask::Task->Wait();
        delete CameraTask::Task;
        CameraTask::Task = nullptr;
    }
}
