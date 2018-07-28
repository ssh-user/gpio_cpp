#include "gpio.h"

using namespace std;


// constructor
Gpio::Gpio(int pin) : _pin(pin), _path("/sys/class/gpio/") {
    ofstream out(_path + "export");

    if (!out.is_open()) cerr << "Constructor - Error. Can't open file EXPORT." << endl;

    out << pin;
    out.close();
};

// destructor
Gpio::~Gpio() {
    this->set_value(0);
    this->set_edge("none");
    this->set_active_low(0);
    this->set_mode("in");

    ofstream out(_path + "unexport");
    out << _pin;
    out.close();

};

// methods

int Gpio::get_pin() {
    return _pin;
};

// get currently pins mode like IN or OUT
string Gpio::get_mode() {
    string mode;

    ifstream in(_path + "gpio" + to_string(_pin) + "/direction");

    if (!in.is_open()) cerr << "GET_MODE - Error. Can't open file to read gpio direction." << endl;

    in >> mode;
    in.close();

    return mode;
};

// set mode to pin
void Gpio::set_mode(string mode) {
    transform(mode.begin(), mode.end(), mode.begin(), ::tolower);
    if (mode == "out" || mode == "in") {
        ofstream out(_path + "gpio" + to_string(_pin) + "/direction", ios_base::trunc);

        if (!out.is_open()) cerr << "SET_MODE - Error. Can't open file to write gpio direction." << endl;

        out << mode;
        out.close();
    } else {
        cerr << "SET_MODE - Error. Wrong incoming data. Only 'in' or 'out'." << endl;
    };
};

// get information about pins voltage. Is it 0 or 3.3v
int Gpio::get_value() {
    int value;

    ifstream in(_path + "gpio" + to_string(_pin) + "/value");

    if (!in.is_open()) cerr << "GET_VALUE - Error. Can't open file to read gpio value." << endl;

    in >> value;
    in.close();

    return value;
};

// set pins voltage. 0 - zero. 1 - 3.3v
void Gpio::set_value(int value) {
    ofstream out(_path + "gpio" + to_string(_pin) + "/value", ios_base::trunc);

    if (!out.is_open()) cerr << "SET_VALUE - Error. Can't open file to write gpio value." << endl;

    out << value;
    out.close();
};

// get information about pins interrupt. Is it turn off (none) or on (rising\falling\both)
string Gpio::get_edge() {
    string edge;

    ifstream in(_path + "gpio" + to_string(_pin) + "/edge");

    if (!in.is_open()) cerr << "GET_EDGE - Error. Can't open file to read gpio edge." << endl;

    in >> edge;
    in.close();

    return edge;
};

// set interrupt mode like none\rising\falling\both
void Gpio::set_edge(string edge) {
    transform(edge.begin(), edge.end(), edge.begin(), ::tolower);

    if (edge == "none" || edge == "rising" || edge == "falling" || edge == "both") {
        ofstream out(_path + "gpio" + to_string(_pin) + "/edge", ios_base::trunc);

        if (!out.is_open()) cerr << "SET_EDGE - Error. Can't open file to write gpio edge." << endl;

        out << edge;
        out.close();
    } else {
        cerr << "SET_EDGE - Error. Wrong incoming data. Only 'none', 'rising', 'falling' or 'both'" << endl;
    };
};


// get information about active mode. Is it direct. 0 - normal, 1 - invert
int Gpio::get_active_low() {
    int level;

    ifstream in(_path + "gpio" + to_string(_pin) + "/active_low");

    if (!in.is_open()) cerr << "GET_ACTIVE_LOW - Error. Can't open file to read gpio active_low." << endl;

    in >> level;
    in.close();

    return level;
};

// set active mode. When set 1, VALUE will be invert, so 1 - zero, 0 - 3.3v
void Gpio::set_active_low(int level) {
    ofstream out(_path + "gpio" + to_string(_pin) + "/active_low", ios_base::trunc);

    if (!out.is_open()) cerr << "SET_ACTIVE_LOW - Error. Can't open file to write gpio active_low." << endl;

    out << level;
    out.close();
};

// PWM. Firstly it's a SOFT pwm, not a hardware.
// I'm not sure, that it's write correctly and unfortunately I haven't a special device to check signal stability (oscilloscope).
// Arguments:
// Hertz(int hz) - the number of oscillations (turn on and off) per second. 100 is optimal.
// Rate (int *rate) - amperage in percent from 0 to 100.
// Loop (bool *loop) - this is to control the cycle, change the value to stop.
void Gpio::pwm(int hz, int *rate, bool *loop) {
    if (hz >= 0 && hz <= 500 && *rate >= 0 && *rate <= 100) {

        const double _hz = (1 / (double) hz) * 1000;

        int t_on;
        int t_off;
        double _rate;

        while (*loop) {

            _rate = (double) *rate / 100;

            t_on = (int) ((_hz - _hz / (_rate + 1)) * 1000);
            t_off = (int) ((_hz / (_rate + 1)) * 1000);

            this->set_value(1);
            this_thread::sleep_for(chrono::microseconds(t_on));

            this->set_value(0);
            this_thread::sleep_for(chrono::microseconds(t_off));
        };

    } else {
        cerr << "PWM - Error. Wrong incoming data. Should be special ranges: HZ from 0 to 500. Rate from 0 to 100." << endl;
    };

};

// this code is working, but requires some improves.
// at this moment I'm not sure which it should be.
// Maybe callback or some terminate mechanism or something else. Will see.
int Gpio::edge_start() {
    int fd, i, epollfd;
    int count = 0;
    struct epoll_event ev;


    epollfd = epoll_create(1);
    if (epollfd == -1) {
        cerr << "EPOLL - Error. Can't create epollfd." << endl;
        return -1;
    };

    if ((fd = open((_path + "gpio" + to_string(_pin) + "/value").c_str(), O_RDONLY | O_NONBLOCK)) == -1) {
        cerr << "EPOLL - Error. Can't open file." << endl;
        return -1;
    };

    //ev.events = read operation | edge triggered | urgent data
    ev.events = EPOLLIN | EPOLLET | EPOLLPRI;

    // attach to file file descriptor
    ev.data.fd = fd;

    //Register the file descriptor on the epoll instance, see: man epoll_ctl
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        cerr << "EPOLL - Error. Can't add control interface." << endl;
        return -1;
    };


    while (count <= 1) {  // ignore the first trigger
        i = epoll_wait(epollfd, &ev, 1, -1);

        if (i == -1) {
            cerr << "EPOLL - Error. Poll Wait fail." << endl;
            count = 5; // terminate loop
        } else {
            count++; // count the triggers up
        };
    };

    close(fd);

    cout << "Interrupt - called and close." << endl;

    return 0;
};

