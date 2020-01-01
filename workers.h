#pragma once

#include <fstream>
#include <iostream>
#include <queue>
#include "figure.h"

template<class T>
class EventChannel
{
public:
    struct Event
    {
        bool file;
        Ngon<T> &ngon;

        Event(bool f, Ngon<T> &n) : file(f), ngon(n) {}
    };

    EventChannel()
    {
        //lambda for checking existance of file
        auto exists = [](int i)
        {
            std::ifstream f(std::to_string(i) + ".txt");
            return f.good();
        };
        int path;
        while(exists(path = rand())); //generate unique filename
        m_ofs_file.open(std::to_string(path) + ".txt"); //open file for writing

        //create workers
        m_consoler = std::make_shared<Worker>(Worker(std::cout));
        m_filer = std::make_shared<Worker>(Worker(m_ofs_file));
    }
    ~EventChannel()
    {
        m_ofs_file.close();
    }

    //add event
    void Push(bool file, Ngon<T> &ngon) { m_events.push(Event(file, ngon)); }
    //execute event
    bool Pop()
    {
        if (m_events.empty()) return false; //no more events

        Event e = m_events.front(); //get event
        m_events.pop(); //remove from queue
        (e.file ? *m_filer : *m_consoler)(name(e.ngon))(": ")(e.ngon)("\n"); 

        return true;
    } 

private:
    //subscriber class
    class Worker
    {
    public:
        Worker(std::ostream &o) : m_out(o) {}
        template<class U> Worker& operator()(U data) { m_out << data; return *this; }
        
    private:
        std::ostream &m_out;
    };
    //subscriber themselves
    std::shared_ptr<Worker> m_consoler, m_filer;
    //queue of events
    std::queue<Event> m_events;

    //stream for file
    std::ofstream m_ofs_file;
};
