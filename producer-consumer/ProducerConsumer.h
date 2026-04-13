#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include<atomic>
#include <chrono>
#include<random>

class ProductQueue{
private:
    mutable std::queue<int> que;
    std::mutex mtx;
    std::condition_variable cv;
public:
    ProductQueue() = default;
    ProductQueue(ProductQueue&);
    ~ProductQueue();
    void push(int elem);
    bool pop(int&);
};

class Producer{
private:
    std::thread pth;
    std::atomic_bool cont;
    ProductQueue& pq;
public:

    void produceTask(std::atomic_bool& cont, ProductQueue& pq) const;
    
    Producer(ProductQueue&);
    ~Producer();
    void start();
    void end();
};



class Consumer{
private:
    std::thread pth;
    std::atomic_bool cont;
    ProductQueue& pq;
public:

    void consumeTask(std::atomic_bool& cont, ProductQueue& pq); 

    Consumer(ProductQueue&);
    ~Consumer();
    void start();
    void end();
};
//test code
// int main()
// {
//     ProductQueue pq;
//     Producer p1(pq);
//     Producer p2(pq);
//     Consumer c1(pq);
//     p1.start();
//     p2.start();
//     c1.start();
//     while(true);
// }