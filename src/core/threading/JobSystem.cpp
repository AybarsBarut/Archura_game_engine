#include "JobSystem.h"
#include <iostream>
#include <algorithm> // for std::max

namespace Archura {

std::vector<std::thread> JobSystem::s_WorkerThreads;
std::deque<JobSystem::Job> JobSystem::s_JobQueue;
std::mutex JobSystem::s_QueueMutex;
std::condition_variable JobSystem::s_Condition;
std::atomic<bool> JobSystem::s_Running = false;
std::atomic<uint32_t> JobSystem::s_ActiveJobs = 0;

void JobSystem::Init() {
    uint32_t numCores = std::thread::hardware_concurrency();
    uint32_t numWorkers = std::max(1u, numCores - 1);

    s_Running = true;

    for (uint32_t i = 0; i < numWorkers; ++i) {
        s_WorkerThreads.emplace_back(WorkerThread);
    }
    
#ifdef _DEBUG
    std::cout << "[JobSystem] Initialized with " << numWorkers << " worker threads.\n";
#endif
}

void JobSystem::Shutdown() {
    s_Running = false;
    
    // Wake up all threads
    s_Condition.notify_all();

    for (auto& thread : s_WorkerThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    s_WorkerThreads.clear();
}

void JobSystem::Execute(const JobSystem::Job& job) {
    s_ActiveJobs++;
    
    {
        std::lock_guard<std::mutex> lock(s_QueueMutex);
        s_JobQueue.push_back(job);
    }
    
    s_Condition.notify_one();
}

void JobSystem::Dispatch(uint32_t jobCount, uint32_t groupSize, const std::function<void(JobSystem::JobDispatchArgs)>& job) {
    if (jobCount == 0 || groupSize == 0) return;

    // TODO: Implement proper chunking for caching efficiency
    // Currently dispatching 1 job per index for simplicity
    
    for (uint32_t i = 0; i < jobCount; ++i) {
         // Copy args by value to lambda
        Execute([i, job]() {
            JobSystem::JobDispatchArgs args;
            args.jobIndex = i;
            args.groupIndex = 0;
            job(args);
        });
    }
}

bool JobSystem::IsBusy() {
    return s_ActiveJobs > 0;
}

void JobSystem::Wait() {
    while (IsBusy()) {
        std::this_thread::yield();
    }
}

void JobSystem::WorkerThread() {
    while (s_Running) {
        JobSystem::Job job;
        {
            std::unique_lock<std::mutex> lock(s_QueueMutex);
            s_Condition.wait(lock, []() { return !s_JobQueue.empty() || !s_Running; });
            
            if (!s_Running && s_JobQueue.empty()) {
                return;
            }
            
                continue;
            
            job = s_JobQueue.front();
            s_JobQueue.pop_front();
        }
        
        // Execute Job
        job();
        
        s_ActiveJobs--;
    }
}

} // namespace Archura
