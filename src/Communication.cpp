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
    /*locations = {
        {0.0f,   0.0f},
        {100.0f, 0.0f},
        {50.5f, 50.5f},
        {0.0f, 200.0f},
        {-100.0f, 0.0f},
        {-50.0f, -50.0f},
        {0.0f, 0.0f},
        {0.0f, -170.0f},
        {-70.0f, -170.0f},
        {0.0f, 0.0f},
        {0.0f, 0.0f},
        {0.0f, 0.0f},
        {0.0f, 0.0f},
        {0.0f, 0.0f},
        {0.0f, 0.0f},
        {0.0f, 0.0f},
        {0.0f, 0.0f},
        {0.0f, 0.0f},
        {0.0f, 0.0f}*/
    
    //test around the board sections in order
    /*locations = {
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
                    { -18.5f,   57.1f }  // 5
                };*/
    locations = {
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
        { -31.7f,  97.8f  }   // 5
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
        if (millis() - lastUpdateTime > 3000) {
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
