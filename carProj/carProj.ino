#include <WiFi.h>
#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>

#define WIFI_SSID "" // change with your own wifi ssid
#define WIFI_PASS "" // change with your own wifi password


int Lmotor1Pin1 = 26; 
int Lmotor1Pin2 = 25; 
int Lenable1Pin = 27; 

int Rmotor1Pin1 = 32; 
int Rmotor1Pin2 = 33; 
int Renable1Pin = 35; 

const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;
int dutyCycle = 200;

bool isUp = false;
bool isDown = false;
bool isLeft = false;
bool isRight = false;

//socket url
char* websockets_server = "wss://";
using namespace websockets;
WebsocketsClient client;


void setup()
{
  Serial.begin(115200);

  // try to connect to the wifi
  if(connect() == 0) { return ; }

  ConnectWebSocket();
  pinMode(Lmotor1Pin1, OUTPUT);
  pinMode(Lmotor1Pin2, OUTPUT);
  pinMode(Lenable1Pin, OUTPUT);
  
  pinMode(Rmotor1Pin1, OUTPUT);
  pinMode(Rmotor1Pin2, OUTPUT);
  pinMode(Renable1Pin, OUTPUT);
  ledcSetup(pwmChannel, freq, resolution);
  ledcAttachPin(Lenable1Pin, pwmChannel);
  delay(100);
}
int counter = 0;
void loop() {
  client.poll();
  delay(100);
  counter = counter + 100;
  if(counter%5000 == 0) {
      client.send("{\"action\":\"update\",\"type\":\"car1\"}");
  }



      if(isUp) {
           up();
      }else if(isDown) {
           down();
      }else if(isLeft) {
           left();
      }else if(isRight) {
          right();
      }else{
          digitalWrite(Lmotor1Pin1, LOW);
          digitalWrite(Rmotor1Pin1, LOW);
          digitalWrite(Lmotor1Pin2, LOW);
          digitalWrite(Rmotor1Pin2, LOW);
      }
 
 
}


void left() { 

  digitalWrite(Lmotor1Pin1, LOW);
  digitalWrite(Rmotor1Pin1, HIGH);
  digitalWrite(Lmotor1Pin2, LOW);
  digitalWrite(Rmotor1Pin2, LOW);
}

void right() { 

  digitalWrite(Lmotor1Pin1, LOW);
  digitalWrite(Rmotor1Pin1, LOW);
  digitalWrite(Lmotor1Pin2, HIGH);
  digitalWrite(Rmotor1Pin2, LOW);

}

void up() { 

  digitalWrite(Lmotor1Pin1, HIGH);
  digitalWrite(Rmotor1Pin1, LOW);
  digitalWrite(Lmotor1Pin2, LOW);
  digitalWrite(Rmotor1Pin2, HIGH);
}


void down() { 

  digitalWrite(Lmotor1Pin1, LOW);
  digitalWrite(Rmotor1Pin1, HIGH);
  digitalWrite(Lmotor1Pin2, HIGH);
  digitalWrite(Rmotor1Pin2, LOW);
}

void ConnectWebSocket(){
      client.setInsecure();
      client.onMessage(onMessageCallback);
      client.onEvent(onEventsCallback);
      // Connect to server
      client.connect(websockets_server);
}

// {"upL":"true","downL":"true","rightL":"true","leftL":"true"}
void onMessageCallback(WebsocketsMessage message) {
    Serial.print("Got Message: ");
    String msg = message.data();
    msg.replace("Echo:","");
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(msg);
    Serial.print(""+msg);
    const char* up = root["upL"];
    const char* down = root["downL"];
    const char* right = root["rightL"];
    const char* left = root["leftL"];
    char leftbuffer[1024];
    char righbuffer[1024];
    char downbuffer[1024];
    char upuffer[1024];
    strcpy(upuffer, up);
    strcpy(downbuffer, down);
    strcpy(righbuffer, right);
    strcpy(leftbuffer, left);
    String upString = String(upuffer);
    String downString = String(downbuffer);
    String leftString = String(leftbuffer);
    String rightString = String(righbuffer);
    Serial.print(""+upString);
    Serial.print("\n");
    isUp = false;
    isDown = false;
    isLeft = false;
    isRight = false;
    
    if(upString=="true"){
      isUp=true;
    }
    if(downString=="true"){
      isDown=true;
    }
    if(leftString=="true"){
      isLeft=true;
    }
    if(rightString=="true"){
      isRight=true;
    }
}
void onEventsCallback(WebsocketsEvent event, String data) {
    Serial.println("111MSG");
    if(event == WebsocketsEvent::ConnectionOpened) {
        Serial.println("Connnection Opened");
        client.send("{\"action\":\"onConnected\",\"type\":\"car1\"}");
    } else if(event == WebsocketsEvent::ConnectionClosed) {
        Serial.println("Connnection Closed");
    } else if(event == WebsocketsEvent::GotPing) {
        Serial.println("Got a Ping!");
    } else if(event == WebsocketsEvent::GotPong) {
        Serial.println("Got a Pong!");
    }
}

char connect()
{
  WiFi.begin(WIFI_SSID, WIFI_PASS); 

  Serial.println("Waiting for wifi");
  int timeout_s = 30;
  while (WiFi.status() != WL_CONNECTED && timeout_s-- > 0) {
      delay(1000);
      Serial.print(".");
  }
  
  if(WiFi.status() != WL_CONNECTED)
  {
    Serial.println("unable to connect, check your credentials");
    return 0;
  }
  else
  {
    Serial.println("Connected to the WiFi network");
    Serial.println(WiFi.localIP());
    return 1;
  }
}
