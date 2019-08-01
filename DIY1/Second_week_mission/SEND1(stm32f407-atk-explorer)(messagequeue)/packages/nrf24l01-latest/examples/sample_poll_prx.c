#include <rtthread.h>
#include "nrf24l01.h"

#include "sample.h"


void sample_nrf24l01_task(void *param)
{
    struct hal_nrf24l01_port_cfg halcfg;
    nrf24_cfg_t cfg;
    int rlen;
    uint8_t rbuf[32 + 1];
    uint8_t tbuf[32] = "first\r\n";
    uint32_t cnt = 0;

    nrf24_default_param(&cfg);
    halcfg.ce_pin = NRF24L01_CE_PIN;
    halcfg.spi_device_name = NRF24L01_SPI_DEVICE;    
    cfg.role = ROLE_PRX;    // PRX
    cfg.ud = &halcfg;
    cfg.use_irq = 0;        // False    
    nrf24_init(&cfg);

    while (1) {
        // polling cycle
        rt_thread_mdelay(5);

        rlen = nrf24_prx_cycle(rbuf, tbuf, rt_strlen((char *)tbuf));
        if (rlen > 0) {       // received data (also indicating that the previous frame of data was sent complete)
            rbuf[rlen] = '\0';
            rt_kputs((char *)rbuf);
            rt_sprintf((char *)tbuf, "i-am-PRX:%dth\r\n", cnt);
            cnt++;
        }
        else {  // no data
            ;
        }
    }
}

static int nrf24l01_sample_init(void)
{
    rt_thread_t thread;

    thread = rt_thread_create("samNrfPRX", sample_nrf24l01_task, RT_NULL, 1024, RT_THREAD_PRIORITY_MAX/2, 20);
    rt_thread_startup(thread);

    return RT_EOK;
}

INIT_APP_EXPORT(nrf24l01_sample_init);
