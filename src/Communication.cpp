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
    //501 Game - 2 Player - Player 1 Wins 
    /*locations = { {3,3} ,
                  {2,2} ,
                  {4,4} ,
                  {3,3} ,
                  {2,2} ,
                  {4,4} ,
                  {3,3} ,
                  {3,3} ,
                  {3,3} ,
                  {3,3} ,
                  {3,3} ,
                  {3,3} ,
                  {3,3} ,
                  {3,3} ,
                  {3,3} ,
                  {3,3} ,
                  {3,3} ,
                  {3,3} ,
                  {2,2} ,
                  {3,3} ,
                  {1,1} , 
                  {3,3} ,
                  {2,2} ,
                  {4,4} ,
                  {3,3}}; */

    //301 Game - 3 Player - Player 2 Wins
    locations = { {3,3} ,
                  {3,3} ,
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
    
    //Around the World - 1 Player
    /*locations = {
        {1,1},
        {2,2},
        {3,3},
        
        {5,5},
        
        {4,4},
        {5,5}, 
        {6,6},
        {7,7},
        {8,8}, 
        {9,9},

        {11, 11},

        {10,10},
        {11,11},
        {12,12},
        {13,13},
        {14,14},
        {15,15},
        {16,16},
        {17,17},
        {18,18},

        {20, 20},

        {19,19},
        {20,20}
    };*/

    //Cricket - 2 Player 
    /*locations = {
    // Player 1
    {1,1}, {1,1}, {1,1},  // 1 closes 20
    
    // Player 2
    {1,2}, {1,2}, {1,2},  // 2 closes 19

    // Player 1
    {1,1}, {1,1}, {1,1},  // 1 scores on 20 three times  

    // Player 2
    {1,1}, {1,1}, {1,1},  //2 closes out 20 - NO MORE SCORING 20

    // Player 1
    {1,1}, {1,1}, {1,2},  // 1 hits 20 twice for no score - 1/3 on 19 

    // Player 2
    {1,2}, {1,2}, {1,2},  // 2 score on 19 three times

    // Player 1
    {1,2}, {1,2}, {1,2},  // 1 closes 19

    // Player 2
    {1,3}, {1,3}, {1,3},  // 2 closes 18

    // Player 1
    {1,3}, {1,3}, {1,3},  // 1 closes 18 

    // Player 2
    {2,1}, {2,1}, {2,1},  // 2 closes 17

    // Player 1
    {2,1}, {2,1}, {2,1},  // 1 closes 17

    // Player 2
    {2,2}, {2,2}, {2,2},  // 2 closes 16

    // Player 1
    {2,2}, {2,2}, {2,2},  // 1 closes 16

    // Player 2
    {2,3}, {2,3}, {2,3},  // 2 closes 15

    // Player 1
    {2,3}, {2,3}, {2,3},  // 1 closes 15

    // Player 2
    {3,3}, {3,3}, {3,3},  // 2 closes bullseye

    // Player 1
    {3,3}, {3,3}, {3,3}   // 1 closes bullseye

    //Player 1 should win
    }; */

  
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

