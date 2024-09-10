#include <Arduino.h>


#include <csv.cpp>
#include <rfid.cpp>
#include <validate.cpp>
#include <ui.cpp>



const char* dbPath="/db.csv";
const char* logPath="";

const int maxDataCount = 14; //dinamikussá tenni! 


Data dataArray[maxDataCount];
File logFile;

String printByteArray(byte* array, int size) {
  String byteStr="";
  for (int i = 0; i < size; i++) {
    byteStr+=String(array[i], HEX);
    if (i < size - 1) {
    }
  }
  return byteStr;
}

void succesPurchase(uint8_t* nfcData, String name, int credit) {
  
  logFile = SD.open(logPath, FILE_APPEND);
  if (logFile) {
    logFile.print(",");
    logFile.print(name);
    logFile.print(",");
    logFile.print(printByteArray(nfcData,4));
    logFile.print(",");
    logFile.print(credit);
    logFile.print(",");
    logFile.println("success");
    logFile.close();
  } else {
    Serial.println("logFile could not open");
  }
}

void noCredit(uint8_t* nfcData, String name, int credit) {
  
  logFile = SD.open(logPath, FILE_APPEND);
  if (logFile) {
    logFile.print(",");
    logFile.print(name);
    logFile.print(",");
    logFile.print(printByteArray(nfcData,4));
    logFile.print(",");
    logFile.print(credit);
    logFile.print(",");
    logFile.println("Not enough credit");
    logFile.close();
  } else {
    Serial.println("logFile could not open");
  }
}

void machineError(uint8_t* nfcData, String name, int credit) {
  
  logFile = SD.open(logPath, FILE_APPEND);
  if (logFile) {
    logFile.print(",");
    logFile.print(name);
    logFile.print(",");
    logFile.print(printByteArray(nfcData,4));
    logFile.print(",");
    logFile.print(credit);
    logFile.print(",");
    logFile.println("The machine has some failure");
    logFile.close();
  } else {
    Serial.println("logFile could not open");
  }
}


void setup() {
  Serial.begin(115200);

  Serial.print("Initializing RFID modul...");
  nfc.begin();

  pinSetup();

  Serial.print("Initializing SD card...");
  // see if the card is present and can be initialized:
  if (!SD.begin()) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
 
  // reading CSV file and store mem
  if (readCSVFile(dbPath, dataArray, maxDataCount)) {
    Serial.println("csv file has been scanned. ");
    for (int i = 0; i < maxDataCount; i++) {
      Serial.print("ID: "); Serial.println(dataArray[i].id);
      Serial.print("Name: "); Serial.println(dataArray[i].name);
      Serial.print("Credit: "); Serial.println(dataArray[i].credit);
      Serial.print("RFID0: "); Serial.println(printByteArray(dataArray[i].rfid0, 4));
      Serial.println("----------------------");
    }

  } else {
    Serial.println("Error reading the CSV file!");
  }
  connectWifi();
	
		WiFi.setSleep(false); //For the ESP32: turn off sleeping to increase UI responsivness (at the cost of power use)
	
	setUpUI();
  Serial.print("Coffee price setup: ");
  Serial.println(coffeePrice);
}


void loop() {
  static unsigned long previousMillis = 0;  // Az utolsó időpillanat tárolására
  const unsigned long interval = 1000;    
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength; 
  
  int userIndex;

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  

    if (success) {
    Serial.println("RFID card detected!");

    if (findPerson(dataArray, maxDataCount,uid, &userIndex)) {
      
      if (checkCredit(dataArray[userIndex].credit,coffeePrice)) {
        
        
        if(machine()){
          Serial.println("Kave keszitese...");
          dataArray[userIndex].credit=monyWithdrawal(dataArray[userIndex].credit,coffeePrice);
          Serial.println("Kave kesz.");
        }
        else{
          Serial.println("Kávégép error");
        }
      }
    } 

    delay(1000); // ide baszni kell egy nonblocking delayt 
  }
}
