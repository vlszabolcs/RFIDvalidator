#include <Adafruit_PN532.h>

Adafruit_PN532 nfc(2, 3);

uint8_t detectRfid(){
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
    uint8_t uidLength; 
    return nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

}