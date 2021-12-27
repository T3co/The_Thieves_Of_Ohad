#include <Arduino.h>
#include <ESP32Servo.h>

#define echoPin 12
#define trigPin 25

int distance;
int serRot;
long duration;

#define pChannel 2
#define piezoPin 27
#define pRes 8

int degreeToDist[180];
void returnToPoint();

#define LED 26

int dist();
void TurnServo();
int indexofSmallestElement();

Servo myservo;

void setup()
{
    Serial.begin(115200);

    ledcSetup(pChannel, 5000, pRes);
    ledcAttachPin(piezoPin, pChannel);

    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(LED, OUTPUT);

    myservo.attach(32);

    myservo.write(0);
}
void TurnServo()
{
    for (int i = 0; i < 180; i++)
    {
        degreeToDist[i] = dist();
        myservo.write(i);
        serRot = i;

        //Serial.print("Angle is:");
        //Serial.println(i);
        //Serial.print("Distance is:");
        //Serial.println(distance);

        delay(20);
    }
}

void returnToPoint()
{
    int deg = indexofSmallestElement();
    myservo.write(deg);
    delay(1000);
    //ESP.restart();
}

void loop()
{
    TurnServo();
    distance = dist();

    if (serRot > 175)
    {
        digitalWrite(LED, HIGH);

        ledcWriteTone(pChannel, 7000);

        delay(1000);
        
        digitalWrite(LED, LOW);

        ledcWriteTone(pChannel, 0);

        returnToPoint();
    }
}
int dist()
{
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;

    if (distance > 100)
    {
        return 100;
    }
    else if (distance <= 10)
    {
        return 10;
    }
    else if (distance > 10 && distance < 100)
    {
        return distance;
    }
}

int indexofSmallestElement()
{
    int index = 0;

    for (int i = 1; i < 180; i++)
    {
        if (degreeToDist[i] < degreeToDist[index])
            index = i;
    }

    return index;
}