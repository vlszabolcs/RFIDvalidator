
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