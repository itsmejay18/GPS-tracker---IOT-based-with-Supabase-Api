#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

static const uint32_t USB_BAUD = 115200;
static const uint32_t GPS_BAUD = 9600;
static const uint32_t SEND_INTERVAL_MS = 10000;
static const uint32_t WIFI_RETRY_INTERVAL_MS = 10000;

// Wi-Fi
static const char *WIFI_SSID = "POCO C85";
static const char *WIFI_PASSWORD = "0000000000";

// Supabase
// Fill these in from your Supabase project's Connect/API page.
static const char *SUPABASE_URL = "https://lpzcsgoctfwagofrsvhw.supabase.co";
static const char *SUPABASE_PUBLISHABLE_KEY = "sb_publishable_BJE6FFXlsY2EmiHJ25tblw_YSzhJ0bW";
static const char *SUPABASE_TABLE = "gps_locations";
// Pair this exact device_id to one car in the WPF IoT tracker settings.
static const char *DEVICE_ID = "esp12e-gps-1";

// Wiring:
// GPS TX -> D7
// GPS RX -> disconnected
// GND -> GND
static const int GPS_RX_PIN = 13;  // D7 = GPIO13
static const int GPS_TX_PIN = -1;  // not used

TinyGPSPlus gps;
EspSoftwareSerial::UART gpsSerial;

uint32_t lastSendAt = 0;
uint32_t lastWifiRetryAt = 0;

void ensureWiFiConnected() {
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  if (millis() - lastWifiRetryAt < WIFI_RETRY_INTERVAL_MS && lastWifiRetryAt != 0) {
    return;
  }

  lastWifiRetryAt = millis();

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");
  uint32_t startAt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAt < 15000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Wi-Fi connected. IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Wi-Fi not connected yet.");
  }
}

bool supabaseConfigured() {
  return String(SUPABASE_URL).startsWith("https://") &&
         String(SUPABASE_PUBLISHABLE_KEY) != "YOUR_PUBLISHABLE_OR_ANON_KEY";
}

bool sendLocationToSupabase(double latitude, double longitude) {
  if (!supabaseConfigured()) {
    Serial.println("Supabase not configured yet.");
    return false;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Skipping send: Wi-Fi is not connected.");
    return false;
  }

  std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
  client->setInsecure();

  HTTPClient https;
  String url = String(SUPABASE_URL) + "/rest/v1/" + SUPABASE_TABLE;

  if (!https.begin(*client, url)) {
    Serial.println("HTTPS begin failed.");
    return false;
  }

  https.addHeader("Content-Type", "application/json");
  https.addHeader("apikey", SUPABASE_PUBLISHABLE_KEY);
  https.addHeader("Prefer", "return=minimal");

  String payload = "{";
  payload += "\"device_id\":\"" + String(DEVICE_ID) + "\",";
  payload += "\"latitude\":" + String(latitude, 6) + ",";
  payload += "\"longitude\":" + String(longitude, 6);
  payload += "}";

  int httpCode = https.POST(payload);
  https.end();

  if (httpCode >= 200 && httpCode < 300) {
    Serial.println("Supabase insert success.");
    return true;
  }

  Serial.print("Supabase insert failed. HTTP ");
  Serial.println(httpCode);
  return false;
}

void setup() {
  Serial.begin(USB_BAUD);
  Serial.setDebugOutput(false);
  delay(300);

  gpsSerial.begin(GPS_BAUD, SWSERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN, false, 256, 32);
  gpsSerial.listen();

  Serial.println("Starting GPS + Supabase tracker...");
  ensureWiFiConnected();

  if (!supabaseConfigured()) {
    Serial.println("Set SUPABASE_URL and SUPABASE_PUBLISHABLE_KEY in the sketch.");
  }
}

void loop() {
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  ensureWiFiConnected();

  if (!gps.location.isValid()) {
    static uint32_t lastWaitMessageAt = 0;
    if (millis() - lastWaitMessageAt >= 2000) {
      Serial.println("Waiting for GPS fix...");
      lastWaitMessageAt = millis();
    }
    return;
  }

  if (millis() - lastSendAt < SEND_INTERVAL_MS) {
    return;
  }

  lastSendAt = millis();

  double latitude = gps.location.lat();
  double longitude = gps.location.lng();

  Serial.print("Latitude: ");
  Serial.println(latitude, 6);
  Serial.print("Longitude: ");
  Serial.println(longitude, 6);
  Serial.print("Google Maps: https://www.google.com/maps?q=");
  Serial.print(latitude, 6);
  Serial.print(",");
  Serial.println(longitude, 6);

  sendLocationToSupabase(latitude, longitude);
  Serial.println();
}

