#include <WiFi.h>
#include <HTTPClient.h>

#define led_blue 9 // Pin used to control the blue led
#define led_green 41 // Pin used to control the green led
#define led_red 40 // Pin used to control the red led
#define led_yellow 10 // Pin used to control the yellow led (changed from 9 to 10)

const int buttonPin = 18;  // the number of the pushbutton pin
int buttonState = 0;  // variable for reading the pushbutton status
int lastButtonState = LOW; 
unsigned long lastDebounceTime = 0; 
const unsigned long debounceDelay = 50; 

const int ldrPin = 4;  // the number of the LDR pin
int threshold = 600;

unsigned long previousMillis = 0; 
const long interval = 1000;
bool ledState = LOW;

enum Mode { NIGHT, DAY };
Mode currentMode = DAY;

enum LightState { GREEN, YELLOW, RED };
LightState currentLightState = GREEN;

unsigned long lightChangeMillis = 0;
unsigned long buttonPressMillis = 0;
bool buttonPressed = false;
int buttonPressCount = 0;

void setup() {
  // Inicial configuration of the pins to control the leds as OUTPUTs of the ESP32
  pinMode(led_blue, OUTPUT);
  pinMode(led_green, OUTPUT);
  pinMode(led_red, OUTPUT);
  pinMode(led_yellow, OUTPUT); // Initialize the yellow LED pin as an output

  // Inicialization of the inputs
  pinMode(buttonPin, INPUT); // Initialize the pushbutton pin as an input
  pinMode(ldrPin, INPUT); // Initialize the LDR pin as an input

  // Set initial state of LEDs to LOW
  digitalWrite(led_blue, LOW);
  digitalWrite(led_green, LOW);
  digitalWrite(led_red, LOW);
  digitalWrite(led_yellow, LOW);

  Serial.begin(9600); // Configuration to debug with the serial interface between ESP and computer with a baud rate of 9600

  WiFi.begin("Wokwi-GUEST", ""); // Connection to the open wifi network with the SSID Wokwi-GUEST

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("Conectado ao WiFi com sucesso!"); // Considering that you exited the loop above, the ESP32 is now connected to WiFi

  // Check the button state
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    Serial.println("Botão pressionado!");
  } else {
    Serial.println("Botão não pressionado!");
  }

  if (WiFi.status() == WL_CONNECTED) { // If the ESP32 is connected to the Internet
    HTTPClient http;

    String serverPath = "http://www.google.com.br/"; // Endpoint of the HTTP request

    http.begin(serverPath.c_str());

    int httpResponseCode = http.GET(); // Result of the HTTP request

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

void loop() {
  int ldrstatus = analogRead(ldrPin);
  unsigned long currentMillis = millis();

  if (ldrstatus <= threshold) {
    if (currentMode != NIGHT) {
      currentMode = NIGHT;
      Serial.println("its dark turn on led");
      digitalWrite(led_green, LOW);
      digitalWrite(led_red, LOW);
      digitalWrite(led_yellow, LOW);
    }

    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      ledState = !ledState;
      digitalWrite(led_yellow, ledState);
    }
  } else {
    if (currentMode != DAY) {
      currentMode = DAY;
      Serial.println("It's bright, turn off light");
      digitalWrite(led_yellow, LOW);
      lightChangeMillis = currentMillis;
      currentLightState = GREEN;
      digitalWrite(led_green, HIGH);
    }

    if (currentMillis - lightChangeMillis >= 3000 && currentLightState == GREEN) {
      lightChangeMillis = currentMillis;
      currentLightState = YELLOW;
      digitalWrite(led_green, LOW);
      digitalWrite(led_yellow, HIGH);
    } else if (currentMillis - lightChangeMillis >= 2000 && currentLightState == YELLOW) {
      lightChangeMillis = currentMillis;
      currentLightState = RED;
      digitalWrite(led_yellow, LOW);
      digitalWrite(led_red, HIGH);
    } else if (currentMillis - lightChangeMillis >= 5000 && currentLightState == RED) {
      lightChangeMillis = currentMillis;
      currentLightState = GREEN;
      digitalWrite(led_red, LOW);
      digitalWrite(led_green, HIGH);
    }

    int reading = digitalRead(buttonPin);

    if (reading != lastButtonState) {
      lastDebounceTime = currentMillis;
    }

    if ((currentMillis - lastDebounceTime) > debounceDelay) {

      if (reading != buttonState) {
        buttonState = reading;

        if (buttonState == HIGH && currentLightState == RED) {
          buttonPressCount++;
          buttonPressMillis = currentMillis;
          Serial.print("Button pressed ");
          Serial.print(buttonPressCount);
          Serial.println(" times");

          if (buttonPressCount == 3) {
            buttonPressed = true;
            buttonPressCount = 0;
          }
        }
      }
    }

    if (buttonPressed && (currentMillis - buttonPressMillis >= 1000)) {
      buttonPressed = false;
      currentLightState = GREEN;
      lightChangeMillis = currentMillis;
      digitalWrite(led_red, LOW);
      digitalWrite(led_green, HIGH);

      // Send HTTP request
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String serverPath = "http://www.google.com.br/"; // Endpoint of the HTTP request
        http.begin(serverPath.c_str());
        int httpResponseCode = http.GET(); // Result of the HTTP request
        if (httpResponseCode > 0) {
          Serial.print("HTTP Response code: ");
          Serial.println(httpResponseCode);
          String payload = http.getString();
          Serial.println(payload);
        } else {
          Serial.print("Error code: ");
          Serial.println(httpResponseCode);
        }
        http.end();
      } else {
        Serial.println("WiFi Disconnected");
      }
    }
    lastButtonState = reading;
  }
}