/*
 * 
 */
 
#include <DS3231M.h>  
#include <CD74HC4067.h>
#include <Adafruit_MAX31865.h>

// instantiate a DS3231 object to control RTC
DS3231M_Class DS3231M; 

// instantiate a CD74HC4067 object to control Multiplexer
CD74HC4067 mux(4, 5, 6, 7);

// instantiate the MAX31865 RTD amplifier
Adafruit_MAX31865 RTD = Adafruit_MAX31865(10, 11, 12, 13);

/*
 *  ---------------------------------------------------
 *  CALIBRATION
 *  ---------------------------------------------------
 *  RREF and RNOMINAL are needed to get precise 
 *  measurements. RREF is the reference resistance 
 *  the RTD gets compared to. It's either 430 Ohm 
 *  for a PT100 or 4300 Ohm for a PT1000.
 *  RNOMINAL is the nominal resistance of the sensor 
 *  at 0°C. As we use a multiplexer BETWEEN the sensors 
 *  and the RTD board, this resistance changes and needs
 *  to be calibrated. Replace with measured value
 */
#define RREF 4300.0
#define RNOMINAL 1000.0

void measure() {
  // DEVELOPMENT
  Serial.begin(115200);
  
  //initialize the RTD amplifier
  RTD.begin(MAX31865_2WIRE);      //_3WIRE and _4WIRE are also possible

  // make measurement
  uint16_t rtd = RTD.readRTD();
  float ratio = rtd;
  ratio /= 32768;

  float resistance = RREF * ratio;
  float temp = RTD.temperature(RNOMINAL, RREF);

  // DEVELOPMENT 
  // return the values here.
  Serial.print(resistance, 8);
  Serial.print(", ");
  Serial.println(temp, 8);

  // TODO: fault handling is missing. RTD.readFault() can be used
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
