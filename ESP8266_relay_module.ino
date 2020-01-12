#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID ""
#define STAPSK  ""
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

const char* page = "<html>Turn <a href=\"/led/on\">ON LED</a><br>Turn <a href=\"/led/off\">OFF LED</a><br>Turn <a href=\"/relay/on\">ON RELAY</a><br>Turn <a href=\"/relay/off\">OFF RELAY</a><br></html>";

ESP8266WebServer server(80);

const int led = 2;
const int relay = 0;

void handleRoot() {
  server.send(200, "text/html", page);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/html", message);
  digitalWrite(led, 0);
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/led/on", []() {
    digitalWrite(led, LOW);
    server.send(200, "text/html", page);
  });

  server.on("/led/off", []() {
    digitalWrite(led, HIGH);
    server.send(200, "text/html", page);
  });
  
  server.on("/relay/on", []() {
    digitalWrite(relay, LOW);
    server.send(200, "text/html", page);
  });

  server.on("/relay/off", []() {
    digitalWrite(relay, HIGH);
    server.send(200, "text/html", page);
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}
