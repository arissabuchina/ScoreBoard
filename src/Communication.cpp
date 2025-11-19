#include <Communication.h>
#include <Arduino.h>
#include <Game.h>
#include <vector>
#include <utility>

#include <WiFi.h>
#include <esp_now.h>




Communication::Communication() 
{
    //read in from txt file locations 
    getLocations();
    currentIndex = 0;
    lastUpdateTime = millis();
}

void Communication::getLocations()
{
    //301 Game - 3 Player - Player 2 Wins
    locations = { {0,0} ,
                  {0,0} ,
                  {3,3} ,
                  {2,2} ,
                  {2,2} ,
                  {3,3} ,
                  {4,4} ,
                  {4,4} ,
                  {4,4} ,
                  {4,4} ,
                  {4,4} ,
                  {4,4} ,
                  {3,3} ,
                  {3,3} ,
                  {4,4} ,
                  {4,4} ,
                  {2,2} ,
                  {3,3} ,
                  {4,4} , 
                  {4,4} ,
                  {4,4} ,
                  {1,1}
                };
    

  
}

void Communication::startSimulation() {
    lastUpdateTime = millis();
}

void Communication::update() 
{

    //need to make emmits like trains to get locations and send??? 
    if (millis() - lastUpdateTime > 2000) 
    {
        lastUpdateTime = millis();
        if (currentIndex < locations.size()) 
        {
            if (callback) callback(locations[currentIndex]);
            currentIndex++;
        }
    }
}

void Communication::onNewLocation(std::function<void(std::pair<int, int>)> callbackFunc) {
    callback = callbackFunc;
}

void Communication::stopSimulation()
{
    currentIndex = locations.size(); // Stop further updates
}

void Communication::begin() {
    // Initialize serial
    Serial.begin(115200);

    // Set Wi-Fi 
    WiFi.mode(WIFI_STA);

    // Print ESP's MAC address
    Serial.print("ESP32 MAC Address: ");
    Serial.println(WiFi.macAddress());

    // Initialize ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    Serial.println("ESP-NOW initialized successfully!");
}

