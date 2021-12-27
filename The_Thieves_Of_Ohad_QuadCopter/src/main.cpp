#include <Arduino.h>
#include <ESP32Servo.h>

#define echoPin 12
#define trigPin 25

int distance = -100;
int serRot;
long duration;

#define pChannel 2
#define piezoPin 27
#define pRes 8

int degreeToDist [180];
void returnToPoint(int _point);

#define LED 26

int dist();
void TurnServo();
int indexofSmallestElement();

Servo myservo;

void setup()
 {
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
    ESP32PWM::allocateTimer(0);

    Serial.begin(115200);

    //piezo
    ledcSetup(pChannel,5000, pRes);  
    ledcAttachPin(piezoPin, pChannel);

    //super sonic thingi 
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(LED,OUTPUT);

    //servo
    myservo.attach(32);
}
void TurnServo(){
    for (int i = 0; i < 180; i++)
    {
        myservo.write(i);
        serRot = i;
        delay(30);
    }
}

void returnToPoint()
{
    int deg = indexofSmallestElement();
    myservo.write(deg);
          digitalWrite(LED, HIGH);
        ledcWriteTone(pChannel, 7000);
            delay(10);
          digitalWrite(LED, LOW);
        ledcWriteTone(pChannel, 0);
            delay(1000);

}

void loop() {
    TurnServo();
    Serial.println(dist());

        degreeToDist[i] = dist();

        if(serRot > 175)
        {
            digitalWrite(LED, HIGH);
            ledcWriteTone(pChannel, 7000);
            delay(1000);
            digitalWrite(LED, LOW);
            ledcWriteTone(pChannel, 0);

            returnToPoint(degreeToDist[indexofSmallestElement()]);
        }
}

int dist(){

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;

    if(distance > 100){
        return 100;
    }
    return distance;
}

int indexofSmallestElement()
{
    int index = 0;

    for(int i = 1; i < 180; i++)
    {
        if(degreeToDist[i] < degreeToDist[index])
            index = i;              
    }

    return index;
}