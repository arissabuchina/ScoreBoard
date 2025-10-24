#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <Arduino.h>
#include <vector>
#include <utility>
#include <functional>
#include <string>

class Communication {
private:
    std::vector<std::pair<int, int>> locations;  // Loaded from CSV
    size_t currentIndex;                         // Current position in the list
    unsigned long lastUpdateTime;                // Time tracking for updates
    std::function<void(std::pair<int, int>)> callback; // Function to call with each new location

    void getLocations(); // Reads location data from CSV

public:
    Communication(); // Constructor

    void startSimulation();  // Resets timer and starts playback
    void update();           // Periodically calls the callback
    void stopSimulation();   // Stops updates

    void onNewLocation(std::function<void(std::pair<int, int>)> callbackFunc); // Register callback
};

#endif // COMMUNICATION_H
