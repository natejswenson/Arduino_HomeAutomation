
#include <SPI.h>
#include <Ethernet.h>
/*************************************************************************************************
**************************ethernet configuration**************************************************
**************************************************************************************************/
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(169,254,92, 1);
EthernetServer server(80);


/*************************************************************************************************
**************************Pin Declarations********************************************************
**************************************************************************************************/
//Outputs
int rlyA_pin = 7;
int pir_pin = 4;
//Inputs
int tmpA_pin = A0;
int tmpB_pin = A1;
int pir_led = 9;


/*************************************************************************************************
**************************Global Variable Declarations********************************************
**************************************************************************************************/
char command[4] = "\0"; 
int tmpA_value =0;
int tmpB_value =0;
int pir_value=LOW;
int pir_pinstate =0;
char linebuf[80];
int charcount=0;
boolean authentificated=false;
 

/*************************************************************************************************
**************************Setup*******************************************************************
**************************************************************************************************/
void setup() {
// Open serial communications and wait for port to open:
  Serial.begin(9600);
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  //Pinmode 
  pinMode(rlyA_pin,OUTPUT);
  pinMode(pir_led,OUTPUT);
  pinMode(pir_pin,INPUT);
}


/*************************************************************************************************
**************************Logged In HTML**********************************************************
**************************************************************************************************/
void SendOKpage(EthernetClient &client){
  client.println("HTTP/1.1 200 OK"); 
  client.println("Content-Type: text/html");
  client.println("Connnection: close");
  client.println();
  client.println("<!DOCTYPE HTML>"); 
  client.println("<html>"); 
  client.println("<form  method=get name=formA>");
  if (digitalRead(7) == HIGH){
    client.println("<button style=\"background-color: #4CAF50;color: #FFFFFF;padding: 15px 32px; text-align: center;display: inline-block;\"name=rlyA value=0 type=submit>Turn Off</button>");
  }
  if (digitalRead(7) == LOW){
    client.println("<button style=\"background-color: #F00;color: #FFFFFF;padding: 15px 32px; text-align: center;display: inline-block;\"name=rlyA value=1 type=submit>Turn On</button>");
  }
  if (digitalRead(8) == HIGH){
    client.println("<button style=\"background-color: #4CAF50;color: #FFFFFF;padding: 15px 32px; text-align: center;display: inline-block;\"name=rlyA value=0 type=submit>Turn Off</button>");
  }
  if (digitalRead(8) == LOW){
    client.println("<button style=\"background-color: #F00;color: #FFFFFF;padding: 15px 32px; text-align: center;display: inline-block;\"name=rlyA value=1 type=submit>Turn On</button>");
  }
  client.println("</form><br />");        
}


/*************************************************************************************************
**************************NOT AUTHORIZED HTML******************************************************
**************************************************************************************************/
void SendAuthentificationpage(EthernetClient &client){
  client.println("HTTP/1.1 401 Authorization Required");
  client.println("WWW-Authenticate: Basic realm=\"Secure Area\"");
  client.println("Content-Type: text/html");
  client.println("Connnection: close");
  client.println();
  client.println("<!DOCTYPE HTML>");
  client.println("<HTML>  <HEAD>   <TITLE>Error</TITLE>");
  client.println(" </HEAD> <BODY><H1>401 Unauthorized.</H1></BODY> </HTML>");
}


/*************************************************************************************************
**************************MAIN LOOP***************************************************************
**************************************************************************************************/
void loop() {
/**************************PIR SENSOR CODE******************************************************/  
pir_pinstate = digitalRead(pir_pin); 
if (pir_value == HIGH) {
  digitalWrite(pir_led, HIGH);
  if (pir_value== LOW) {
    Serial.println("Motion detected!");
    pir_value = HIGH;
  }
} else {
  digitalWrite(pir_led, LOW); // turn LED OFF
  if (pir_value == HIGH){
    // we have just turned of
    Serial.println("Motion ended!");
    // We only want to print on the output change, not state
    pir_value = LOW;
  }
}
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    memset(linebuf,0,sizeof(linebuf));
    charcount=0;
    authentificated=false;
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        boolean current_line_is_first = true;
        char c = client.read();
        linebuf[charcount]=c;
        if (charcount<sizeof(linebuf)-1) charcount++;
        Serial.write(c);
       
        /**************************Athentication Code******************************************************/
         if (c == '\n') {
          currentLineIsBlank = true;
          current_line_is_first = false;
          if (strstr(linebuf,"Authorization: Basic")>0 && strstr(linebuf,"YWRtaW46YWRtaW4=")>0)
            authentificated=true;
          memset(linebuf,0,sizeof(linebuf));
          charcount=0;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
         /**************************Website Build Code ******************************************************/
        if (c == '\n' && currentLineIsBlank) {
         
          if (authentificated)
            SendOKpage(client);
            if (digitalRead(7) == HIGH){

          else
            SendAuthentificationpage(client);  
          break;
        }
        /**************************Control Code ******************************************************/
         if (current_line_is_first && c == '=') {
          for (int i = 0; i < 1; i++) {
            c = client.read();
            command[i] = c;
          }
        }
        if (!strcmp(command, "1")/*||(readingA == LOW)*/){
          digitalWrite(Lt_DnStrs_Lvng_A_Out, true);
          }
          else if (!strcmp(command, "0")){
                digitalWrite(Lt_DnStrs_Lvng_A_Out,false);
          }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}