#include <json.cpp>
#include <rfid.cpp>
#include <test.cpp>

JsonArray users;

void hexStringToUint8Array(String hexString, uint8_t *array, int length) {

  int result = sscanf(hexString.c_str(), "0x%2hhx %2hhx %2hhx %2hhx", &array[0], &array[1], &array[2], &array[3]);
  if (result != length) {
    Serial.println("Hiba történt a konverzió során.");
  }
}

void setup() {

  Serial.begin(115200);

  nfc.begin();
  
  users=openJson("/json/db2024.json");

  for (JsonObject user : users) {

    
    user ["key"]=strtol(user["key"], NULL, 16);
    

      long int key = user["key"];
      int credit = user["credit"];
      String name = user["name"];
      

      // Print user details
      Serial.print("Key: ");
      Serial.print(key);
      
      Serial.print(", Name: ");
      Serial.print(name);
      Serial.print(", Credit: ");
      Serial.println(credit);
  }
 
  // Iterate through each user object in the array

}

void loop() {
  uint8_t success=false;
  uint8_t uid[] = { 0, 0, 0, 0};  // Buffer to store the returned UID
  uint8_t uidLength; 
  long int rfidINT = 0;
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  for (int i = 0; i < sizeof(uid); i++) {
        Serial.print(uid[i]);
        Serial.print(" ");
  }

  uint32_t v2 = ((uint32_t) uid[0]) << 24; // put the MSB in place
  v2 |= ((uint32_t) uid[1]) << 16; // add next byte
  v2 |= ((uint32_t) uid[2]) << 8; // add next byte
  v2 |= ((uint32_t) uid[3]); // add LSB

  Serial.println(v2, HEX);
  
  Serial.println();
  
  for (int i =0; i <sizeof(uid) ; i++) {
    rfidINT += uid[i] << (8 * (i));
  }

  Serial.println(rfidINT);

  
/*  if (success) {
  
    for (JsonObject user : users) {

    
    
      user ["key"]=strtol(user["key"], NULL, 16);

      long int key = user["key"];
      int credit = user["credit"];
      String name = user["name"];
      

      // Print user details
      Serial.print("Key: ");
      Serial.print(key);
      
      Serial.print(", Name: ");
      Serial.print(name);
      Serial.print(", Credit: ");
      Serial.println(credit);
  }
  }*/
}
