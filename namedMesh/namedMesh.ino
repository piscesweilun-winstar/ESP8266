//************************************************************
// this is a simple example that uses the painlessMesh library
// 
// This example shows how to build a mesh with named nodes
//
//************************************************************
#include "namedMesh.h"

#define   MESH_SSID       "SystemIntegration"
#define   MESH_PASSWORD   "88888888"
#define   MESH_PORT       8888

#define   PIN_LED 3

Scheduler  userScheduler; // to control your personal task
namedMesh  mesh;

static String nodeName = "node_5"; // Name needs to be unique
static unsigned long id = 5;
//static String cmdOn = "ON";
//static String cmdOff = "OFF";

Task taskSendMessage( TASK_SECOND*30, TASK_FOREVER, []() {
  //String msg = String("This is a message from: ") + nodeName + String(" for logNode");
  //String to = "logNode";
  //mesh.sendSingle(to, msg); 
}); // start with a one second interval

void setup() {
  Serial.begin(115200);

  mesh.setDebugMsgTypes(ERROR | DEBUG | CONNECTION);  // set before init() so that you can see startup messages

  mesh.init(MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT);

  mesh.setName(nodeName); // This needs to be an unique name! 

  if(nodeName != "node_0")
    pinMode(PIN_LED, OUTPUT);

  mesh.onReceive([](uint32_t from, String &msg) {
    Serial.printf("Received message by id from: %u, %s\n", from, msg.c_str());
  });

  mesh.onReceive([](String &from, String &msg) {
    Serial.printf("Received message by name from: %s, %s\n", from.c_str(), msg.c_str());
    unsigned long cmdMask = strtol(msg.c_str(), 0, 16);
    unsigned long myMask = (1 << id);

    Serial.println("MyMask = " + String(myMask, HEX) + ", CmdMask = " + String(cmdMask, HEX));

    if((cmdMask & myMask) != 0) {
      Serial.println("Set to HIGH");
      digitalWrite(PIN_LED, HIGH);
    }
    else {
      Serial.println("Set to LOW");
      digitalWrite(PIN_LED, LOW);
    }
      
    /*
    if(msg.indexOf(cmdOn) >= 0) {
      Serial.println("Set to HIGH");
      digitalWrite(PIN_LED, HIGH);
    }
    else {
      Serial.println("Set to LOW");
      digitalWrite(PIN_LED, LOW);
    }
    */
  });

  mesh.onChangedConnections([]() {
    Serial.printf("Changed connection\n");
  });

  userScheduler.addTask(taskSendMessage);
  taskSendMessage.enable();
}

void loop() {
  if(nodeName == "node_0") {
    long time = millis();
    static int prevIndex = 0;
    int index = (int)(((time / 1000) % 5) + 1);  // 1...5

    if(index != prevIndex) {
      unsigned long mask = (1 << index);
      String cmd = String(mask, HEX);
      Serial.println("Mask = " + cmd);
      mesh.sendBroadcast(cmd);

      prevIndex = index;
    }
    /*
    if(index != prevIndex) {
      for(int i = 1; i <= 5; i++) {
        String node = String("node_") + i;
        if(i == index) {
          String msg = String("Set ") + node + " " + cmdOn;
          Serial.println(msg);
          mesh.sendSingle(node, cmdOn); 
        }
        else
          mesh.sendSingle(node, cmdOff); 
      }
      prevIndex = index;
    }
    */
  }

  // it will run the user scheduler as well
  mesh.update();
}
