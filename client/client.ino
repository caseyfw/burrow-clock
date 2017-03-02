#include <ESP8266WiFi.h>
#include <Servo.h>

// Change these to suit your setup.
const char ssid[] = "Your Wifi Network";
const char password[] = "your-wifi-password";
const char hostname[] = "server.hostname";
const char path[] = "/burrow-clock";

// The delay in seconds between clockface updates.
const unsigned long sleepTime = 60;

// The number of servos / hands on the clock.
const int numberOfServos = 5;

// The Arduino pins the servos are attached to.
int servoPins[] = {4, 5, 12, 13, 14};

// The pulse widths in microseconds corresponding to the minimum (0-degree) and
// maximum (180-degree) angle on each servo.
int servoMins[] = {544, 544, 544, 544, 544};
int servoMaxs[] = {2400, 2400, 2400, 2400, 2400};

// The servo controllers to be attached to each pin.
Servo servos[numberOfServos];

WiFiClient client;
String response;

void setup()
{
  Serial.begin(9600);

  // Attach servo controllers to their respective pins.
  for (int i = 0; i < numberOfServos; i++) {
    servo[i].attach(servoPins[i], servoMins[i], servoMaxs[i]);
  }

  // Join wifi network.
  Serial.print("Joining Wifi network... ");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }

  // Connect to remote host.
  Serial.print("Connecting to server... ");
  while (!client.connect(hostname, 80)) {
    delay(100);
  }

  // Issue request for instructions.
  Serial.println("Issuing request.");
  client.print("GET ");
  client.print(path);
  client.println("/instructions HTTP/1.1");
  client.print("Host: ");
  client.println(hostname);
  client.println("Content-Type: text/plain");
  client.println("Connection: close");
  client.println();
}

void loop()
{
  // If there are incoming bytes available from the server, read them.
  while (client.available()) {
    char c = client.read();
    if (response.length() < 1000) {
      response += c;
    }
  }

  // If the server has disconnected, stop the client, do action and go to sleep.
  if (!client.connected()) {
    Serial.println("Server has disconnected.");
    Serial.println("Response:");
    Serial.println(response);

    client.stop();

    // Parse response and move servos as required.
    int delimiterIndex = 0;
    String instruction = "";
    for (int i = 0; delimiterIndex < string.length() - 1; i++) {
      nextDelimiterIndex = response.indexOf(":", delimiterIndex + 1);
      instruction = response.substring(delimiterIndex, nextDelimiterIndex);

      if (instruction.indexOf("move_to")) {
        // Tell the servo to move to the location specified by the instruction.
        servos[i].write(instruction.substring(9).toInt());
      }
      delimiterIndex = nextDelimiterIndex;
    }

    Serial.println("Sleeping.");
    delay(500);
    Serial.end();
    ESP.deepSleep(sleepTime * 1000000);
  }
}
