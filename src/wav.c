#include "../include/pfxr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Create WAV data from float samples
char* pfxr_create_wav_data(const float* samples, int sample_count, int* wav_size) {
    if (!samples || sample_count <= 0 || !wav_size) {
        return NULL;
    }
    
    // Calculate sizes
    int data_size = sample_count * sizeof(int16_t);
    int file_size = sizeof(pfxr_wav_header_t) + data_size;
    
    // Allocate memory for WAV data
    char* wav_data = malloc(file_size);
    if (!wav_data) {
        return NULL;
    }
    
    // Create WAV header
    pfxr_wav_header_t* header = (pfxr_wav_header_t*)wav_data;
    
    // RIFF header
    memcpy(header->riff, "RIFF", 4);
    header->chunk_size = file_size - 8;
    memcpy(header->wave, "WAVE", 4);
    
    // Format chunk
    memcpy(header->fmt, "fmt ", 4);
    header->fmt_size = 16;
    header->audio_format = 1;  // PCM
    header->num_channels = 1;  // Mono
    header->sample_rate = PFXR_SAMPLE_RATE;
    header->bits_per_sample = 16;
    header->block_align = header->num_channels * header->bits_per_sample / 8;
    header->byte_rate = header->sample_rate * header->block_align;
    
    // Data chunk
    memcpy(header->data, "data", 4);
    header->data_size = data_size;
    
    // Convert float samples to 16-bit PCM
    int16_t* pcm_data = (int16_t*)(wav_data + sizeof(pfxr_wav_header_t));
    for (int i = 0; i < sample_count; i++) {
        // Clamp and scale to 16-bit range
        float sample = samples[i];
        if (sample > 1.0f) sample = 1.0f;
        if (sample < -1.0f) sample = -1.0f;
        
        pcm_data[i] = (int16_t)(sample * 32767.0f);
    }
    
    *wav_size = file_size;
    return wav_data;
}

// Write WAV file to disk
int pfxr_write_wav_file(const char* filename, const float* samples, int sample_count) {
    if (!filename || !samples || sample_count <= 0) {
        return -1;
    }
    
    int wav_size;
    char* wav_data = pfxr_create_wav_data(samples, sample_count, &wav_size);
    if (!wav_data) {
        return -1;
    }
    
    FILE* file = fopen(filename, "wb");
    if (!file) {
        free(wav_data);
        return -1;
    }
    
    size_t written = fwrite(wav_data, 1, wav_size, file);
    fclose(file);
    free(wav_data);
    
    return (written == (size_t)wav_size) ? 0 : -1;
}

// Free WAV data allocated by pfxr_create_wav_data
void pfxr_free_wav_data(char* wav_data) {
    if (wav_data) {
        free(wav_data);
    }
}