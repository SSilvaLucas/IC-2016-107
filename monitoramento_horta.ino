/*
Made by Lucas Silva
License: none (public domain)
*/

#include <LiquidCrystal.h>
#include <DHT.h>

#define DHTPIN A5
#define DHTTYPE DHT11

#define pino_sensor_solo A0
#define pino_led_vermelho 8
#define pino_led_amarelo 6
#define pino_led_verde 7

int valor_solo;
int pino_sensor_nivel = 9;
int porta_LDR = A4;

DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

byte simbolo_grau[8] ={ B00001100,
                        B00010010,
                        B00010010,
                        B00001100,
                        B00000000,
                        B00000000,
                        B00000000,
                        B00000000,};

byte simbolo_folha[8] ={ B00000100,
                         B00001010,
                         B00010001,
                         B00010101,
                         B00010101,
                         B00001010,
                         B00000100,
                         B00000100,};

byte simbolo_gota[8] ={  B00000100,
                         B00000100,
                         B00001110,
                         B00011111,
                         B00011111,
                         B00011111,
                         B00011111,
                         B00001110,};                         

int porta_rele1 = 10;

void setup()
{
  Serial.begin(9600);
  pinMode(pino_sensor_nivel, INPUT);
  pinMode(pino_sensor_solo, INPUT);
  pinMode(pino_led_vermelho, OUTPUT);
  pinMode(pino_led_amarelo, OUTPUT);
  pinMode(pino_led_verde, OUTPUT);
  pinMode(porta_rele1, OUTPUT); 

  digitalWrite(porta_rele1, HIGH);

  lcd.begin(16,2);
  lcd.clear();
  lcd.createChar(0, simbolo_grau);
  lcd.createChar(1, simbolo_folha);
  lcd.createChar(2, simbolo_gota);

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
  lcd.setCursor(3,0);
  lcd.print("Iniciando");
  lcd.setCursor(3,1);
  lcd.print("Sistema...");
  delay(2000);
}

void loop(){
  verifica_luminosidade();
  delay(2000);
  verifica_reservatorio();
  delay(2000);
  verifica_ar();
  delay(2500);
  bool solo_bom = true;
  solo_bom = verifica_solo();
  delay(2000);
  if(!solo_bom){
    liga_bomba();
    while(!solo_bom){
      exibe_status_irrigando();
      solo_bom = verifica_solo();
    }
    desliga_bomba();
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Bomba Desligada!");
  delay(2000);
}




void apagaleds(){
  digitalWrite(pino_led_vermelho, LOW);
  digitalWrite(pino_led_amarelo, LOW);
  digitalWrite(pino_led_verde, LOW);
}

void liga_bomba(){
  digitalWrite(porta_rele1, LOW);
}

void desliga_bomba(){
  digitalWrite(porta_rele1, HIGH);
}

void verifica_luminosidade(){
  int status_LDR = analogRead(porta_LDR);
  Serial.println(status_LDR);
  if(status_LDR>930){
    lcd.clear();
    lcd.setCursor(5,0);
    lcd.print("Baixa");
    lcd.setCursor(2,1);
    lcd.print("Luminosidade");
  }else{
    lcd.clear();
    lcd.setCursor(6,0);
    lcd.print("Boa");
    lcd.setCursor(2,1);
    lcd.print("Luminosidade");
  }
}

void verifica_reservatorio(){
  int status_reservatorio = digitalRead(pino_sensor_nivel);
  Serial.print("Estado reservatorio : ");
  if(status_reservatorio==1){
    Serial.println("Reservatório vazio");
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("Reservatorio");
    lcd.setCursor(5,1);
    lcd.print("Vazio!");
  }else{
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("Reservatorio");
    lcd.setCursor(5,1);
    lcd.print("Estavel");
  }
}

bool verifica_solo(){
  valor_solo = analogRead(pino_sensor_solo);
  //Solo umido
  if(valor_solo > 0 && valor_solo < 400){
    Serial.println("Solo umido");
    apagaleds();
    digitalWrite(pino_led_verde, HIGH);
    lcd.clear();
    lcd.setCursor(3,0);
    lcd.print("Solo Umido");
    return true;
  }
  //Solo com umidade moderada
  else if(valor_solo > 400 && valor_solo < 800){
    Serial.println("Solo com umidade moderada");
    apagaleds();
    digitalWrite(pino_led_amarelo, HIGH);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Solo com Umidade");
    lcd.setCursor(4,1);
    lcd.print("Moderada");
    return true;
  }
  //Solo seco
  else if(valor_solo > 800 && valor_solo < 1024){
    Serial.println("Solo seco");
    apagaleds();
    digitalWrite(pino_led_vermelho, HIGH);
    return false;
  }
}

void verifica_ar(){
  float umidade = dht.readHumidity();
  float temperatura = dht.readTemperature();
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.println("°C");
  Serial.print("Umidade: ");
  Serial.print(umidade);
  Serial.println("%");
  //linha temperatura
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Tempera : ");
  lcd.print(" ");
  lcd.setCursor(10,0);
  lcd.print(temperatura,1);
  lcd.setCursor(14,0);
  lcd.write((byte)0);
  lcd.print("C");
  //linha umidade
  lcd.setCursor(0,1);
  lcd.print("Umidade : ");
  lcd.setCursor(10,1);
  lcd.print(umidade,1);
  lcd.setCursor(14,1);
  lcd.print("%");
}

void exibe_status_irrigando(){
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("Solo Seco!");
  lcd.setCursor(0,1);
  lcd.print("Irrigando Solo");
  pinga_gota();
  delay(1000);
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Bomba Ligada");
  pinga_gota();
  delay(1000);
}

void pinga_gota(){
  lcd.setCursor(15,0);
  lcd.write((byte)2);
  delay(450);
  lcd.setCursor(15,0);
  lcd.print(".");
  lcd.setCursor(15,1);
  lcd.write((byte)2);
  delay(450);
  lcd.setCursor(15,1);
  lcd.print(".");
}

