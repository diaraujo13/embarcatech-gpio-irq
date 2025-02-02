// Aluno: IZAÍAS ARAÚJO GOMES DA SILVA
// Matrícula: tic370100473

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/bootrom.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"

#include "pio_matrix.pio.h"

//-------------------------------------------------------------------------

const uint BTN_A_PIN = 5;
const uint BTN_B_PIN = 6;

const uint LED_MATRIX_PIN = 7;
const uint LED_MATRIX_NUM_PINS = 25;

const uint LED_RED_PIN = 13;

//-------------------------------------------------------------------------

int NORMALIZED_ARRAY_LOCATION[25] = {
    24, 23, 22, 21, 20,
    15, 16, 17, 18, 19,
    14, 13, 12, 11, 10,
    5, 6, 7, 8, 9,
    4, 3, 2, 1, 0};

double digit0[25] = {
    0.0, 1.0, 1.0, 1.0, 0.0,
    1.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 0.0, 0.0, 0.0, 1.0,
    1.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 1.0, 1.0, 1.0, 0.0};

double digit1[25] = {
    0.0, 0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 1.0, 1.0, 0.0,
    0.0, 1.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0, 0.0};

double digit2[25] = {
    0.0, 1.0, 1.0, 1.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 1.0, 1.0, 1.0, 1.0,
    0.0, 1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 1.0, 1.0, 1.0};

double digit3[25] = {
    0.0, 0.0, 1.0, 1.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 1.0, 1.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 1.0, 1.0, 1.0};

double digit4[25] = {
    1.0, 0.0, 0.0, 1.0, 0.0,
    1.0, 0.0, 0.0, 1.0, 0.0,
    1.0, 1.0, 1.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0, 0.0};

double digit5[25] = {
    0.0, 1.0, 1.0, 1.0, 1.0,
    0.0, 1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 1.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 1.0, 1.0, 1.0, 0.0};

double digit6[25] = {
    1.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 1.0, 1.0, 1.0, 0.0,
    1.0, 0.0, 0.0, 1.0, 0.0,
    1.0, 1.0, 1.0, 1.0, 0.0};

double digit7[25] = {
    1.0, 1.0, 1.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 1.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0, 0.0};

double digit8[25] = {
    1.0,
    1.0,
    1.0,
    0.0,
    0.0,
    1.0,
    0.0,
    1.0,
    0.0,
    0.0,
    1.0,
    1.0,
    1.0,
    0.0,
    0.0,
    1.0,
    0.0,
    1.0,
    0.0,
    0.0,
    1.0,
    1.0,
    1.0,
    0.0,
    0.0,
};

double digit9[25] = {
    0.0, 0.0, 1.0, 1.0, 1.0,
    0.0, 0.0, 1.0, 0.0, 1.0,
    0.0, 0.0, 1.0, 1.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 1.0};

//-------------------------------------------------------------------------
void initialize_gpio_pins();
void blink_led();
void gpio_irq_handler_cb(uint gpio, uint32_t events);
uint32_t matrix_rgb(double b, double r, double g);
void display_digit(int digit, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b);

//-------------------------------------------------------------------------

volatile uint digit = 0;
volatile bool digit_changed = false;
volatile absolute_time_t last_interrupt_time = {0};

//-------------------------------------------------------------------------

int main()
{

    PIO pio = pio0;
    bool ok;
    uint16_t i;
    uint32_t valor_led;
    double r = 0.1, b = 0.0, g = 0.0;

    ok = set_sys_clock_khz(128000, false);

    stdio_init_all();
    initialize_gpio_pins();

    uint offset = pio_add_program(pio, &pio_matrix_program);
    uint sm = pio_claim_unused_sm(pio, true);
    pio_matrix_program_init(pio, sm, offset, LED_MATRIX_PIN);

    while (1)
    {
        if (digit_changed)
        {
            display_digit(digit, valor_led, pio, sm, r, g, b);
            digit_changed = false;
        }
        blink_led();
    };

    return 0;
}

//-------------------------------------------------------------------------

void initialize_gpio_pins()
{
    gpio_init(BTN_A_PIN);
    gpio_init(BTN_B_PIN);
    gpio_init(LED_MATRIX_PIN);
    gpio_init(LED_RED_PIN);

    gpio_set_dir(BTN_A_PIN, GPIO_IN);
    gpio_set_dir(BTN_B_PIN, GPIO_IN);
    gpio_set_dir(LED_MATRIX_PIN, GPIO_OUT);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);

    gpio_pull_up(BTN_A_PIN);
    gpio_pull_up(BTN_B_PIN);

    gpio_set_irq_enabled_with_callback(BTN_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler_cb);
    gpio_set_irq_enabled_with_callback(BTN_B_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler_cb);
}

void blink_led()
{
    for (int i = 0; i < 5; i++)
    {
        gpio_put(LED_RED_PIN, 1);
        sleep_ms(100);
        gpio_put(LED_RED_PIN, 0);
        sleep_ms(100);
    }
}

void gpio_irq_handler_cb(uint gpio, uint32_t events)
{
    absolute_time_t now = get_absolute_time();
    if (absolute_time_diff_us(last_interrupt_time, now) < 200000)
    {
        return;
    }
    last_interrupt_time = now;

    if (gpio == BTN_A_PIN)
    {
        if (digit == 9)
        {
            digit = 0;
        }
        else
        {
            digit += 1;
        }
        digit_changed = true;
    }
    else if (gpio == BTN_B_PIN)
    {
        if (digit == 0)
        {
            digit = 9;
        }
        else
        {
            digit -= 1;
        }
        digit_changed = true;
    }
}

uint32_t matrix_rgb(double b, double r, double g)
{
    uint8_t R = (uint8_t)(r * 255);
    uint8_t G = (uint8_t)(g * 255);
    uint8_t B = (uint8_t)(b * 255);
    return (G << 24) | (R << 16) | (B << 8);
}

void display_digit(int digit, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b)
{
    double *digitsMap[] = {digit0, digit1, digit2, digit3, digit4, digit5, digit6, digit7, digit8, digit9};
    double *desenho = digitsMap[digit];

    for (int i = 0; i < LED_MATRIX_NUM_PINS; i++)
    {
        int n = NORMALIZED_ARRAY_LOCATION[i];
        valor_led = matrix_rgb(0, desenho[n] * r, 0);
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}
