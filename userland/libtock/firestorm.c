#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <firestorm.h>
#include <tock.h>


static void delay_cb( __attribute__ ((unused)) int unused0,
                      __attribute__ ((unused)) int unused1,
                      __attribute__ ((unused)) int unused2,
                      void* ud) {
  *((bool*)ud) = true;
}

void delay_ts() {}

void delay_ms(uint32_t ms) {
  bool cond = false;
  timer_subscribe(delay_cb, &cond);
  timer_oneshot(ms);
  yield_for(&cond);
}
int spi_init() {return 0;}
int spi_set_chip_select(unsigned char cs) {return command(4, 2, cs);}
int spi_get_chip_select()                 {return command(4, 3, 0);}
int spi_set_rate(int rate)                {return command(4, 4, rate);}
int spi_get_rate()                        {return command(4, 5, 0);}
int spi_set_phase(bool phase)             {return command(4, 6, (unsigned char)phase);}
int spi_get_phase()                       {return command(4, 7, 0);}
int spi_set_polarity(bool pol)            {return command(4, 8, (unsigned char)pol);}
int spi_get_polarity()                    {return command(4, 9, 0);}
int spi_hold_low()                        {return command(4, 10, 0);}
int spi_release_low()                     {return command(4, 11, 0);}

int spi_write_byte(unsigned char byte) {
  return command(4, 0, byte);
}

int spi_read_buf(const char* str, size_t len) {
  return allow(4, 0, (void*)str, len);
}

static void spi_cb( __attribute__ ((unused)) int unused0,
                    __attribute__ ((unused)) int unused1,
                    __attribute__ ((unused)) int unused2,
                    __attribute__ ((unused)) void* ud) {
  *((bool*)ud) = true;
}

int spi_write(const char* str,
   	      size_t len,
	      subscribe_cb cb, bool* cond) {
  int err;
  err = allow(4, 1, (void*)str, len);
  if (err < 0 ) {
    return err;
  }
  err = subscribe(4, 0, cb, cond);
  if (err < 0 ) {
    return err;
  }
  return command(4, 1, len);
}

int spi_read_write(const char* write,
		   char* read,
		   size_t  len,
		   subscribe_cb cb, bool* cond) {

  int err = allow(4, 0, (void*)read, len);
  if (err < 0) {
    return err;
  }
  return spi_write(write, len, cb, cond);
}

int spi_write_sync(const char* write,
		   size_t  len) {
  bool cond = false;
  spi_write(write, len, spi_cb, &cond);
  yield_for(&cond);
  return 0;
}

int spi_read_write_sync(const char* write,
		        char* read,
		        size_t  len) {
  bool cond = false;
  int err = spi_read_write(write, read, len, spi_cb, &cond);
  if (err < 0) {
    return err;
  }
  yield_for(&cond);
  return 0;
}

void nrf51822_serialization_subscribe (subscribe_cb cb) {
  // get some callback love
  subscribe(5, 0, cb, NULL);
}

void nrf51822_serialization_setup_rx_buffer (char* rx, int rx_len) {
  // Pass the RX buffer for the UART module to use.
  allow(5, 0, rx, rx_len);
}

void nrf51822_serialization_write (char* tx, int tx_len) {
  // Pass in the TX buffer.
  allow(5, 1, tx, tx_len);

  // Do the write!!!!!
  command(5, 0, 0);
}
