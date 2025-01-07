#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h> // Perlu library ArduinoJson, instal melalui Library Manager

const char* ssid = "Galaxy A2044D7";
const char* password = "aopt9791";

//Your Domain name with URL path or IP address with path
String serverName = "http://192.168.122.56:8080/members";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 5 seconds (5000)
unsigned long timerDelay = 5000;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Timer set to 5 seconds (timerDelay variable).");
}

void loop() {
  // Send an HTTP GET request every 5 seconds
  if ((millis() - lastTime) > timerDelay) {
    // Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;

      // Build server path
      String serverPath = serverName + "?temperature=24.37";

      // Connect to server
      http.begin(serverPath.c_str());

      // Send HTTP GET request
      int httpResponseCode = http.GET();

      if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);

        String payload = http.getString();
        Serial.println("Received Payload:");
        Serial.println(payload);

        // Parse JSON payload
        StaticJsonDocument<1024> doc; // Sesuaikan ukuran buffer sesuai kebutuhan
        DeserializationError error = deserializeJson(doc, payload);

        if (!error) {
          // Print members data
          JsonArray members = doc["members"].as<JsonArray>();
          for (JsonObject member : members) {
            Serial.print("ID: ");
            Serial.println(member["id"].as<String>());
            Serial.print("Name: ");
            Serial.println(member["name"].as<String>());
            Serial.print("Category: ");
            Serial.println(member["category_name"].as<String>());
            Serial.println("--------------------------");
          }
        } else {
          Serial.print("JSON Deserialization failed: ");
          Serial.println(error.c_str());
        }
      } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }

      // Free resources
      http.end();
    } else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}
