/*#include <Arduino.h>
#include <BluetoothSerial.h>

BluetoothSerial BT;

uint8_t endSlave[] = {0xC0, 0x49, 0xEF, 0xBC, 0x02, 0x66};

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(5000);

  if (BT.begin("espMasterMariana", true)) {
    Serial.println("Bluetooth iniciado com sucesso");
  } else {
    Serial.println("Erro ao iniciar o Bluetooth");
  }

  if (BT.connect(endSlave)) {
    Serial.println("Conectado com sucesso");
  } else {
    Serial.println("Erro ao conectar");
  }
}

void loop() {
 
  if (BT.available()) {
    String mensagemRecebida = BT.readStringUntil('\n');
    mensagemRecebida.trim();
    Serial.printf("Mensagem Recebida: %s\n", mensagemRecebida.c_str());
  }

  if (Serial.available()) {
    String mensagemEnviar = Serial.readStringUntil('\n');
    mensagemEnviar.trim();
    BT.println(mensagemEnviar);
  }
}

*/
/*#include <Arduino.h>
#include <BluetoothSerial.h>

BluetoothSerial BT;

uint8_t endSlave [] = {0xC0, 0x49, 0xEF, 0xBC, 0x02, 0x66}; 

const int botaoPin = 0; 
const int ledPin = 2;   

enum EstadoLed { DESLIGADO, ACESO, PISCANDO };
EstadoLed estadoLed = DESLIGADO;

bool botaoAnterior = HIGH;
unsigned long tempoPressionado = 0;
unsigned long ultimoMillis = 0;
bool ledLigado = false;

void setup() {
  Serial.begin(115200);
  pinMode(botaoPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  if (!BT.begin("espMasterPietra0", true)) {
    Serial.println("Erro ao iniciar Bluetooth Master");
    while (1);
  }
  Serial.println("Bluetooth Master iniciado");

  if (BT.connect(endSlave)) {
    Serial.println("Conectado ao Slave com sucesso!");
  } else {
    Serial.println("Falha ao conectar ao Slave!");
  }
}

void loop() {
  bool botaoAtual = digitalRead(botaoPin);

  if (botaoAnterior == HIGH && botaoAtual == LOW) tempoPressionado = millis();

  if (botaoAnterior == LOW && botaoAtual == HIGH) {
    unsigned long duracao = millis() - tempoPressionado;

    if (duracao < 2000) {
      BT.println("toggle");
      Serial.println("Comando enviado: toggle");
    } 
  
    else {
      BT.println("pisca");
      Serial.println("Comando enviado: pisca");
    }
  }
  botaoAnterior = botaoAtual;

  if (BT.available()) {
    String comando = BT.readStringUntil('\n');
    comando.trim();
    Serial.printf("Comando recebido: %s\n", comando.c_str());

    if (comando.equalsIgnoreCase("toggle")) {
      if (estadoLed == DESLIGADO) estadoLed = ACESO;
      else if (estadoLed == ACESO || estadoLed == PISCANDO) estadoLed = DESLIGADO;
    } 
    else if (comando.equalsIgnoreCase("pisca")) {
      estadoLed = PISCANDO;
      ledLigado = false;
      ultimoMillis = millis();
    }
  }


  if (estadoLed == DESLIGADO) {
    digitalWrite(ledPin, LOW);
  } 
  else if (estadoLed == ACESO) {
    digitalWrite(ledPin, HIGH);
  } 
  else if (estadoLed == PISCANDO) {
    unsigned long agora = millis();
    if (agora - ultimoMillis >= 500) {
      ledLigado = !ledLigado;
      digitalWrite(ledPin, ledLigado ? HIGH : LOW);
      ultimoMillis = agora;
    }
  }
}


*/

#include <Arduino.h>
#include <BluetoothSerial.h>
#include "DHT.h"

#define DHTPIN 23       
#define DHTTYPE DHT22  

BluetoothSerial BT;
DHT dht(DHTPIN, DHTTYPE);


uint8_t enderecoSlave[] = {0xC0, 0x49, 0xEF, 0xBC, 0x02, 0x66};
float tempC_ant = 0, hum_ant = 0;

void setup() {
  Serial.begin(9600);
  delay(2000);
  Serial.println("Iniciando DHT22...");
  dht.begin();

  if (!BT.begin("ESP32_Master", true)) {
    Serial.println("Erro ao iniciar Bluetooth Master!");
    while (true);
  }

  Serial.println(" Bluetooth Master iniciado");
  Serial.print("Conectando ao Slave... ");
  if (BT.connect(enderecoSlave)) {
    Serial.println("Conectado ao Slave!");
  } else {
    Serial.println("Falha na conexão ao Slave!");
  }

  delay(2000);
}

void loop() {
 
  float h = NAN, t = NAN;
  for (int i = 0; i < 3; i++) {
    h = dht.readHumidity();
    t = dht.readTemperature();
    if (!isnan(h) && !isnan(t)) break;
    Serial.println(" Erro ao ler DHT22! Tentando novamente...");
    delay(2000);
  }

  if (isnan(h) || isnan(t)) {
    Serial.println(" Falha persistente no DHT22!");
    delay(3000);
    return;
  }

  float f = t * 1.8 + 32;

  Serial.print(" Umidade: ");
  Serial.print(h);
  Serial.print("%   Temp: ");
  Serial.print(t);
  Serial.print("°C / ");
  Serial.print(f);
  Serial.println("°F");

  
  if (abs(t - tempC_ant) >= 0.5 || abs(h - hum_ant) >= 1.0) {
    String msg = String(h, 1) + "," + String(t, 1) + "," + String(f, 1);
    BT.println(msg);
    Serial.println(" Enviado: " + msg);

    tempC_ant = t;
    hum_ant = h;
  }

  delay(3000);
}