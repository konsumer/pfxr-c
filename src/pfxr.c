#include "../include/pfxr.h"
#include <stdlib.h>
#include <stdio.h>

// Main API function to create sound from template and return WAV data
char* pfxr_create_sound_from_template(pfxr_template_t template, int seed) {
    // Apply template to get sound configuration
    pfxr_sound_t config = pfxr_apply_template(template, seed);
    
    // Generate sound from configuration
    return pfxr_create_sound_from_config(&config);
}

// Create sound from template and save to file
int pfxr_create_sound_from_template_to_file(pfxr_template_t template, int seed, const char* filename) {
    if (!filename) {
        return -1;
    }
    
    // Apply template to get sound configuration
    pfxr_sound_t config = pfxr_apply_template(template, seed);
    
    // Generate sound from configuration and save to file
    return pfxr_create_sound_from_config_to_file(&config, filename);
}

// Create sound from configuration and return WAV data
char* pfxr_create_sound_from_config(const pfxr_sound_t* config) {
    if (!config) {
        return NULL;
    }
    
    // Create audio buffer
    pfxr_audio_buffer_t* buffer = pfxr_create_audio_buffer(PFXR_MAX_SAMPLES);
    if (!buffer) {
        return NULL;
    }
    
    // Generate sound
    pfxr_generate_sound(config, buffer);
    
    // Convert to WAV data
    int wav_size;
    char* wav_data = pfxr_create_wav_data(buffer->samples, buffer->sample_count, &wav_size);
    
    // Clean up
    pfxr_free_audio_buffer(buffer);
    
    return wav_data;
}

// Create sound from configuration and save to file
int pfxr_create_sound_from_config_to_file(const pfxr_sound_t* config, const char* filename) {
    if (!config || !filename) {
        return -1;
    }
    
    // Create audio buffer
    pfxr_audio_buffer_t* buffer = pfxr_create_audio_buffer(PFXR_MAX_SAMPLES);
    if (!buffer) {
        return -1;
    }
    
    // Generate sound
    pfxr_generate_sound(config, buffer);
    
    // Write to file
    int result = pfxr_write_wav_file(filename, buffer->samples, buffer->sample_count);
    
    // Clean up
    pfxr_free_audio_buffer(buffer);
    
    return result;
}