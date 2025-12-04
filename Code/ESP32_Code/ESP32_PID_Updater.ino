#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

// ---- CONFIGURATION ----
const char* ssid = "Robot_PID_Tuner";  // AP Name
const char* password = "password123";  // AP Password

// Pin connected to Arduino RESET pin
#define ARDUINO_RESET_PIN 4 

// UART to Arduino (Serial2)
#define RXD2 16
#define TXD2 17

Preferences preferences;
WebServer server(80);

// PID Values Structure
struct PIDValues {
  float kp;
  float ki;
  float kd;
};

PIDValues linePID = {0.25, 0.0, 70.0};
PIDValues wallPID = {0.25, 0.1, 90.0};
PIDValues straightPID = {1.5, 1.0, -0.25};

// HTML Page
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>PID Tuner</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial; text-align: center; margin: 0px auto; padding-top: 30px; }
    input, select { padding: 10px; font-size: 20px; width: 80%; max-width: 300px; margin: 10px; }
    input[type=submit] { background-color: #4CAF50; border: none; color: white; padding: 16px 32px; text-decoration: none; margin: 4px 2px; cursor: pointer; font-size: 20px;}
    .label { display: inline-block; width: 50px; text-align: right; font-weight: bold; }
  </style>
  <script>
    var pids = {
      0: { kp: )rawliteral" + String(linePID.kp) + R"rawliteral(, ki: )rawliteral" + String(linePID.ki) + R"rawliteral(, kd: )rawliteral" + String(linePID.kd) + R"rawliteral( },
      1: { kp: )rawliteral" + String(wallPID.kp) + R"rawliteral(, ki: )rawliteral" + String(wallPID.ki) + R"rawliteral(, kd: )rawliteral" + String(wallPID.kd) + R"rawliteral( },
      2: { kp: )rawliteral" + String(straightPID.kp) + R"rawliteral(, ki: )rawliteral" + String(straightPID.ki) + R"rawliteral(, kd: )rawliteral" + String(straightPID.kd) + R"rawliteral( }
    };

    function updateFields() {
      var type = document.getElementById("pidType").value;
      document.getElementById("kp").value = pids[type].kp;
      document.getElementById("ki").value = pids[type].ki;
      document.getElementById("kd").value = pids[type].kd;
    }
  </script>
</head>
<body onload="updateFields()">
  <h2>Robot PID Tuner</h2>
  <form action="/update" method="POST">
    <select id="pidType" name="type" onchange="updateFields()">
      <option value="0">Line Follower</option>
      <option value="1">Wall Follower</option>
      <option value="2">Straight Line</option>
    </select><br>
    
    <span class="label">Kp:</span> <input type="text" id="kp" name="kp"><br>
    <span class="label">Ki:</span> <input type="text" id="ki" name="ki"><br>
    <span class="label">Kd:</span> <input type="text" id="kd" name="kd"><br>
    
    <input type="submit" value="Update Robot">
  </form>
</body>
</html>
)rawliteral";
  server.send(200, "text/html", html);
}

void handleUpdate() {
  if (!server.hasArg("type") || !server.hasArg("kp") || !server.hasArg("ki") || !server.hasArg("kd")) {
    server.send(400, "text/plain", "Missing arguments");
    return;
  }

  int type = server.arg("type").toInt();
  float kp = server.arg("kp").toFloat();
  float ki = server.arg("ki").toFloat();
  float kd = server.arg("kd").toFloat();

  // Save to Preferences and update global struct
  preferences.begin("pid", false);
  if (type == 0) {
    preferences.putFloat("l_kp", kp); preferences.putFloat("l_ki", ki); preferences.putFloat("l_kd", kd);
    linePID = {kp, ki, kd};
  } else if (type == 1) {
    preferences.putFloat("w_kp", kp); preferences.putFloat("w_ki", ki); preferences.putFloat("w_kd", kd);
    wallPID = {kp, ki, kd};
  } else if (type == 2) {
    preferences.putFloat("s_kp", kp); preferences.putFloat("s_ki", ki); preferences.putFloat("s_kd", kd);
    straightPID = {kp, ki, kd};
  }
  preferences.end();

  String message = "Type:" + String(type) + ",Kp:" + String(kp) + ",Ki:" + String(ki) + ",Kd:" + String(kd) + "\n";
  
  Serial.println("Received Update: " + message);

  // 1. Reset Arduino
  Serial.println("Resetting Arduino...");
  digitalWrite(ARDUINO_RESET_PIN, LOW);
  delay(100);
  digitalWrite(ARDUINO_RESET_PIN, HIGH);
  
  // 2. Wait for Arduino to boot
  delay(500);

  // 3. Send PID values
  Serial.println("Sending to Arduino: " + message);
  Serial2.print(message);

  server.send(200, "text/html", "<h1>Updated!</h1><p>Values saved and sent to robot.</p><a href=\"/\">Back</a>");
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  pinMode(ARDUINO_RESET_PIN, OUTPUT);
  digitalWrite(ARDUINO_RESET_PIN, HIGH);

  // Load Preferences
  preferences.begin("pid", true); // Read-only mode first
  linePID.kp = preferences.getFloat("l_kp", linePID.kp);
  linePID.ki = preferences.getFloat("l_ki", linePID.ki);
  linePID.kd = preferences.getFloat("l_kd", linePID.kd);

  wallPID.kp = preferences.getFloat("w_kp", wallPID.kp);
  wallPID.ki = preferences.getFloat("w_ki", wallPID.ki);
  wallPID.kd = preferences.getFloat("w_kd", wallPID.kd);

  straightPID.kp = preferences.getFloat("s_kp", straightPID.kp);
  straightPID.ki = preferences.getFloat("s_ki", straightPID.ki);
  straightPID.kd = preferences.getFloat("s_kd", straightPID.kd);
  preferences.end();

  WiFi.softAP(ssid, password);
  Serial.println("AP Started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/update", handleUpdate);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
