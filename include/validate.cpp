const int vOut = 26;
const int vIn = 25;

void pinSetup(){
  pinMode(vIn,INPUT);
  pinMode(vOut,OUTPUT);
  digitalWrite(vOut,1);
}

bool findPerson(Data* dataArray, int dataSize, byte* nfcData, int *personIndex){
  
  for (int i = 0; i < dataSize; i++) {
    if (memcmp(dataArray[i].rfid0, nfcData, sizeof(dataArray[i].rfid0)) == 0) {
      *personIndex=i;
      Serial.print("Known user: ");
      Serial.println(dataArray[i].name);
      return true;
    }
  }
    *personIndex=-1;
    Serial.println("User unknown!");
    return false;
}

bool checkCredit(int credit, int amount ){
  if (credit >= amount){
    Serial.println("User has enough credit!");
    return true;
  }else{
   Serial.println("User has NOT enough credit!");
   return false;
  }
}

int monyWithdrawal(int credit, int amount ){
  Serial.print("Amount: ");
  Serial.println(amount);
  credit -= amount;
  Serial.print("Credit: ");
  Serial.println(credit);
 

  Serial.print("Remaining credit: ");
  Serial.println(credit);

  return credit;
}


bool machine() {
  
  unsigned long lowStartTime = millis();

  bool inputWasLow = false;
  bool inputLowDetected = false;
  unsigned long inputLowStartTime = 0;
  
  digitalWrite(vOut, LOW);
  while (millis() - lowStartTime < 10) {
    if (digitalRead(vIn) == LOW) {
      if (!inputWasLow) {
        inputLowStartTime = millis();
        inputWasLow = true;
        
      } else if (millis() - inputLowStartTime >= 1) {
        inputLowDetected = true;
        Serial.print("Coin released by machine: ");
        Serial.println(inputLowDetected);
        delay(10);
        digitalWrite(vOut, LOW);
        delay(50);
        digitalWrite(vOut, HIGH);

        return true; 
      }
    } else {
      inputWasLow = false;
      Serial.print("Coin rejected by machine: ");
      Serial.println(inputLowDetected);
      digitalWrite(vOut, HIGH);
      return false; 
    }
  }
}