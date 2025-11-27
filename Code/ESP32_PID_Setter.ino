#include <WiFi.h>
#include <WebServer.h>

const char *ssid = "ESP32_PID_Tuner";
const char *password = "12345678";

WebServer server(80);

// HTML Page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>PID Tuner</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial; text-align: center; margin: 20px; }
    select, input, button { font-size: 1.2rem; margin: 10px; padding: 10px; width: 80%; }
    .label { font-weight: bold; margin-top: 20px; display: block; }
  </style>
</head>
<body>
  <h2>Robot PID Tuner</h2>
  
  <label class="label">Select Controller</label>
  <select id="pidType">
    <option value="0">Straight Line</option>
    <option value="1">Line Follower</option>
    <option value="2">Wall Follower</option>
  </select>

  <label class="label">P (Proportional)</label>
  <input type="number" step="0.000001" id="kp" placeholder="Kp">
  
  <label class="label">I (Integral)</label>
  <input type="number" step="0.000001" id="ki" placeholder="Ki">
  
  <label class="label">D (Derivative)</label>
  <input type="number" step="0.000001" id="kd" placeholder="Kd">

  <br><br>
  <button onclick="getCurrent()">Get Current Values</button>
  <button onclick="updatePID()">Update PID</button>
  
  <p id="status" style="color: blue;"></p>

<script>
  function getCurrent() {
    var id = document.getElementById("pidType").value;
    document.getElementById("status").innerText = "Requesting...";
    
    fetch("/get?id=" + id)
      .then(response => response.json())
      .then(data => {
        if(data.success) {
            document.getElementById("kp").value = data.p;
            document.getElementById("ki").value = data.i;
            document.getElementById("kd").value = data.d;
            document.getElementById("status").innerText = "Loaded current values.";
        } else {
            document.getElementById("status").innerText = "Failed to load.";
        }
      })
      .catch(e => document.getElementById("status").innerText = "Error: " + e);
  }

  function updatePID() {
    var id = document.getElementById("pidType").value;
    var p = document.getElementById("kp").value;
    var i = document.getElementById("ki").value;
    var d = document.getElementById("kd").value;
    
    document.getElementById("status").innerText = "Sending...";
    
    fetch("/set?id=" + id + "&p=" + p + "&i=" + i + "&d=" + d)
      .then(response => response.text())
      .then(text => document.getElementById("status").innerText = text)
      .catch(e => document.getElementById("status").innerText = "Error: " + e);
  }
</script>
</body>
</html>
)rawliteral";

// UART to Mega (Serial2 is typically pins 16/17 on ESP32, verify your board)
#define RXD2 16
#define TXD2 17

void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleSet() {
  if (server.hasArg("id") && server.hasArg("p") && server.hasArg("i") && server.hasArg("d")) {
    String id = server.arg("id");
    String p = server.arg("p");
    String i = server.arg("i");
    String d = server.arg("d");
    
    // Send to Mega: SET <ID> <P> <I> <D>
    Serial2.print("SET ");
    Serial2.print(id); Serial2.print(" ");
    Serial2.print(p); Serial2.print(" ");
    Serial2.print(i); Serial2.print(" ");
    Serial2.println(d);
    
    server.send(200, "text/plain", "Update Sent!");
  } else {
    server.send(400, "text/plain", "Missing arguments");
  }
}

void handleGet() {
  if (server.hasArg("id")) {
    String id = server.arg("id");
    
    // Clear buffer
    while(Serial2.available()) Serial2.read();
    
    // Send request to Mega: GET <ID>
    Serial2.print("GET ");
    Serial2.println(id);
    
    // Wait for response: VAL <ID> <P> <I> <D>
    unsigned long start = millis();
    String response = "";
    bool received = false;
    
    while (millis() - start < 1000) {
        if (Serial2.available()) {
            char c = Serial2.read();
            if (c == '\n') {
                received = true;
                break;
            }
            response += c;
        }
    }
    
    if (received && response.startsWith("VAL")) {
        // Parse VAL <ID> <P> <I> <D>
        // Example: VAL 1 0.5 0.01 10.0
        int firstSpace = response.indexOf(' ');
        int secondSpace = response.indexOf(' ', firstSpace + 1);
        int thirdSpace = response.indexOf(' ', secondSpace + 1);
        int fourthSpace = response.indexOf(' ', thirdSpace + 1);
        
        String p = response.substring(secondSpace + 1, thirdSpace);
        String i = response.substring(thirdSpace + 1, fourthSpace);
        String d = response.substring(fourthSpace + 1);
        
        String json = "{\"success\":true, \"p\":" + p + ", \"i\":" + i + ", \"d\":" + d + "}";
        server.send(200, "application/json", json);
    } else {
        server.send(500, "application/json", "{\"success\":false}");
    }
  } else {
    server.send(400, "text/plain", "Missing id");
  }
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2); // Communicate with Mega at 9600
  
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.on("/get", handleGet);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
