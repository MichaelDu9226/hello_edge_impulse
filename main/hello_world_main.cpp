#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string>
#include "sdkconfig.h"
#include "Audio_test_0218_inferencing.h"

#include "esp_log.h"

typedef struct {
    int16_t *buffer;
    uint8_t buf_ready;
    uint32_t buf_count;
    uint32_t n_samples;
} inference_t;

static inference_t inference;
static signed short sampleBuffer[2048];
static bool debug_nn = false; // Set this to true to see e.g. features generated from the raw signal
void ei_printf(const char *format, ...);

static bool microphone_inference_start(uint32_t n_samples);
static int microphone_audio_signal_get_data(size_t offset, size_t length, float *out_ptr);
extern "C"
{
    void app_main(void)
    {
        if (microphone_inference_start(EI_CLASSIFIER_RAW_SAMPLE_COUNT) == false)
        {
            //ei_printf("ERR: Failed to setup audio sampling\r\n");
            return;
        }
        signal_t signal;
        signal.total_length = EI_CLASSIFIER_RAW_SAMPLE_COUNT;
        signal.get_data = &microphone_audio_signal_get_data;
        ei_impulse_result_t result = { 0 };
        EI_IMPULSE_ERROR r = run_classifier(&signal, &result, debug_nn);
    }
}

void ei_printf(const char *format, ...) {
    // static char print_buf[1024] = { 0 };

    // va_list args;
    // va_start(args, format);
    // int r = vsnprintf(print_buf, sizeof(print_buf), format, args);
    // va_end(args);

    // if (r > 0) {
    //     Serial.write(print_buf);
    // }
}

/**
 * @brief      Init inferencing struct and setup/start PDM
 *
 * @param[in]  n_samples  The n samples
 *
 * @return     { description_of_the_return_value }
 */
static bool microphone_inference_start(uint32_t n_samples)
{
    inference.buffer = (int16_t *)malloc(n_samples * sizeof(int16_t));

    if(inference.buffer == NULL) {
        return false;
    }

    inference.buf_count  = 0;
    inference.n_samples  = n_samples;
    inference.buf_ready  = 0;

    // configure the data receive callback
    //PDM.onReceive(&pdm_data_ready_inference_callback);

    //PDM.setBufferSize(4096);

    // initialize PDM with:
    // - one channel (mono mode)
    // - a 16 kHz sample rate
    // if (!PDM.begin(1, EI_CLASSIFIER_FREQUENCY)) {
    //     ei_printf("Failed to start PDM!");
    //     microphone_inference_end();

    //     return false;
    // }

    // set the gain, defaults to 20
    //PDM.setGain(127);

    return true;
}

/**
 * Get raw audio signal data
 */
static int microphone_audio_signal_get_data(size_t offset, size_t length, float *out_ptr)
{
    numpy::int16_to_float(&inference.buffer[offset], out_ptr, length);

    return 0;
}