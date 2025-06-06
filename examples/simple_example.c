#include <stdio.h>
#include <stdlib.h>
#include "../include/pfxr.h"

int main() {
    printf("PFXR C Library Example\n");
    printf("======================\n\n");
    
    // Test all templates
    pfxr_template_t templates[] = {
        PFXR_TEMPLATE_LASER,
        PFXR_TEMPLATE_PICKUP,
        PFXR_TEMPLATE_JUMP,
        PFXR_TEMPLATE_FALL,
        PFXR_TEMPLATE_POWERUP,
        PFXR_TEMPLATE_EXPLOSION,
        PFXR_TEMPLATE_BLIP,
        PFXR_TEMPLATE_HIT,
        PFXR_TEMPLATE_FART
    };
    
    const char* template_names[] = {
        "LASER",
        "PICKUP", 
        "JUMP",
        "FALL",
        "POWERUP",
        "EXPLOSION",
        "BLIP",
        "HIT",
        "FART"
    };
    
    int num_templates = sizeof(templates) / sizeof(templates[0]);
    
    // Generate sounds from templates
    for (int i = 0; i < num_templates; i++) {
        printf("Generating %s sound...\n", template_names[i]);
        
        // Create filename
        char filename[64];
        snprintf(filename, sizeof(filename), "%s_999.wav", template_names[i]);
        
        // Generate sound with seed 999
        int result = pfxr_create_sound_from_template_to_file(templates[i], 999, filename);
        
        if (result == 0) {
            printf("  ✓ Created %s\n", filename);
        } else {
            printf("  ✗ Failed to create %s\n", filename);
        }
    }
    
    printf("\n");
    
    // Example of creating sound from custom configuration
    printf("Creating custom sound...\n");
    pfxr_sound_t custom_sound = pfxr_get_default_sound();
    custom_sound.waveForm = PFXR_WAVE_SQUARE;
    custom_sound.frequency = 440.0f;  // A4 note
    custom_sound.sustainTime = 0.5f;
    custom_sound.decayTime = 0.2f;
    custom_sound.volume = 0.7f;
    
    int result = pfxr_create_sound_from_config_to_file(&custom_sound, "custom_beep.wav");
    if (result == 0) {
        printf("  ✓ Created custom_beep.wav\n");
    } else {
        printf("  ✗ Failed to create custom_beep.wav\n");
    }
    
    printf("\n");
    
    // Example of getting WAV data in memory instead of saving to file
    printf("Creating sound in memory...\n");
    char* wav_data = pfxr_create_sound_from_template(PFXR_TEMPLATE_BLIP, 12345);
    if (wav_data) {
        printf("  ✓ Generated WAV data in memory\n");
        printf("  WAV header starts with: %.4s\n", wav_data);
        
        // You could write this to a file manually if needed:
        FILE* file = fopen("memory_blip.wav", "wb");
        if (file) {
            // Get the file size from the WAV header
            pfxr_wav_header_t* header = (pfxr_wav_header_t*)wav_data;
            size_t file_size = header->chunk_size + 8;
            fwrite(wav_data, 1, file_size, file);
            fclose(file);
            printf("  ✓ Saved memory_blip.wav\n");
        }
        
        // Don't forget to free the memory
        pfxr_free_wav_data(wav_data);
    } else {
        printf("  ✗ Failed to generate WAV data\n");
    }
    
    printf("\nExample completed! Check the generated WAV files.\n");
    return 0;
}