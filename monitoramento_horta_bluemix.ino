/*
 * Made by Lucas Silva.
 * License: none (public domain).
 */

/* -- Para conexão via Ethernet -- */
#include <SPI.h>
#include <Ethernet.h>
#include <IPStack.h>
#include <Countdown.h>
#include <MQTTClient.h>
#define MS_PROXY "quickstart.messaging.internetofthings.ibmcloud.com"
#define MQTT_PORT 1883
#define MQTT_MAX_PACKET_SIZE 100

byte mac[] = { 0x00, 0xFF, 0xBB, 0xCC, 0xDE, 0x02 };

/* -- Especificação para conectar com o bluemix d:quickstart:iotsample-arduino:<MAC Address> -- */
#define MQTT_CLIENTID "d:quickstart:iotsample-arduino:00ffbbccde02"
#define MQTT_TOPIC "iot-2/evt/status/fmt/json"

EthernetClient c;
IPStack ipstack(c);

MQTT::Client<IPStack, Countdown, 100, 1> client = MQTT::Client<IPStack, Countdown, 100, 1>(ipstack);

String deviceEvent;

/* -- Para o Sensor DH11 -- */
#include <DHT.h>
#define DHTPIN A5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

/* -- Para o display 16x2 -- */
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

byte simbolo_folha[8] ={ B00000100,
                         B00001010,
                         B00010001,
                         B00010101,
                         B00010101,
                         B00001010,
                         B00000100,
                         B00000100,};

/* -- Para o sensor do solo -- */
#define pino_sensor_solo A0
#define pino_led_vermelho 8
#define pino_led_amarelo 6
#define pino_led_verde 7
int valor_solo;

/* -- Para o sensor LDR -- */
int porta_LDR = A4;

void setup() {
  Serial.begin(9600);

  /* -- Definindo portas para os sensores -- */
  pinMode(pino_sensor_solo, INPUT);
  pinMode(pino_led_vermelho, OUTPUT);
  pinMode(pino_led_amarelo, OUTPUT);
  pinMode(pino_led_verde, OUTPUT);

  /* -- Criando símbolos par ao display -- */
  lcd.begin(16,2);
  lcd.clear();
  lcd.createChar(1, simbolo_folha);

  /* -- Seção de boas vindas -- */
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("Bem Vindo!");
  for(int i=0; i<=16; i+=2){
    lcd.setCursor(i,1);
    lcd.write((byte)1);
  }
  delay(3000);
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("Feito por:");
  lcd.setCursor(1,1);
  lcd.print("Lucas S. Silva");
  delay(4000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("conectando-se ao");
  lcd.setCursor(1,1);
  lcd.print("bluemix IOT...");
  delay(2000);
  lcd.clear();
  
  Ethernet.begin(mac);
  delay(1000);
}

void loop() {
  
  /* -- Conexão com o servidor -- */
  int rc = -1;
  
  if (!client.isConnected()) {
    Serial.println("Conectando com o servidor IOT...");
    rc = ipstack.connect(( (char *) MS_PROXY), MQTT_PORT);
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = (char *)MQTT_CLIENTID;    
    rc = -1;
    while ((rc = client.connect(data)) != 0);
    Serial.println("Conectado com Sucesso\n");
    Serial.println("Dados dos sensores em formato JSON");
    Serial.println("__________________________________");
  }

  MQTT::Message message;
  message.qos = MQTT::QOS0;
  message.retained = false;
  deviceEvent = buildJson();
  char json[200];
  deviceEvent.toCharArray(json,200);
  message.payload = json;
  message.payloadlen = strlen(json);
  Serial.println(json);
  rc = client.publish(MQTT_TOPIC, message);

  if (rc != 0) {
    Serial.print("Problema ao conectar");
    Serial.println(rc);
  }
  client.yield(1000);  
}

String buildJson() {
  String data = "{";
  data+="\"temperatura\":";
  data+= get_temperatura();
  data+= ",";
  data+="\"umidade\":";
  data+= get_umidade();
  data+= ",";
  data+="\"luminosidade\":";
  data+= get_luminosidade();
  data+=",";
  data+="\"solo\":";
  data+= get_solo();
  data+="}"; 
  return data;
}


/* -- STATUS dos Sensores -- */
double get_temperatura() {
  double temperatura = dht.readTemperature();
  return temperatura;
}

double get_umidade(){
  double umidade = dht.readHumidity();
  return umidade;
}

int get_luminosidade(){
  int luminosidade = analogRead(porta_LDR);
  return luminosidade;
}

int get_solo(){
  int solo = analogRead(pino_sensor_solo);
  return solo;
}


