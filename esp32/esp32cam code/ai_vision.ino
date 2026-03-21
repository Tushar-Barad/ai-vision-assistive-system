#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Base64.h>
#include "esp_camera.h"
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "your wife name ";
const char* password = "your pass";

// OpenAI API Key
const String apiKey = "YOUR_NEW_API_KEY_HERE"; // ⚠️ Replace with new key

// Pin definitions
#define BUTTON_PIN 13

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = 5;
  config.pin_d1       = 18;
  config.pin_d2       = 19;
  config.pin_d3       = 21;
  config.pin_d4       = 36;
  config.pin_d5       = 39;
  config.pin_d6       = 34;
  config.pin_d7       = 35;
  config.pin_xclk     = 0;
  config.pin_pclk     = 22;
  config.pin_vsync    = 25;
  config.pin_href     = 23;
  config.pin_sscb_sda = 26;
  config.pin_sscb_scl = 27;
  config.pin_pwdn     = 32;
  config.pin_reset    = -1;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size   = FRAMESIZE_QQVGA;
  config.jpeg_quality = 12;
  config.fb_count     = 1;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed: 0x%x\n", err);
    return;
  }

  Serial.println("Camera Initialized");
  Serial.println("Press button to capture.");
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    Serial.println("Button pressed!");
    captureAndAnalyzeImage();
    delay(1000); // Debounce
  }
}

void captureAndAnalyzeImage() {
  Serial.println("Capturing image...");

  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  String base64Image = encodeImageToBase64(fb->buf, fb->len);
  esp_camera_fb_return(fb);

  if (base64Image.isEmpty()) {
    Serial.println("Image encoding failed");
    return;
  }

  AnalyzeImage(base64Image);
}

void AnalyzeImage(const String& base64Image) {
  Serial.println("Sending image to OpenAI...");

  String url = "data:image/jpeg;base64," + base64Image;

  DynamicJsonDocument doc(4096);
  doc["model"] = "gpt-4o";
  JsonArray messages = doc.createNestedArray("messages");
  JsonObject message = messages.createNestedObject();
  message["role"] = "user";
  JsonArray content = message.createNestedArray("content");

  JsonObject textContent = content.createNestedObject();
  textContent["type"] = "text";
  textContent["text"] = "Describe the objects in this image for a blind person.";

  JsonObject imageContent = content.createNestedObject();
  imageContent["type"] = "image_url";
  JsonObject imageUrlObject = imageContent.createNestedObject("image_url");
  imageUrlObject["url"] = url;
  imageUrlObject["detail"] = "auto";

  doc["max_tokens"] = 400;

  String jsonPayload;
  serializeJson(doc, jsonPayload);

  String result;
  if (sendPostRequest(jsonPayload, result)) {
    DynamicJsonDocument responseDoc(4096);
    deserializeJson(responseDoc, result);
    String responseContent = responseDoc["choices"][0]["message"]["content"].as<String>();

    Serial.println("OpenAI Response:");
    Serial.println(responseContent);

    // ✅ Send JSON to Flask
    sendTextToFlask(responseContent, base64Image);
  } else {
    Serial.println("Error: " + result);
  }
}

bool sendPostRequest(const String& payload, String& result) {
  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  http.setReuse(false);
  http.begin(client, "https://api.openai.com/v1/chat/completions");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + apiKey);
  http.setTimeout(20000);

  int httpResponseCode = http.POST(payload);

  if (httpResponseCode > 0) {
    result = http.getString();
    Serial.println("HTTP Code: " + String(httpResponseCode));
    return true;
  } else {
    result = http.errorToString(httpResponseCode);
    return false;
  }
}

// ✅ FIXED: Now sends JSON with both description and image
void sendTextToFlask(const String& text, const String& base64Image) {
  HTTPClient http;
  http.begin("http://192.168.52.232:5000/post"); // Change IP if needed
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(15000);

  DynamicJsonDocument doc(4096);
  doc["description"] = text;
  doc["image"]       = base64Image;

  String payload;
  serializeJson(doc, payload);

  int httpResponseCode = http.POST(payload);

  if (httpResponseCode > 0) {
    Serial.println("Sent to Flask successfully.");
    Serial.println("Response: " + http.getString());
  } else {
    Serial.println("Failed to send to Flask: " + http.errorToString(httpResponseCode));
  }

  http.end();
}

String encodeImageToBase64(const uint8_t* imageData, size_t imageSize) {
  return base64::encode(imageData, imageSize);
}