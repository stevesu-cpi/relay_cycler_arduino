// notes: revised by Steve Su 10/26/2022 for DongYa contactor cycle test
// 1) hook up solenoid (also refered to as contactor) comm lead (color x) to match arduino ch 6,7,10,11
  

int led = 1;                  // changed from D1 to verify compile
int loop_count = 0;
int num_cycles = 5;
int switchs[] = {2, 3, 4, 5}; // These are the logic pins to activate the relay
int sensor1[] = {6, 7, 10, 11};  // These are the read pins from the contactor - physical.
int sensor2[] = {8, 9, 12, 13};  // These are the read pins from the contactor - aux.
int delay1 = 1000;
int delay2 = 30000;
unsigned long stop[] = {0, 0, 0, 0};
unsigned long counter[] = {0, 0, 0, 0};             // Cycle counters
unsigned long errorEngageCounter[] = {0, 0, 0, 0};  // Error counters when the magnet fails to engage the mechanism
unsigned long errorReleaseCounter[] = {0, 0, 0, 0}; // Error counters when the magnet fails to release the mechanism
unsigned long errorAuxCantClose[] = {0, 0, 0, 0};   // Error counters when the contactor is close but the Aux. port is still opened
unsigned long errorAuxCantOpen[] = {0, 0, 0, 0};    // Error counters when the contactor is open but the Aux. port is still closed

void setup()
{
  // put your setup code here, to run once:
  for (int i = 0; i < 4; i++){
    pinMode(sensor1[i], INPUT);
    pinMode(sensor2[i], INPUT);
    pinMode(switchs[i], OUTPUT);
  }
  Serial.begin(9600);
}

void loop()
{
  /***************close UUT****************/
  //Serial.println("Dongya contactor code r0");
  Serial.println();
  Serial.print("loop count = ");
  Serial.print(loop_count);
  Serial.println();
  Serial.println("Turning UUT contactors on; ch2, ch3 (step4)");  //step4
  for (int i = 0; i < 2; i++){    //apply power to ch2, ch3
    //if (stop[i] > 0) continue;    //index00 -> ch2, index01 -> ch3 (mapping) 
    digitalWrite(switchs[i], HIGH);
  }
                                 
  Serial.println("wait 1 sec (step5)");
  delay(delay1);    //step5 
  
  // read sensors from UUT; ch2, ch3
  for (int i = 0; i < 2; i++){
    //if (stop[i] > 0) continue;
    int sensorReading = digitalRead(sensor1[i]);  // Read the signal from the contactor 
    int auxReading = digitalRead(sensor2[i]);     // Read the signal from the auxiliary terminal
    sensorReading = auxReading;                    //note: this is just a work around since we do not have signal from contactor
    Serial.println("sensor readings for UUT - ch2, ch3");
    Serial.println(sensorReading);               // Send the signal from the contactor to the Serial window
    Serial.println(auxReading);                  // Send the signal from the auxiliary to the Serial window
    
    if (sensorReading == HIGH && auxReading == HIGH)                                               
      counter[i]++;                                             //success
      
    else if (sensorReading == HIGH && auxReading == LOW)
      errorAuxCantClose[i]++;
      
    else if (sensorReading == LOW && auxReading == LOW)
      errorEngageCounter[i]++;
      
    else if (sensorReading == LOW && auxReading == HIGH)
      errorEngageCounter[i]++;
      
    //errorAuxCantOpen[i]++;  //??? 
  }

/***********closing SW1*****************/
  Serial.println("close SW1 (step6)");
  
  for (int i = 2; i < 4; i++){    //apply power to ch4, ch5; step6
    //if (stop[i] > 0) continue;    //index02 -> ch4, index03 -> ch5 (mapping)
    digitalWrite(switchs[i], HIGH);
  }
                                 
  // read sensors SW1; ch4, ch5
  
  for (int i = 2; i < 4; i++){
    //if (stop[i] > 0) continue;
    int sensorReading = digitalRead(sensor1[i]);   // Read the signal from the contactor
    int auxReading = digitalRead(sensor2[i]);      // Read the signal from the auxiliary terminal
    sensorReading = auxReading;                    //note: this is just a work around since we do not have signal from contactor
    Serial.println("sensor readings for SW1 - ch4, ch5");
    Serial.println(sensorReading);               // Send the signal from the contactor to the Serial window
    Serial.println(auxReading);                  // Send the signal from the auxiliary to the Serial window
    
    if (sensorReading == HIGH && auxReading == HIGH)                                               
      counter[i]++;                                             //success
      
    else if (sensorReading == HIGH && auxReading == LOW)
      errorAuxCantClose[i]++;
      
    else if (sensorReading == LOW && auxReading == LOW)
      errorEngageCounter[i]++;
      
    else if (sensorReading == LOW && auxReading == HIGH)
      errorEngageCounter[i]++;
      
    //errorAuxCantOpen[i]++;  //??? 
  }
  
  Serial.println("wait 30 sec(step7)");  
  delay(delay2);  //step7


/********************************/
  Serial.println("open SW1 (step8)");
  for (int i = 2; i < 4; i++){
    digitalWrite(switchs[i], LOW);
  }
  
  Serial.println("wait 1 sec (step9)");
  delay(delay1);    //step9 
  
  
  Serial.println("Turning UUT contactors off.(step10)");
  for (int i = 0; i < 2; i++){
    digitalWrite(switchs[i], LOW);
  }

  delay(delay1);    //step11
  Serial.println("wait 1 sec (step11)"); 
 
  // read sensors after power off; do they close? 

  for (int i = 0; i < 4; i++){
    if (stop[i] > 0) continue;
    int sensorReading = digitalRead(sensor1[i]);
    int auxReading = digitalRead(sensor2[i]);
    sensorReading = auxReading;                    //note: this is just a work around since we do not have signal from contactor

    // Serial.println(sensorReading1); // Send the signal to the Serial window
    // Serial.println(auxReading1);    // Send the aux signal to the Serial window
    //delay(100);
    
    if (sensorReading == HIGH && auxReading == HIGH)
    {
      errorReleaseCounter[i]++;
    }
    else if (sensorReading == HIGH && auxReading == LOW)
    {
      errorReleaseCounter[i]++;
      errorAuxCantClose[i]++;
    }
    else if (sensorReading == LOW && auxReading == HIGH)
    {
      errorAuxCantOpen[i]++;
    }
  }

  
  for (int i = 0; i < 4; i++){
    if (counter[i] > num_cycles || errorEngageCounter[i] > num_cycles || errorReleaseCounter[i] > num_cycles)
    {
      Serial.println("Reach the lifetime cycling goal. Stop testing. Please record the data.");
      delay(100);
      exit(0);
    }
    /*
    if (stop[i] == 0) {
      if (errorAuxCantClose[i] > 0 || errorEngageCounter[i] > 0 || errorReleaseCounter[i] > 0 || errorAuxCantOpen[i] > 0)
      {
        //stop[i] = counter[i] + 1;
        Serial.println();
        Serial.println((String)"Contactor #" + (i + 1) + " has failed. Cycling will be stopped for this contactor.");
      }
    
    }
    */
    Serial.println();
    Serial.print((String)"Contactor #" + (i + 2) + " status: ");
    if (stop[i] > 0) {
      Serial.println("FAILED.");
    } else {
      Serial.println("RUNNING.");
    }
    Serial.print("# of completed cycles for contactor #");
    Serial.print(i + 2);
    Serial.print(" is ");
    Serial.print(counter[i]);
    Serial.println(); // Print the cycle count to the serial port.
    Serial.print("# of cycles failed to engage the mechanism: ");
    Serial.print(errorEngageCounter[i]);
    Serial.println();
    Serial.print("# of cycles the Aux mechanism failed to engage: ");
    Serial.print(errorAuxCantClose[i]);
    Serial.println();
    Serial.print("# of cycles failed to Release the mechanism: ");
    Serial.print(errorReleaseCounter[i]);
    Serial.println();
    Serial.print("# of cycles the Aux mechanism failed to release: ");
    Serial.print(errorAuxCantOpen[i]);
    Serial.println();
    delay(150);
  }
  /*if (stop[0] > 0 && stop[1] > 0 && stop[2] > 0 && stop[3] > 0)
  {
    Serial.println("All contactors have failed. Stopping the test.");
    delay(100);
    exit(0);
  }
  */ 
  loop_count++; 
  
}
