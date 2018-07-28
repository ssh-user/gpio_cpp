#ifndef BLINK_GPIO_H
#define BLINK_GPIO_H


#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <thread>
#include <chrono>

// special for edge/epoll
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

using namespace std;

class Gpio {

private:
    int _pin;
    string _path;

public:
    //constructor/destructor
    Gpio(int pin);
    ~Gpio();

    // getters
    int get_pin();
    string get_mode();
    int get_value();
    string get_edge();
    int get_active_low();

    // setters
    void set_mode(string mode);
    void set_value(int value);
    void set_edge(string edge);
    void set_active_low(int level);

    // another methods
    void pwm(int hz, int *rate, bool *loop);
    int edge_start();
};


#endif //BLINK_GPIO_H
