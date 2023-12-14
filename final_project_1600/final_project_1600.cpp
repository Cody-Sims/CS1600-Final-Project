#include "final_project_1600.h"
#include <SPI.h>

// Global Variables
String wakeup_time = "08:00";
String sleep_time = "23:00";
int port = 7834;
WiFiServer server(port);
WiFiUDP Udp;
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[NTP_PACKET_SIZE];
unsigned long secsSince1900;
IPAddress timeServer(129, 6, 15, 28); 
unsigned int localPort = 2390;   



/**
 *  Initialize WiFi connection
 */
void initializeWifi() {
  char ssid[] = "Brown-Guest";
  WiFi.begin(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  server.begin();
  Udp.begin(localPort);
}



/**
 * Connect to an NTP server and request time
 */
void connectToNTP() {
  sendNTPpacket(timeServer); // Send an NTP packet to the server
  // Wait for a response
  unsigned long startWait = millis();
  while (millis() - startWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Udp.read(packetBuffer, NTP_PACKET_SIZE);
      unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
      unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
      secsSince1900 = highWord << 16 | lowWord;
      break;
    }
  }
}


/**
 * Send an NTP request to the server
 */
unsigned long sendNTPpacket(IPAddress& address) {
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011; 
  packetBuffer[1] = 0; 
  packetBuffer[2] = 6;
  packetBuffer[3] = 0xEC;
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  Udp.beginPacket(address, 123);
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}


/**
 * Used to handle the wifi client
 */
void handleWiFiClient() {
  WiFiClient client = server.available();
  if (client) {
    while (!client.available()) {
      delay(1);
    }

    String request = client.readStringUntil('\r');
    client.flush();

    // Parsing the client request to extract wakeup and sleep times
    int wakeupPos = request.indexOf("GET /set?wakeup=");
    if (wakeupPos != -1) {
        int sleepPos = request.indexOf("&sleep=", wakeupPos);
        if (sleepPos != -1) {
            wakeup_time = urlDecode(request.substring(wakeupPos + 16, sleepPos));
            sleep_time = urlDecode(request.substring(sleepPos + 7, request.indexOf(" ", sleepPos)));

            // Logging the extracted times
            Serial.print("Wakeup Time Set: ");
            Serial.println(wakeup_time);
            Serial.print("Sleep Time Set: ");
            Serial.println(sleep_time);
        }
    }

    // Sending a response back to the client
    sendResponseToClient(client);
    client.stop();
  }
}

/**
 * Prints the Status of the wifi to determine the IP the server is running on
 */
void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.print(ip);
  Serial.print(":");
  Serial.println(port);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void sendResponseToClient(WiFiClient &client) {
  String currentTime = getCurrentTime(); // Get the current time

  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head><title>Alarm Time Set</title></head>");
  client.println("<body>");
  client.println("<h1>Alarm Times</h1>");

  // Form for setting wakeup and sleep times
  client.println("<form action=\"/set\" method=\"GET\">");
  client.println("Wakeup Time: <input type=\"text\" name=\"wakeup\" value=\"" + wakeup_time + "\"><br>");
  client.println("Sleep Time: <input type=\"text\" name=\"sleep\" value=\"" + sleep_time + "\"><br>");
  client.println("<input type=\"submit\" value=\"Set Times\">");
  client.println("</form>");

  // Displaying the current time
  client.println("<p>Current Time: " + currentTime + "</p>");

  // Displaying the set times
  client.println("<p>Set Wakeup Time: " + wakeup_time + "</p>");
  client.println("<p>Set Sleep Time: " + sleep_time + "</p>");
  client.println("</body></html>");
}

/**
 * Convert NTP time to a human-readable format
 */
String getCurrentTime() {
  unsigned long epoch = secsSince1900 + (millis() / 1000) - 2208988800UL;
  epoch -= 5 * 3600;
  unsigned long hour = (epoch % 86400L) / 3600;
  String hourStr = hour < 10 ? "0" + String(hour) : String(hour);
  unsigned long minute = (epoch % 3600) / 60;
  String minuteStr = minute < 10 ? "0" + String(minute) : String(minute);
  return hourStr + ":" + minuteStr;
}

/**
 * Decodes the URL, so it can be used to set the wakeup and sleep times. 
 *
 * @param str a url
 */
String urlDecode(String str) {
    String decodedString = "";
    for (int i = 0; i < str.length(); i++) {
        char tempChar = str[i];
        if (tempChar == '+') {
            decodedString += ' ';
        } else if (tempChar == '%') {
            String hexValue = str.substring(i + 1, i + 3);
            char decodedChar = (char) strtoul(hexValue.c_str(), NULL, 16);
            decodedString += decodedChar;
            i += 2;
        } else {
            decodedString += tempChar;
        }
    }
    return decodedString;
}

unsigned long getSecsSince1900() {
  return secsSince1900;
}