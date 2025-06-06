#include <stdio.h>
#include <stdlib.h>
#include "../include/pfxr.h"

int main() {
    printf("PFXR URL Functions Demo\n");
    printf("======================\n\n");

    // Example 1: Create a sound from a URL
    printf("Example 1: Creating sound from URL\n");
    printf("----------------------------------\n");
    
    const char* example_url = "https://example.com/sound?fx=2,0.8,0,0.2,0,0.5,440,0,1,0,10,20,0,0,0,0,4000,0,100,50,0,0";
    printf("URL: %s\n", example_url);
    
    pfxr_sound_t* sound_from_url = pfxr_create_sound_from_url(example_url);
    if (sound_from_url) {
        printf("Successfully parsed sound from URL:\n");
        printf("  Wave Form: %d (Square)\n", sound_from_url->waveForm);
        printf("  Volume: %.2f\n", sound_from_url->volume);
        printf("  Frequency: %.1f Hz\n", sound_from_url->frequency);
        printf("  Sustain Time: %.2f s\n", sound_from_url->sustainTime);
        printf("  Vibrato Rate: %.1f\n", sound_from_url->vibratoRate);
        printf("  Vibrato Depth: %.1f\n", sound_from_url->vibratoDepth);
        
        // Generate WAV file from URL sound
        int result = pfxr_create_sound_from_config_to_file(sound_from_url, "sound_from_url.wav");
        if (result == 0) {
            printf("  Generated: sound_from_url.wav\n");
        }
        
        pfxr_free_sound_config(sound_from_url);
    } else {
        printf("Failed to parse sound from URL\n");
    }

    printf("\n");

    // Example 2: Create a URL from a sound configuration
    printf("Example 2: Creating URL from sound configuration\n");
    printf("-----------------------------------------------\n");
    
    // Create a custom laser-like sound
    pfxr_sound_t laser_sound = pfxr_get_default_sound();
    laser_sound.waveForm = PFXR_WAVE_SAWTOOTH;
    laser_sound.volume = 0.7f;
    laser_sound.frequency = 800.0f;
    laser_sound.pitchDelta = -400.0f;
    laser_sound.pitchDuration = 1.0f;
    laser_sound.sustainTime = 0.3f;
    laser_sound.decayTime = 0.2f;
    
    printf("Custom laser sound configuration:\n");
    printf("  Wave Form: %d (Sawtooth)\n", laser_sound.waveForm);
    printf("  Volume: %.2f\n", laser_sound.volume);
    printf("  Frequency: %.1f Hz\n", laser_sound.frequency);
    printf("  Pitch Delta: %.1f Hz\n", laser_sound.pitchDelta);
    
    char* url_from_sound = pfxr_get_url_from_sound(&laser_sound);
    if (url_from_sound) {
        printf("Generated URL parameter: %s\n", url_from_sound);
        printf("Full URL: https://myapp.com/sound%s\n", url_from_sound);
        
        // Generate WAV file from custom sound
        int result = pfxr_create_sound_from_config_to_file(&laser_sound, "custom_laser.wav");
        if (result == 0) {
            printf("Generated: custom_laser.wav\n");
        }
        
        free(url_from_sound);
    } else {
        printf("Failed to generate URL from sound\n");
    }

    printf("\n");

    // Example 3: Round-trip conversion
    printf("Example 3: Round-trip URL conversion\n");
    printf("-----------------------------------\n");
    
    // Start with a template
    pfxr_sound_t jump_sound = pfxr_apply_template(PFXR_TEMPLATE_JUMP, 42);
    printf("Original jump sound frequency: %.1f Hz\n", jump_sound.frequency);
    
    // Convert to URL
    char* jump_url = pfxr_get_url_from_sound(&jump_sound);
    if (jump_url) {
        printf("Jump sound URL: %s\n", jump_url);
        
        // Convert back from URL
        char full_url[1024];
        snprintf(full_url, sizeof(full_url), "https://game.com/sound%s", jump_url);
        
        pfxr_sound_t* restored_jump = pfxr_create_sound_from_url(full_url);
        if (restored_jump) {
            printf("Restored jump sound frequency: %.1f Hz\n", restored_jump->frequency);
            printf("Round-trip successful: %s\n", 
                   (jump_sound.frequency == restored_jump->frequency) ? "✓" : "✗");
            
            // Generate both sounds for comparison
            pfxr_create_sound_from_config_to_file(&jump_sound, "original_jump.wav");
            pfxr_create_sound_from_config_to_file(restored_jump, "restored_jump.wav");
            printf("Generated: original_jump.wav and restored_jump.wav\n");
            
            pfxr_free_sound_config(restored_jump);
        }
        
        free(jump_url);
    }

    printf("\n");

    // Example 4: Error handling
    printf("Example 4: Error handling\n");
    printf("------------------------\n");
    
    // Test with invalid URL
    pfxr_sound_t* invalid_sound = pfxr_create_sound_from_url("not-a-valid-url");
    printf("Invalid URL result: %s\n", invalid_sound ? "Got default sound" : "NULL");
    if (invalid_sound) {
        pfxr_free_sound_config(invalid_sound);
    }
    
    // Test with URL missing fx parameter
    pfxr_sound_t* no_fx_sound = pfxr_create_sound_from_url("https://example.com?other=param");
    printf("No fx parameter result: %s\n", no_fx_sound ? "Got default sound" : "NULL");
    if (no_fx_sound) {
        pfxr_free_sound_config(no_fx_sound);
    }

    printf("\nDemo complete! Check the generated WAV files.\n");
    
    return 0;
}