/*  original idea https://www.reboot.ms/forum/resources/arduino-bitcoin-ticker.565/
 *  Configurazione per IDE Arduino
 *  Cheda Wemos D1 R2 & mini
 *  CPU 80Mhz
 *  flash 4M
 *  Speed 921600
 */

#include <ESP8266WiFi.h>
#include <LiquidCrystal.h>
#include <ArduinoJson.h>

// La numerazione dei pin su ESP8266 si differenzia da quella di Arduino UNO.
// con i define qui sotto è possibile ridefinire i nomi dei pin all'interno del programma
// in modo da riprendere la numerazione di un Arduino UNO. I nomi dichiarati alcune volte
// non corrispondono tuttavia a quelli scritti sulla scheda Wemos D1.
#define D0 3 // esempio: il punto GPIO3 su ESP8266 corrisponde al punto in posizione digital 0 su Arduino UNO
#define D1 1 // il punto GPIO1 su ESP8266 corrisponde a quello in posizione digital 1 su Arduino UNO
#define D2 16// ...e via dicendo
#define D3 5
#define D4 4
#define D5 14
#define D6 12
#define D7 13
#define D8 0
#define D9 2
#define D10 15
#define D11 13
#define D12 12
#define D13 14

////////// VALORI DA MODIFICARE //////////

const char* ssid     = "----your ssid----";
const char* password = "----your password----";
//////////////////////////////////////////

const char* host = "api.coindesk.com";

LiquidCrystal lcd(D8,D9,D4,D5,D6,D7); 

void setup() {
  
  // Fase iniziale di preparazione
  Serial.begin(115200);
  int posizioneCursore=0;
  lcd.begin(16, 2);
  lcd.print(" BITCOIN TICKER");
  lcd.setCursor(0,2); 
  lcd.print("                ");
  delay(1000);
  
  // Connessione alla rete Wifi con i dati inseriti qui sopra
  lcd.clear();
  lcd.print("Connessione");
  WiFi.begin(ssid, password);
  
  // Finchè l'ESP8266 non è connesso, stampa un
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    lcd.setCursor(posizioneCursore,2); 
    lcd.print(".");
    posizioneCursore++;
  }
  lcd.clear();
  lcd.print("Wifi Connesso!");
  delay(1000);
  lcd.clear();
  lcd.print("IP: ");
  lcd.print(WiFi.localIP());
  delay(1000);

}

void loop() {
  
  // WiFiClient crea una connessione TCP
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    return;
  }
  
  // URI per la richiesta
  String url = "/v1/bpi/currentprice.json";
  
  // Richiesta da inviare al server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(100);
  
  // Lettura della risposta dal server
  String answer;
  while(client.available()){
    String line = client.readStringUntil('\r');
    answer += line;
  }

  client.stop();

  // Conversione della risposta in JSON
  String jsonAnswer;
  int jsonIndex;

  for (int i = 0; i < answer.length(); i++) {
    if (answer[i] == '{') {
      jsonIndex = i;
      break;
    }
  }

  // Lettura dei dati JSON
  jsonAnswer = answer.substring(jsonIndex);
  jsonAnswer.trim();

  // Ottenimento del prezzo come valore float
  int rateIndex = jsonAnswer.indexOf("rate_float");
  String priceString = jsonAnswer.substring(rateIndex + 12, rateIndex + 18);
  priceString.trim();
  float price = priceString.toFloat();

  // Stampa a schermo il prezzo attuale
  lcd.clear();
  lcd.print("BTC: ");
  lcd.print(price);

  // Attendi 30 secondi prima di ripetere la richiesta
  delay(30000);
}
