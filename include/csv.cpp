#include <SD.h>

struct Data {
  int id;
  String name;
  int credit;
  byte rfid0[4];
}; 

bool readCSVFile(const char* filename, Data* dataArray, int maxSize) {
  File dataFile = SD.open(filename);
  if (!dataFile) {
    Serial.println("Hiba a fajl megnyitasakor.");
    return false;
  }

  int dataCount = 0; 

  while (dataFile.available()) {
    
    if (dataCount >= maxSize) {
      Serial.println("Tul sok adat, nem fer bele a tombbe!");
      dataFile.close();
      return false;
    }

    char lineBuffer[256];
    memset(lineBuffer, 0, sizeof(lineBuffer));
    
    int pos = 0;  
    while (dataFile.available() && pos < sizeof(lineBuffer) - 1) {
      char c = dataFile.read();
      lineBuffer[pos++] = c;
      if (c == '\n') break; 
    }

    int id, credit;
    byte rfid0[4], rfid1[4], rfid2[4];
    char name[50];
    
    sscanf(lineBuffer, "%d,%[^,],%d,0x%hhx 0x%hhx 0x%hhx 0x%hhx",
         &id, name, &credit,
         &rfid0[0], &rfid0[1], &rfid0[2], &rfid0[3]);
      
    dataArray[dataCount].id = id;
    dataArray[dataCount].name = String(name);
    dataArray[dataCount].credit = credit;
    memcpy(dataArray[dataCount].rfid0, rfid0, sizeof(rfid0));

    dataCount++;
  }

  dataFile.close();
  return true;
}

