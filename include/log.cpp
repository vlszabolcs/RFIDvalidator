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
