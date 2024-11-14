#include <Keypad.h>
#include <Password.h>
#include <LiquidCrystal_PCF8574.h>
#include <WiFi.h>
#include <Wire.h>
#include <HTTPClient.h>

// Configuração do WiFi
const char* ssid = "projeto";
const char* password = "12345678";

WiFiClient client;
LiquidCrystal_PCF8574 lcd(0x3F);

// Configuração das Senhas e URLs dos Usuários
struct Usuario {
  String nome;
  Password senha;
  const char* url;
};

Usuario usuarios[] = {
  {"Igor", Password("1234"), "https://api.thingspeak.com/update?api_key=0PE67D6HAFFP0SW9&field1=1234"},
  {"Joao", Password("5678"), "https://api.thingspeak.com/update?api_key=0PE67D6HAFFP0SW9&field1=5678"},
  {"Carlos", Password("4321"), "https://api.thingspeak.com/update?api_key=0PE67D6HAFFP0SW9&field1=4321"},
  {"Jorge", Password("8765"), "https://api.thingspeak.com/update?api_key=0PE67D6HAFFP0SW9&field1=8765"}
};

constexpr int PINO_LED_VERM = 15;
constexpr int PINO_LED_AZUL = 14;

// Configuração do Teclado
constexpr uint8_t COLUNAS = 4;
constexpr uint8_t LINHAS = 4;
char teclasMatriz[LINHAS][COLUNAS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

uint8_t pinosLinhas[LINHAS] = {4, 27, 26, 25};
uint8_t pinosColunas[COLUNAS] = {33, 32, 13, 12};
Keypad teclado = Keypad(makeKeymap(teclasMatriz), pinosLinhas, pinosColunas, LINHAS, COLUNAS);

int cursorPos = 0;

void apagarLEDs() {
  digitalWrite(PINO_LED_VERM, LOW);
  digitalWrite(PINO_LED_AZUL, LOW);
}

void piscarLED(int pino, int vezes, int intervalo) {
  for (int i = 0; i < vezes; i++) {
    digitalWrite(pino, HIGH);
    delay(intervalo);
    digitalWrite(pino, LOW);
    delay(intervalo);
  }
}

void setup() {
  Serial.begin(9600);
  lcd.begin(16,2);
  lcd.setBacklight(255);
  pinMode(PINO_LED_VERM, OUTPUT);
  pinMode(PINO_LED_AZUL, OUTPUT);
  apagarLEDs();
  initWifi();
  Serial.println("Digite a senha:");
}

void loop() {
  char tecla = teclado.getKey();
  lcd.home();
  lcd.print("Digite sua Senha:");

  if (tecla) {
    Serial.print("Tecla pressionada: ");
    Serial.println(tecla);

    if (tecla == '#') {  // '#' confirma a senha
      verificaSenha();
      cursorPos = 0;
      resetarSenhas();
      lcd.clear();
      Serial.println("Digite a senha:");
    } else if (tecla == '*') {  // '*' para resetar a senha atual
      resetarSenhas();
      lcd.clear();
      lcd.print("Senha resetada");
      cursorPos = 0;
      Serial.println("Senha resetada. Digite novamente:");
      delay(1000);
      lcd.clear();
    } else {
      // Adiciona o caractere pressionado às senhas
      for (auto& usuario : usuarios) {
        usuario.senha.append(tecla);
      }
      lcd.setCursor(cursorPos, 1);
      lcd.print("*");
      cursorPos++;
    }
  }
}

void verificaSenha() {
  for (auto& usuario : usuarios) {
    if (usuario.senha.evaluate()) {
      Serial.println("Acesso liberado para " + usuario.nome);
      lcd.clear();
      lcd.print("Bem vindo " + usuario.nome);
      AcenderLED();
      apagarLEDs();
      sendRequest(usuario.url);
      return;
    }
  }

  // Senha incorreta
  Serial.println("Senha incorreta!");
  lcd.clear();
  lcd.print("Acesso Negado");
  piscarLED(PINO_LED_VERM, 15, 50);
  apagarLEDs();
}

void resetarSenhas() {
  for (auto& usuario : usuarios) {
    usuario.senha.reset();
  }
}

void AcenderLED() {
  digitalWrite(PINO_LED_AZUL, HIGH);
  delay(5000);
}

void initWifi() {
  WiFi.disconnect(true);
  delay(1000);
  
  lcd.clear();
  lcd.print("Conectando-se ");
  lcd.setCursor(0,1);
  lcd.print("ao Wi-Fi:");
  Serial.print("Conectando-se ao Wi-Fi: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);

  int tentativa = 0;
  while (WiFi.status() != WL_CONNECTED && tentativa < 20) {
    delay(500);
    Serial.print(".");
    tentativa++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConectado com sucesso!");
    lcd.clear();
    lcd.print("Conectado!");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
    delay(2000);
    lcd.clear();
    digitalWrite(2,HIGH);
  } else {
    lcd.clear();
    lcd.print("Falha");
    Serial.println("\nFalha ao conectar ao Wi-Fi.");
  }
}

void sendRequest(const char* url) {
  HTTPClient http;
  http.begin(url);
  int httpResponseCode = http.GET();
  
  if (httpResponseCode > 0) {
    Serial.print("Código de resposta: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Erro na requisição: ");
    Serial.println(http.errorToString(httpResponseCode).c_str());
  }
  http.end();
}

