#include"ProducerConsumer.h"

ProductQueue::~ProductQueue(){

}
void ProductQueue::push(int elem) {//元素入队：通知消费者
    {
        std::lock_guard<std::mutex> lock(mtx);
        que.push(elem);
    }
    cv.notify_one();  //在锁外通知，减少锁竞争
}
// 阻塞式出队（消费者线程使用）
bool ProductQueue::pop(int& out) {
    std::unique_lock<std::mutex> lock(mtx);
    // 等待队列非空 或 收到关闭信号
    cv.wait(lock, [this] { return !que.empty(); });
    
    if (que.empty()) return false;  // 队列空且已关闭，正常退出
    
    out = que.front();
    que.pop();
    return true;
}



void Producer::produceTask(std::atomic_bool& cont, ProductQueue& pq) const{//生产线程执行函数
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1500, 3000);
    std::uniform_int_distribution<> dist2(10, 30);

    while(true){
        if(!cont.load()) break;
        int sleepTime = dist(gen);
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
        int product = dist2(gen);
        pq.push(product);
        std::cout<<"thread:"<<std::this_thread::get_id()<<" push:"<<product<<std::endl;
    }
}   
    
Producer::Producer(ProductQueue& a):pq(a){
    this->cont.exchange(true);
}
Producer::~Producer(){
    end();
    if(pth.joinable()){
        pth.join();
    }
}
void Producer::start(){//线程启动
    pth = std::thread(&Producer::produceTask,this,std::ref(cont),std::ref(pq));
}
void Producer::end(){//线程内部函数停止
    cont.exchange(false);
}


void Consumer::consumeTask(std::atomic_bool& cont, ProductQueue& pq){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1500, 3000);

    while(true){
        if(!cont.load()) break;
        int sleepTime = dist(gen);
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
        int product;
        bool ret = pq.pop(product);
        if(!ret){
            std::cout<<"thread:"<<std::this_thread::get_id()<<" find no product now!"<<std::endl;
        }else{
            std::cout<<"thread:"<<std::this_thread::get_id()<<" consume:"<<product<<std::endl;
        }
    }
}

Consumer::Consumer(ProductQueue& a):pq(a){
    this->cont.exchange(true);
}
Consumer::~Consumer(){
    end();
    if(this->pth.joinable()){
        pth.join();
    }
}
void Consumer::start(){
    pth = std::thread(&Consumer::consumeTask,this,std::ref(cont),std::ref(pq));
}
void Consumer::end(){
    cont.exchange(false);
}