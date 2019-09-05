#include <DS3231M.h>  

DS3231M_Class DS3231M; 


void setup() {
  pinMode(2, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(2, LOW);
 while (!DS3231M.begin()) {                                                  
   digitalWrite(LED_BUILTIN, HIGH);                                                         
 }
 DS3231M.clearAlarm();
 digitalWrite(LED_BUILTIN, LOW);
 //DS3231M.adjust();                                                           // Set to library compile Date/Time //
  
  // MESSUNG
  delay(2000);

  // set Alarm
  DateTime now = DS3231M.now();
  DS3231M.setAlarm(secondsMinutesHoursDateMatch,now+TimeSpan(0,0,0,12));
  //DS3231M.pinSquareWave();                                                    // Make 1Hz signal on INT/SQW pin   //

  //go to sleep
  digitalWrite(2, HIGH);
}

void loop() {

}
