#include "HashMap.h"
#include <Timer.h>
#include <Event.h>

Timer t;

int enemyCount = 2;

// Replace 3 and 1 by the number of tiles and towers
int tilePins[3] = {10,11,12};
int towerPins[1] = {13};

// A hash map containing the states of the pins
const byte HASH_SIZE = sizeof(tilePins) + sizeof(towerPins); 
HashType<int,bool> hashRawArray[HASH_SIZE]; 
HashMap<int,bool> pinStatusHashMap = HashMap<int,bool>( hashRawArray , HASH_SIZE );

// Array mapping tower i to tile j. In this case, tower 0 will be near tile 1. 
int tilesAdgacentToTile[1];

void setup() {
  // Set tower i to corresponding adjacent tile.
  tilesAdgacentToTile[0] = 1; // Tower 0 is near tile 1 (tiles go from 0 to n-1)

  // TEMPORARY BEGIN -- will have to send binary in reality
  // Set pin mode OUTPUT to all pins
  for (int i = 0; i < sizeof(tilePins); i++){
    pinMode(tilePins[i], OUTPUT);
    pinStatusHashMap[i](tilePins[i], false);
  }
  for (int i = 0; i < sizeof(towerPins); i++){
    pinMode(towerPins[i], OUTPUT);
    pinStatusHashMap[sizeof(tilePins) + i](towerPins[i], false);
  }
  // TEMPORARY END --

  // Shift lights acordingly
  t.every(1000, runGame);
  
  Serial.begin(9600);
}

bool gameFinished = false;

void loop() {
  if (!gameFinished){
    t.update(); 
  }
}

// Performs game logic
void runGame(){
  
  // Shift tiles
  shiftTiles();
  
  fireTowers();
  
  // Check if won
  if (pinStatusHashMap.getValueOf(sizeof(tilePins) - 1) == true){
    
    flashAllTiles( 3 ); // Flash tiles 3 times
    gameFinished = true;
  }
  Serial.println(gameFinished);
}


void shiftTiles(){
  for (int i = sizeof(tilePins); i >= 1; i--){
    // If previous tile is ON
    if (pinStatusHashMap.getValueOf(tilePins[i-1]) == true){
       // Turn current pin ON if it was OFF
       if(pinStatusHashMap.getValueOf(tilePins[i]) == false){
        setPinStatus(tilePins[i], true);     
       }
    }
    else { // Previous pin is OFF
      if(pinStatusHashMap.getValueOf(tilePins[i]) == true){
        // Turn off current pin
        setPinStatus(tilePins[i], false);
      }
    }
  }

  // Handle first tile
  // If there are still enemies to 'deploy' and first tile is off, turn it on.
  if (enemyCount > 0){
    if (pinStatusHashMap.getValueOf(tilePins[0]) == false){
      setPinStatus(tilePins[0], true);
    }
    enemyCount--;
  }
  else if (pinStatusHashMap.getValueOf(tilePins[0]) == true){
    setPinStatus(tilePins[0], false);
  }
}

void fireTowers(){
  
}

void setPinStatus(int pin, bool pinStatus){
  int value = pinStatus? HIGH : LOW;
  
  analogWrite(pin, value);
  
  // Update value in hash
  int index = pinStatusHashMap.getIndexOf(pin);
  pinStatusHashMap[index](pin, pinStatus );
}

void flashAllTiles(int n){
  for (int i = 0; i < 3; i++){
    for (int i = 0; i < sizeof(tilePins); i++){
      analogWrite(tilePins[i], HIGH);  
    }
    delay(500);
    for (int i = 0; i < sizeof(tilePins); i++){
      analogWrite(tilePins[i], LOW);  
    }
    delay(500);
  }
}


