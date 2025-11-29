#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <Arduino.h>
#include <vector>
#include <utility>
#include <functional>
#include <string>
#include <chrono>

class Communication {
private:
    std::vector<std::pair<float, float>> locations;  // Loaded from CSV
    size_t currentIndex;                         // Current position in the list
    unsigned long lastUpdateTime;                // Time tracking for updates
    
    void getLocations(); // Reads location data from CSV

public:
    Communication(); // Constructor

    void startSimulation();  // Resets timer and starts playback
    void update();           // Periodically calls the callback
    void stopSimulation();   // Stops updates

    void begin();
    std::function<void(std::pair<float, float>)> callback; // Function to call with each new location


    void onNewLocation(std::function<void(std::pair<float, float>)> callbackFunc); // Register callback

    bool calibrating = false;
    bool simulate;

    unsigned long lastReceiveTime = 0;
};

#endif // COMMUNICATION_H
