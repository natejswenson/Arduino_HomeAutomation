
#include <Keypad.h>
#include <SPI.h>
#include <Ethernet.h>


// *************************************************************************************************
// **************************ethernet configuration*************************************************
// *************************************************************************************************
byte mac[] = {  };
byte ip[] = { }; // P1 --> { 10, 1, 1, 5 };
EthernetServer server(80);              // port 80 is default for HTTP

// *************************************************************************************************
// **************************Variable Declarations**************************************************
// *************************************************************************************************
int LtSnr_DnStrs_Lvng_In  = 0;        //Down Stairs Light SensorInput  to Analog pin
int Lt_DnStrs_Lvng_A_Out  = 9;       // DownStairs Relay for  Light is connected to pin 9
int PIR_Entry             =0;
int Lt_DnStrs_Lvng_B_Out  = 0;       // DownStairs Relay for Light connected to digital pin 10
int PIR_DnStrs_Lvng       = 0;       // DownStairs Motion sensor connected to digital pin 12/int PIR_Entry             = 8;       // Entry Motion sensor connected to digital pin 13
int PIR_DnStrs_Lvng_State =0;
int PIR_Entry_State       = 0;
int Lt_DnStrs_Button_A    = 0;      // Down stairs Light Switch button A
//int Lt_DnStrs_Button_B    = 15;      // Down Stairs Light Switch Button B
char c = 0;                          // received data
char command[4] = "\0";             // command
boolean Security_Active = false;
int lastButton_StateA = LOW;
int lastButton_StateB = LOW;
int Button_StateA;
int Button_StateB;
long lastDebounceTime = 0;
long debounceDelay = 50;
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] =
{{'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}};
byte rowPins[ROWS] = {
 5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {
  8, 7, 6}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

char PIN[4]={'1','2','3','4'}; // our secret (!) number
char attempt[]={
  0,0,0,0}; // used for comparison
int z=0;

void setup()
{
  Ethernet.begin(mac, ip);
  server.begin();
  pinMode(Lt_DnStrs_Lvng_A_Out,OUTPUT);
  pinMode(Lt_DnStrs_Lvng_B_Out,OUTPUT);
  pinMode(PIR_DnStrs_Lvng,INPUT);
  pinMode(PIR_Entry,INPUT);
  pinMode(Lt_DnStrs_Button_A,INPUT);
}

// *************************************************************************************************
// **************************MAIN LOOP**************************************************************
// *************************************************************************************************
void loop()
{

 EthernetClient client = server.available();
  // detect if current is the first line
  boolean current_line_is_first = true;
  if (client) {
    // an http request ends with a blank line
    boolean current_line_is_blank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // if we've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so we can send a reply
        if (c == '\n' && current_line_is_blank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          // auto reload webpage every 30 second
          client.println("<META HTTP-EQUIV=REFRESH CONTENT=30 URL=>");
        // the next line sets the background color to something nice like blue
         client.println(F("<body style='background-color: rgb(0, 0, 153); color: rgb(255, 255, 0);'alink='#cc6600' link='#cc0000' vlink='#993300'>"));
        //  load in a background .jpg for the web page.
        client.println(F("<BODY BACKGROUND='http://i51.tinypic.com/155gmr8.jpg' style='width: 1280px; height: 700px;'></span><br>"));

          // webpage title
          client.println("<center><p><h1>SWENSON'S HOUSE MONITORING</h1></p><center><hr><br />");

        client.println("<form name=myform  method=get action="">");
        client.println("<input type=text name=textbox size=10 >");
       client.println("</form><br />");
        client.println("<form  method=get name=formA>");
          if (Security_Active == true){
          client.println("<button name=b value=6 type=submit style=height:80px;width:150px>Security Active</button>");
          }
          else{
          client.println("<button name=b value=5 type=submit style=height:80px;width:150px>Security Inactive</button>");
          }
        CorrectPinOnline();
         if (Security_Active == true){//Security System Is Active
         client.println("<center><p><h2><font color=red>Security Active</h2></p><center><br />");
         }
         else{//Security System Not Active
         client.println("<center><p><h2><font color = green>Security Inactive</h2></p><center><br />");
         }
          // read digital pin 12 for the state of PIR sensor
          PIR_DnStrs_Lvng_State = digitalRead(7);
          if (PIR_DnStrs_Lvng_State == HIGH) { // PIR sensor detected movement
            client.println("<p><h2><font color=red>Motion Detected In Basement</h1></p><center><br />");
          }
          else { // No movement is detected
            client.println("<p><h2><font color=green>No Motion Detected In Basement</h1></p><center><br />");
          }

          // read digital pin 12 for the state of PIR sensor
          PIR_DnStrs_Lvng_State = digitalRead(8);
          if (PIR_DnStrs_Lvng_State == HIGH) { // PIR sensor detected movement
            client.println("<p><h2><font color=red>Motion Detected In Entry</h1></p><center><hr><br />");
          }
          else { // No movement is detected
            client.println("<p><h2><font color=green>No Motion Detected In Entry</h1></p><center><hr><br />");
          }
          // button functions
          client.println("<form  method=get name=formA>");
          if (digitalRead(9) == HIGH){
          client.println("<button name=b value=2 type=submit style=height:80px;width:150px>LightA Is ON</button>");
          }
          else{
          client.println("<button name=b value=1 type=submit style=height:80px;width:150px>LightA is OFF</button>");
          }
          if (digitalRead(6) == HIGH){
           client.println("<button name=c value=4 type=submit style=height:80px;width:150px>LightB Is ON</button>");
          }
          else{
          client.println("<button name=c value=3 type=submit style=height:80px;width:150px>LightB is OFF</button>");
          }
          client.println("</form><br />");

          // read digital pin 6 for the state of PIR sensor
          PIR_DnStrs_Lvng_State = digitalRead(5);
          if (LtSnr_DnStrs_Lvng_In == HIGH) { // PIR sensor detected movement
            client.println("<p><h2><font color=red>Light A ON</h1></p><center><hr><br />");
          }
          else { // No movement is detected
            client.println("<p><h2><font color=green>Light A Off</h1></p><center><hr><br />");
          }

          // webpage footer
          client.println("<center><p><h2>font conlor=black>SITE MODIFIED 2-3-13 NJS<br />");
          client.println("<p>P.S.: This page will automatically refresh every 5 seconds.</p></center>");

          break;
        }
        if (c == '\n') {
          // we're starting a new line
          current_line_is_first = false;
          current_line_is_blank = true;
        }
        else if (c != '\r') {
          // we've gotten a character on the current line
          current_line_is_blank = false;
        }
        // get the first http request
        if (current_line_is_first && c == '=') {
          for (int i = 0; i < 1; i++) {
            c = client.read();
            command[i] = c;
          }
        int readingA = digitalRead(Lt_DnStrs_Button_A);

  // If the switch changed, due to noise or pressing:
  if (readingA != lastButton_StateA) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

// *************************************************************************************************
// **************************RelayA Control**********************************************************
// *************************************************************************************************
  if (!strcmp(command, "1")/*||(readingA == LOW)*/){
          digitalWrite(Lt_DnStrs_Lvng_A_Out, true);
  }
  else if (!strcmp(command, "2")){
        digitalWrite(Lt_DnStrs_Lvng_A_Out,false);
  }
  lastButton_StateA = readingA;


// *************************************************************************************************
// **************************RelayB Control**********************************************************
// *************************************************************************************************
  if (!strcmp(command, "3")) {
            digitalWrite(Lt_DnStrs_Lvng_B_Out, LOW);

          }
   else if (!strcmp(command, "4")) {
            digitalWrite(Lt_DnStrs_Lvng_B_Out, HIGH);
          }
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    client.stop();
  }
}
