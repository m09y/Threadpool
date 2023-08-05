#ifndef THREAD_POOL_H
#define THREAD_POOL_H
#include <functional>
#include <future>
#include <iostream>
#include <queue>
#include <thread>
#include <vector>
#include <mutex>

class IThreadPoolTask
{
    public:
    virtual int execute() = 0;
};

class MyTask : public IThreadPoolTask
{
    int val;
    int res;
    public:
    MyTask(int x):val(x){}

    virtual int execute()
    {
        res = val * val; 
        std::cout << "Execute : "<<ret_res()<<std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(val));
        return 0;
    }
    int ret_res(){return res;}
    int ret_val(){return val;}
};


class ThreadPool
{
  private:
    int numWorkerThreads;
    std::vector<std::thread> workerThreads;
    std::queue<std::shared_ptr<IThreadPoolTask>> taskPipeline;

    std::mutex mutex;
    std::condition_variable cv;
    bool completed;
    bool startThreadPool;

    int initThreadPool()
    {
        for (size_t i = 0; i < numWorkerThreads; ++i)
        {
            std::thread worker([this]() 
            {
                while (true)
                {
                    std::shared_ptr<IThreadPoolTask> cur_task;
                    {
                        std::unique_lock<std::mutex> lck(mutex);
                        cv.wait(lck, 
                                [this]() { return completed || !taskPipeline.empty(); }
                                );
                        if (completed && taskPipeline.empty())return;

                        cur_task = taskPipeline.front();
                        taskPipeline.pop();
                    }
                    cur_task->execute();
                }
            });
            workerThreads.emplace_back(std::move(worker));
        }
        return 0;
    }

  public:
    ThreadPool(const ThreadPool &) = delete;
    ThreadPool(ThreadPool &&) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;
    ThreadPool &operator=(ThreadPool &&) = delete;


    ThreadPool(size_t numthreads , bool startThreadPool = true) :startThreadPool(startThreadPool), numWorkerThreads(numthreads),completed(false)
    {
        if(startThreadPool)
        {
            initThreadPool();
        } 
    }
    
    int WaitForTask() 
    {
        if(!startThreadPool)
        {
            throw std::runtime_error("The Thread Pool has not been started.");
        }
        for (auto &worker : workerThreads) 
        {
            worker.join();
        }

        return 0;

    }

    int WaitForCompletion() {
        if(!startThreadPool)
        {
            throw std::runtime_error("The Thread Pool has not been started.");
        }

        {
            std::unique_lock<std::mutex> lck(mutex);
            completed = true;
        }
        cv.notify_all();
        for (auto &worker : workerThreads) 
        {
            worker.join();
        }

        return 0;
    }

    virtual ~ThreadPool() {
        if(!completed) WaitForCompletion();
    }

    std::shared_ptr<IThreadPoolTask> enqueue(std::shared_ptr<IThreadPoolTask> t)
    {
        std::unique_lock<std::mutex> lck(mutex);
        if (completed) 
        {
            throw std::runtime_error("The Thread Pool has been completed.");
        }

        taskPipeline.emplace(t);
        cv.notify_one();
        return t;
    }

};
#endif
  