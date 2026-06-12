/*
 * ============================================================
 * RC//CTRL — ESP8266 NodeMCU Firmware v2.1 (DRV8833 Version)
 * Efficient AP mode + HTTP 80 + WebSocket 81 + raw commands
 * ============================================================
 */

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* AP_SSID = "RC_CTRL";
const char* AP_PASSWORD = "12345678";
const IPAddress AP_IP(192, 168, 4, 1);
const IPAddress AP_SUBNET(255, 255, 255, 0);

// DRV8833 Pin Definitions (100% Safe / No Boot Issues)
#define AIN1 D5  // Motor L (Forward)
#define AIN2 D6  // Motor L (Reverse)
#define BIN1 D1  // Motor R (Forward)
#define BIN2 D2  // Motor R (Reverse)

#define FAILSAFE_TIMEOUT_MS 500
#define CLEANUP_INTERVAL_MS 5000

AsyncWebServer httpServer(80);
AsyncWebServer wsServer(81);
AsyncWebSocket ws("/ws");

int motorL = 0;
int motorR = 0;
bool motorsActive = false;
unsigned long lastMsgTime = 0;
unsigned long lastCleanup = 0;

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head><meta charset="UTF-8"><title>RC CTRL</title></head>
<body style="background:#020408;color:#00e5ff;font-family:monospace;display:flex;align-items:center;justify-content:center;height:100vh;margin:0;">
<div style="text-align:center">
  <div style="font-size:24px;letter-spacing:4px;margin-bottom:10px">RC // CTRL</div>
  <div style="opacity:0.6">Firmware is running</div>
  <div style="opacity:0.4;font-size:12px;margin-top:10px">Open the v4.2 HTML controller and tap LINK</div>
</div>
</body></html>
)rawliteral";

// --- DRV8833 Motor Control Logic ---

void driveLeft(int pwm) {
  pwm = constrain(pwm, -255, 255);
  if (pwm > 0) {
    analogWrite(AIN1, pwm);
    analogWrite(AIN2, 0);
  } else if (pwm < 0) {
    analogWrite(AIN1, 0);
    analogWrite(AIN2, -pwm);
  } else {
    analogWrite(AIN1, 0);
    analogWrite(AIN2, 0);
  }
}

void driveRight(int pwm) {
  pwm = constrain(pwm, -255, 255);
  if (pwm > 0) {
    analogWrite(BIN1, pwm);
    analogWrite(BIN2, 0);
  } else if (pwm < 0) {
    analogWrite(BIN1, 0);
    analogWrite(BIN2, -pwm);
  } else {
    analogWrite(BIN1, 0);
    analogWrite(BIN2, 0);
  }
}

void stopAll() {
  driveLeft(0);
  driveRight(0);
  motorL = 0;
  motorR = 0;
  motorsActive = false;
}

bool parseMotorCmd(const char* msg, int &l, int &r) {
  return sscanf(msg, "L%dR%d", &l, &r) == 2;
}

void applyMotors(int l, int r) {
  motorL = constrain(l, -255, 255);
  motorR = constrain(r, -255, 255);
  driveLeft(motorL);
  driveRight(motorR);
  motorsActive = true;
  lastMsgTime = millis();
}

void onWsEvent(AsyncWebSocket* server,
               AsyncWebSocketClient* client,
               AwsEventType type,
               void* arg,
               uint8_t* data,
               size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("[WS] Client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      client->text("RC_CTRL_READY");
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("[WS] Client #%u disconnected -> stop\n", client->id());
      stopAll();
      break;
    case WS_EVT_ERROR:
      Serial.printf("[WS] Client #%u error\n", client->id());
      stopAll();
      break;
    case WS_EVT_DATA: {
      AwsFrameInfo* info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        char buf[48];
        size_t n = min(len, (size_t)47);
        memcpy(buf, data, n);
        buf[n] = '\0';
        int l = 0, r = 0;
        if (parseMotorCmd(buf, l, r)) {
          applyMotors(l, r);
          Serial.printf("[CMD] L=%d R=%d\n", motorL, motorR);
        }
      }
      break;
    }
    default:
      break;
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("=== RC//CTRL ESP8266 Boot ===");

  pinMode(AIN1, OUTPUT); 
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT); 
  pinMode(BIN2, OUTPUT);

  analogWriteRange(255);
  analogWriteFreq(1000);
  stopAll();
  
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(AP_IP, AP_IP, AP_SUBNET);
  bool ok = WiFi.softAP(AP_SSID, AP_PASSWORD);
  Serial.printf("[WiFi] AP %s, IP: %s\n", ok ? "started" : "failed", WiFi.softAPIP().toString().c_str());
  
  ws.onEvent(onWsEvent);
  wsServer.addHandler(&ws);
  wsServer.begin();

  httpServer.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
    req->send_P(200, "text/html", INDEX_HTML);
  });
  httpServer.on("/app", HTTP_GET, [](AsyncWebServerRequest* req) {
    req->send_P(200, "text/html", INDEX_HTML);
  });
  httpServer.on("/ping", HTTP_GET, [](AsyncWebServerRequest* req) {
    req->send(200, "text/plain", "pong");
  });
  httpServer.onNotFound([](AsyncWebServerRequest* req) {
    req->send(404, "text/plain", "Not Found");
  });

  httpServer.begin();
  Serial.println("[HTTP] Server started on port 80");
  Serial.println("[WS] WebSocket at ws://192.168.4.1:81/ws");
}

void loop() {
  unsigned long now = millis();
  
  if (now - lastCleanup > CLEANUP_INTERVAL_MS) {
    ws.cleanupClients();
    lastCleanup = now;
  }

  if (motorsActive && (now - lastMsgTime > FAILSAFE_TIMEOUT_MS)) {
    Serial.println("[FAILSAFE] Timeout -> stopAll()");
    stopAll();
  }

  yield();
}
