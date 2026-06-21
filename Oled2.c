#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/i2c.h"
#include "ssd1306.h"

#define LED_R_PIN 13
#define LED_G_PIN 11
#define LED_B_PIN 12

#define BTN_A_PIN 5

#define I2C_PORT i2c1
#define SDA_PIN 14
#define SCL_PIN 15

int A_state = 0;    // Botao A está pressionado?

// Inicializando a tela OLED
ssd1306_t display;

void SinalAberto(){
    gpio_put(LED_R_PIN, 0);
    gpio_put(LED_G_PIN, 1);
    gpio_put(LED_B_PIN, 0);   
    ssd1306_clear(&display);
    ssd1306_draw_string(&display, 0, 0,1, "SINAL VERDE");
    ssd1306_draw_string(&display, 0, 16,1, "ATRAVESSAR A RUA");
    ssd1306_show(&display);
}

void SinalFechado(){
    gpio_put(LED_R_PIN, 1);
    gpio_put(LED_G_PIN, 0);
    gpio_put(LED_B_PIN, 0); 
    ssd1306_clear(&display);
    ssd1306_draw_string(&display, 0, 0,1, "SINAL VERMELHO");
    ssd1306_draw_string(&display, 0, 16,1, "AGUARDE");
    ssd1306_show(&display);
}

int WaitWithRead(int timeMS){
    for(int i = 0; i < timeMS; i = i+100){
        A_state = !gpio_get(BTN_A_PIN);
        if(A_state == 1){
            return 1;
        }
        sleep_ms(100);
    }
    return 0;
}

int main(){
    // INICIANDO LEDS
    gpio_init(LED_R_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_init(LED_G_PIN);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_init(LED_B_PIN);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);

    // INICIANDO BOTÃO
    gpio_init(BTN_A_PIN);
    gpio_set_dir(BTN_A_PIN, GPIO_IN);
    gpio_pull_up(BTN_A_PIN);
    
    // INICIANDO O I2C PARA OLED
    i2c_init(I2C_PORT, 400000);  // 400kHz é a velocidade do barramento I2C
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    // Inicializando o display OLED
    ssd1306_init(&display, 128, 64, 0x3C,I2C_PORT);  // Endereço I2C padrão do SSD1306

    while(true){
        // Semáforo verde para os carros
        SinalFechado();
        A_state = WaitWithRead(8000);   // Espera com leitura do botão

        if(A_state){  // O pedestre apertou o botão quando o sinal está VERDE
            // Acelera o semáforo para o pedestre, transição mais rápida para VERMELHO
            // Sinal vermelho para os carros por 5s
            sleep_ms(1000);
            SinalAberto();
            sleep_ms(5000);

            // Sinal verde para os carros
            SinalFechado();
            sleep_ms(8000);
        } else {  // Nenhum aperto de botão - continua no semáforo normal
            // Sinal vermelho para os carros por 8s
            SinalAberto();
            sleep_ms(8000);
        }
    }

    return 0;
}