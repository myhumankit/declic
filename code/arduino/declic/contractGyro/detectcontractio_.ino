#include <Mouse.h>
#include <MHK_Button.h>
#include <Wire.h>
#include <MadgwickAHRS.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(11,10);
VirtualButton bouton(LOW);

struct Axis_infos {
	int val = 0;
	int delta = 0;
	unsigned long stable_time = 0;
}

const int MPU_addr=0x68; //I2C address of the MPU-6050

unsigned long microsPerReading, microsPrevious;

Madgwick filter;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Mouse.begin();

  // configure Button object
  //bouton.setDebounceTime(50); // duration in ms: 0 to 255 (default: 0)
  bouton.setHoldTime(500);   // duration in ms: 0 to 65535 (default: 1000)
  bouton.setIdleTime(3000);   // duration in ms: 0 to  65535 (default: 1000)
  bouton.setMultiTime(255);   // duration in ms: 0 to 255 (default: 127)

	// gyro configuration
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  
  // initialize variables to pace updates to correct rate
  filter.begin(25);
	microsPerReading = 1000000 / 25;
  microsPrevious = micros();

	pinMode(11,INPUT);
	pinMode(10,OUTPUT);
	mySerial.begin(9600);
}

float convertRawAcceleration(int aRaw) {
  // we are using 1G range, -1g maps to a raw value of -32768 and +1g maps to a raw value of 32767
  
  float a = (aRaw * 2.0) / 32768.0;
  return a;
}

float convertRawGyro(int gRaw) {
  // since we are using 250 degrees/seconds range, -250 maps to a raw value of -32768 and +250 maps to a raw value of 32767
  
  float g = (gRaw * 250.0) / 32768.0;
  return g;
}

int16_t AcX,AcY,AcZ,GyX,GyY,GyZ;

void getData() {
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
}

float ax, ay, az, gx, gy, gz;

void convert() {
  ax = convertRawAcceleration(AcX);
  ay = convertRawAcceleration(AcY);
  az = convertRawAcceleration(AcZ);
  gx = convertRawGyro(GyX);
  gy = convertRawGyro(GyY);
  gz = convertRawGyro(GyZ);
}

#define SENSITIVITY 20
#define STABILITY 3*1000000
#define HOLD_RIGHT_TIME 500

Axis_infos X, Y;
unsigned long start_time = 0;
bool is_hold = 0;
bool is_long_press = 0;
bool long_press_end = 0;
int contraction_seuil = 400;

char data_to_send = 'a';
// DICTIONARY
// 'a' = nothing
// 'b' = up
// 'c' = upleft
// 'd' = upright
// 'e' = down
// 'f' = downleft
// 'g' = downright
// 'h' = left
// 'i' = right
// 'j' = clickleft
// 'k' = clickright
// 'l' = longclickstart
// 'm' = longclickstop

void loop() {

	//-------------------------------------------------------
	// MOVEMENT PART
	//-------------------------------------------------------

	float X_axis, Y_axis;
	unsigned long microsNow, microsDelay;
	microsNow = micros();
	microsDelay = microsNow - microsPrevious;

	getData();

  if (microsDelay >= microsPerReading) {

    // convert from raw data to gravity and degrees/second units
    convert();

    // update the filter, which computes orientation
    filter.updateIMU(gx, gy, gz, ax, ay, az);

    // get X value
    X_axis = filter.getRoll()-180;
    if (X_axis < - 180) { X_axis = X_axis + 360; }
    X_axis = map(X_axis,  -180, 180, 360, -360);

    // compare to old X value
    if (abs(X_axis - X.val) < SENSITIVITY) {
      X.stable_time += microsDelay;
    }
    else {
      X.stable_time = 0;
    }

    X.val = X_axis;

    // get Y value
    Y_axis = filter.getPitch();
    Y_axis = map(Y_axis, -180, 180, 360, -360);

    // compare to old Y value
    if (abs(Y_axis - Y.val) < SENSITIVITY) {
      Y.stable_time += microsDelay;
    }
    else {
      Y.stable_time = 0;
    }

    Y.val = Y_axis;
    
    Serial.print(X.stable_time);
    Serial.print(" ");
    Serial.println(Y.stable_time);

    if (X.stable_time > STABILITY && Y.stable_time > STABILITY) {
      X.delta = X.val;
      X.stable_time = 0;

      Y.delta = Y.val;
      Y.stable_time = 0;
    }

    // Mouse.move(-(X.val - X.delta), (Y.val - Y.delta), 0);

		int Xdata = -(X.val - X.delta);
		int Ydata = Y.val - Y.delta;

		if (Ydata > 20)
			if (Xdata > 20)
				data_to_send = 'd';
			else if (Xdata < -20);
				data_to_send = 'c';
			else
				data_to_send = 'b';
		else if (Ydata < -20)
			if (Xdata > 20)
				data_to_send = 'g';
			else if (Xdata < -20)
				data_to_send = 'f';
			else
				data_to_send = 'e';
		else
			if (Xdata > 20)
				data_to_send = 'i';
			else if (Xdata < -20)
				data_to_send = 'h';
			else 
				data_to_send = 'a';

    // increment previous time, so we keep proper pace
    microsPrevious = microsPrevious + microsPerReading;

		//-------------------------------------------------------
		// CONTRACTION PART
		//-------------------------------------------------------
	
		int contraction = analogRead(A5);
		Serial.println(contraction);

		if (contraction > contraction_seuil) {
	    bouton.set (1);
	  }
	  else {
		  bouton.set (0);
	  }

	  if (bouton.hold()) {
		  start_time = millis();
			is_hold = 1;
	  }

		if (is_hold and (millis() > HOLD_RIGHT_TIME + start_time)) {
			if (data_to_send == 'a')
				data_to_send = 'l';
		    //Mouse.press();
	    is_hold = 0;
	    is_long_press = 1;
	  }

		if (bouton.released()) {
	    if (is_hold) {
	      if (millis() < HOLD_RIGHT_TIME + start_time) {
					if (data_to_send == 'a')
						data_to_send = 'k';
	        //Mouse.click(MOUSE_RIGHT);
	      }
	      is_hold = 0;
		  }
			else {
				if ((not is_long_press) and (not long_press_end)) {
	        if (data_to_send == 'a')
						data_to_send = 'j';_
						//Mouse.click();
	      }
	    }
	    long_press_end = 0;
	  }

	  if (bouton.pressed() and is_long_press) {
		  is_long_press = 0;
	    long_press_end = 1;
	    if (data_to_send == 'a')
				data_to_send = 'm';
				//Mouse.release();
	  }
		
		if (data_to_sent != 'a')
			mySerial.write(data_to_send);

}
