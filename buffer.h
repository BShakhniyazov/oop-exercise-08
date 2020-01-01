#pragma once

#include <functional>
#include <thread>
#include "factory.h"
#include "workers.h"

//document
template<class T>
class Buffer
{
public:
    Buffer(int maxSize)
    {
        figures.reserve(maxSize);
        srand((unsigned int)time(NULL));
    }
    //clear buffer
    void Clear()
    {
        figures.clear();
    }
    //add figure
    void Add(int n, T ox, T oy, T ax, T ay)
    {
        figures.push_back(Factory<T>::Generate(n, ox, oy, ax, ay));
        if (figures.size() == figures.capacity()) Export();
    }
    //show buffer
    void Show()
    {
        for (auto f : figures) std::cout << name(*f) << ": " << *f << std::endl;
    }
    //TASK
    void Export()
    {
        //start of export
        std::cout << "The export is beginning!" << std::endl
                  << "========================" << std::endl << std::endl;

        EventChannel<T> eventChannel; //create event channel
        //fill events
        for (auto pf : figures)
        {
            eventChannel.Push(false, *pf);
            eventChannel.Push(true, *pf);
        }

        //start thread
        std::thread eventThread([&eventChannel]()
        {
            while (eventChannel.Pop());
        });
        eventThread.join(); //wait

        //export ended
        std::cout << std::endl
            << "=======================" << std::endl
            << "The export is finished!" << std::endl;
        Clear(); //clear buffer
    }

private:
    std::vector<std::shared_ptr<Ngon<T>>> figures;
};
