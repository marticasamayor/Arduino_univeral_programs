/*
 * ESP32-C3 -> MAX98357A (I2S class-D amp)
 * Generates a sine tone over I2S.  LEGACY driver -> ESP32 Arduino core 2.x
 * (also compiles on core 3.x with a deprecation warning).
 *
 *   Board:  "ESP32C3 Dev Module"
 *
 * Wiring:
 *   ESP32-C3 GPIO1  -> DIN    (SD, serial data)
 *   ESP32-C3 GPIO4  -> BCLK   (bit clock)
 *   ESP32-C3 GPIO5  -> LRCLK  (LRC, word/left-right select)
 *   3V3 (or 5V)     -> VIN
 *   GND             -> GND
 *   (MAX98357A GAIN floating = 9 dB, SD floating/high = enabled)
 */

#include <driver/i2s.h>
#include <math.h>

// ---- Pins ----
static const int PIN_DIN   = 1;   // data out
static const int PIN_BCLK  = 4;   // bit clock
static const int PIN_LRCLK = 5;   // word select

// ---- Audio config ----
static const uint32_t SAMPLE_RATE = 44100;   // Hz
static const i2s_port_t I2S_PORT  = I2S_NUM_0;

// ---- Tone config ----
float    toneFreq  = 440.0f;   // Hz (A4)
float    volume    = 0.8f;     // 0.0 .. 1.0
uint32_t toneMs    = 500;      // note length
uint32_t silenceMs = 500;      // gap

// Write `ms` of a sine at `freq`. amp = 0..1
static void playTone(float freq, uint32_t ms, float amp) {
  const uint32_t totalSamples = (uint64_t)SAMPLE_RATE * ms / 1000;
  const float    step = 2.0f * (float)M_PI * freq / (float)SAMPLE_RATE;
  const int16_t  peak = (int16_t)(amp * 32767.0f);
  float phase = 0.0f;
  size_t written;

  for (uint32_t i = 0; i < totalSamples; i++) {
    int16_t s = (int16_t)(sinf(phase) * peak);
    phase += step;
    if (phase >= 2.0f * (float)M_PI) phase -= 2.0f * (float)M_PI;

    // MAX98357A is mono (averages L+R): send same sample to both slots.
    int16_t frame[2] = { s, s };
    i2s_write(I2S_PORT, frame, sizeof(frame), &written, portMAX_DELAY);
  }
}

static void playSilence(uint32_t ms) {
  const uint32_t totalSamples = (uint64_t)SAMPLE_RATE * ms / 1000;
  int16_t frame[2] = { 0, 0 };
  size_t written;
  for (uint32_t i = 0; i < totalSamples; i++)
    i2s_write(I2S_PORT, frame, sizeof(frame), &written, portMAX_DELAY);
}

void setup() {
  Serial.begin(115200);

  i2s_config_t cfg = {
    .mode                 = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate          = SAMPLE_RATE,
    .bits_per_sample      = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format       = I2S_CHANNEL_FMT_RIGHT_LEFT,   // stereo frames
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags     = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count        = 8,
    .dma_buf_len          = 256,
    .use_apll             = false,
    .tx_desc_auto_clear   = true,
    .fixed_mclk           = 0
  };

  i2s_pin_config_t pins = {
    .mck_io_num   = I2S_PIN_NO_CHANGE,
    .bck_io_num   = PIN_BCLK,
    .ws_io_num    = PIN_LRCLK,
    .data_out_num = PIN_DIN,
    .data_in_num  = I2S_PIN_NO_CHANGE
  };

  if (i2s_driver_install(I2S_PORT, &cfg, 0, NULL) != ESP_OK) {
    Serial.println("i2s_driver_install failed!");
    while (true) delay(1000);
  }
  i2s_set_pin(I2S_PORT, &pins);
  i2s_zero_dma_buffer(I2S_PORT);
  Serial.println("I2S ready.");
}

void loop() {
  playTone(toneFreq, toneMs, volume);
  playSilence(silenceMs);
}