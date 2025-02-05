#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

template <class Function>
class SafeQueue{
public:
    std::queue<Function> tasks{};
    std::mutex mutex{};
    std::condition_variable conditionVariable{};
    void Push(const Function& function){
        std::lock_guard<std::mutex> lockGuard{mutex};
        tasks.push(function);
        conditionVariable.notify_one();
    }
    void Pop(){
        std::unique_lock<std::mutex> uniqueLock{mutex};
        conditionVariable.wait(uniqueLock);
        tasks.pop();
        uniqueLock.unlock();
    }
    Function Front(){
        std::lock_guard<std::mutex> lockGuard{mutex};
        return tasks.front();
    }
};

template <class Function>
class ThreadPool{
public:
    std::vector<std::thread> threads{};
    SafeQueue<Function> works;
    ThreadPool(){
        threads.resize(std::thread::hardware_concurrency());
    }
    void Work(){
        std::thread thread{works.Front()};
        thread.join();
        works.Pop();
    }
    void Submit(Function function){
        works.Push(function);
    }
    ~ThreadPool(){
        threads.clear();
    }
};

void PrintNameFirst(){
    std::cout << "Dima" << std::endl;
}

int main(int argc, char* argv[]){
    ThreadPool<void()> threadPool();
    while(true){
        threadPool.Submit(PrintNameFirst());
    }
    return EXIT_SUCCESS;
}