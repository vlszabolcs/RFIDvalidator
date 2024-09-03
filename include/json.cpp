#include <SPI.h>
#include <SD.h>
#include <ArduinoJson.h>

DynamicJsonDocument doc(1024);

JsonArray openJson(String path) {
    String jsonContent;
    File dataFile;

    if (!SD.begin()) {
        Serial.println("Hiba az SD kártya inicializálásakor!");
        
    }

    // Nyisd meg a JSON fájlt az SD kártyáról
    dataFile = SD.open(path);

    if (!dataFile) {
        Serial.println("Hiba a fájl megnyitásakor!");
        
    }

    while (dataFile.available()) {
        jsonContent += (char)dataFile.read();
    }
    
    dataFile.close();
    
    DeserializationError error = deserializeJson(doc, jsonContent);
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
    
  }
    
    return doc["users"];
}


