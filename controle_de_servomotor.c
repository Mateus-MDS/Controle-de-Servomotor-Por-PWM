#include <stdio.h>            // Biblioteca padrão de entrada/saída
#include "pico/stdlib.h"      // Biblioteca do SDK do Raspberry Pi Pico
#include "pico/time.h"        // Biblioteca para gerenciamento de tempo
#include "hardware/irq.h"     // Biblioteca para gerenciamento de interrupções
#include "hardware/pwm.h"     // Biblioteca para controlar o hardware de PWM

// Definição dos pinos
#define LED_AZUL 12           // Pino do LED (GPIO 12)
#define PWM_SERVOMOTOR 22     // Pino do servomotor (GPIO 22)

// Definição das constantes
const uint16_t FPWM = 50;                 // Frequência do PWM em Hz (50Hz)
const uint32_t FREQUENCIA_CLOCK = 125000000; // Clock do RP2040 em Hz (125MHz)
const float DIVISOR = 50.0;                // Divisor de clock do PWM, valor escolhido
const uint16_t TPWM = 20000;               // Período do PWM em microssegundos (20ms)

// Variáveis globais
uint16_t wrap; // Armazena o valor de wrap calculado

// Função para calcular o valor do wrap baseado na frequência do PWM
void calcular_wrap() {
    wrap = (FREQUENCIA_CLOCK / (DIVISOR * FPWM));
    printf("Wrap calculado: %u\n", wrap);
}

// Posição 180° (PWM de 2400µs)
void primeira_acao() {

    // Obtendo o número do slice com base no pino do PWM
    uint slice_servo = pwm_gpio_to_slice_num(PWM_SERVOMOTOR);
    uint slice_led = pwm_gpio_to_slice_num(LED_AZUL);

    // Definindo o nivel de PWM no pino, duty cycle
    pwm_set_gpio_level(PWM_SERVOMOTOR, (2400 * wrap) / TPWM);
    pwm_set_gpio_level(LED_AZUL, (2400 * wrap) / TPWM);

    // Habilitando o PWM no slice correspodente
    pwm_set_enabled(slice_servo, true);
    pwm_set_enabled(slice_led, true);
}

// Posição 90° (PWM de 1470µs)
void segunda_acao() {

    // Obtendo o número do slice com base no pino do PWM
    uint slice_servo = pwm_gpio_to_slice_num(PWM_SERVOMOTOR);
    uint slice_led = pwm_gpio_to_slice_num(LED_AZUL);

    // Definindo o nivel de PWM no pino, duty cycle
    pwm_set_gpio_level(PWM_SERVOMOTOR, (1470 * wrap) / TPWM);
    pwm_set_gpio_level(LED_AZUL, (1470 * wrap) / TPWM);

    // Habilitando o PWM no slice correspodente
    pwm_set_enabled(slice_servo, true);
    pwm_set_enabled(slice_led, true);
}

// Posição 0° (PWM de 500µs)
void terceira_acao() {
    
    // Obtendo o número do slice com base no pino do PWM
    uint slice_servo = pwm_gpio_to_slice_num(PWM_SERVOMOTOR);
    uint slice_led = pwm_gpio_to_slice_num(LED_AZUL);

    // Definindo o nivel de PWM no pino, duty cycle
    pwm_set_gpio_level(PWM_SERVOMOTOR, (500 * wrap) / TPWM);
    pwm_set_gpio_level(LED_AZUL, (500 * wrap) / TPWM);

    // Habilitando o PWM no slice correspodente
    pwm_set_enabled(slice_servo, true);
    pwm_set_enabled(slice_led, true);
}

// Movimento suave entre 0° e 180°
void quarta_acao() {

    // Definindo as váriaveis
    uint16_t incremento = 5; // Incremento de 5µs no PWM
    uint16_t incremento_pwm = (uint16_t)((incremento * wrap) / TPWM);
    uint16_t tempo_on = (500 * wrap) / TPWM; // Começa em 0°
    uint16_t wrap_maximo = (2400 * wrap) / TPWM; // 180°
    uint16_t wrap_minimo = (500 * wrap) / TPWM;  // 0°
    uint ciclo = 1; // Variável para alternar entre crescente/decrescente

    printf("Iniciando movimentação suave\n");

    // Habilita o PWM antes do loop
    uint slice_servo = pwm_gpio_to_slice_num(PWM_SERVOMOTOR);
    uint slice_led = pwm_gpio_to_slice_num(LED_AZUL);

    // Habilitando o PWM no slice correspodente
    pwm_set_enabled(slice_servo, true);
    pwm_set_enabled(slice_led, true);

    while (true) {
        static uint16_t ultimo_tempo = 0;

        // Apenas imprime se o valor mudar, evitando sobrecarga
        if (tempo_on != ultimo_tempo) {
            printf("Ciclo ativo: %d\n", tempo_on);
            ultimo_tempo = tempo_on;
        }

        // Define o nível do PWM
        pwm_set_gpio_level(PWM_SERVOMOTOR, tempo_on);
        pwm_set_gpio_level(LED_AZUL, tempo_on);

        // Aguarda 10ms
        sleep_ms(10);

        // Alterna entre aumentar e diminuir o duty cycle
        if (ciclo) {
            tempo_on += incremento_pwm;
            if (tempo_on >= wrap_maximo) ciclo = 0;
        } else {
            tempo_on -= incremento_pwm;
            if (tempo_on <= wrap_minimo) ciclo = 1;
        }
    }
}

// Função principal
int main() {
    stdio_init_all(); // Inicializa comunicação serial
    calcular_wrap();  // Calcula o valor do wrap

    // Configuração dos pinos
    gpio_init(LED_AZUL);
    gpio_set_dir(LED_AZUL, GPIO_OUT);
    gpio_set_function(PWM_SERVOMOTOR, GPIO_FUNC_PWM);
    gpio_set_function(LED_AZUL, GPIO_FUNC_PWM);

    // Obtém os slices PWM dos pinos
    uint slice_servo = pwm_gpio_to_slice_num(PWM_SERVOMOTOR);
    uint slice_led = pwm_gpio_to_slice_num(LED_AZUL);

    // Configura o clock do PWM
    pwm_set_clkdiv(slice_servo, DIVISOR);
    pwm_set_clkdiv(slice_led, DIVISOR);

    // Define o wrap para o PWM
    pwm_set_wrap(slice_servo, wrap);
    pwm_set_wrap(slice_led, wrap);

    // Executa as posições fixas do servo
    primeira_acao();
    sleep_ms(5000);

    segunda_acao();
    sleep_ms(5000);

    terceira_acao();
    sleep_ms(5000);

    // Inicia o movimento suave do servo
    quarta_acao();
}
