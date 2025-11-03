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
#include <Arduino.h>
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







