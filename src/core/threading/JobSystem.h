#pragma once

#include <functional>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <deque>

namespace Archura {

class JobSystem {
public:
    using Job = std::function<void()>;

    // Utility struct for parallel loops
    struct JobDispatchArgs {
        uint32_t jobIndex;
        uint32_t groupIndex;
    };

    static void Init();
    static void Shutdown();

    static void Execute(const Job& job);
    static void Dispatch(uint32_t jobCount, uint32_t groupSize, const std::function<void(JobDispatchArgs)>& job);
    
    static bool IsBusy();
    static void Wait();

private:
    static void WorkerThread();

    // shared state
    static std::vector<std::thread> s_WorkerThreads;
    static std::deque<Job> s_JobQueue;
    static std::mutex s_QueueMutex;
    static std::condition_variable s_Condition;
    static std::atomic<bool> s_Running;
    static std::atomic<uint32_t> s_ActiveJobs; // Tracks jobs currently running + in queue
};

} // namespace Archura
