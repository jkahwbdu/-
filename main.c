#include "stm32f10x.h"

#define LED_COUNT   4U

typedef struct
{
    GPIO_TypeDef *port;
    uint8_t       pin;
    uint8_t       mode;
    uint8_t       active_low;
    uint16_t      mask;
} LedItem;


static const LedItem g_leds[LED_COUNT] =
{
    { GPIOA,  0U, 0x3U, 0U, (uint16_t)(1U << 0)  },
    { GPIOA,  3U, 0x3U, 0U, (uint16_t)(1U << 3)  },
    { GPIOB,  0U, 0x3U, 0U, (uint16_t)(1U << 0)  },
    { GPIOC, 13U, 0x2U, 1U, (uint16_t)(1U << 13) }
};


static void gpio_clock_enable(void)
{
    RCC->APB2ENR |= (1U << 2) | (1U << 3) | (1U << 4);
}


static void led_pin_init(const LedItem *item)
{
    uint32_t shift;
    volatile uint32_t *cr;

    shift = ((uint32_t)item->pin & 7U) * 4U;

    if (item->pin < 8U)
    {
        cr = &item->port->CRL;
    }
    else
    {
        cr = &item->port->CRH;
    }

    *cr = (*cr & ~(0xFUL << shift)) | ((uint32_t)item->mode << shift);
}


static void led_write(const LedItem *item, uint8_t on)
{
    if ((on != 0U) ^ (item->active_low != 0U))
    {
        item->port->BSRR = item->mask;
    }
    else
    {
        item->port->BSRR = (uint32_t)item->mask << 16;
    }
}


static void leds_off(void)
{
    uint32_t i;

    for (i = 0U; i < LED_COUNT; ++i)
    {
        led_write(&g_leds[i], 0U);
    }
}


static void delay_ms(uint32_t ms)
{
    volatile uint32_t i;

    while (ms != 0U)
    {
        for (i = 0U; i < 7200U; ++i)
        {
        }
        --ms;
    }
}

int main(void)
{
    uint32_t i;

    gpio_clock_enable();

    for (i = 0U; i < LED_COUNT; ++i)
    {
        led_pin_init(&g_leds[i]);
    }

    leds_off();

    while (1)
    {
        for (i = 0U; i < LED_COUNT; ++i)
        {
            leds_off();
            led_write(&g_leds[i], 1U);
            delay_ms(1000U);
        }
    }
}