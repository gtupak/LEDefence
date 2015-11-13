#include "HashMap.h"
#include <Timer.h>
#include <Event.h>

Timer t;

int _ENEMYCOUNT = 2;

// Replace 3 and 1 by the number of tiles and towers
int tilePins[3] = {10,11,12};
int towerPins[1] = {13};

const byte HASH_SIZE = sizeof(tilePins) + sizeof(towerPins); 
HashType<int,bool> hashRawArray[HASH_SIZE]; 
HashMap<int,bool> hashMap = HashMap<int,bool>( hashRawArray , HASH_SIZE );

// Array mapping tower i to tile j. In this case, tower 0 will be near tile 1. 
int tilesAdgacentToTile[1];

void setup() {
  // Set tower i to corresponding adjacent tile.
  tilesAdgacentToTile[0] = 1; // Tower 0 is near tile 1

  // TEMPORARY BEGIN -- will have to send binary in reality
  // Set pin mode OUTPUT to all pins
  for(int i = 0; i < sizeof(tilePins); i++){
    pinMode(tilePins[i], OUTPUT);
    hashMap[i](tilePins[i], false);
  }
  for(int i = 0; i < sizeof(towerPins); i++){
    pinMode(towerPins[i], OUTPUT);
    hashMap[sizeof(tilePins) + i](towerPins[i], false);
  }
  // TEMPORARY END --

  // Shift lights acordingly
  t.every(1000, runGame);
}

void loop() {
  t.update(); 
}

// Shift lights
void runGame(){
  // If there are still enemies to 'deploy' and first tile is off, turn it on.
  if (_ENEMYCOUNT != 0 && hashMap.getValueOf(tilePins[0]) == false){
    analogWrite(tilePins[0], 255); // Turn the LED on
    int index = hashMap.getIndexOf(tilePins[0]);
    hashMap[index](tilePins[0], true);
    _ENEMYCOUNT--;
  }
}

