#include <WiFi.h>
#include <WebServer.h>

// ---- CONFIGURATION ----
const char* ssid = "Robot_PID_Tuner";  // AP Name
const char* password = "password123";  // AP Password

// Pin connected to Arduino RESET pin
// WARNING: ESP32 is 3.3V, Arduino Mega Reset is 5V logic (usually pulled up).
// Direct connection might work if ESP32 pulls LOW, but a level shifter or transistor is safer.
// If connecting directly, ensure ESP32 pin is Open Drain or only drives LOW.
#define ARDUINO_RESET_PIN 4 

// UART to Arduino (Serial2)
#define RXD2 16
#define TXD2 17

WebServer server(80);

// HTML Page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>PID Tuner</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial; text-align: center; margin: 0px auto; padding-top: 30px; }
    input { padding: 10px; font-size: 20px; width: 80px; margin: 10px; }
    input[type=submit] { background-color: #4CAF50; border: none; color: white; padding: 16px 32px; text-decoration: none; margin: 4px 2px; cursor: pointer; font-size: 20px;}
  </style>
</head>
<body>
  <h2>Robot PID Tuner</h2>
  <form action="/update">
    Kp: <input type="text" name="kp" value="1.0"><br>
    Ki: <input type="text" name="ki" value="0.0"><br>
    Kd: <input type="text" name="kd" value="0.0"><br>
    <input type="submit" value="Update Robot">
  </form>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleUpdate() {
  String kp = server.arg("kp");
  String ki = server.arg("ki");
  String kd = server.arg("kd");

  String message = "Kp:" + kp + ",Ki:" + ki + ",Kd:" + kd + "\n";
  
  Serial.println("Received: " + message);

  // 1. Reset Arduino
  Serial.println("Resetting Arduino...");
  digitalWrite(ARDUINO_RESET_PIN, LOW); // Pull RESET LOW
  delay(100);                           // Hold for 100ms
  digitalWrite(ARDUINO_RESET_PIN, HIGH); // Release RESET
  
  // 2. Wait for Arduino to boot and enter setup() (it waits 2s for input)
  delay(500); // Wait 500ms for bootloader/init

  // 3. Send PID values
  Serial.println("Sending to Arduino: " + message);
  Serial2.print(message);

  server.send(200, "text/html", "<h1>Updated!</h1><p>Arduino reset and PID sent.</p><a href=\"/\">Back</a>");
}

void setup() {
  Serial.begin(115200);
  
  // Setup UART to Arduino
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  // Setup Reset Pin
  pinMode(ARDUINO_RESET_PIN, OUTPUT);
  digitalWrite(ARDUINO_RESET_PIN, HIGH); // Default HIGH (Running)

  // Setup WiFi AP
  WiFi.softAP(ssid, password);
  Serial.println("AP Started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Setup Server
  server.on("/", handleRoot);
  server.on("/update", handleUpdate);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
