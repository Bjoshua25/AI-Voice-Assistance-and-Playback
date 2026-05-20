#include <Arduino.h>
#include <driver/i2s.h>

#define I2S_SCK  14
#define I2S_WS   33
#define I2S_SD   32
#define I2S_PORT I2S_NUM_0

void setup() {
  Serial.begin(115200);
  
  const i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 16000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 128,
    .use_apll = false
  };

  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK, .ws_io_num = I2S_WS,
    .data_out_num = -1, .data_in_num = I2S_SD
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_PORT, &pin_config);
  Serial.println("Mic Test Starting... Speak into the mic!");
}

void loop() {
  int16_t buffer[64];
  size_t bytes_read;
  i2s_read(I2S_PORT, &buffer, sizeof(buffer), &bytes_read, portMAX_DELAY);

  int samples_read = bytes_read / 2;
  float mean = 0;
  for (int i = 0; i < samples_read; i++) {
    mean += abs(buffer[i]); // Calculate average loudness
  }
  mean /= samples_read;

  // Print a visual bar based on loudness
  Serial.print("Volume: ");
  for (int i = 0; i < (mean / 100); i++) Serial.print("=");
  Serial.println();
  delay(10);
}