// subscribe.cpp take no arguments
// It control on ioadafruit website
// e.g. ./subcribe
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"
#include "GPIO.h"
#include "led.h"
using namespace std;
using namespace exploringBB;

#define ADDRESS     "tcp://io.adafruit.com:1883"
#define CLIENTID    "Beagle1"
#define TOPIC       "Bentan01/feeds/project.external-led"
#define TOPIC_LED   "Bentan01/feeds/project.led"
#define AUTHMETHOD  "Bentan01"
#define AUTHTOKEN   "aio_xMRh089wclbukiOk8W1HxnULc2q3"
#define QOS         1
#define TIMEOUT     10000L
#define LED_GPIO    "/sys/class/gpio/gpio60/"

// Use this function to control the LED

volatile MQTTClient_deliveryToken deliveredtoken;

void delivered(void *context, MQTTClient_deliveryToken dt) {
    printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    char* payloadptr;
    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("   message: ");
    payloadptr = (char*) message->payload;
    string status(payloadptr);
    if (status == "ON" || status == "OFF") {
        cout << "The LED is " <<  status << endl;
        GPIO gpio60(60);
        status == "ON" ? gpio60.setValue(HIGH) : gpio60.setValue(LOW);
    }
    else if (stoi(status) >= 0) {
        int number = stoi(status);
        LED led(number);
        if (led.readBrightness() == "1") {
          led.turnOff();
        }
        else if (led.readBrightness() == "0") {
          led.turnOn();
        }
    }
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void connlost(void *context, char *cause) {
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

int main(int argc, char* argv[]) {
    MQTTClient client;
    MQTTClient_connectOptions opts = MQTTClient_connectOptions_initializer;
    int rc;
    int ch;

    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    opts.keepAliveInterval = 20;
    opts.cleansession = 1;
    opts.username = AUTHMETHOD;
    opts.password = AUTHTOKEN;
    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
    if ((rc = MQTTClient_connect(client, &opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    }
    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n", TOPIC, CLIENTID, QOS);
    MQTTClient_subscribe(client, TOPIC, QOS);
    MQTTClient_subscribe(client, TOPIC_LED, QOS);
    do {
        ch = getchar();
    } while(ch!='Q' && ch != 'q');
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
}

