#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/pfxr.h"

void print_sound_config(const pfxr_sound_t* config, const char* name) {
    printf("\n%s Configuration:\n", name);
    printf("  waveForm: %d\n", config->waveForm);
    printf("  volume: %.3f\n", config->volume);
    printf("  attackTime: %.3f\n", config->attackTime);
    printf("  sustainTime: %.3f\n", config->sustainTime);
    printf("  sustainPunch: %.3f\n", config->sustainPunch);
    printf("  decayTime: %.3f\n", config->decayTime);
    printf("  frequency: %.1f\n", config->frequency);
    printf("  pitchDelta: %.1f\n", config->pitchDelta);
    printf("  pitchDuration: %.3f\n", config->pitchDuration);
    printf("  pitchDelay: %.3f\n", config->pitchDelay);
    printf("  vibratoRate: %.1f\n", config->vibratoRate);
    printf("  vibratoDepth: %.1f\n", config->vibratoDepth);
    printf("  tremoloRate: %.1f\n", config->tremoloRate);
    printf("  tremoloDepth: %.3f\n", config->tremoloDepth);
    printf("  highPassCutoff: %.1f\n", config->highPassCutoff);
    printf("  highPassResonance: %.1f\n", config->highPassResonance);
    printf("  lowPassCutoff: %.1f\n", config->lowPassCutoff);
    printf("  lowPassResonance: %.1f\n", config->lowPassResonance);
    printf("  phaserBaseFrequency: %.1f\n", config->phaserBaseFrequency);
    printf("  phaserLfoFrequency: %.1f\n", config->phaserLfoFrequency);
    printf("  phaserDepth: %.1f\n", config->phaserDepth);
    printf("  noiseAmount: %.1f\n", config->noiseAmount);
}

void test_url_roundtrip() {
    printf("==================================================\n");
    printf("Testing URL Round-trip Conversion\n");
    printf("==================================================\n");

    // Create a custom sound configuration
    pfxr_sound_t original = pfxr_get_default_sound();
    original.waveForm = PFXR_WAVE_SQUARE;
    original.volume = 0.8f;
    original.frequency = 440.0f;
    original.sustainTime = 0.2f;
    original.decayTime = 0.5f;
    original.vibratoRate = 10.0f;
    original.vibratoDepth = 20.0f;

    print_sound_config(&original, "Original");

    // Convert to URL
    char* url = pfxr_get_url_from_params(&original);
    if (!url) {
        printf("ERROR: Failed to create URL from sound\n");
        return;
    }
    printf("\nGenerated URL: %s\n", url);

    // Create a full URL for testing
    char full_url[1024];
    snprintf(full_url, sizeof(full_url), "https://example.com/sound%s", url);
    printf("Full URL: %s\n", full_url);

    // Convert back from URL
    pfxr_sound_t* restored = pfxr_create_params_from_url(full_url);
    if (!restored) {
        printf("ERROR: Failed to create sound from URL\n");
        free(url);
        return;
    }

    print_sound_config(restored, "Restored");

    // Compare values
    printf("\nComparison:\n");
    printf("  waveForm: %d -> %d %s\n", original.waveForm, restored->waveForm,
           original.waveForm == restored->waveForm ? "✓" : "✗");
    printf("  volume: %.3f -> %.3f %s\n", original.volume, restored->volume,
           (original.volume - restored->volume) < 0.001f ? "✓" : "✗");
    printf("  frequency: %.1f -> %.1f %s\n", original.frequency, restored->frequency,
           (original.frequency - restored->frequency) < 0.1f ? "✓" : "✗");
    printf("  vibratoRate: %.1f -> %.1f %s\n", original.vibratoRate, restored->vibratoRate,
           (original.vibratoRate - restored->vibratoRate) < 0.1f ? "✓" : "✗");

    // Clean up
    free(url);
    pfxr_free_sound_config(restored);
}

void test_template_urls() {
    printf("\n==================================================\n");
    printf("Testing Template URL Generation\n");
    printf("==================================================\n");

    pfxr_template_t templates[] = {
        PFXR_TEMPLATE_PICKUP,
        PFXR_TEMPLATE_LASER,
        PFXR_TEMPLATE_JUMP,
        PFXR_TEMPLATE_EXPLOSION
    };

    const char* template_names[] = {
        "PICKUP",
        "LASER",
        "JUMP",
        "EXPLOSION"
    };

    for (int i = 0; i < 4; i++) {
        printf("\n%s Template:\n", template_names[i]);

        pfxr_sound_t config = pfxr_apply_template(templates[i], 12345);
        char* url = pfxr_get_url_from_params(&config);

        if (url) {
            printf("  URL: %s\n", url);

            // Test parsing it back
            char full_url[1024];
            snprintf(full_url, sizeof(full_url), "https://example.com/sound%s", url);
            pfxr_sound_t* parsed = pfxr_create_params_from_url(full_url);

            if (parsed) {
                printf("  Parse test: ✓ (waveForm=%d, freq=%.1f)\n",
                       parsed->waveForm, parsed->frequency);
                pfxr_free_sound_config(parsed);
            } else {
                printf("  Parse test: ✗\n");
            }

            free(url);
        } else {
            printf("  ERROR: Failed to generate URL\n");
        }
    }
}

void test_edge_cases() {
    printf("\n==================================================\n");
    printf("Testing Edge Cases\n");
    printf("==================================================\n");

    // Test NULL inputs
    printf("Testing NULL inputs:\n");
    pfxr_sound_t* null_sound = pfxr_create_params_from_url(NULL);
    printf("  NULL URL: %s\n", null_sound ? "✗ (should be NULL)" : "✓");

    char* null_url = pfxr_get_url_from_params(NULL);
    printf("  NULL config: %s\n", null_url ? "✗ (should be NULL)" : "✓");

    // Test URL without fx parameter
    printf("\nTesting URL without fx parameter:\n");
    pfxr_sound_t* default_sound = pfxr_create_params_from_url("https://example.com/test?other=value");
    if (default_sound) {
        printf("  Returns default sound: ✓ (waveForm=%d, freq=%.1f)\n",
               default_sound->waveForm, default_sound->frequency);
        pfxr_free_sound_config(default_sound);
    } else {
        printf("  Returns default sound: ✗\n");
    }

    // Test malformed fx parameter
    printf("\nTesting malformed fx parameter:\n");
    pfxr_sound_t* malformed_sound = pfxr_create_params_from_url("https://example.com/test?fx=abc,def,123");
    if (malformed_sound) {
        printf("  Handles malformed data: ✓ (waveForm=%d, freq=%.1f)\n",
               malformed_sound->waveForm, malformed_sound->frequency);
        pfxr_free_sound_config(malformed_sound);
    } else {
        printf("  Handles malformed data: ✗\n");
    }

    // Test URL encoded characters
    printf("\nTesting URL encoded characters:\n");
    pfxr_sound_t* encoded_sound = pfxr_create_params_from_url("https://example.com/test?fx=2%2C0.5%2C0%2C0.07");
    if (encoded_sound) {
        printf("  URL decoding: ✓ (waveForm=%d, volume=%.2f)\n",
               encoded_sound->waveForm, encoded_sound->volume);
        pfxr_free_sound_config(encoded_sound);
    } else {
        printf("  URL decoding: ✗\n");
    }
}

int main() {
    printf("PFXR URL Function Test\n");
    printf("=====================\n");

    test_url_roundtrip();
    test_template_urls();
    test_edge_cases();

    printf("\n==================================================\n");
    printf("URL Function Tests Complete\n");
    printf("==================================================\n");

    return 0;
}
