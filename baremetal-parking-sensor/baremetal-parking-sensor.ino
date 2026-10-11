#include <avr/io.h>
#include <avr/interrupt.h>

// Pinos
const int pinoSensor = 2; // OBRIGATÓRIO (Interrupção Externa 0)
const int pinoBuzzer = 8;
const int pinoLed     = 9; // LED de uma cor só

// Variável "slot" exigida na arquitetura de Máquina de Estados (Aula 5)
volatile int slot = 0; // 0 = Caminho Livre, 1 = Alerta
volatile bool pisca = false;

void setup() {
  pinMode(pinoSensor, INPUT_PULLUP);
  pinMode(pinoBuzzer, OUTPUT);
  pinMode(pinoLed, OUTPUT);

  // === 1. INTERRUPÇÃO EXTERNA (Muda o estado do slot instantaneamente) ===
  attachInterrupt(digitalPinToInterrupt(pinoSensor), leSensor, CHANGE);

  // === 2. INTERRUPÇÃO DE TIMER (Timer1 - Slide 29) ===
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 34286;  // Tempo para estourar a cada 500ms
  TCCR1B |= (1 << CS12);   // Prescaler de 256
  TIMSK1 |= (1 << TOIE1);  // Habilita o Timer
  interrupts();

  leSensor(); // Leitura inicial
}

// Rotina da Interrupção Externa
void leSensor() {
  if (digitalRead(pinoSensor) == LOW) {
    slot = 1; // Obstáculo detectado -> vai para o slot 1
  } else {
    slot = 0; // Caminho livre -> vai para o slot 0
  }
}

// === MÁQUINA DE ESTADOS DENTRO DO TIMER (Slide 29) ===
ISR(TIMER1_OVF_vect) {
  TCNT1 = 34286; // Reinicia o relógio

  // O switch case exigido pelo projeto
  switch (slot) {
    case 0: // ESTADO: SEGURO
      digitalWrite(pinoLed, LOW); // Apaga
      noTone(pinoBuzzer);         // Fica mudo
      pisca = false;
      break;

    case 1: // ESTADO: ALERTA (Pisca e Apita)
      pisca = !pisca; // Inverte para criar o efeito de piscar

      if (pisca) {
        digitalWrite(pinoLed, HIGH);
        tone(pinoBuzzer, 1000);
      } else {
        digitalWrite(pinoLed, LOW);
        noTone(pinoBuzzer);
      }
      break;

    default:
      slot = 0;
      break;
  }
}

// === LOOP BAREMETAL (Vazio) ===
void loop() {
  // O processador não faz nada aqui.
  // O controle é todo cooperativo via Timer e Interrupção externa.
}