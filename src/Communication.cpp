#include <Communication.h>
#include <Arduino.h>
#include <Game.h>
#include <vector>
#include <utility>



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
                  {3,3} ,}; */

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
                  {1,1} };

  
}

void Communication::startSimulation() {
    lastUpdateTime = millis();
}

void Communication::update() 
{
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
