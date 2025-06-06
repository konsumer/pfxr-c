#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/pfxr.h"

void print_sound_config(const pfxr_sound_t* config, const char* name) {
    printf("\n%s Configuration:\n", name);
    printf("  Wave Form: %d\n", config->waveForm);
    printf("  Volume: %.2f\n", config->volume);
    printf("  Frequency: %.1fHz\n", config->frequency);
    printf("  Attack: %.3fs, Sustain: %.3fs, Decay: %.3fs\n", 
           config->attackTime, config->sustainTime, config->decayTime);
    printf("  Pitch Delta: %.1fHz, Duration: %.2f, Delay: %.2f\n",
           config->pitchDelta, config->pitchDuration, config->pitchDelay);
    
    if (config->vibratoRate > 0 || config->vibratoDepth > 0) {
        printf("  Vibrato: Rate=%.1f, Depth=%.1f\n", config->vibratoRate, config->vibratoDepth);
    }
    
    if (config->tremoloRate > 0 || config->tremoloDepth > 0) {
        printf("  Tremolo: Rate=%.1f, Depth=%.2f\n", config->tremoloRate, config->tremoloDepth);
    }
    
    if (config->noiseAmount > 0) {
        printf("  Noise Amount: %.1f\n", config->noiseAmount);
    }
    
    if (config->phaserDepth > 0) {
        printf("  Phaser: Base=%.1fHz, LFO=%.1fHz, Depth=%.1f\n",
               config->phaserBaseFrequency, config->phaserLfoFrequency, config->phaserDepth);
    }
}

void test_template_variations(pfxr_template_t template, const char* name) {
    printf("\n==================================================\n");
    printf("Testing %s Template with Different Seeds\n", name);
    printf("==================================================\n");
    
    // Test same template with different seeds
    for (int seed = 1; seed <= 5; seed++) {
        pfxr_sound_t config = pfxr_apply_template(template, seed);
        
        char filename[128];
        snprintf(filename, sizeof(filename), "%s_seed_%d.wav", name, seed);
        
        printf("\nSeed %d:\n", seed);
        int result = pfxr_create_sound_from_config_to_file(&config, filename);
        
        if (result == 0) {
            printf("  ✓ Created %s\n", filename);
            printf("  Frequency: %.1fHz, Duration: %.2fs\n", 
                   config.frequency, config.sustainTime + config.decayTime);
        } else {
            printf("  ✗ Failed to create %s\n", filename);
        }
    }
}

void create_template_comparison() {
    printf("\n============================================================\n");
    printf("Creating Template Comparison with Fixed Seed (999)\n");
    printf("============================================================\n");
    
    typedef struct {
        pfxr_template_t template;
        const char* name;
        const char* description;
    } template_info_t;
    
    template_info_t templates[] = {
        {PFXR_TEMPLATE_LASER, "LASER", "Sci-fi laser/zap sound"},
        {PFXR_TEMPLATE_PICKUP, "PICKUP", "Item collection sound"},
        {PFXR_TEMPLATE_JUMP, "JUMP", "Character jump sound"},
        {PFXR_TEMPLATE_FALL, "FALL", "Falling/descending sound"},
        {PFXR_TEMPLATE_POWERUP, "POWERUP", "Power-up/level-up sound"},
        {PFXR_TEMPLATE_EXPLOSION, "EXPLOSION", "Explosion/impact sound"},
        {PFXR_TEMPLATE_BLIP, "BLIP", "Short beep/blip sound"},
        {PFXR_TEMPLATE_HIT, "HIT", "Attack/hit sound"},
        {PFXR_TEMPLATE_FART, "FART", "Comedic fart sound"},
        {PFXR_TEMPLATE_RANDOM, "RANDOM", "Completely random parameters"}
    };
    
    int num_templates = sizeof(templates) / sizeof(templates[0]);
    
    for (int i = 0; i < num_templates; i++) {
        printf("\n%d. %s - %s\n", i+1, templates[i].name, templates[i].description);
        
        pfxr_sound_t config = pfxr_apply_template(templates[i].template, 999);
        print_sound_config(&config, templates[i].name);
        
        char filename[128];
        snprintf(filename, sizeof(filename), "comparison_%s.wav", templates[i].name);
        
        int result = pfxr_create_sound_from_config_to_file(&config, filename);
        printf("  File: %s %s\n", filename, result == 0 ? "✓" : "✗");
    }
}

void test_manual_configurations() {
    printf("\n==================================================\n");
    printf("Testing Manual Sound Configurations\n");
    printf("==================================================\n");
    
    // Test 1: Pure sine wave beep
    printf("\n1. Pure Sine Wave Beep (440Hz)\n");
    pfxr_sound_t sine_beep = pfxr_get_default_sound();
    sine_beep.waveForm = PFXR_WAVE_SINE;
    sine_beep.frequency = 440.0f;
    sine_beep.sustainTime = 0.5f;
    sine_beep.decayTime = 0.1f;
    sine_beep.volume = 0.6f;
    
    pfxr_create_sound_from_config_to_file(&sine_beep, "manual_sine_beep.wav");
    print_sound_config(&sine_beep, "Sine Beep");
    
    // Test 2: Square wave with vibrato
    printf("\n2. Square Wave with Vibrato\n");
    pfxr_sound_t square_vibrato = pfxr_get_default_sound();
    square_vibrato.waveForm = PFXR_WAVE_SQUARE;
    square_vibrato.frequency = 220.0f;
    square_vibrato.sustainTime = 1.0f;
    square_vibrato.decayTime = 0.2f;
    square_vibrato.vibratoRate = 5.0f;
    square_vibrato.vibratoDepth = 20.0f;
    square_vibrato.volume = 0.5f;
    
    pfxr_create_sound_from_config_to_file(&square_vibrato, "manual_square_vibrato.wav");
    print_sound_config(&square_vibrato, "Square Vibrato");
    
    // Test 3: Sawtooth with pitch sweep
    printf("\n3. Sawtooth with Pitch Sweep\n");
    pfxr_sound_t sawtooth_sweep = pfxr_get_default_sound();
    sawtooth_sweep.waveForm = PFXR_WAVE_SAWTOOTH;
    sawtooth_sweep.frequency = 800.0f;
    sawtooth_sweep.pitchDelta = -600.0f;
    sawtooth_sweep.pitchDuration = 1.0f;
    sawtooth_sweep.sustainTime = 0.8f;
    sawtooth_sweep.decayTime = 0.3f;
    sawtooth_sweep.volume = 0.7f;
    
    pfxr_create_sound_from_config_to_file(&sawtooth_sweep, "manual_sawtooth_sweep.wav");
    print_sound_config(&sawtooth_sweep, "Sawtooth Sweep");
    
    // Test 4: Triangle wave with filters
    printf("\n4. Triangle Wave with Filters\n");
    pfxr_sound_t triangle_filtered = pfxr_get_default_sound();
    triangle_filtered.waveForm = PFXR_WAVE_TRIANGLE;
    triangle_filtered.frequency = 300.0f;
    triangle_filtered.sustainTime = 0.6f;
    triangle_filtered.decayTime = 0.4f;
    triangle_filtered.lowPassCutoff = 1000.0f;
    triangle_filtered.lowPassResonance = 5.0f;
    triangle_filtered.volume = 0.8f;
    
    pfxr_create_sound_from_config_to_file(&triangle_filtered, "manual_triangle_filtered.wav");
    print_sound_config(&triangle_filtered, "Triangle Filtered");
}

int main() {
    printf("PFXR C Library - Comprehensive Template Test\n");
    printf("============================================\n");
    
    // Create template comparison
    create_template_comparison();
    
    // Test template variations
    test_template_variations(PFXR_TEMPLATE_LASER, "LASER");
    test_template_variations(PFXR_TEMPLATE_EXPLOSION, "EXPLOSION");
    
    // Test manual configurations
    test_manual_configurations();
    
    printf("\n============================================================\n");
    printf("All tests completed!\n");
    printf("Check the generated WAV files to hear the results.\n");
    printf("============================================================\n");
    
    return 0;
}