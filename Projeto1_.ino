#include <Keypad.h>
#include <Password.h>
#include <LiquidCrystal_PCF8574.h>
#include <WiFi.h>
#include <Wire.h>
#include <HTTPClient.h>


const char* ssid = "projeto"; //login nome da rede
const char* password = "12345678"; // senha

const char* serverNameigor = "https://api.thingspeak.com/update?api_key=0PE67D6HAFFP0SW9&field1=1234";
const char* serverNamejoao = "https://api.thingspeak.com/update?api_key=0PE67D6HAFFP0SW9&field1=5678"; 
const char* serverNamecarlos = "https://api.thingspeak.com/update?api_key=0PE67D6HAFFP0SW9&field1=4321";
const char* serverNamejorge = "https://api.thingspeak.com/update?api_key=0PE67D6HAFFP0SW9&field1=8765";

WiFiClient client;

// Configuração das Senhas
Password senha1 = Password("1234"); // Senha do Usuário 1
Password senha2 = Password("5678"); // Senha do Usuário 2
Password senha3 = Password("4321"); // Senha do Usuário 3
Password senha4 = Password("8765"); // Senha do Usuário 4

char senhaIgor[] = "1234";
char senhaJoao[] = "5678";
char senhaCarlos[] = "4321";
char senhaJorge[] = "8765";


String postStr;

// Configuração do Teclado
constexpr uint8_t COLUNAS = 4;
constexpr uint8_t LINHAS = 4;

LiquidCrystal_PCF8574 lcd(0x3F);

constexpr int PINO_LED_VERM = 15;
constexpr int PINO_LED_AZUL = 14;

char teclasMatriz[LINHAS][COLUNAS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

uint8_t pinosLinhas[LINHAS] = {4, 27, 26, 25}; 
uint8_t pinosColunas[COLUNAS] = {33, 32, 13, 12};

int cursorPos = 0; // controla a posição do cursor LCD

Keypad teclado = Keypad(makeKeymap(teclasMatriz), pinosLinhas, pinosColunas, LINHAS, COLUNAS);



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
// Wifi 
  Serial.begin(9600);
  
  lcd.begin(16,2);
  lcd.setBacklight(255);
  lcd.home(); 
  initWifi();

  pinMode(PINO_LED_VERM, OUTPUT);
  pinMode(PINO_LED_AZUL, OUTPUT);
  pinMode(2, OUTPUT);
  apagarLEDs();
  Serial.println("Digite a senha:");

}

void loop() {
  char tecla = teclado.getKey(); // leitura da tecla pressionada
  lcd.home();
  lcd.print("Digite sua Senha:");
  

  if (tecla) {
    Serial.print("Tecla pressionada: ");
    Serial.println(tecla);

    if (tecla == '#') {  // '#' confirma a senha
      if (senha1.evaluate()) { // Se a senha1 está correta
        Serial.println("Acesso liberado para Usuario 1!");
        lcd.clear();
        lcd.print("Bem vindo Igor");
        AcenderLED();
        apagarLEDs();// Função para apagar o led
        sendRequestigor();
          
      } else if (senha2.evaluate()) { // Se a senha2 está correta
        Serial.println("Acesso liberado para Usuario 2!");
        lcd.clear();
        lcd.print("Bem vindo Joao");
        AcenderLED(); // acender led por 5 segundos
        apagarLEDs(); // Função para apagar o led
        sendRequestjoao();

      } else if (senha3.evaluate()) { // Se a senha2 está correta
        Serial.println("Acesso liberado para Usuario 3!");
        lcd.clear();
        lcd.print("Bem vindo Carlos");
        AcenderLED(); // acender led por 5 segundos
        apagarLEDs();
        sendRequestcarlos();

      } else if (senha4.evaluate()) { // Se a senha2 está correta
        Serial.println("Acesso liberado para Usuario 4!");
        lcd.clear();
        lcd.print("Bem vindo Jorge");
        AcenderLED(); // acender led por 5 segundos
        apagarLEDs();
        sendRequestjorge();
      }
       else {
        Serial.println("Senha incorreta!");
        lcd.clear();
        lcd.home();
        lcd.print("Acesso Negado");
        lcd.setCursor(8,1);
        piscarLED(PINO_LED_VERM,15,50);
        apagarLEDs();
      }
      cursorPos = 0; // reinicia posição do cursor
      senha1.reset(); // Limpa a senha
      senha2.reset();
      senha3.reset();
      senha4.reset(); // Limpa a senha
      lcd.clear(); // Limpa tela lcd
      Serial.println("Digite a senha:");

    } else if (tecla == '*') {  // '*' para resetar a senha atual
      senha1.reset(); // reseta a senha para usuario recomeçar
      senha2.reset();
      senha3.reset();
      senha4.reset();
      lcd.clear();
      lcd.home();
      lcd.print("Senha resetada"); // reseta a senha para usuario recomeçar
      cursorPos = 0; // reinicia posição do cursor
      
      Serial.println("Senha resetada. Digite novamente:");
      lcd.clear();

    } else {
      // Adiciona o caractere pressionado à senha atual
      senha1.append(tecla);
      senha2.append(tecla);
      senha3.append(tecla);
      senha4.append(tecla);


      lcd.setCursor(cursorPos, 1); // exibe * para cada caractere digitado 
      lcd.print("*"); 
      cursorPos++; // Move cursor no lcd para proxima posição 
    }
  }
}


void AcenderLED(){
  digitalWrite(PINO_LED_AZUL, HIGH);
  delay(5000);
}

void initWifi(){

  WiFi.disconnect(true);  // Limpa qualquer configuração Wi-Fi antiga
  delay(1000);
  
  lcd.clear();
  lcd.home();
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
    Serial.print("Endereço IP: ");
    lcd.clear();
    lcd.print("Conectado com sucesso!");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
    Serial.println(WiFi.localIP());
    lcd.clear();
    digitalWrite(2,HIGH); // aciona led integrado na esp no pino 2
   
  } 
  else {
    lcd.clear();
    lcd.print("Falha");
    Serial.println("\nFalha ao conectar ao Wi-Fi.");
    
  }

}


void sendRequestigor() {

    HTTPClient http;
    // Configura o endpoint do servidor
    http.begin(serverNameigor);
    
    // Envia a solicitação GET
    int httpResponseCode = http.GET();
    // Processa o código de resposta
    // Encerra a conexão HTTP
    http.end();

    http.begin("http://35.247.202.97:5000/relatorio");
    // Corpo da requisição
    String jsonBody = "{\"message\":\"hello world\"}";
    // Envia a requisição POST
    int httpResponseCode = http.POST(jsonBody);
    http.end();
}


void sendRequestjoao() {

    HTTPClient http;
    // Configura o endpoint do servidor
    http.begin(serverNamejoao);
    // Envia a solicitação GET
    int httpResponseCode = http.GET();
    // Processa o código de resposta
    // Encerra a conexão HTTP
    http.end();
  
}


void sendRequestcarlos() {

    HTTPClient http;
    // Configura o endpoint do servidor
    http.begin(serverNamecarlos);
    // Envia a solicitação GET
    int httpResponseCode = http.GET();
    // Processa o código de resposta
    // Encerra a conexão HTTP
    http.end();
  
}

void sendRequestjorge() {

    HTTPClient http;
    // Configura o endpoint do servidor
    http.begin(serverNamejorge);
    // Envia a solicitação GET
    int httpResponseCode = http.GET();
    // Processa o código de resposta
    // Encerra a conexão HTTP
    http.end();
  
}
