#include <iostream>
#include <random>
#include "TrafficLight.h"


/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
    std::unique_lock<std::mutex> unique_lock(_mutex);
    _condition.wait(unique_lock, [this]{return !_queue.empty();});
    T msg = std::move(_queue.back());
    _queue.pop_back();
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    std::lock_guard<std::mutex> lock_guard(_mutex);
    _queue.emplace_back(msg);
    _condition.notify_one();
}


/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    while(true)
    {
        TrafficLightPhase light_state = _light_state_messages.receive();
        std::lock_guard<std::mutex> lock_guard(_mutex);
        if(light_state == TrafficLightPhase::green)
            return;
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 
    int min_cycle = 4000;
    int max_cycle = 6000;
    std::default_random_engine random_generator;
    std::uniform_int_distribution<int> random_distribution(min_cycle, max_cycle);

    // generates a uniform distribution from the min(4000) and max(6000) values..
    int rand_cycle = random_distribution(random_generator);
    std::cout << "rando_cycle: " <<rand_cycle << std::endl;
    auto timer_start = std::chrono::system_clock::now();
    return;
    
    while (true)
    {
        // std::cout << "difference time" << (std::chrono::system_clock::now() - timer_start).count() << std::endl;
        if ((std::chrono::system_clock::now() - timer_start).count() >= rand_cycle)
        {
            _currentPhase = getCurrentPhase() == TrafficLightPhase::red ? TrafficLightPhase::green : TrafficLightPhase::red;
            timer_start = std::chrono::system_clock::now();
        } 
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

