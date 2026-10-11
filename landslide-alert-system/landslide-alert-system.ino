#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

// ==========================================
// CONFIGURAÇÕES DOS PINOS (HARDWARE)
// ==========================================

// Pinos do Sensor de Umidade/Temperatura (DHT)
#define DHTPIN 4          // Pino de dados do DHT conectado ao pino D4 do ESP32
#define DHTTYPE DHT11     // Mude para DHT11 se estiver usando o DHT11
DHT dht(DHTPIN, DHTTYPE);

// Pinos do Sensor Ultrassônico HC-SR04
#define TRIG_PIN 5        // Pino TRIG
#define ECHO_PIN 18       // Pino ECHO

// Pinos dos LEDs
#define LED_VERDE 19
#define LED_AMARELO 21
#define LED_VERMELHO 2

// Pinos dos Buzzers
#define BUZZER_1 23
#define BUZZER_2 15

// ==========================================
// VARIÁVEIS GLOBAIS E LIMITES
// ==========================================

// Limites de risco
const float LIMITE_UMIDADE = 70.0;    // Umidade do ar >= 70% aciona estado de atenção
const float LIMITE_VARIACAO_CM = 5.0; // Movimentação maior que 5cm aciona perigo

// Variáveis para controle do ultrassônico
float distanciaBase = 0.0;
float distanciaAtual = 0.0;
float umidadeAtual = 0.0;
float temperaturaAtual = 0.0;

// Estado atual do sistema
// 0 = Seguro, 1 = Atenção, 2 = Perigo Eminente
int statusAtual = 0; 

// Variáveis para controle assíncrono do buzzer (sem usar delay longo)
unsigned long tempoAnteriorBuzzer = 0;
bool estadoBuzzer = false;
const long intervaloBuzzer = 500; // 500ms ligado, 500ms desligado

// Variáveis de tempo para leitura dos sensores
unsigned long tempoAnteriorDHT = 0;
const long intervaloDHT = 2000; // Ler DHT a cada 2s (limitação do sensor)

unsigned long tempoAnteriorUltrassonico = 0;
const long intervaloUltrassonico = 200; // Ler Ultrassônico a cada 200ms para resposta rápida e baixa latência

// Servidor Web na porta 80
WebServer server(80);

// ==========================================
// FUNÇÕES DO SENSOR ULTRASSÔNICO
// ==========================================

float lerDistancia() {
  // Garante que o TRIG esteja LOW
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  
  // Envia um pulso de 10 microssegundos para acionar o ultrassônico
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Lê o tempo que o eco levou para retornar
  long duracao = pulseIn(ECHO_PIN, HIGH, 30000); // Timeout de 30ms para evitar travamentos
  
  if (duracao == 0) {
    return -1.0; // Erro na leitura
  }
  
  // Calcula a distância em centímetros (Velocidade do som: 343 m/s)
  float distancia = (duracao / 2.0) * 0.0343;
  return distancia;
}

void calibrarSensorBase() {
  Serial.println("Calibrando sensor ultrassônico...");
  float soma = 0;
  int leiturasValidas = 0;
  
  // Tira 10 medidas ao ligar para calcular a média e definir a distância base
  for (int i = 0; i < 10; i++) {
    float dist = lerDistancia();
    if (dist > 0) {
      soma += dist;
      leiturasValidas++;
    }
    delay(100);
  }
  
  if (leiturasValidas > 0) {
    distanciaBase = soma / leiturasValidas;
    Serial.print("Calibração concluída. Distância base: ");
    Serial.print(distanciaBase);
    Serial.println(" cm");
  } else {
    distanciaBase = 100.0; // Valor padrão em caso de falha
    Serial.println("Falha na calibração. Usando valor padrão.");
  }
}

// ==========================================
// FUNÇÕES DO SERVIDOR WEB
// ==========================================

// Retorna apenas os dados em formato JSON para o AJAX do front-end
void handleDados() {
  String json = "{";
  json += "\"status\":" + String(statusAtual) + ",";
  json += "\"umidade\":" + String(umidadeAtual, 1) + ",";
  json += "\"distancia\":" + String(distanciaAtual, 1) + ",";
  json += "\"distanciaBase\":" + String(distanciaBase, 1);
  json += "}";
  server.send(200, "application/json", json);
}

// Retorna a página HTML principal
void handleRoot() {
  String html = "<!DOCTYPE html><html lang='pt-BR'><head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>Defesa Civil - Alerta</title>";
  html += "<style>";
  html += "* { box-sizing: border-box; }";
  html += "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; text-align: center; margin: 0; padding: 20px; transition: background-color 0.5s; color: #fff; display: flex; flex-direction: column; align-items: center; justify-content: center; min-height: 100vh; }";
  html += ".container { max-width: 600px; width: 100%; margin: 0 auto; background: rgba(0,0,0,0.4); padding: 30px 20px; border-radius: 20px; box-shadow: 0 8px 32px rgba(0,0,0,0.3); backdrop-filter: blur(10px); }";
  html += "h1 { font-size: 2.2em; margin-bottom: 5px; text-transform: uppercase; letter-spacing: 2px; }";
  html += "h2 { font-size: 1.2em; margin-top: 0; margin-bottom: 30px; font-weight: 400; opacity: 0.9; }";
  html += ".status-text { font-size: 1.5em; font-weight: bold; margin: 20px 0; padding: 20px; border-radius: 12px; background: rgba(255,255,255,0.2); box-shadow: inset 0 2px 4px rgba(0,0,0,0.1); white-space: pre-line; }";
  html += ".info-box { display: flex; justify-content: space-around; flex-wrap: wrap; margin-top: 30px; gap: 15px; }";
  html += ".info { background: rgba(0,0,0,0.3); padding: 15px; border-radius: 12px; flex: 1; min-width: 120px; }";
  html += ".info-title { font-size: 0.9em; opacity: 0.8; margin-bottom: 5px; display: block; }";
  html += ".info-value { font-size: 1.4em; font-weight: bold; }";
  html += ".safe { background-color: #2e7d32; } /* Verde */";
  html += ".warning { background-color: #e65100; } /* Laranja */";
  html += ".danger { background-color: #b71c1c; animation: piscar 1s infinite; } /* Vermelho */";
  html += "@keyframes piscar { 0% { background-color: #b71c1c; } 50% { background-color: #d32f2f; } 100% { background-color: #b71c1c; } }";
  html += "</style>";
  html += "</head><body id='bg' class='safe'>";
  
  html += "<div class='container'>";
  html += "<h1>Sistema de Alerta</h1>";
  html += "<h2>Monitoramento de Deslizamento</h2>";
  
  html += "<div id='status-msg' class='status-text'>Conectando aos sensores...</div>";
  
  html += "<div class='info-box'>";
  html += "<div class='info'><span class='info-title'>Umidade do Solo/Ar</span><span class='info-value'><span id='umi'>--</span>%</span></div>";
  html += "<div class='info'><span class='info-title'>Distância Atual</span><span class='info-value'><span id='dist'>--</span> cm</span></div>";
  html += "<div class='info'><span class='info-title'>Base Calibrada</span><span class='info-value'><span id='dist-base'>--</span> cm</span></div>";
  html += "</div>";
  html += "</div>";

  html += "<script>";
  html += "function atualizarDados() {";
  html += "  fetch('/dados').then(response => response.json()).then(data => {";
  html += "    document.getElementById('umi').innerText = data.umidade;";
  html += "    document.getElementById('dist').innerText = data.distancia;";
  html += "    document.getElementById('dist-base').innerText = data.distanciaBase;";
  
  html += "    let bg = document.getElementById('bg');";
  html += "    let msg = document.getElementById('status-msg');";
  
  html += "    if(data.status === 0) {";
  html += "      bg.className = 'safe';";
  html += "      msg.innerText = 'STATUS: SEGURO\\nMonitoramento Estável';";
  html += "    } else if(data.status === 1) {";
  html += "      bg.className = 'warning';";
  html += "      msg.innerText = 'STATUS: ATENÇÃO\\nSolo Saturado (Risco Possível)';";
  html += "    } else {";
  html += "      bg.className = 'danger';";
  html += "      msg.innerText = 'ALERTA: PERIGO EMINENTE\\nEvacue a Área!';";
  html += "    }";
  html += "  }).catch(err => console.log('Erro na requisição AJAX', err));";
  html += "}";
  html += "setInterval(atualizarDados, 2000);"; // Fetch a cada 2s
  html += "atualizarDados();"; 
  html += "</script>";

  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

// ==========================================
// CONFIGURAÇÃO INICIAL (SETUP)
// ==========================================

void setup() {
  Serial.begin(115200);
  
  // Configura pinos dos LEDs e Buzzers
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(BUZZER_1, OUTPUT);
  pinMode(BUZZER_2, OUTPUT);
  
  // Configura pinos do Ultrassônico
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Inicializa estado do hardware
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_AMARELO, LOW);
  digitalWrite(LED_VERMELHO, LOW);
  digitalWrite(BUZZER_1, LOW);
  digitalWrite(BUZZER_2, LOW);
  
  // Inicializa o DHT
  dht.begin();
  
  // Configura o ESP32 como Access Point (AP)
  Serial.println("\nConfigurando Ponto de Acesso (AP)...");
  WiFi.softAP("Proj_terra");
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("Access Point Iniciado. Conecte-se na rede: Alerta_Defesa_Civil\n");
  Serial.print("Acesse no navegador pelo IP: ");
  Serial.println(IP);
  
  // Configura rotas do Servidor Web
  server.on("/", HTTP_GET, handleRoot);
  server.on("/dados", HTTP_GET, handleDados);
  server.begin();
  Serial.println("Servidor Web HTTP iniciado na porta 80.");
  
  // Calibra a distância base inicial
  calibrarSensorBase();
}

// ==========================================
// LOOP PRINCIPAL
// ==========================================

void loop() {
  // Mantém o servidor web rodando
  server.handleClient();
  
  unsigned long tempoAtual = millis();
  
  // ==========================================
  // LEITURA DOS SENSORES E ATUALIZAÇÃO DO STATUS
  // ==========================================
  bool statusAlterado = false;
  bool printLog = false;

  // 1. Leitura do Ultrassônico (Alta Frequência - Resposta rápida em 200ms)
  if (tempoAtual - tempoAnteriorUltrassonico >= intervaloUltrassonico) {
    tempoAnteriorUltrassonico = tempoAtual;
    float novaDistancia = lerDistancia();
    if (novaDistancia > 0) {
      distanciaAtual = novaDistancia;
    }
    statusAlterado = true;
  }

  // 2. Leitura do DHT (Baixa Frequência - Respeita limite de 2s do sensor)
  if (tempoAtual - tempoAnteriorDHT >= intervaloDHT) {
    tempoAnteriorDHT = tempoAtual;
    float novaUmidade = dht.readHumidity();
    if (!isnan(novaUmidade)) {
      umidadeAtual = novaUmidade;
    }
    statusAlterado = true;
    printLog = true; // Imprime no serial a cada 2 segundos para não floodar
  }

  if (statusAlterado) {
    float variacao = abs(distanciaAtual - distanciaBase);
    
    // Lógica de determinação do Status do sistema
    if (variacao > LIMITE_VARIACAO_CM) {
      statusAtual = 2; // PERIGO EMINENTE: Movimentação detectada
    } 
    else if (umidadeAtual >= LIMITE_UMIDADE) {
      statusAtual = 1; // ATENÇÃO: Solo muito úmido
    } 
    else {
      statusAtual = 0; // SEGURO: Normal
    }
    
    // Atualiza o estado dos LEDs
    switch (statusAtual) {
      case 0:
        digitalWrite(LED_VERDE, HIGH);
        digitalWrite(LED_AMARELO, LOW);
        digitalWrite(LED_VERMELHO, LOW);
        break;
      case 1:
        digitalWrite(LED_VERDE, LOW);
        digitalWrite(LED_AMARELO, HIGH);
        digitalWrite(LED_VERMELHO, LOW);
        break;
      case 2:
        digitalWrite(LED_VERDE, LOW);
        digitalWrite(LED_AMARELO, LOW);
        digitalWrite(LED_VERMELHO, HIGH);
        break;
    }
    
    if (printLog) {
      Serial.print("Umid: "); Serial.print(umidadeAtual); Serial.print("% | ");
      Serial.print("Dist Atual: "); Serial.print(distanciaAtual); Serial.print("cm | ");
      Serial.print("Variação: "); Serial.print(variacao); Serial.print("cm | ");
      Serial.print("Status: "); Serial.println(statusAtual);
    }
  }
  
  // ==========================================
  // CONTROLE DOS BUZZERS (SIRENE)
  // ==========================================
  // Os buzzers devem apitar APENAS em caso de perigo eminente (Status 2)
  if (statusAtual == 2) {
    if (tempoAtual - tempoAnteriorBuzzer >= intervaloBuzzer) {
      tempoAnteriorBuzzer = tempoAtual;
      estadoBuzzer = !estadoBuzzer; // Alterna entre ligado e desligado (sirene)
      digitalWrite(BUZZER_1, estadoBuzzer ? HIGH : LOW);
      digitalWrite(BUZZER_2, estadoBuzzer ? HIGH : LOW);
    }
  } else {
    // Garante que os buzzers permaneçam desligados nos outros estados
    digitalWrite(BUZZER_1, LOW);
    digitalWrite(BUZZER_2, LOW);
    estadoBuzzer = false;
  }
}