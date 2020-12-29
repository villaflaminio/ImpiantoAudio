/*
 * http://192.168.2.89/
 * i due canali dell'amplificatore dx e sx vengono comandati da 2 relè 22 e 24 
 * un relè interrompe l'alimentazione dei led con il pin 26
 * i rele che comandano i 3 colori ( 32.34.26) commutano sul negativo
 * pin A1 input del sensore di corrente , il pin 8 invece gli fornisce l'alimentazione
 * pin 7 sensore DHT22
 * 
colori e rispettivi collegamenti tra ciabatta con relè e pin di arduino mega
- giallo -> 31
- viola -> 33
- 35 -> verde
- 37 -> bianco 
- 39 -> blu
- 41 -> arancione
  *
*/


#include <Ethernet.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include "DHT.h"
#include <String.h> 
#include <math.h>
#include "EmonLib.h"

//---------------CONTROLLO TEMPERATURE --------------------------------------------//
#define DHTPIN 6
#define DHTTYPE DHT22 // DHT 22 
DHT dht(DHTPIN, DHTTYPE);
float hum;  //Stores humidity value
float temp; //Stores temperature value

//---------------------- Potenza-------------------------------------------//
EnergyMonitor emon1;
String toPrint1, toPrint2;
double Irms, thePower;
char buffer1[30], buffer2[30];

//-----------------configurazioni web-------------------------------------//
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE };   // mac address
byte ip[] = { 192, 168, 2, 89};                      //ip ("192.168.2.99")
byte gateway[] = { 192, 168, 2 , 1};                   // internet access router
byte subnet[] = { 255, 255, 255, 0 };                  //subnet mask
EthernetServer server(80);     
String readString;
String Stringa_01;

//------------------ PIN & MEMORY -------------------
// memorizzo lo stato attuale dei pin
String ScassaSx = "off";
String ScassaDx = "off";
String SalimLed = "off";
String Sventola = "off";
String Srosso = "off";
String Sverde = "off";
String Sblu = "off";
String SaudioBack = "off";
String SaudioFront = "off";
String SluceCaminetto = "off";
String Salimentazione = "off";


 //31 alimentatore sub 33 stereo pioneer 35 stereo front 37? luce caminetto
// Assegno un nome da associare ai pin di arduino
const int cassaSx = 22;
const int cassaDx = 24;
const int alimLed = 26;
const int ventola = 28;

const int rosso = 32;
const int verde = 34;
const int blu = 36;

const int alimentazione = 31; // sub e alimentatore 12V 
const int audioBack = 33; // stereo pioneer 
const int audioFront = 35; // stereo mitsubihi
const int luceCaminetto = 37;

//---------------- 
int timeSend = 2000;
unsigned long time_now = 0;
long randNumber;
String header;

//-----------------------Section Time-----------------------
// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;
//------------------------END DICHIARAZIONI---------------------------

void setup() {
  // string
  Stringa_01 = "";
  dht.begin();
  emon1.current(1, 10);
   // Initialize the output variables as outputs

  pinMode(cassaSx , OUTPUT);
  pinMode(cassaDx  , OUTPUT);
  pinMode(alimLed  , OUTPUT);
  pinMode(ventola  , OUTPUT);
  pinMode(rosso  , OUTPUT);
  pinMode(verde  , OUTPUT);
  pinMode(blu  , OUTPUT);
  
  pinMode(alimentazione  , OUTPUT);
  pinMode(audioBack  , OUTPUT);
  pinMode(audioFront  , OUTPUT);
  pinMode(luceCaminetto  , OUTPUT);

  
  digitalWrite(cassaSx , LOW);
  digitalWrite(cassaDx  , LOW);
  digitalWrite(alimLed  , LOW);
  digitalWrite(ventola  , LOW);
  digitalWrite(rosso  , LOW);
  digitalWrite(verde  , LOW);
  digitalWrite(blu  , LOW);
  digitalWrite(alimentazione  , LOW);
  digitalWrite(audioBack  , LOW);
  digitalWrite(audioFront  , LOW);
  digitalWrite(luceCaminetto  , LOW);
  

  Serial.begin(115200);
  
    // comunicazione seriale
    while (!Serial) {
      ;
    }
   

  // Inizio la comunicazione Ethernet con il server:
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {
  EthernetClient client = server.available();

    if (client) {                             // If a new client connects,
      Serial.println("New Client.");          // print a message out in the serial port
      String currentLine = "";                // make a String to hold incoming data from the client
      currentTime = millis();
      previousTime = currentTime;
      while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
        currentTime = millis();
        if (client.available()) {             // if there's bytes to read from the client,
          char c = client.read();             // read a byte, then
          Serial.write(c);                    // print it out the serial monitor
          header += c;
          if (c == '\n') {                    // if the byte is a newline character
            // if the current line is blank, you got two newline characters in a row.
            // that's the end of the client HTTP request, so send a response:
            if (currentLine.length() == 0) {
              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line:
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();
              //--------- EFFETTUO LA DECODIFICA DELL' URL, MODIFICANDO LO STATO DEI PIN ----------------------------------------------//
              modPin();

              //------------------------------------- JSON RISPOSTA------------------------------------------------------------//
              if (header.indexOf("GET /dati") >= 0) {
                    StaticJsonDocument<200> doc;
                    doc["cassa sinistra"] = ScassaSx;
                    doc["cassa destra"] = ScassaDx;
                    doc["alimentazione led"] = SalimLed;
                    doc["ventola"] = Sventola;
                    doc["rosso"] = Srosso;
                    doc["verde"] = Sverde;
                    doc["blu"] = Sblu;
                    doc["alimentazione 12V"] = Salimentazione;

                    doc["audio back"] = SaudioBack;
                    doc["audio front"] = SaudioFront;
                    doc["luce caminetto"] = SluceCaminetto;
                    
                    getPower();
                    doc["Power"] = toPrint1;
                    doc["Current"] = toPrint2;
                    
                    getTemp();
                    doc["hum"] = hum;
                    doc["temp"] = temp;
                 
                serializeJsonPretty(doc, Serial);
              
                    Serial.println("manda i dati");
              
              
                serializeJsonPretty(doc, client);

              }

              // The HTTP response ends with another blank line
              client.println();
              // Break out of the while loop
              break;
            } else { // if you got a newline, then clear currentLine
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


  
  void modPin() {
    //Si occupa di modificare lo stato dei pin in base alla modifica url
    if (header.indexOf("GET /cassaSx/on") >= 0) {
      Serial.println("cassaSx on");
      ScassaSx = "on";
      digitalWrite(cassaSx, HIGH);
    } else if (header.indexOf("GET /cassaSx/off") >= 0) {
      Serial.println("cassaSx off");
      ScassaSx = "off";
      digitalWrite(cassaSx, LOW);      
    } else if (header.indexOf("GET /cassaDx/on") >= 0) {
      Serial.println("GPIO cassaDx  on");
      ScassaDx  = "on";
      digitalWrite(cassaDx , HIGH);
    } else if (header.indexOf("GET /cassaDx/off") >= 0) {
      Serial.println("GPIO cassaDx off");
      ScassaDx  = "off";
      digitalWrite(cassaDx , LOW);
    }else if (header.indexOf("GET /alimLed/on") >= 0) {
      Serial.println("GPIO alimLed   on");
      SalimLed  = "on";
//-------------------------------------------------------------
      digitalWrite(alimLed , HIGH);
    } else if (header.indexOf("GET /alimLed/off") >= 0) {
      Serial.println("GPIO alimLed  off");
      SalimLed  = "off";
      digitalWrite(alimLed , LOW);
    }else if (header.indexOf("GET /ventola/on") >= 0) {
      Serial.println("GPIO ventola on");
      Sventola  = "on";
      digitalWrite(ventola , HIGH);
    } else if (header.indexOf("GET /ventola/off") >= 0) {
      Serial.println("GPIO ventola off");
      Sventola  = "off";
      digitalWrite(ventola , LOW);
    }else if (header.indexOf("GET /rosso/on") >= 0) {
      Serial.println("GPIO  on");
      Srosso  = "on";      
      digitalWrite(rosso , HIGH);
    } else if (header.indexOf("GET /rosso/off") >= 0) {
      Serial.println("GPIO rosso  off");
      Srosso  = "off";
      digitalWrite(rosso , LOW);
    }else if (header.indexOf("GET /verde/on") >= 0) {
      Serial.println("GPIO verde  on");
      Sverde  = "on";
      digitalWrite(verde, HIGH);
    } else if (header.indexOf("GET /verde/off") >= 0) {
      Serial.println("GPIO verde  off");
      Sverde  = "off";
      digitalWrite(verde, LOW);
    }else if (header.indexOf("GET /blu/on") >= 0) {
      Serial.println("GPIO blu  on");
      Sblu  = "on";
      digitalWrite(blu , HIGH);
    } else if (header.indexOf("GET /blu/off") >= 0) {
      Serial.println("GPIO blu  off");
      Sblu  = "off";
      digitalWrite(blu , LOW);
    }else if (header.indexOf("GET /audioBack/on") >= 0) {
      Serial.println("GPIO audioBack  on");
      SaudioBack  = "on";
//------------------------------------------------------------
      digitalWrite(audioBack , HIGH);
    } else if (header.indexOf("GET /audioBack/off") >= 0) {
      Serial.println("GPIO audioBack  off");
      SaudioBack  = "off";
      digitalWrite(audioBack , LOW);
    }else if (header.indexOf("GET /audioFront/on") >= 0) {
      Serial.println("GPIO audioFront  on");
      SaudioFront  = "on";
      digitalWrite(audioFront, HIGH);
    } else if (header.indexOf("GET /audioFront/off") >= 0) {
      Serial.println("GPIO audioFront off");
      SaudioFront  = "off";
      digitalWrite(audioFront , LOW);
    }else if (header.indexOf("GET /luceCaminetto/on") >= 0) {
      Serial.println("GPIO luceCaminetto  on");
      SluceCaminetto = "on";
      digitalWrite(luceCaminetto, HIGH);
    } else if (header.indexOf("GET /luceCaminetto/off") >= 0) {
      Serial.println("GPIO luceCaminetto off");
      SluceCaminetto = "off";
      digitalWrite(luceCaminetto, LOW);
    }else if (header.indexOf("GET /alimentazione/on") >= 0) {
      Serial.println("GPIO alimentazione  on");
      Salimentazione = "on";
      digitalWrite(alimentazione, HIGH);
    } else if (header.indexOf("GET /alimentazione/off") >= 0) {
      Serial.println("GPIO alimentazione off");
      Salimentazione = "off";
      digitalWrite(alimentazione, LOW);
      
    }
    //----------------------------- MODALITA'----------------------------------//
    else if (header.indexOf("GET /modSoft") >= 0) {
      Serial.println("GPIO modSoft on");
      digitalWrite(cassaSx , LOW);
      digitalWrite(cassaDx  , LOW);
      digitalWrite(luceCaminetto, LOW);
      digitalWrite(audioBack , LOW);
      digitalWrite(audioFront , HIGH);
      digitalWrite(alimentazione , HIGH);
      ScassaSx  = "off";
      ScassaDx  = "off";
      SluceCaminetto  = "off";
      Salimentazione  = "on";
      SaudioFront  = "on";
     

    } else if (header.indexOf("GET /modFull") >= 0) {
      Serial.println("GPIO modFull off");
      digitalWrite(cassaSx , HIGH);
      digitalWrite(cassaDx  , HIGH);
      digitalWrite(audioBack , HIGH);
      digitalWrite(audioFront , HIGH);    
      digitalWrite(alimentazione , HIGH);    
      ScassaSx  = "on";
      ScassaDx  = "on";
      SluceCaminetto  = "off";
      SaudioBack  = "on";
      SaudioFront  = "on";
      Salimentazione = "on";

      
    }else if (header.indexOf("GET /tuttoOff") >= 0) {
    Serial.println("GPIO tuttoOff on");
    digitalWrite( audioBack , LOW);
    digitalWrite( alimentazione , LOW);

    delay(500);
    digitalWrite(audioFront , LOW);
    digitalWrite(cassaSx , LOW);
    digitalWrite(cassaDx , LOW);
    digitalWrite(ventola , LOW);
    digitalWrite(alimLed  , LOW);
    digitalWrite(rosso , LOW);
    digitalWrite(verde , LOW);
    digitalWrite(blu , LOW);

    ScassaSx  = "off";
    ScassaDx  = "off";
    SluceCaminetto  = "off";
    SaudioBack  = "off";
    SaudioFront  = "off";
    Sventola  = "off";
    SalimLed  = "off";
    SaudioFront  = "off";
    Srosso  = "off";
    Sverde  = "off";
    Sblu  = "off";
    Salimentazione = "off";
    

    } else if (header.indexOf("GET /tuttoLuciOff") >= 0) {
      Serial.println("GPIO tuttoLuciOff off");
      digitalWrite(alimLed   , LOW);
      digitalWrite(rosso  , LOW);
      digitalWrite(verde   , LOW);
      digitalWrite(blu   , LOW);
      Srosso  = "off";
      Sverde  = "off";
      Sblu  = "off";   
      SalimLed  = "off";

      
    }else if (header.indexOf("GET /tuttoAudioOff") >= 0) {
      Serial.println("GPIO tuttoAudioOff  on");
      Serial.println("GPIO tuttoOff on");
      digitalWrite( audioBack , LOW);
      digitalWrite( alimentazione , LOW);

      delay(500);
      digitalWrite(audioFront  , LOW);
      digitalWrite(cassaSx  , LOW);
      digitalWrite(cassaDx  , LOW);
      digitalWrite(ventola  , LOW);
      ScassaSx  = "off";
      ScassaDx  = "off";
      SaudioBack  = "off";
      SaudioFront  = "off";
      Sventola  = "off";     
      Salimentazione = "off";
      
    }

  }
  
void getTemp() {
    temp= dht.readTemperature();    
    hum = dht.readHumidity();

    Serial.print(temp);    
    Serial.print(hum);

    if(temp>40){      
      digitalWrite(ventola , HIGH);

    }

}

void getPower()
{
 Irms = emon1.calcIrms(1480); // measure current
 if(Irms>10){
  Irms -= 10; // measure current
 }

 thePower = (Irms*230.0)/2;  // we assume voltage is 230VAC if you add transformer to connect to other input you can measure real voltage too

 toPrint1 = "Power:" + String((int)thePower) + "." + String(((unsigned int)(thePower*100))%100) + " ";
 toPrint2 = "Current:" + String((int)Irms) + "." + String(((unsigned int)(Irms*100))%100) + " ";
 toPrint1.toCharArray(buffer1, 30);
 toPrint2.toCharArray(buffer2, 30);
 Serial.println(toPrint1);
 Serial.println(toPrint2);

 delay(10);
}
