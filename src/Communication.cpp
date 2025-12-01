/*#include <Communication.h>
#include <Arduino.h>
#include <Game.h>
#include <vector>
#include <utility>

#include <WiFi.h>
#include <esp_now.h>


Communication::Communication() 
{
    getLocations();
    currentIndex = 0;
    lastUpdateTime = millis();
}

void Communication::getLocations()
{
    locations = { {0,0} ,
                  {0,0} ,
                  {50,50} ,
                  {100,0} ,
                  {0,200},
                  {0,0} ,
                  {0,0} ,
                  {50,50} ,
                  {100,0} ,
                  {0,200}
                };
     
}

void Communication::startSimulation() {
    lastUpdateTime = millis();
}

void Communication::update() 
{

        if (millis() - lastUpdateTime > 5000) 
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
*/

#include <Communication.h>
#include <Arduino.h>
#include <Game.h>
#include <vector>
#include <utility>
#include <WiFi.h>
#include <esp_now.h>

//packet structure for receiving location data
typedef struct {
    float x;
    float y;
} LocationPacket;

// Forward reference so static callback can reach class instance
static Communication* globalCommPtr = nullptr;

Communication::Communication()
{
    globalCommPtr = this;

    getLocations(); // Demo simulation locations (floats now)
    currentIndex = 0;
    lastUpdateTime = millis();
    simulate = true;   // Start in simulation mode until real data arrives
}

// Demo path for fallback simulation
void Communication::getLocations()
{
    //running 301 game till end of game 
    locations = {
        //Player 1
        {0.0f,   60.0f},  //20
        {0.0f,  166.5f},  //40 (20 x 2)
        { 0.0f,  103.0f},  //60 (20 x 3)

        //Player 2
        {18.5f,   57.1f}, //1
        {57.1f,  -18.5f},  //10
        {-31.7f,  97.8f},  //15 (5 x 3)

        //Player 1
        {-98.0f, -137.8f},  //14 (7 x 2)
        {48.8f,  -35.3f},  //15
        {-48.8f,  -35.3f},  //16

        //Player 2
        {0.0f,  -60.0f},  //3
        {-60.0f,    0.0f},  //11
        {83.1f,  60.5f},  //12 (4 x 3)

        //Player 1
        {0.0f,  0.0f},  //50
        {0.0f,    11.1f},  //25
        {-18.5f,   57.1f},  //5

        //Player 2 
        { 0.0f,   60.0f},  //20
        {0.0f,  -60.0f},  //3 
        {60.0f,    0.0f},  //6

        //Player 1
        {0.0f,    11.1f},  //25
        {0.0f,    11.1f},  //25
        { 0.0f,  -166.5f}  //6 (3 x 2)
        
    };
    
    //test around the board sections in order
    /*
    locations = {

                    //bullseyes 
                    { 0.0f,    0.0f },
                    { 0.0f,    11.1f },

                    //regular ring
                    {   0.0f,   60.0f },  // 20
                    {  18.5f,   57.1f },  // 1
                    {  35.3f,   49.0f },  // 18
                    {  48.8f,   35.3f },  // 4
                    {  57.1f,   18.5f },  // 13
                    {  60.0f,    0.0f },  // 6
                    {  57.1f,  -18.5f },  // 10
                    {  48.8f,  -35.3f },  // 15
                    {  35.3f,  -49.0f },  // 2
                    {  18.5f,  -57.1f },  // 17
                    {   0.0f,  -60.0f },  // 3
                    { -18.5f,  -57.1f },  // 19
                    { -35.3f,  -49.0f },  // 7
                    { -48.8f,  -35.3f },  // 16
                    { -57.1f,  -18.5f },  // 8
                    { -60.0f,    0.0f },  // 11
                    { -57.1f,   18.5f },  // 14
                    { -48.8f,   35.3f },  // 9
                    { -35.3f,   49.0f },  // 12
                    { -18.5f,   57.1f },  // 5

                    //triple ring
                    {   0.0f,  103.0f },   // 20
                    {  31.7f,  97.8f  },   // 1
                    {  60.5f,  83.1f  },   // 18
                    {  83.1f,  60.5f  },   // 4
                    {  97.8f,  31.7f  },   // 13
                    { 103.0f,   0.0f  },   // 6
                    {  97.8f, -31.7f  },   // 10
                    {  83.1f, -60.5f  },   // 15
                    {  60.5f, -83.1f  },   // 2
                    {  31.7f, -97.8f  },   // 17
                    {   0.0f, -103.0f },   // 3
                    { -31.7f, -97.8f  },   // 19
                    { -60.5f, -83.1f  },   // 7
                    { -83.1f, -60.5f  },   // 16
                    { -97.8f, -31.7f  },   // 8
                    { -103.0f,  0.0f  },   // 11
                    { -97.8f,  31.7f  },   // 14
                    { -83.1f,  60.5f  },   // 9
                    { -60.5f,  83.1f  },   // 12
                    { -31.7f,  97.8f  },   // 5

                    //double ring
                    {   0.0f,  166.5f },  // 20
                    {  51.6f,  158.3f },  // 1
                    {  98.0f,  137.8f },  // 18
                    { 134.8f,  98.0f  },  // 4
                    { 158.3f,  51.6f  },  // 13
                    { 166.5f,   0.0f  },  // 6
                    { 158.3f, -51.6f  },  // 10
                    { 134.8f, -98.0f  },  // 15
                    { 98.0f,  -137.8f },  // 2
                    { 51.6f,  -158.3f },  // 17
                    {  0.0f,  -166.5f },  // 3
                    { -51.6f, -158.3f },  // 19
                    { -98.0f, -137.8f },  // 7
                    { -134.8f, -98.0f },  // 16
                    { -158.3f, -51.6f },  // 8
                    { -166.5f,  0.0f  },  // 11
                    { -158.3f,  51.6f },  // 14
                    { -134.8f,  98.0f },  // 9
                    { -98.0f,  137.8f },  // 12
                    { -51.6f,  158.3f }  // 5

                };
    */

    //testing bust fix 
    locations = {
        //Player 1
        {0.0f,   0.0f},  //50
        {0.0f,  0.0f},  //50
        { 0.0f,  0.0f},  //50

        //Player 2
        {18.5f,   57.1f}, //1
        {57.1f,  -18.5f},  //10
        {-31.7f,  97.8f},  //15 (5 x 3)

        //Player 1
        {0.0f, 0.0f},  //50
        {0.0f,  0.0f},  //50
        {0.0f,  0.0f},  //50

        //Player 2
        {0.0f,  -60.0f},  //3
        {-60.0f,    0.0f},  //11
        {83.1f,  60.5f},  //12 (4 x 3)

        //Player 1
        {0.0f,  0.0f},  //50
        {0.0f,    0.0f},  //50
        {0.0f,   0.0f},  //50

        //Player 2 
        { 0.0f,   60.0f},  //20
        {0.0f,  -60.0f},  //3 
        {60.0f,    0.0f},  //6

        //Player 1
        {0.0f,    11.1f},  //25
        {0.0f,    11.1f},  //25
        { 0.0f,  -166.5f},  //6 (3 x 2)

        {0.0f, 0.0f},  //50
        {0.0f,  0.0f},  //50
        {0.0f,  0.0f},  //50

        {0.0f, 0.0f},  //50
        {0.0f,  0.0f},  //50
        {0.0f,  0.0f},  //50

        {0.0f, 0.0f},  //50
        {0.0f,  0.0f},  //50
        {0.0f,  0.0f},  //50

        {0.0f, 0.0f},  //50
        {0.0f,  0.0f},  //50
        {0.0f,  0.0f},  //50

        {0.0f, 0.0f},  //50
        {0.0f,  0.0f},  //50
        {0.0f,  0.0f},  //50

        {0.0f, 0.0f},  //50
        {0.0f,  0.0f},  //50
        {0.0f,  0.0f},  //50

        {0.0f, 0.0f},  //50
        {0.0f,  0.0f},  //50
        {0.0f,  0.0f},  //50

        {0.0f, 0.0f},  //50
        {0.0f,  0.0f},  //50
        {0.0f,  0.0f},  //50
        
    };
    
}

void Communication::startSimulation()
{
    simulate = true;
    currentIndex = 0;
    lastUpdateTime = millis();
}

void Communication::stopSimulation()
{
    simulate = false;
}

void Communication::update()
{
    if (!simulate) return;

    if(!calibrating)
    {
        if (millis() - lastUpdateTime > 2000) {
            lastReceiveTime = millis();
            lastUpdateTime = millis();

            if (currentIndex < locations.size()) {
                if (callback) callback(locations[currentIndex]);
                currentIndex++;
            }
        }
    }
        
}


// Register callback for location updates
void Communication::onNewLocation(std::function<void(std::pair<float, float>)> callbackFunc)
{
    callback = callbackFunc;
}

void onEspNowReceive(const uint8_t* mac, const uint8_t* data, int len)
{
    if (!globalCommPtr) return;

    Serial.print("Received data");

    globalCommPtr->lastReceiveTime = millis();

    if (len == sizeof(LocationPacket)) {
        LocationPacket pkt;
        memcpy(&pkt, data, sizeof(pkt));
        /*Serial.print(" X: ");
        Serial.print(pkt.x);
        Serial.print(" Y: ");
        Serial.print(pkt.y);
        Serial.println();*/

        if (globalCommPtr->callback) {
            globalCommPtr->callback({ pkt.x, pkt.y });
        }

        globalCommPtr->stopSimulation();
    }
}

void Communication::begin()
{
    Serial.begin(115200);
    delay(200);

    WiFi.mode(WIFI_STA);

    Serial.print("ESP32 MAC Address: ");
    Serial.println(WiFi.macAddress());

    //initialize ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    Serial.println("ESP-NOW initialized successfully!");

    //register the receive callback
    //esp_now_register_recv_cb(onEspNowReceive);

    esp_now_peer_info_t peerInfo = {};
    const uint8_t peerAddress[6] = {0xFC, 0x01, 0x2C, 0xDC, 0x52, 0x68};

    memcpy(peerInfo.peer_addr, peerAddress, 6);
    peerInfo.channel = 0;         //auto, any channel
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add ESP-NOW peer!");
        return;
    }

    Serial.println("ESP-NOW peer added successfully!");
}
