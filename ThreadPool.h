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
        
#if 0
void test_mythreadpool()
{
    ThreadPool threadpool(4);

    std::shared_ptr<MyTask> task1 = std::make_shared<MyTask>(8);
    //threadpool.enqueue(task1);
    
    std::shared_ptr<MyTask> task2 = std::make_shared<MyTask>(2);
    //threadpool.enqueue(task2);

    threadpool.WaitForTask();
    threadpool.WaitForCompletion();

    std::cout <<"Result : "<< task1->ret_res()<< std::endl;
    std::cout <<"Result : "<< task2->ret_res()<< std::endl;
}


class ThreadPool
{
  private:
    std::vector<std::thread> workerThreads;
    std::queue<std::function<void()>> taskPipeline;

    /* For sync usage, protect the `taskPipeline` queue and `completed` flag. */
    std::mutex mutex;
    std::condition_variable cv;
    bool completed;

  public:
    ThreadPool(const ThreadPool &) = delete;
    ThreadPool(ThreadPool &&) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;
    ThreadPool &operator=(ThreadPool &&) = delete;

    ThreadPool(size_t nr_threads) : completed(false)
    {
        for (size_t i = 0; i < nr_threads; ++i)
        {
            std::thread worker([this]() {
                while (true)
                {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lck(mutex);
                        cv.wait(lck, [this]() { return completed || !taskPipeline.empty(); });
                        if (completed && taskPipeline.empty())   return;
                        /* even if completed = 1, once taskPipeline is not empty, then
                        * excucte the task until taskPipeline queue become empty
                        */
                        task = std::move(taskPipeline.front());
                        taskPipeline.pop();
                    }
                    task();
                }
                        });
            workerThreads.emplace_back(std::move(worker));
        }
    }
    
    virtual ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lck(mutex);
            completed = true;
        }
        cv.notify_all();
        for (auto &worker : workerThreads) worker.join();
    }

    
    template <class F, class... Args>
    std::future<std::result_of_t<F(Args...)>> enqueue(F &&f, Args &&...args)
    {
        /* The return type of task `F` */
        using return_type = std::result_of_t<F(Args...)>;
        
        /* wrapper for no arguments */
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lck(mutex);

            if (completed)
                throw std::runtime_error("The thread pool has been completed.");
            
            /* wrapper for no returned value */
            taskPipeline.emplace([task]() -> void { (*task)(); });
        }
        cv.notify_one();
        return res;
    }
        
    template <class F, class... Args>
    std::future<std::result_of_t<F(Args...)>> enqueue(F &&f, Args &&...args)
    {
        /* The return type of task `F` */
        using return_type = std::result_of_t<F(Args...)>;
        
        /* wrapper for no arguments */
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lck(mutex);

            if (completed)
                throw std::runtime_error("The thread pool has been completed.");
            
            /* wrapper for no returned value */
            taskPipeline.emplace([task]() -> void { (*task)(); });
        }
        cv.notify_one();
        return res;
    }

};
#endif