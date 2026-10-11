// Definição dos pinos para os LEDs e o Botão
int ledVerde = 8;
int ledAmarelo = 9;
int ledVermelho = 11;
int botaoPedestre = 10;

void setup() {
  // Configura os pinos dos LEDs como saída
  pinMode(ledVerde, OUTPUT);
  pinMode(ledAmarelo, OUTPUT);
  pinMode(ledVermelho, OUTPUT);

  // Configura o pino do botão como entrada
  pinMode(botaoPedestre, INPUT);

  // Estado inicial: Verde para os carros aceso
  digitalWrite(ledVerde, HIGH);
  digitalWrite(ledAmarelo, LOW);
  digitalWrite(ledVermelho, LOW);
}

void loop() {
  // Verifica se o botão foi pressionado (Nível ALTO)
  if (digitalRead(botaoPedestre) == LOW) {
    // Pequeno atraso para confirmação da intenção de atravessar
    delay(2000);

    // 1. Inicia a transição: Apaga o Verde e acende o Amarelo
    digitalWrite(ledVerde, LOW);
    digitalWrite(ledAmarelo, HIGH);
    delay(4000); // Aguarda 4 segundos no Amarelo

    // 2. Para os carros: Apaga o Amarelo e acende o Vermelho
    digitalWrite(ledAmarelo, LOW);
    digitalWrite(ledVermelho, HIGH);

    // 3. Tempo para o pedestre atravessar com segurança
    delay(8000); // 8 segundos no Vermelho

    // 4. Finaliza o ciclo: Apaga o Vermelho e volta ao Verde
    digitalWrite(ledVermelho, LOW);
    digitalWrite(ledVerde, HIGH);

    // Espera um pouco antes de permitir uma nova ativação pelo botão
    delay(5000);
  }
  else {
    digitalWrite(ledVerde, HIGH);
  }
}