

#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(169,254,92, 1);
int Lt_DnStrs_Lvng_A_Out  = 7;
char command[4] = "\0"; 
EthernetServer server(80);

void setup() {
// Open serial communications and wait for port to open:
  Serial.begin(9600);
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  pinMode(Lt_DnStrs_Lvng_A_Out,OUTPUT);
}


void SendOKpage(EthernetClient &client)
{
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connnection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
            client.println("<form  method=get name=formA>");
            if (digitalRead(7) == HIGH){
              client.println("<button style=\"background-color: #4CAF50;color: #FFFFFF;padding: 15px 32px; text-align: center;display: inline-block;\"name=Relay value=0 type=submit>Turn Off</button>");
            }
            else{
              client.println("<button style=\"background-color: #F00;color: #FFFFFF;padding: 15px 32px; text-align: center;display: inline-block;\"name=Relay value=1 type=submit>Turn On</button>");
            }
            
          
          client.println("</form><br />");
  
 
          
}


void SendAuthentificationpage(EthernetClient &client)
{
          client.println("HTTP/1.1 401 Authorization Required");
          client.println("WWW-Authenticate: Basic realm=\"Secure Area\"");
          client.println("Content-Type: text/html");
          client.println("Connnection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<HTML>  <HEAD>   <TITLE>Error</TITLE>");
          client.println(" </HEAD> <BODY><H1>401 Unauthorized.</H1></BODY> </HTML>");
}

char linebuf[80];
int charcount=0;
boolean authentificated=false;

void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    memset(linebuf,0,sizeof(linebuf));
    charcount=0;
    authentificated=false;
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        boolean current_line_is_first = true;
        char c = client.read();
        linebuf[charcount]=c;
        if (charcount<sizeof(linebuf)-1) charcount++;
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
          // get the first http request
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
        if (c == '\n' && currentLineIsBlank) {
         
          if (authentificated)
            SendOKpage(client);
          else
            SendAuthentificationpage(client);  
          break;
           
        }
        if (c == '\n') {
          // you're starting a new line
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
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}