#include "../include/pfxr.h"
#include <string.h>
#include <math.h>

// Default sound configuration (matches TypeScript defaultSound)
pfxr_sound_t pfxr_get_default_sound(void) {
    pfxr_sound_t sound;
    
    // Waveform and volume
    sound.waveForm = 0;  // SINE
    sound.volume = 0.5f;
    
    // Sound envelope
    sound.attackTime = 0.0f;
    sound.sustainTime = 0.07f;
    sound.sustainPunch = 0.0f;
    sound.decayTime = 0.3f;
    
    // Pitch
    sound.frequency = 700.0f;
    sound.pitchDelta = 0.0f;
    sound.pitchDuration = 1.0f;
    sound.pitchDelay = 0.0f;
    
    // Vibrato
    sound.vibratoRate = 0.0f;
    sound.vibratoDepth = 0.0f;
    
    // Tremolo
    sound.tremoloRate = 0.0f;
    sound.tremoloDepth = 0.0f;
    
    // Filters
    sound.highPassCutoff = 0.0f;
    sound.highPassResonance = 0.0f;
    sound.lowPassCutoff = 4000.0f;
    sound.lowPassResonance = 0.0f;
    
    // Phaser
    sound.phaserBaseFrequency = 100.0f;
    sound.phaserLfoFrequency = 50.0f;
    sound.phaserDepth = 0.0f;
    
    // Noise
    sound.noiseAmount = 0.0f;
    
    return sound;
}



// Apply template to generate sound configuration
pfxr_sound_t pfxr_apply_template(pfxr_template_t template, int seed) {
    pfxr_sound_t sound = pfxr_get_default_sound();
    pfxr_random_t rng;
    pfxr_random_init(&rng, (uint32_t)seed);
    
    switch (template) {
        case PFXR_TEMPLATE_DEFAULT:
            // No changes from default
            break;
            
        case PFXR_TEMPLATE_PICKUP: {
            int wave_choices[] = {0, 1, 2, 3};
            sound.waveForm = pfxr_random_choice(&rng, wave_choices, 4);
            sound.sustainPunch = pfxr_random_float(&rng, 0.0f, 0.8f);
            sound.sustainTime = pfxr_random_float(&rng, 0.05f, 0.2f);
            sound.decayTime = pfxr_random_float(&rng, 0.1f, 0.3f);
            sound.frequency = pfxr_random_float(&rng, 900.0f, 1700.0f);
            
            if (pfxr_random_bool(&rng, 0.5f)) {
                sound.pitchDelta = pfxr_random_float(&rng, 100.0f, 500.0f);
                sound.pitchDuration = 0.0f;
                sound.pitchDelay = pfxr_random_float(&rng, 0.0f, 0.7f);
            }
            break;
        }
        
        case PFXR_TEMPLATE_LASER: {
            int wave_choices[] = {0, 1, 2, 3};
            sound.waveForm = pfxr_random_choice(&rng, wave_choices, 4);
            sound.sustainPunch = pfxr_random_float(&rng, 0.0f, 0.8f);
            sound.sustainTime = pfxr_random_float(&rng, 0.05f, 0.1f);
            sound.decayTime = pfxr_random_float(&rng, 0.0f, 0.2f);
            sound.frequency = pfxr_random_float(&rng, 100.0f, 1300.0f);
            sound.pitchDelta = pfxr_random_float(&rng, -sound.frequency, -100.0f);
            sound.pitchDuration = 1.0f;
            
            float delay_choices[] = {0.0f, pfxr_random_float(&rng, 0.0f, 0.3f)};
            sound.pitchDelay = delay_choices[pfxr_random_bool(&rng, 0.5f)];
            break;
        }
        
        case PFXR_TEMPLATE_JUMP: {
            int wave_choices[] = {1, 2};
            sound.waveForm = pfxr_random_choice(&rng, wave_choices, 2);
            sound.sustainPunch = pfxr_random_float(&rng, 0.0f, 0.8f);
            sound.sustainTime = pfxr_random_float(&rng, 0.2f, 0.5f);
            sound.decayTime = pfxr_random_float(&rng, 0.1f, 0.2f);
            sound.frequency = pfxr_random_float(&rng, 100.0f, 500.0f);
            sound.pitchDelta = pfxr_random_float(&rng, 200.0f, 500.0f);
            sound.pitchDuration = 1.0f;
            
            float delay_choices[] = {0.0f, pfxr_random_float(&rng, 0.0f, 0.3f)};
            sound.pitchDelay = delay_choices[pfxr_random_bool(&rng, 0.5f)];
            break;
        }
        
        case PFXR_TEMPLATE_FALL: {
            int wave_choices[] = {1, 2, 3};
            sound.waveForm = pfxr_random_choice(&rng, wave_choices, 3);
            sound.sustainPunch = 0.0f;
            sound.sustainTime = pfxr_random_float(&rng, 0.2f, 0.5f);
            sound.decayTime = pfxr_random_float(&rng, 0.2f, 0.5f);
            sound.frequency = pfxr_random_float(&rng, 80.0f, 500.0f);
            sound.pitchDelta = -sound.frequency;
            sound.pitchDuration = 1.0f;
            sound.pitchDelay = pfxr_random_float(&rng, 0.0f, 0.2f);
            sound.vibratoRate = pfxr_random_float(&rng, 8.0f, 18.0f);
            sound.vibratoDepth = pfxr_random_float(&rng, 10.0f, 30.0f);
            sound.tremoloRate = pfxr_random_float(&rng, 5.0f, 18.0f);
            sound.tremoloDepth = pfxr_random_float(&rng, 0.0f, 1.0f);
            break;
        }
        
        case PFXR_TEMPLATE_POWERUP: {
            int wave_choices[] = {0, 1, 2, 3};
            sound.waveForm = pfxr_random_choice(&rng, wave_choices, 4);
            sound.sustainPunch = pfxr_random_float(&rng, 0.0f, 1.0f);
            sound.sustainTime = pfxr_random_float(&rng, 0.2f, 0.5f);
            sound.decayTime = pfxr_random_float(&rng, 0.1f, 0.5f);
            sound.frequency = pfxr_random_float(&rng, 200.0f, 1000.0f);
            sound.pitchDelta = pfxr_random_float(&rng, 100.0f, 300.0f);
            sound.pitchDuration = 1.0f;
            
            float delay_choices[] = {0.0f, pfxr_random_float(&rng, 0.0f, 0.3f)};
            sound.pitchDelay = delay_choices[pfxr_random_bool(&rng, 0.5f)];
            sound.vibratoRate = pfxr_random_float(&rng, 10.0f, 18.0f);
            sound.vibratoDepth = pfxr_random_float(&rng, 50.0f, 100.0f);
            break;
        }
        
        case PFXR_TEMPLATE_EXPLOSION: {
            int wave_choices[] = {0, 1, 2, 3};
            sound.waveForm = pfxr_random_choice(&rng, wave_choices, 4);
            sound.volume = 0.3f;
            sound.sustainPunch = pfxr_random_float(&rng, 0.0f, 0.3f);
            sound.sustainTime = pfxr_random_float(&rng, 0.4f, 1.3f);
            sound.decayTime = pfxr_random_float(&rng, 0.1f, 0.5f);
            sound.frequency = pfxr_random_float(&rng, 0.0f, 200.0f);
            sound.pitchDelta = -sound.frequency;
            sound.pitchDuration = 1.0f;
            sound.pitchDelay = pfxr_random_float(&rng, 0.0f, 0.3f);
            sound.vibratoRate = pfxr_random_float(&rng, 0.0f, 70.0f);
            sound.vibratoDepth = pfxr_random_float(&rng, 0.0f, 100.0f);
            sound.tremoloRate = pfxr_random_float(&rng, 0.0f, 70.0f);
            sound.tremoloDepth = pfxr_random_float(&rng, 0.0f, 1.0f);
            sound.phaserDepth = pfxr_random_float(&rng, 300.0f, 1000.0f);
            sound.noiseAmount = pfxr_random_float(&rng, 300.0f, 500.0f);
            break;
        }
        
        case PFXR_TEMPLATE_BLIP: {
            int wave_choices[] = {0, 1, 2, 3};
            sound.waveForm = pfxr_random_choice(&rng, wave_choices, 4);
            sound.sustainTime = pfxr_random_float(&rng, 0.02f, 0.1f);
            sound.decayTime = pfxr_random_float(&rng, 0.0f, 0.04f);
            sound.frequency = pfxr_random_float(&rng, 600.0f, 3000.0f);
            break;
        }
        
        case PFXR_TEMPLATE_HIT: {
            int wave_choices[] = {0, 1, 2, 3};
            sound.waveForm = pfxr_random_choice(&rng, wave_choices, 4);
            sound.sustainTime = pfxr_random_float(&rng, 0.01f, 0.03f);
            sound.sustainPunch = pfxr_random_float(&rng, 0.0f, 0.5f);
            sound.decayTime = pfxr_random_float(&rng, 0.0f, 0.2f);
            sound.frequency = pfxr_random_float(&rng, 20.0f, 500.0f);
            sound.pitchDelta = pfxr_random_float(&rng, -sound.frequency, -sound.frequency * 0.2f);
            sound.noiseAmount = pfxr_random_float(&rng, 0.0f, 100.0f);
            break;
        }
        
        case PFXR_TEMPLATE_FART: {
            sound.waveForm = 1; // SAWTOOTH
            sound.volume = 0.7f;
            sound.sustainPunch = pfxr_random_float(&rng, 0.0f, 0.2f);
            sound.sustainTime = pfxr_random_float(&rng, 0.1f, 0.5f);
            sound.decayTime = pfxr_random_float(&rng, 0.3f, 0.5f);
            sound.frequency = pfxr_random_float(&rng, 30.0f, 150.0f);
            sound.pitchDelta = -sound.frequency / 2.0f;
            sound.pitchDuration = 1.0f;
            sound.pitchDelay = 0.1f;
            sound.vibratoRate = pfxr_random_float(&rng, 8.0f, 18.0f);
            sound.vibratoDepth = pfxr_random_float(&rng, 10.0f, 30.0f);
            sound.tremoloRate = pfxr_random_float(&rng, 35.0f, 70.0f);
            sound.tremoloDepth = pfxr_random_float(&rng, 0.6f, 1.0f);
            sound.lowPassCutoff = sound.frequency * 10.0f;
            sound.lowPassResonance = 10.0f;
            sound.noiseAmount = pfxr_random_float(&rng, 0.0f, 30.0f);
            break;
        }
        
        case PFXR_TEMPLATE_RANDOM: {
            // Generate completely random parameters within valid ranges
            int wave_choices[] = {0, 1, 2, 3};
            sound.waveForm = pfxr_random_choice(&rng, wave_choices, 4);
            sound.volume = pfxr_random_float(&rng, 0.0f, 1.0f);
            sound.attackTime = pfxr_random_float(&rng, 0.0f, 2.0f);
            sound.sustainTime = pfxr_random_float(&rng, 0.0f, 2.0f);
            sound.sustainPunch = pfxr_random_float(&rng, 0.0f, 1.0f);
            sound.decayTime = pfxr_random_float(&rng, 0.0f, 2.0f);
            sound.frequency = pfxr_random_float(&rng, 0.0f, 4000.0f);
            sound.pitchDelta = pfxr_random_float(&rng, -4000.0f, 4000.0f);
            sound.pitchDuration = pfxr_random_float(&rng, 0.0f, 1.0f);
            sound.pitchDelay = pfxr_random_float(&rng, 0.0f, 1.0f);
            sound.vibratoRate = pfxr_random_float(&rng, 0.0f, 70.0f);
            sound.vibratoDepth = pfxr_random_float(&rng, 0.0f, 100.0f);
            sound.tremoloRate = pfxr_random_float(&rng, 0.0f, 70.0f);
            sound.tremoloDepth = pfxr_random_float(&rng, 0.0f, 1.0f);
            sound.highPassCutoff = pfxr_random_float(&rng, 0.0f, 4000.0f);
            sound.highPassResonance = pfxr_random_float(&rng, 0.0f, 30.0f);
            sound.lowPassCutoff = pfxr_random_float(&rng, 0.0f, 4000.0f);
            sound.lowPassResonance = pfxr_random_float(&rng, 0.0f, 30.0f);
            sound.phaserBaseFrequency = pfxr_random_float(&rng, 0.0f, 1000.0f);
            sound.phaserLfoFrequency = pfxr_random_float(&rng, 0.0f, 200.0f);
            sound.phaserDepth = pfxr_random_float(&rng, 0.0f, 1000.0f);
            sound.noiseAmount = pfxr_random_float(&rng, 0.0f, 500.0f);
            break;
        }
    }
    
    return sound;
}