// MPU-6050 Short Example Sketch
// By Arduino User JohnChi
// August 17, 2014
// Public Domain
#include<Wire.h>
#include<MadgwickAHRS.h>
#include<Mouse.h>

int X_axis = 0, Y_axis = 0;

struct X_infos {
  int val = 0;
  int dX = 0;
  unsigned long stable_time = 0;
};

struct Y_infos {
  int val = 0;
  int dY = 0;
  unsigned long stable_time = 0;
};

const int MPU_addr=0x68;  // I2C address of the MPU-6050

unsigned long microsPerReading, microsPrevious;

Madgwick filter;

void setup(){
  Mouse.begin();
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(9600);
  filter.begin(25);

  // initialize variables to pace updates to correct rate
  microsPerReading = 1000000 / 25;
  microsPrevious = micros();
}

float convertRawAcceleration(int aRaw) {
  // since we are using 2G range
  // -2g maps to a raw value of -32768
  // +2g maps to a raw value of 32767
  
  float a = (aRaw * 1.0) / 32768.0;
  return a;
}

float convertRawGyro(int gRaw) {
  // since we are using 250 degrees/seconds range
  // -250 maps to a raw value of -32768
  // +250 maps to a raw value of 32767
  
  float g = (gRaw * 250.0) / 32768.0;
  return g;
}

int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

void getData() {
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
}

float ax, ay, az;
float gx, gy, gz;

void convert() {
  ax = convertRawAcceleration(AcX);
  ay = convertRawAcceleration(AcY);
  az = convertRawAcceleration(AcZ);
  gx = convertRawGyro(GyX);
  gy = convertRawGyro(GyY);
  gz = convertRawGyro(GyZ);
}

X_infos X;
Y_infos Y;

#define SENSITIVITY 20
#define STABILITY 3*1000000

void loop(){

  float roll, pitch, heading;
  unsigned long microsNow;
  microsNow = micros();

  getData();

  if (microsNow - microsPrevious >= microsPerReading) {

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
      X.stable_time += microsNow - microsPrevious;
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
      Y.stable_time += microsNow - microsPrevious;
    }
    else {
      Y.stable_time = 0;
    }

    Y.val = Y_axis;
    
    // heading = filter.getYaw();
    
    Serial.print(X.stable_time);
    Serial.print(" ");
    Serial.println(Y.stable_time);

    if (X.stable_time > STABILITY && Y.stable_time > STABILITY) {
      X.dX = X.val;
      X.stable_time = 0;

      Y.dY = Y.val;
      Y.stable_time = 0;
    }
    
    //if (Y.stable_time > 30*100000) {
      //Y.dY = Y.val;
      //Y.stable_time = 0;
    //}

    Mouse.move(-(X.val - X.dX), (Y.val - Y.dY), 0);

    // increment previous time, so we keep proper pace
    microsPrevious = microsPrevious + microsPerReading;
  }
}
