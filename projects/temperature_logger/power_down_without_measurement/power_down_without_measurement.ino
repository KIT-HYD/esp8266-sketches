/*
 * 
 */
 
#include <DS3231M.h>  
#include <CD74HC4067.h>

// instantiate a DS3231 object to control RTC
DS3231M_Class DS3231M; 

// instantiate a CD74HC4067 object to control Multiplexer
CD74HC4067 mux(4, 5, 6, 7);

void measure() {
  // development
  delay(2000);
}

void setup() {
  /* 
   *  This has to be the first command. Otherwise
   *  the Arduino might instantly turn off the 
   *  power board again
   */
  pinMode(2, OUTPUT);           // set OUT pin mode
  
  /* 
   *  Set Builtin LED to High as long as RTC 
   *  is not found. 
   */
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(2, LOW);
 while (!DS3231M.begin()) {                                                  
   digitalWrite(LED_BUILTIN, HIGH);                                                         
 }
 DS3231M.clearAlarm();
 digitalWrite(LED_BUILTIN, LOW);
 //DS3231M.adjust();                                                           // Set to library compile Date/Time //
  
  /*
   * MEASUREMENT
   */
  measure();

  /* 
   *  Set Alarm to 12 seconds
   *  The Alarm pin is connected to the 
   *  Power control board and will turn it on again.
   *  Then this script is restarted.
   */
  DateTime now = DS3231M.now();
  DS3231M.setAlarm(secondsMinutesHoursDateMatch,now+TimeSpan(0,0,0,12));
  //DS3231M.pinSquareWave();                                                    // Make 1Hz signal on INT/SQW pin   //

  /*
   * Go to sleep
   * 
   * The connected power down board will switch off VCC
   * make sure, that DS3231 is still supplied, otherwise
   * the Alarm will not switch on the power board
   */
  digitalWrite(2, HIGH);
}

void loop() {

}
