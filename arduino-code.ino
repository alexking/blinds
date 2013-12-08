#include <SoftwareSerial.h> 
#include <SmartThings.h>
#include <Servo.h> 
#include "DHT.h"

#define PIN_THING_RX    3
#define PIN_THING_TX    2
#define DHTPIN         11
#define PIN_SERVO      12

// Settings 
#define OPEN_PERCENT   60
#define CLOSE_PERCENT   1

#define DHTTYPE     DHT11   

DHT dht(DHTPIN, DHTTYPE);

Servo myservo;  

int ledPin = 13;
int stateLED;

SmartThingsCallout_t messageCallout;    // call out function forward decalaration
SmartThings smartthing(PIN_THING_RX, PIN_THING_TX, messageCallback, "GenericShield", false);  // constructor

void setup()
{

	// setup default state of global variables
	stateLED = 0;                 // matches state of hardware pin set below
	
	// setup hardware pins 
	pinMode(ledPin, OUTPUT);     // define PIN_LED as an output
	digitalWrite(ledPin, LOW);   // set value to LOW (off) to match stateLED=0
	
	dht.begin();

	// Do this last 	
	Serial.begin(115200);
	Serial.println("Mini Blinds");
 	Serial.println("===========");
 
}

// Keep track of current sensor readings 
int   currentLight;
int   currentHumidity;
float currentTemperature; 

// Intervals 
unsigned long   lightInterval    = (     1 * 1000); 
unsigned long   humidityInterval = (    25 * 1000); 
unsigned long   reportInterval   = (2 * 60 * 1000); 


unsigned long   lastHumidityCheckAt  = 0; 
unsigned long   lastLightCheckAt     = 0; 
unsigned long 	lastReportAt         = 0; 

void loop()
{
	// SmartThings 
	smartthing.run();

	// Timing code 
	unsigned long currentMillis = millis();

	// First and at checkInterval 
	if (currentMillis - lastHumidityCheckAt > humidityInterval || lastHumidityCheckAt == 0)
	{
		lastHumidityCheckAt = currentMillis;

		// Do a check 
		checkHumidity();

	}

	// First and at checkInterval 
	if (currentMillis - lastLightCheckAt > lightInterval || lastLightCheckAt == 0)
	{
		lastLightCheckAt = currentMillis;

		// Do a check 
		checkLight();

	}

	// First and at reportInterval 
	if (currentMillis - lastReportAt > reportInterval || lastReportAt == 0)
	{
		lastReportAt = currentMillis;

		// Do a report  
		reportData();

	}

}

void checkData()
{
	checkHumidity();
	checkLight();
}

void checkLight() 
{
	Serial.println("Checking light...");

	int light = ( (int) ( ( (float) analogRead(1)) * (100.0 / 1023.0) ) * 2) / 2; 

	// If notice a large change, we let them know immediately.
	if (light > currentLight + 5 || light < currentLight - 5)
	{
		currentLight = light; 
		reportData();
	}

	currentLight = light; 
	
	Serial.println(currentLight);

}

// Keep a hot cache with data, ready to deliver to respond to a poll instantly 
void checkHumidity() 
{
	Serial.println("Checking humidity...");

	// Read light, humidity, and temperature 
	int humidity    = (int) dht.readHumidity();
	int temperature = (int) dht.readTemperature();

	// Discard any data that is NaN
	if (isnan(temperature) || isnan(humidity)) 
	{
		Serial.println("Failed to read from DHT");
	} else {
		currentTemperature = temperature;
		currentHumidity = humidity;
	}
	
	Serial.println(currentLight);


}

void reportData()
{
	Serial.println("Reporting data...");

	// We must insist on actually having some data to send 
	if (isnan(currentHumidity) || isnan(currentLight)) {
		
		smartthing.shieldSetLED(4, 0, 0);

		Serial.println("We're not in a loop are we?");

		checkData(); 
		reportData();
		return; 
	}

	smartthing.shieldSetLED(3, 3, 3);

	// Report humidity 
	String humidityMessage = "h";
	humidityMessage.concat(currentHumidity); 
	smartthing.send(humidityMessage);

	// Report light 
	String msg2 = "l";
	msg2 += currentLight; 
	smartthing.send(msg2);

	// Reset light 
	smartthing.shieldSetLED(1, 1, 1);

}

// Keep track of request progress
int request_in_progress = 0; 

// Keep track of open percent 
int rotation_percent = 0;

void messageCallback(String message)
{
	Serial.print("--Last Check :");

	
	if (message.equals("on"))
	{
	 
		// Open is yellow 
		smartthing.shieldSetLED(4, 4, 0);
		openToPercent(OPEN_PERCENT);
		
		smartthing.send("!on");

		String msgOpen = "%";
		msgOpen += OPEN_PERCENT; 
		smartthing.send(msgOpen);
	}
	else if (message.equals("off"))
	{
	 
		// Close is blue 
		smartthing.shieldSetLED(0, 4, 0);
		openToPercent(CLOSE_PERCENT);
		
		smartthing.send("!off");

		String msgClose = "%";
		msgClose += CLOSE_PERCENT; 
		smartthing.send(msgClose);

	} else if (message.equals("close")) {
			
		 // Calibrate 
		 servoRotateRightTimes(1);
		 rotation_percent = 0; 
	
	} else if (message.length() > 0 && message.startsWith("%")) {

		int percentToOpen = message.substring(1).toInt(); 
		
		// Eager message
		String msg = "%";
		msg += percentToOpen; 
		smartthing.send(msg);

		if (percentToOpen >= OPEN_PERCENT)
		{
			smartthing.send("on");
		} else {
			smartthing.send("off");
		}

		openToPercent(percentToOpen);
 
	} else if (message.equals("poll")) {

		reportData();

	}
	
	Serial.println(message);
	
	smartthing.shieldSetLED(1, 1, 1);
}



void openToPercent(int percent)
{
	Serial.print("Request to move to");
	Serial.println(percent); 
 
	if (request_in_progress == 1) { 
		Serial.println("Request already in progress, please try again later"); 
		return; 
	}
	
	// Is this more or less than the current percentage?
	if (percent == rotation_percent) {
		 
		 // Well that was easy
		 Serial.println("Already at correct position");
		 return;
		 
	} else if (percent > rotation_percent) {
	 
		 // How many points to open?
		 int open_points = percent - rotation_percent; 
		 
			smartthing.shieldSetLED(4, 4, 0);
			
			Serial.print("Opening by: ");
			Serial.println(open_points); 
			
			// Request
			openPercents(open_points);
	} else if (percent < rotation_percent) {
	 
		 // How many points to close
		 int close_points = abs( percent - rotation_percent ); 
		 
		 smartthing.shieldSetLED(0, 4, 0);

		 Serial.print("Closing by: ");
		 Serial.println(close_points); 
			
		 // Request 
		 closePercents(close_points);
		
	}
		 
}


void openPercents(int percents)
{  
	// Refuse to turn above 100 percent
	int will_rotate_to = rotation_percent + percents;    
	if (will_rotate_to > 100) { 
		return;
	}
	
	// Set
	rotation_percent = will_rotate_to; 
	
	// Talk
	Serial.print("Opening To: ");
	Serial.println(rotation_percent); 
	
	// Note
	request_in_progress = 1; 
	
	// Turn
	servoRotateLeftTimes(percents * 0.07);  
	 
	// Finish
	request_in_progress = 0; 
	 
}

void closePercents(int percents)
{
		// Refuse to turn below 0 percent
		int will_rotate_to = rotation_percent - percents;
		if (will_rotate_to < 0) 
		{
			return;
		}
		
		// Set 
		rotation_percent = will_rotate_to;
		
		// Talk 
		Serial.print("Closing To: ");
		Serial.println(rotation_percent); 
		
		// Note
		request_in_progress = 1; 
		
		// Turn
		servoRotateRightTimes(percents * 0.07); 
		
		// Finish
		request_in_progress = 0; 
}

// Handly Parallax Code 
void servoRotateLeftTimes(float times)
{
	 servoRotateLeftForSeconds((times * 1.2));
}

void servoRotateRightTimes(float times)
{
	 servoRotateRightForSeconds((times * 1.2));
}


void servoRotateLeftForSeconds(float seconds)
{
	servoRotateDirectionForSeconds(1, seconds); 
}

void servoRotateRightForSeconds(float seconds)
{
	servoRotateDirectionForSeconds(0, seconds);  
}

void servoRotateDirectionForSeconds(int direction, float seconds)
{
	servoRotateDirectionAtSpeedForSeconds(direction, 0, seconds); 
}

void servoRotateDirectionAtSpeedForSeconds(int direction, int speed, float seconds) 
{
	int microseconds; 
	
	// Rotate at speed
	if (direction == 1)
	{
		microseconds = 1300 + speed;
	} else {
		microseconds = 1700 - speed; 
	}
	
	servoWriteMicrosecondsForSeconds(microseconds, seconds);
	
}

void servoStopForSeconds(float seconds)
{
	servoWriteMicrosecondsForSeconds(1500, seconds);
}

void servoWriteMicrosecondsForSeconds(int microseconds, float seconds)
{
	// Attach 
	myservo.attach(PIN_SERVO);
	
	// Write
	myservo.writeMicroseconds(microseconds);
	
	// Delay 
	delay(1000 * seconds);
	
	// Always detach afterwards 
	myservo.detach();
	
}

