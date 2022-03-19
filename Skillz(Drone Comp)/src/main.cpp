#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <MPU6050_light.h>
#include <Wire.h>
#include <ESP32Servo.h>

#define PixelCount 24
#define PixelPin 22

Adafruit_NeoPixel strip(PixelCount, PixelPin, NEO_GRB + NEO_KHZ800);

#define trigPin 14
#define echoPin 21

int gyroRotationZ;
double gyroScale;

int servo1Pin = 2; //הגדרת פין סרוו 1
int servo2Pin = 4; //הגדרת פין סרוו 2

int rot1; //משתנה סיבוב סרוו 1
int rot2; //משתנה סיבוב סרוו 2

void DropBall1(); //הצהרת פונקציית סיבוב סרוו1
void DropBall2(); //הצהרת פונקציית

Servo servo1; // הגדרת סרוו 1
Servo servo2; //הגדרת סרוו 2

int distance; //משתנה המרחק
int duration; //משתנה האורך של הדופק שיוצא מחיישן המרחק

MPU6050 mpu(Wire); //אוביקט חיישן הג'ירו

//הצהרת פונקציות
int DISTANCE();
void DropBallByDistance(void *param);
void NeoPixelByGyro(void *param);

TaskHandle_t Task1, Task2;

void setup()
{
  Serial.begin(115200);

  Wire.begin(26, 25);    //הגדרת חיבור האיי2סי של הפינים של חיישן הגיירו
  mpu.begin();           //אתחול חיישן הג'ירו
  mpu.calcGyroOffsets(); //כיול חיישן הג'ירו

  servo1.attach(servo1Pin); //שיוך הפין של סרוו 1
  servo2.attach(servo2Pin); //שיוך הפין של סרוו 2

  pinMode(trigPin, OUTPUT); //הגדרת פין הטריג כמוצא המערכת
  pinMode(echoPin, INPUT);  //הגדרת פין האקו ככניסה של המערכת

  strip.begin(); //מאתחל את הניאו פיקסל

  //מצמיד פונקציה לליבה מספר 0 של הבקר
  xTaskCreatePinnedToCore(
      DropBallByDistance,
      "DropBallByDistance",
      5024,
      NULL,
      0,
      &Task1,
      0);

  delay(100);
  //מצמיד פונקציה לליבה מספר 1 של הבקר
  xTaskCreatePinnedToCore(
      NeoPixelByGyro,
      "NeoPixelByGyro",
      5024,
      NULL,
      0,
      &Task2,
      1);
}

void loop()
{
  //הלופ נשאר ריק מכיוון שהצמדנו םפונקציות לליבות של הבקר
}

void NeoPixelByGyro(void *param)
{
  mpu.update(); //מעדכן את חיישן הג'ירו

  for (;;) //לולאה אינסופית
  {
    mpu.update(); //מעדכן את חיישן הג'ירו

    gyroRotationZ = mpu.getAngleZ();    //מקבל את זווית חיישן הג'ירו
    int ledAmount = gyroRotationZ / 30; //מחלק את הזוית של הג'ירו כדי לקבל את כמות המנורות שצריך להדליק

    switch (ledAmount)
    {
    case (-1):
      ledAmount = 23;
      break;
    case (-2):
      ledAmount = 22;
      break;
    case (-3):
      ledAmount = 21;
      break;
    case (-4):
      ledAmount = 20;
      break;
    case (-5):
      ledAmount = 19;
      break;
    case (-6):
      ledAmount = 18;
      break;
    case (-7):
      ledAmount = 17;
      break;
    case (-8):
      ledAmount = 16;
      break;
    case (-9):
      ledAmount = 15;
      break;
    case (-10):
      ledAmount = 14;
      break;
    case (-11):
      ledAmount = 13;
      break;
    case (-12):
      ledAmount = 12;
      break;
    }

    if (gyroRotationZ > 0)
    {
      if (ledAmount > 0 && ledAmount < 12) //אם כמות הזוית היא בין 0 ל-360
      {
        strip.clear(); //אתחול רצועת הלדים
        for (int i = 0; i < ledAmount; i++)
        {
          strip.setPixelColor(i, 128, 0, 128); // הדלקת הלדים מ-0 עד הכמות הנדרשת
        }
      }
    }
    else //אם הסיבוב נגד כיוון השעון
    {
      strip.clear();                         // אתחול רצועת הלדים
      if (ledAmount > 11 && ledAmount <= 24) // אם כמות הלדים היא בין 12 ל-24
      {
        for (int i = 24; i > ledAmount; i--)
        {
          strip.setPixelColor(i, 255, 255, 0); //מדליק את הלדים מהלד האחרון ומטה עד הכמות הנדרשת
        }
        if (gyroRotationZ / 30 < 0 && gyroRotationZ / 30 > -30)
        {
          strip.setPixelColor(0, 255, 255, 0); // אם הזוית היא בין 0 ל 30- לכבות את הלד הראשון
        }
      }
    }

    strip.show();
  }
}

void DropBallByDistance(void *param)
{
  for (;;)//לולאה אינסופית
  {
    distance = DISTANCE(); //חישוב מרחק והצבתו במשתנה

    if (distance > 200 && distance < 300)
    {
      DropBall1(); //אם המרחק בין 2 מטר ל3 מטר הפעלת פונקציית סיבוב סרוו 1
    }
    else if (distance > 300)
    {
      DropBall2(); // 2 אם המרחק בין 2 מטר ל3 מטר הפעלת פונקציית סיבוב סרוו
    }
  }
}

int DISTANCE()
{
  duration = 0;
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  return distance;
}

void DropBall1()
{
  for (rot1 = 0; rot1 <= 90; rot1++) //סיבוב של הסרוו בזוית של 90 מעלות
  {
    servo1.write(rot1);
    delay(50); //דיליי של 50 מילי שניות
  }
  delay(1000);                       //דיליי של שנייה
  for (rot1 = 90; rot1 <= 0; rot1--) //סיבוב של הסרוו חזרת לזוית של 0 מעלות
  {
    servo1.write(rot1);
    delay(50); //דיליי של 50 מילי שניות
  }
}

void DropBall2()
{
  for (rot2 = 0; rot2 <= 90; rot2++) //סיבוב של הסרוו בזוית של 90 מעלות
  {
    servo2.write(rot2);
    delay(50); //דיליי של 50 מילי שניות
  }
  delay(1000);
  for (rot2 = 90; rot2 <= 0; rot2--) //סיבוב של הסרוו חזרת לזוית של 0 מעלות
  {
    servo2.write(rot2);
    delay(50); //דיליי של 50 מילי שניות
  }
}
