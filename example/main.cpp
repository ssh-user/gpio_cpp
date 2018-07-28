// REMEMBER. Interrupts don't work at PC* & PD* pins. Look pins at picture GPIO.png

#include "gpio.h"


int main() {

    Gpio sensor(14);
    sensor.set_mode("in");
    sensor.set_edge("both");
    sensor.edge_start();

    return 0;
};


/*
 * // рабочий пример ШИМа.

    bool loop = true;
    int hz = 100;
    int rate = 1;

    Gpio led(13);
    led.set_mode("out");

    thread t(&Gpio::pwm, &led, hz, &rate, &loop);

    for(int i = 1; i < 101; i++){
        rate = i;
        this_thread::sleep_for(chrono::milliseconds(50));
        if(i == 100){
            loop = false;
        }
    };

    t.join();



    // показуха\тестирование
    cout << "Base configuration: " << endl << endl;

    Gpio led(13);
    cout << "gpio pin - " << led.get_pin() << endl;
    cout << "gpio mode - " << led.get_mode() << endl;
    cout << "gpio value - " << led.get_value() << endl;
    cout << "gpio edge - " << led.get_edge() << endl;
    cout << "gpio active_low - " << led.get_active_low() << endl;

    this_thread::sleep_for(chrono::seconds(2));

    cout << "Changed configuration: " << endl << endl;

    led.set_value(1);
    led.set_mode("out");
    led.set_edge("both");
    led.set_active_low(1);

    cout << "gpio mode - " << led.get_mode() << endl;
    cout << "gpio value - " << led.get_value() << endl;
    cout << "gpio edge - " << led.get_edge() << endl;
    cout << "gpio active_low - " << led.get_active_low() << endl;

    this_thread::sleep_for(chrono::seconds(2));
 */
