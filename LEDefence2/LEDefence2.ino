#include "HashMap.h"

//#include <Timer.h>
//#include <Event.h>

//Timer t;
const int _towerCooldown = 750; // Stores cooldown time in ms of the towers --- CURRENTLY ONLY 1 COOLDOWN TYPE


int enemyCount = 3;
int gameFinishedFlag = 0;

// Replace 3 and 1 by the number of tiles and towers
int tilePins[4] = {2,3,4,5};
int towerPins[2] = {6,7};
int towerDelays[1] = {-_towerCooldown}; // Stores the time in ms of each tower


// A hash map containing the states of the pins
const byte HASH_SIZE = sizeof(tilePins)/sizeof(int) + sizeof(towerPins)/sizeof(int); 
HashType<int,bool> hashRawArray[HASH_SIZE]; 
HashMap<int,bool> pinStatusHashMap = HashMap<int,bool>( hashRawArray , HASH_SIZE );

// Array mapping tower i to tile j.
int tilesAdgacentToTower[2];

void setup() {
  
  // Set tower i to corresponding adjacent tile.
  tilesAdgacentToTower[0] = 2; // Tower 0 is near tile 1 (tiles go from 0 to n-1)
  tilesAdgacentToTower[1] = 3;
  
  // TEMPORARY BEGIN -- will have to send binary in reality
  // Set pin mode OUTPUT to all pins
  for (int i = 0; i < sizeof(tilePins)/sizeof(int); i++){
    pinMode(tilePins[i], OUTPUT);
    pinStatusHashMap[i](tilePins[i], false);
  }
  for (int i = 0; i < sizeof(towerPins)/sizeof(int); i++){
    pinMode(towerPins[i], OUTPUT);
    pinStatusHashMap[sizeof(tilePins)/sizeof(int) + i](towerPins[i], false);
  }
  // TEMPORARY END --

  // Shift lights accordingly
  // t.every(10000,runGame);
  
  Serial.begin(9600);
}


void loop() {
  if (!gameFinishedFlag){
    shiftTiles();
    fireTowers();
    delay(500);

    // Check if there is an enemy at the last tile
    if (pinStatusHashMap.getValueOf(tilePins[sizeof(tilePins)/sizeof(int) - 1])){
      flashTiles(3);
      gameFinishedFlag++;
    }
    else if (!aliveEnemiesLeft()){
      flashTowers(3);
      gameFinishedFlag++;
    }
  }

  
}

// Shifts tiles
// Tested OK
void shiftTiles(){
//  Serial.println("Shifting tiles...");  
  for(int i = sizeof(tilePins)/sizeof(int); i >= 0; i--){
    if (i == 0) {
      // If there are still enemies left
      if (enemyCount){
        setPinStatus(tilePins[0], true);
        enemyCount--;
      }
      else{
        if (pinStatusHashMap.getValueOf(tilePins[0])){ // First tile is ON
          setPinStatus(tilePins[0], false);
        }
      }
      continue;
    }
    bool currentPinStatus = pinStatusHashMap.getValueOf(tilePins[i]);
    bool previousPinStatus = pinStatusHashMap.getValueOf(tilePins[i-1]);
    
    // If previous tile is ON
    if (previousPinStatus){
      // Turn current ON
      setPinStatus(tilePins[i], true);
    }
    else{ // Previous tile is OFF
      // Turn off current
      setPinStatus(tilePins[i], false);
    }
  }
}

void fireTowers(){
  int towerElapsedTime;
  int currentTileIndex;
  int adgacentTileStatus;
  
  for (int i = 0; i < sizeof(tilesAdgacentToTower)/sizeof(int); i++){
    // i is the current tower id
    currentTileIndex = tilesAdgacentToTower[i];
    adgacentTileStatus = pinStatusHashMap.getValueOf(tilePins[currentTileIndex]);


    towerElapsedTime = millis() - towerDelays[i];

//    Serial.print("towerElapsedTime = ");
//    Serial.println(towerElapsedTime);

    if (adgacentTileStatus && (towerElapsedTime > _towerCooldown)){ // Adgacent tile status is ON and the elapsed time is greater than the preset cooldown
        setPinStatus(towerPins[i], true);
        setPinStatus(tilePins[currentTileIndex], false);
//        Serial.println("waitin 1000...");
//        delay(1000);
//        setPinStatus(towerPins[i], false);        
        towerDelays[i] = millis(); // Update tower's delay


    }
    else{
      setPinStatus(towerPins[i], false);
    }
  }
}

// Flashes an array of pins a num of times
void flashTiles(int num){
  // Turn OFF all towers
  for (int i = 0; i < sizeof(towerPins)/sizeof(int); i++){
    setPinStatus(towerPins[i], false);
  }
  
  for (int n = 0; n < num; n++){
    for (int i = 0; i < sizeof(tilePins)/sizeof(int); i++){
      setPinStatus(tilePins[i], true);
    }
    delay(200);
    for (int i = 0; i < sizeof(tilePins)/sizeof(int); i++){
      setPinStatus(tilePins[i], false);
    }
    delay(200);
  }
}

void flashTowers(int num){
  // Turn OFF all tiles
  for (int i = 0; i < sizeof(tilePins)/sizeof(int); i++){
    setPinStatus(tilePins[i], false);
  }
  
  for (int n = 0; n < num; n++){
    for (int i = 0; i < sizeof(towerPins)/sizeof(int); i++){
      setPinStatus(towerPins[i], true);
    }
    delay(200);
    for (int i = 0; i < sizeof(towerPins)/sizeof(int); i++){
      setPinStatus(towerPins[i], false);
    }
    delay(200);
  }
}

// Returns 1 if there are still enemies left. 0 otherwise
int aliveEnemiesLeft(){
  if (enemyCount) return enemyCount;

  for (int i = 0; i < sizeof(tilePins)/sizeof(int); i++){
    if (pinStatusHashMap.getValueOf(tilePins[i])) return 1;
  }
  return 0;
}

/* UTILITY METHODS */

// Set pin to pinStatus (ON, OFF)
// Tested OK
void setPinStatus(int pin, bool pinStatus){
  int value = pinStatus? 255 : 0;
  
  analogWrite(pin, value);
    
  // Update value in hash
  int index = pinStatusHashMap.getIndexOf(pin);
  pinStatusHashMap[index](pin, pinStatus );
}
