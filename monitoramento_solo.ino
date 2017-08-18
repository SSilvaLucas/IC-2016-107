#include <LiquidCrystal.h>
#include <DHT.h>

#define DHTPIN A5
#define DHTTYPE DHT11

#define pino_sinal_analogico A0
#define pino_led_vermelho 8
#define pino_led_amarelo 6
#define pino_led_verde 7

int valor_analogico;
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

void setup()
{
  Serial.begin(9600);
  pinMode(pino_sinal_analogico, INPUT);
  pinMode(pino_led_vermelho, OUTPUT);
  pinMode(pino_led_amarelo, OUTPUT);
  pinMode(pino_led_verde, OUTPUT);
  pinMode(pino_sensor_nivel, INPUT);

  Serial.begin(9600);
  lcd.begin(16,2);
  lcd.clear();
  lcd.createChar(0, simbolo_grau);
}

void loop(){
  int status_LDR = analogRead(porta_LDR);
  Serial.println(status_LDR);
  if(status_LDR>930){
    lcd.clear();
    lcd.setCursor(5,0);
    lcd.print("Baixa");
    lcd.setCursor(2,1);
    lcd.print("Luminosidade");
    delay(3000);
  }else{
    lcd.clear();
    lcd.setCursor(6,0);
    lcd.print("Boa");
    lcd.setCursor(2,1);
    lcd.print("Luminosidade");
    delay(3000);
  }
  
  int status_reservatorio = digitalRead(pino_sensor_nivel);
  Serial.print("Estado reservatorio : ");
  if(status_reservatorio==1){
    Serial.println("Reservatório vazio");
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("Reservatorio");
    lcd.setCursor(5,1);
    lcd.print("Vazio!");
    delay(3000);
  }
  
  valor_analogico = analogRead(pino_sinal_analogico);

  //Solo umido
  if (valor_analogico > 0 && valor_analogico < 400){
    Serial.println("Solo umido");
    apagaleds();
    digitalWrite(pino_led_verde, HIGH);
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("Solo Umido!");
    delay(3000);
  }
  
  //Solo com umidade moderada
  if (valor_analogico > 400 && valor_analogico < 800){
    Serial.println("Solo com umidade moderada");
    apagaleds();
    digitalWrite(pino_led_amarelo, HIGH);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Solo com Umidade");
    lcd.setCursor(4,1);
    lcd.print("Moderada");
    delay(3000);
  }

  //Solo seco
  if (valor_analogico > 800 && valor_analogico < 1024){
    Serial.println("Solo seco");
    apagaleds();
    digitalWrite(pino_led_vermelho, HIGH);
    lcd.clear();
    lcd.setCursor(3,0);
    lcd.print("Solo Seco!");
    delay(3000);
  }

  float umidade = dht.readHumidity();
  float temperatura = dht.readTemperature();
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.println("°C");
  Serial.print("Umidade: ");
  Serial.print(umidade);
  Serial.println("%");

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Tempera : ");
  lcd.print(" ");
  lcd.setCursor(10,0);
  lcd.print(temperatura,1);
  lcd.setCursor(14,0);
  lcd.write((byte)0);
  lcd.print("C");
  
  lcd.setCursor(0,1);
  lcd.print("Umidade : ");
  lcd.setCursor(10,1);
  lcd.print(umidade,1);
  lcd.setCursor(14,1);
  lcd.print("%");
  
  delay(3000);
}

void apagaleds(){
  digitalWrite(pino_led_vermelho, LOW);
  digitalWrite(pino_led_amarelo, LOW);
  digitalWrite(pino_led_verde, LOW);
}
