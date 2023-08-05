#include<iostream>
#include "ThreadPool.h"

int main()
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