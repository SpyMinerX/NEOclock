
#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Time.h>
#include <NTPClient.h>
#include <Preferences.h>

#define PIN D4
#define NUMPIXELS 114

const char* ssid = "ssid";
const char* password = "Password";


Adafruit_NeoPixel NEOstrip(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
WiFiServer server(80);
Preferences preferences;
int uhr[11] = { 22, 21, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
int ein_uhr[11] = { 107, 97, 86, 86, 86, 86, 86, 86, 86, 86, 86 };
int eins_uhr[11] = { 107, 97, 86, 77, 77, 77, 77, 77, 77, 77, 77 };
int zwei_uhr[11] = { 37, 26, 17, 5, 5, 5, 5, 5, 5, 5, 5 };
int drei_uhr[11] = { 106, 98, 85, 78, 78, 78, 78, 78, 78, 78, 78 };
int vier_uhr[11] = { 38, 25, 18, 4, 4, 4, 4, 4, 4, 4, 4 };
int fuenf_uhr[11] = { 36, 27, 18, 6, 6, 6, 6, 6, 6, 6, 6 };
int sechs_uhr[11] = { 105, 99, 84, 79, 64, 64, 64, 64, 64, 64, 64 };
int sieben_uhr[11] = { 104, 100, 83, 80, 63, 60, 60, 60, 60, 60, 60 };
int acht_uhr[11] = { 39, 24, 19, 3, 3, 3, 3, 3, 3, 3, 3 };
int neun_uhr[11] = { 81, 62, 61, 42, 42, 42, 42, 42, 42, 42, 42 };
int zehn_uhr[11] = { 103, 101, 82, 81, 81, 81, 81, 81, 81, 81, 81 };
int elf_uhr[11] = { 56, 47, 36, 36, 36, 36, 36, 36, 36, 36, 36 };
int zwoelf_uhr[11] = { 43, 40, 23, 20, 2, 2, 2, 2, 2, 2, 2 };
int es_ist[11] = { 112, 92, 72, 71, 52, 52, 52, 52, 52, 52, 52 };
int fuenf[11] = { 32, 31, 12, 10, 10, 10, 10, 10, 10, 10, 10 };
int zehn[11] = { 111, 93, 90, 73, 73, 73, 73, 73, 73, 73, 73 };
int zwanzig[11] = { 70, 53, 50, 33, 30, 13, 9, 9, 9, 9, 9 };
int dreiviertel[11] = { 110, 94, 89, 74, 69, 54, 49, 34, 29, 14, 8 };
int viertel[11] = { 69, 54, 49, 34, 29, 14, 8, 8, 8, 8, 8 };
int vor[11] = { 109, 95, 88, 88, 88, 88, 88, 88, 88, 88, 88 };
int funk[11] = { 75, 68, 55, 48, 48, 48, 48, 48, 48, 48, 48 };
int nach[11] = { 35, 28, 15, 7, 7, 7, 7, 7, 7, 7, 7 };
int halb[11] = { 108, 96, 87, 76, 76, 76, 76, 76, 76, 76, 76 };
int minute_1[11] = { 113, 113, 113, 113, 113, 113, 113, 113, 113, 113, 113 };
int minute_2[11] = { 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11 };
int minute_3[11] = { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 };
int minute_4[11] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
WiFiUDP udp;
NTPClient timeClient(udp);
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;
String header;
String redString = "0";
String greenString = "0";
String blueString = "0";
String title = "unchecked";
String onTime = "00:00";
String offTime = "00:00";
String dimm = "unchecked";
int pos1 = 0;
int pos2 = 0;
int pos3 = 0;
int pos4 = 0;
int pos5 = 0;
int pos6 = 0;
int pos7 = 0;
int pos8 = 0;
bool isDimmed = false;
int dimmerDividerAmount = 4;
void setup() {
  Serial.begin(115200);
  NEOstrip.begin();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.hostname("neoclock.local");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    redString = "255";
    blueString = "0";
    greenString = "0";
    render(funk);
    NEOstrip.show();
    delay(1000);
  }
  NEOstrip.clear();
  NEOstrip.show();
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.getHostname());
  server.begin();
  timeClient.begin();
  timeClient.setTimeOffset(3600);
  timeClient.update();
  preferences.begin("Setting", false);
  redString = preferences.getString("redString");
  blueString = preferences.getString("blueString");
  greenString = preferences.getString("greenString");
  title = preferences.getString("title");
  onTime = preferences.getString("onTime");
  offTime = preferences.getString("offTime");
  dimm = preferences.getString("dimm");
  preferences.end();
}

void loop() {
  manageServer();
  //Serial.println(redString + " " + blueString + " " + greenString);
  //Serial.println(timeClient.getFormattedTime());
  setLEDs(timeClient);
  delay(100);
}

void manageServer() {
  WiFiClient client = server.available();  // Listen for incoming clients

  if (client) {  // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");                                             // print a message out in the serial port
    String currentLine = "";                                                   // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {  // if there's bytes to read from the client,
        char c = client.read();  // read a byte, then
        Serial.write(c);         // print it out the serial monitor
        header += c;
        if (c == '\n') {  // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Display the HTML web page
            client.println("<!DOCTYPE html>");
            client.println("<html>");
            client.println("<head>");
            client.println("    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("    <link rel=\"icon\" href=\"data:,\">");
            client.println("    <title>NEOclockOs Setup</title>");
            client.println("    <link rel=\"stylesheet \" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\">");
            client.println("    <script src=\"https://cdnjs.cloudflare.com/ajax/libs/jscolor/2.0.4/jscolor.min.js\"></script>");
            client.println("    <link rel=\"preconnect\" href=\"https://fonts.googleapis.com\">");
            client.println("    <link rel=\"preconnect\" href=\"https://fonts.gstatic.com\" crossorigin>");
            client.println("    <link href=\"https://fonts.googleapis.com/css2?family=Inter:wght@400;900&family=Poppins&display=swap\"");
            client.println("        rel=\"stylesheet\">");
            client.println("    <style>");
            client.println("        body {");
            client.println("            background-color: #1e1e1e;");
            client.println("            font-family: poppins;");
            client.println("            color: white;");
            client.println("        }");
            client.println("        .container {");
            client.println("            margin-top: calc(50vh - 20em);");
            client.println("            background-color: #2d2d30;");
            client.println("            width: 30em;");
            client.println("            height: 24em;");
            client.println("            border-radius: 10px;");
            client.println("            animation: topbarfadein 400ms;");
            client.println("        }");
            client.println("        @keyframes topbarfadein {");
            client.println("            from {");
            client.println("                transform: translateY(-100px);");
            client.println("                opacity: 0;");
            client.println("            }");
            client.println("            to {");
            client.println("                transform: translateY(0);");
            client.println("                opacity: 1;");
            client.println("            }");
            client.println("        }");
            client.println("        h1 {");
            client.println("            margin-top: 20px;");
            client.println("            margin-left: 15px;");
            client.println("        }");
            client.println("        .line {");
            client.println("            margin-top: -9px;");
            client.println("            margin-left: 0px;");
            client.println("            margin-right: 0px;");
            client.println("            border: 2px solid #3e3e42;");
            client.println("        }");
            client.println("        label {");
            client.println("            margin-top: 20px;");
            client.println("            margin-left: 0px;");
            client.println("        }");
            client.println("        .jscolor {");
            client.println("            margin-top: -3px;");
            client.println("            margin-left: 0px;");
            client.println("            width: 100%;");
            client.println("            height: 40px;");
            client.println("            border: none;");
            client.println("            border-radius: 8px;");
            client.println("            text-align: center;");
            client.println("        }");
            client.println("        input[type=time] {");
            client.println("            align-self: left;");
            client.println("            margin-left: 50px;");
            client.println("            width: 100px;");
            client.println("            height: 40px;");
            client.println("            border: none;");
            client.println("            border-radius: 8px;");
            client.println("            text-align: center;");
            client.println("            background-color: #3e3e42;");
            client.println("            color: #ffffff;");
            client.println("        }");
            client.println("        .sep {");
            client.println("            margin-left: 25px;");
            client.println("        }");
            client.println("        #offTime {");
            client.println("            margin-left: 25px;");
            client.println("        }");
            client.println("        input[type=checkbox] {");
            client.println("            margin-left: 0px;");
            client.println("            margin-top: 20px;");
            client.println("        }");
            client.println("        .checkboxcontainer {");
            client.println("            margin-top: -10px;");
            client.println("            margin-left: 0px;");
            client.println("            width: 100%;");
            client.println("        }");
            client.println("        #change_color {");
            client.println("            margin-top: 20px;");
            client.println("            margin-left: 0px;");
            client.println("            width: 100%;");
            client.println("            height: 50px;");
            client.println("            border: none;");
            client.println("            border-radius: 8px;");
            client.println("        }");
            client.println("        .footer {");
            client.println("            position: fixed;");
            client.println("            left: 0;");
            client.println("            bottom: 0px;");
            client.println("            width: 100%;");
            client.println("            height: 15px;");
            client.println("            font-size: 10px;");
            client.println("            background-color: #000000;");
            client.println("            color: white;");
            client.println("            text-align: center;");
            client.println("            transition: 250ms;");
            client.println("            opacity: 0;");
            client.println("        }");
            client.println("        .footer:hover {");
            client.println("            opacity: 1;");
            client.println("                }");
            client.println("    </style>");
            client.println("</head>");
            client.println("<body>");
            client.println("    <div class=\"container\">");
            client.println("        <div class=\"row\">");
            client.println("            <h1>NEOclock OS</h1>");
            client.println("        </div>");
            client.println("        <div class=\"line\"></div>");
            client.println("        <label for=\"rgb\">Farbe</label>");
            client.println("        <input class=\"jscolor {onFineChange:'update(this)'}\" id=\"rgb\">");
            client.println("        <div class=\"checkboxcontainer\">");
            client.println("            <input type=\"checkbox\" id=\"checkboxtitle\" name=\"title\">");
            client.println("            <label for=\"title\">\"Es Ist\" Einblenden</label>");
            client.println("        </div>");
            client.println("        <div class=\"checkboxcontainer\">");
            client.println("            <input type=\"checkbox\" id=\"checkboxdimm\" name=\"dimm\">");
            client.println("            <label for=\"dimm\">Uhr Dimmen</label>");
            client.println("        </div>");
            client.println("        <label>Zwishen</label>");
            client.println("        <input type=\"time\" id=\"onTime\">");
            client.println("        <label class=\"sep\">-</label>");
            client.println("        <input type=\"time\" id=\"offTime\">");
            client.println("        <a class=\"btn btn-primary btn-lg\" href=\"#\" id=\"change_color\" role=\"button\">Save</a>");
            client.println("    </div>");
            client.println("    <div class=\"footer\">");
            client.println("        <p>(C) 2023 <a href=\"spyminer.dev\">SpyMiner</a></p> All rights reserved.");
            client.println("    </div>");
            client.println("    <script>function update(picker) {");
            client.println("            document.getElementById('rgb').innerHTML = Math.round(picker.rgb[0]) + ', ' + Math.round(picker.rgb[1]) + ', ' + Math.round(picker.rgb[2]);");
            client.println("            var checkbox = document.getElementById(\"checkbox\");");
            client.println("            var value = checkbox.checked ? \"checked\" : \"unchecked\"; var onTime = document.getElementByID(\"onTime\"); var offTime = document.getElementByID(\"offTime\"); var dimm = document.getElementById(\"checkboxdimm\"); var dimmvalue = dimm.checked ? \"checked\" : \"unchecked\";");
            client.println("            document.getElementById(\"change_color\").href = \"?r\" + Math.round(picker.rgb[0]) + \"g\" + Math.round(picker.rgb[1]) + \"b\" + Math.round(picker.rgb[2]) + \"t\" + value + \"o\" + onTime + \"f\" + offTime + \"x\" + dimmvalue + \"& \";");
            client.println("        }</script>");
            client.println("</body>");
            client.println("</html>");
            // The HTTP response ends with another blank line
            client.println();

            // Request sample: /?r201g32b255&
            // Red = 201 | Green = 32 | Blue = 255
            if (header.indexOf("GET /?r") >= 0) {
              pos1 = header.indexOf('r');
              pos2 = header.indexOf('g');
              pos3 = header.indexOf('b');
              pos4 = header.indexOf('t');
              pos5 = header.indexOf('o');
              pos6 = header.indexOf('f');
              pos7 = header.indexOf('x');
              pos8 = header.indexOf('&');
              redString = header.substring(pos1 + 1, pos2);
              greenString = header.substring(pos2 + 1, pos3);
              blueString = header.substring(pos3 + 1, pos4);
              title = header.substring(pos4 + 1, pos5);
              onTime = header.substring(pos5 + 1, pos6);
              offTime = header.substring(pos6 + 1, pos7);
              dimm = header.substring(pos7 + 1, pos8);
              preferences.begin("Setting", false);
              preferences.putString("redString", redString);
              preferences.putString("greenString", greenString);
              preferences.putString("blueString", blueString);
              preferences.putString("title", title);
              preferences.putString("onTime", onTime);
              preferences.putString("offTime", offTime);
              preferences.putString("dimm", dimm);
              preferences.end();
            }
            // Break out of the while loop
            break;
          } else {  // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
void setLEDs(NTPClient time) {
  NEOstrip.clear();
  int hours = time.getHours();
  if (hours > 12) {
    hours = hours - 12;
  }
  int mins = time.getMinutes();

  if (onTime == (time.getHours() + ":" + mins)) {
    isDimmed = false;
  }
  if (offTime == (time.getHours() + ":" + mins)) {
    isDimmed = true;
  }

  //Serial.println(hours);
  if (title == "checked") {
    render(es_ist);
  }
  if (mins < 4) {
    render(uhr);
  }
  if ((mins < 10) && (mins > 4)) {
    render(fuenf);
    render(nach);
  }
  if ((mins < 15) && (mins > 9)) {
    render(zehn);
    render(nach);
  }
  if ((mins < 20) && (mins > 14)) {
    render(viertel);
    render(nach);
  }
  if ((mins < 25) && (mins > 19)) {
    render(zwanzig);
    render(nach);
  }
  if ((mins < 30) && (mins > 24)) {
    render(fuenf);
    render(vor);
    render(halb);
  }
  if ((mins < 35) && (mins > 29)) {
    render(halb);
  }
  if ((mins < 40) && (mins > 34)) {
    render(fuenf);
    render(nach);
    render(halb);
  }
  if ((mins < 45) && (mins > 39)) {
    render(zwanzig);
    render(vor);
  }
  if ((mins < 50) && (mins > 44)) {
    render(dreiviertel);
  }
  if ((mins < 55) && (mins > 49)) {
    render(zehn);
    render(vor);
  }
  if ((mins < 60) && (mins > 54)) {
    render(fuenf);
    render(vor);
  }
  if ((mins == 1) || (mins == 6) || (mins == 11) || (mins == 16) || (mins == 21) || (mins == 26) || (mins == 31) || (mins == 36) || (mins == 41) || (mins == 46) || (mins == 51) || (mins == 56)) {
    render(minute_1);
  }
  if ((mins == 2) || (mins == 7) || (mins == 12) || (mins == 17) || (mins == 22) || (mins == 27) || (mins == 32) || (mins == 37) || (mins == 42) || (mins == 47) || (mins == 52) || (mins == 57)) {
    render(minute_2);
    render(minute_1);
  }
  if ((mins == 3) || (mins == 8) || (mins == 13) || (mins == 18) || (mins == 23) || (mins == 28) || (mins == 33) || (mins == 38) || (mins == 43) || (mins == 48) || (mins == 53) || (mins == 58)) {
    render(minute_3);
    render(minute_2);
    render(minute_1);
  }
  if ((mins == 4) || (mins == 9) || (mins == 14) || (mins == 19) || (mins == 24) || (mins == 29) || (mins == 34) || (mins == 39) || (mins == 44) || (mins == 49) || (mins == 54) || (mins == 59)) {
    render(minute_4);
    render(minute_3);
    render(minute_2);
    render(minute_1);
  }

  if ((((hours == 12) || (hours == 0)) && (mins < 25)) || ((hours == 11) && (mins > 24))) {
    render(zwoelf_uhr);
  }
  if ((hours == 1) && (mins < 5)) {
    render(ein_uhr);
  } else if (((hours == 1) && (mins < 25)) || (((hours == 12) || (hours == 0)) && (mins > 24))) {
    render(eins_uhr);
  }
  if (((hours == 2) && (mins < 25)) || ((hours == 1) && (mins > 24))) {
    render(zwei_uhr);
  }
  if (((hours == 3) && (mins < 25)) || ((hours == 2) && (mins > 24))) {
    render(drei_uhr);
  }
  if (((hours == 4) && (mins < 25)) || ((hours == 3) && (mins > 24))) {
    render(vier_uhr);
  }
  if (((hours == 5) && (mins < 25)) || ((hours == 4) && (mins > 24))) {
    render(fuenf_uhr);
  }
  if (((hours == 6) && (mins < 25)) || ((hours == 5) && (mins > 24))) {
    render(sechs_uhr);
  }
  if (((hours == 7) && (mins < 25)) || ((hours == 6) && (mins > 24))) {
    render(sieben_uhr);
  }
  if (((hours == 8) && (mins < 25)) || ((hours == 7) && (mins > 24))) {
    render(acht_uhr);
  }
  if (((hours == 9) && (mins < 25)) || ((hours == 8) && (mins > 24))) {
    render(neun_uhr);
  }
  if (((hours == 10) && (mins < 25)) || ((hours == 9) && (mins > 24))) {
    render(zehn_uhr);
  }
  if (((hours == 11) && (mins < 25)) || ((hours == 10) && (mins > 24))) {
    render(elf_uhr);
  }

  NEOstrip.show();
}

void render(int leds[]) {
  for (int i = 0; i < 11; i++) {
    if (isDimmed) {
      NEOstrip.setPixelColor(leds[i], NEOstrip.Color(redString.toInt() / dimmerDividerAmount, greenString.toInt() / dimmerDividerAmount, blueString.toInt() / dimmerDividerAmount));
    } else {
      NEOstrip.setPixelColor(leds[i], NEOstrip.Color(redString.toInt(), greenString.toInt(), blueString.toInt()));
    }
  }
}
