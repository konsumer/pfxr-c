#define PFXR_IMPLEMENTATION
#include "../pfxr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Example function that matches the user's requested API
char* createSoundFromTemplate(pfxr_template_t template, int seed) {
    return pfxr_create_sound_from_template(template, seed);
}

// Helper function to save WAV data to file
int save_wav_to_file(char* wav_data, const char* filename) {
    if (!wav_data || !filename) return -1;
    
    // Get WAV file size from header
    pfxr_wav_header_t* header = (pfxr_wav_header_t*)wav_data;
    size_t file_size = header->chunk_size + 8;
    
    FILE* file = fopen(filename, "wb");
    if (!file) return -1;
    
    size_t written = fwrite(wav_data, 1, file_size, file);
    fclose(file);
    
    return (written == file_size) ? 0 : -1;
}

int main() {
    printf("PFXR C API Demo - Matching User's Requested Interface\n");
    printf("=====================================================\n\n");
    
    // Example 1: TypeScript equivalent - createSoundFromTemplate(TEMPLATES.LASER, 999)
    printf("1. Creating LASER sound with seed 999 (TypeScript equivalent)\n");
    char* laser_wav = createSoundFromTemplate(PFXR_TEMPLATE_LASER, 999);
    if (laser_wav) {
        printf("   ✓ Generated LASER sound WAV data in memory\n");
        printf("   ✓ WAV header signature: %.4s\n", laser_wav);
        
        // Save to file
        if (save_wav_to_file(laser_wav, "api_demo_laser_999.wav") == 0) {
            printf("   ✓ Saved to api_demo_laser_999.wav\n");
        }
        
        // Free the memory
        pfxr_free_wav_data(laser_wav);
    } else {
        printf("   ✗ Failed to generate LASER sound\n");
    }
    
    printf("\n");
    
    // Example 2: All template types with same seed
    printf("2. Generating all templates with seed 999\n");
    
    typedef struct {
        pfxr_template_t template;
        const char* name;
    } template_mapping_t;
    
    template_mapping_t templates[] = {
        {PFXR_TEMPLATE_LASER, "LASER"},
        {PFXR_TEMPLATE_PICKUP, "PICKUP"},
        {PFXR_TEMPLATE_JUMP, "JUMP"},
        {PFXR_TEMPLATE_FALL, "FALL"},
        {PFXR_TEMPLATE_POWERUP, "POWERUP"},
        {PFXR_TEMPLATE_EXPLOSION, "EXPLOSION"},
        {PFXR_TEMPLATE_BLIP, "BLIP"},
        {PFXR_TEMPLATE_HIT, "HIT"},
        {PFXR_TEMPLATE_FART, "FART"}
    };
    
    int template_count = sizeof(templates) / sizeof(templates[0]);
    
    for (int i = 0; i < template_count; i++) {
        char* wav_data = createSoundFromTemplate(templates[i].template, 999);
        if (wav_data) {
            char filename[64];
            snprintf(filename, sizeof(filename), "api_demo_%s_999.wav", templates[i].name);
            
            if (save_wav_to_file(wav_data, filename) == 0) {
                printf("   ✓ %s -> %s\n", templates[i].name, filename);
            } else {
                printf("   ✗ %s -> failed to save\n", templates[i].name);
            }
            
            pfxr_free_wav_data(wav_data);
        } else {
            printf("   ✗ %s -> failed to generate\n", templates[i].name);
        }
    }
    
    printf("\n");
    
    // Example 3: Different seeds for same template
    printf("3. LASER template with different seeds\n");
    int seeds[] = {1, 42, 123, 999, 12345};
    int seed_count = sizeof(seeds) / sizeof(seeds[0]);
    
    for (int i = 0; i < seed_count; i++) {
        char* wav_data = createSoundFromTemplate(PFXR_TEMPLATE_LASER, seeds[i]);
        if (wav_data) {
            char filename[64];
            snprintf(filename, sizeof(filename), "api_demo_LASER_%d.wav", seeds[i]);
            
            if (save_wav_to_file(wav_data, filename) == 0) {
                printf("   ✓ LASER seed %d -> %s\n", seeds[i], filename);
            }
            
            pfxr_free_wav_data(wav_data);
        }
    }
    
    printf("\n");
    
    // Example 4: Demonstrate deterministic behavior
    printf("4. Demonstrating deterministic behavior (same seed = same sound)\n");
    
    char* sound1 = createSoundFromTemplate(PFXR_TEMPLATE_EXPLOSION, 42);
    char* sound2 = createSoundFromTemplate(PFXR_TEMPLATE_EXPLOSION, 42);
    
    if (sound1 && sound2) {
        // Compare first few bytes after WAV header
        pfxr_wav_header_t* header1 = (pfxr_wav_header_t*)sound1;
        pfxr_wav_header_t* header2 = (pfxr_wav_header_t*)sound2;
        
        int16_t* samples1 = (int16_t*)(sound1 + sizeof(pfxr_wav_header_t));
        int16_t* samples2 = (int16_t*)(sound2 + sizeof(pfxr_wav_header_t));
        
        int samples_to_check = 100; // Check first 100 samples
        int identical = 1;
        
        for (int i = 0; i < samples_to_check; i++) {
            if (samples1[i] != samples2[i]) {
                identical = 0;
                break;
            }
        }
        
        if (identical && header1->data_size == header2->data_size) {
            printf("   ✓ Same seed produces identical sounds (verified)\n");
        } else {
            printf("   ✗ Same seed produced different sounds (unexpected!)\n");
        }
        
        pfxr_free_wav_data(sound1);
        pfxr_free_wav_data(sound2);
    }
    
    printf("\n");
    
    // Example 5: Show memory management pattern
    printf("5. Proper memory management pattern\n");
    printf("   char* wav_data = createSoundFromTemplate(PFXR_TEMPLATE_PICKUP, 999);\n");
    printf("   if (wav_data) {\n");
    printf("       // Use the WAV data...\n");
    printf("       pfxr_free_wav_data(wav_data);  // Always free!\n");
    printf("   }\n");
    
    printf("\n");
    
    // Example 6: Integration with direct PFXR API
    printf("6. Integration with direct PFXR API\n");
    
    // You can also use the direct API functions
    if (pfxr_create_sound_from_template_to_file(PFXR_TEMPLATE_RANDOM, 777, "api_demo_direct.wav") == 0) {
        printf("   ✓ Direct API: pfxr_create_sound_from_template_to_file() works\n");
    }
    
    // Custom configuration
    pfxr_sound_t custom = pfxr_get_default_sound();
    custom.waveForm = PFXR_WAVE_TRIANGLE;
    custom.frequency = 523.25f; // C5 note
    custom.sustainTime = 0.8f;
    custom.decayTime = 0.2f;
    custom.vibratoRate = 6.0f;
    custom.vibratoDepth = 25.0f;
    
    char* custom_wav = pfxr_create_sound_from_config(&custom);
    if (custom_wav) {
        save_wav_to_file(custom_wav, "api_demo_custom_note.wav");
        printf("   ✓ Custom configuration: C5 note with vibrato\n");
        pfxr_free_wav_data(custom_wav);
    }
    
    printf("\n=====================================================\n");
    printf("API Demo completed!\n");
    printf("Check the generated WAV files to hear the results.\n");
    printf("\nKey points:\n");
    printf("- createSoundFromTemplate() returns WAV file bytes in memory\n");
    printf("- Same seed always produces identical sounds\n");
    printf("- Always call pfxr_free_wav_data() to prevent memory leaks\n");
    printf("- WAV data can be saved to file or used directly\n");
    
    return 0;
}