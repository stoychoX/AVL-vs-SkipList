#include<thread>
#include<chrono>
 
struct Timer
{
    std::chrono::time_point<std::chrono::steady_clock> start, end;
    std::chrono::duration<float> duration;
 
    Timer() {
        start = std::chrono::steady_clock::now();
    }
 
    ~Timer() {
        end = std::chrono::steady_clock::now();
        duration = end - start;
        float output = duration.count() * 1.000f;
        std::cout << "executed for: " << output << " ms"<<std::endl;
    }
};
 
void timedFunction() {
    Timer time;
    /*
    
        code
            
    */
}
