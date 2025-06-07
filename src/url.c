#include "../include/pfxr.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

// Helper function to URL decode a string
static char* url_decode(const char* encoded) {
    if (!encoded) return NULL;

    size_t len = strlen(encoded);
    char* decoded = malloc(len + 1);
    if (!decoded) return NULL;

    size_t i = 0, j = 0;
    while (i < len) {
        if (encoded[i] == '%' && i + 2 < len) {
            // Convert hex to char
            char hex[3] = {encoded[i+1], encoded[i+2], '\0'};
            char* endptr;
            long val = strtol(hex, &endptr, 16);
            if (*endptr == '\0') {
                decoded[j++] = (char)val;
                i += 3;
            } else {
                decoded[j++] = encoded[i++];
            }
        } else if (encoded[i] == '+') {
            decoded[j++] = ' ';
            i++;
        } else {
            decoded[j++] = encoded[i++];
        }
    }
    decoded[j] = '\0';
    return decoded;
}

// Helper function to URL encode a string
static char* url_encode(const char* str) {
    if (!str) return NULL;

    size_t len = strlen(str);
    // Worst case: every character needs encoding (3 chars each)
    char* encoded = malloc(len * 3 + 1);
    if (!encoded) return NULL;

    size_t j = 0;
    for (size_t i = 0; i < len; i++) {
        char c = str[i];
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded[j++] = c;
        } else if (c == ' ') {
            encoded[j++] = '+';
        } else {
            sprintf(&encoded[j], "%%%02X", (unsigned char)c);
            j += 3;
        }
    }
    encoded[j] = '\0';
    return encoded;
}

// Helper function to find query parameter value
static char* get_query_param(const char* url, const char* param) {
    if (!url || !param) return NULL;

    // Find the start of query string
    const char* query_start = strchr(url, '?');
    if (!query_start) return NULL;
    query_start++; // Skip the '?'

    // Look for the parameter
    char param_with_eq[256];
    snprintf(param_with_eq, sizeof(param_with_eq), "%s=", param);

    const char* param_pos = strstr(query_start, param_with_eq);
    if (!param_pos) return NULL;

    // Move to start of value
    const char* value_start = param_pos + strlen(param_with_eq);

    // Find end of value (next & or end of string)
    const char* value_end = value_start;
    while (*value_end && *value_end != '&' && *value_end != '#') {
        value_end++;
    }

    // Copy the value
    size_t value_len = value_end - value_start;
    char* value = malloc(value_len + 1);
    if (!value) return NULL;

    strncpy(value, value_start, value_len);
    value[value_len] = '\0';

    return value;
}

// Helper function to set sound field by index
static void set_sound_field(pfxr_sound_t* sound, int index, float value) {
    switch (index) {
        case 0: sound->waveForm = (int)value; break;
        case 1: sound->volume = value; break;
        case 2: sound->attackTime = value; break;
        case 3: sound->sustainTime = value; break;
        case 4: sound->sustainPunch = value; break;
        case 5: sound->decayTime = value; break;
        case 6: sound->frequency = value; break;
        case 7: sound->pitchDelta = value; break;
        case 8: sound->pitchDuration = value; break;
        case 9: sound->pitchDelay = value; break;
        case 10: sound->vibratoRate = value; break;
        case 11: sound->vibratoDepth = value; break;
        case 12: sound->tremoloRate = value; break;
        case 13: sound->tremoloDepth = value; break;
        case 14: sound->highPassCutoff = value; break;
        case 15: sound->highPassResonance = value; break;
        case 16: sound->lowPassCutoff = value; break;
        case 17: sound->lowPassResonance = value; break;
        case 18: sound->phaserBaseFrequency = value; break;
        case 19: sound->phaserLfoFrequency = value; break;
        case 20: sound->phaserDepth = value; break;
        case 21: sound->noiseAmount = value; break;
        default: break; // Ignore unknown indices
    }
}

// Helper function to get sound field by index
static float get_sound_field(const pfxr_sound_t* sound, int index) {
    switch (index) {
        case 0: return (float)sound->waveForm;
        case 1: return sound->volume;
        case 2: return sound->attackTime;
        case 3: return sound->sustainTime;
        case 4: return sound->sustainPunch;
        case 5: return sound->decayTime;
        case 6: return sound->frequency;
        case 7: return sound->pitchDelta;
        case 8: return sound->pitchDuration;
        case 9: return sound->pitchDelay;
        case 10: return sound->vibratoRate;
        case 11: return sound->vibratoDepth;
        case 12: return sound->tremoloRate;
        case 13: return sound->tremoloDepth;
        case 14: return sound->highPassCutoff;
        case 15: return sound->highPassResonance;
        case 16: return sound->lowPassCutoff;
        case 17: return sound->lowPassResonance;
        case 18: return sound->phaserBaseFrequency;
        case 19: return sound->phaserLfoFrequency;
        case 20: return sound->phaserDepth;
        case 21: return sound->noiseAmount;
        default: return 0.0f;
    }
}

// Number of fields in the sound structure
#define PFXR_FIELD_COUNT 22

pfxr_sound_t* pfxr_create_params_from_url(const char* url) {
    if (!url) return NULL;

    // Allocate memory for the sound configuration
    pfxr_sound_t* sound = malloc(sizeof(pfxr_sound_t));
    if (!sound) return NULL;

    // Initialize with default values
    *sound = pfxr_get_default_sound();

    // Get the 'fx' query parameter
    char* fx_param = get_query_param(url, "fx");
    if (!fx_param) {
        return sound; // Return default sound if no fx parameter
    }

    // URL decode the parameter
    char* decoded = url_decode(fx_param);
    free(fx_param);
    if (!decoded) {
        return sound; // Return default sound if decoding fails
    }

    // Split by comma and parse values
    char* token = strtok(decoded, ",");
    int index = 0;

    while (token && index < PFXR_FIELD_COUNT) {
        char* endptr;
        float value = strtof(token, &endptr);

        // Only set the value if it's a valid number
        if (*endptr == '\0' || *endptr == ' ') {
            set_sound_field(sound, index, value);
        }

        token = strtok(NULL, ",");
        index++;
    }

    free(decoded);
    return sound;
}

char* pfxr_get_url_from_params(const pfxr_sound_t* config) {
    if (!config) return NULL;

    // Calculate buffer size needed
    // Each float can be up to ~15 characters, plus comma
    size_t buffer_size = PFXR_FIELD_COUNT * 16 + 100; // Extra space for URL parts
    char* url_buffer = malloc(buffer_size);
    if (!url_buffer) return NULL;

    // Create comma-separated values string
    char values_buffer[PFXR_FIELD_COUNT * 16];
    values_buffer[0] = '\0';

    for (int i = 0; i < PFXR_FIELD_COUNT; i++) {
        float value = get_sound_field(config, i);
        char value_str[16];

        // Format the value, removing unnecessary decimal places
        if (i == 0) { // waveForm is an integer
            snprintf(value_str, sizeof(value_str), "%d", (int)value);
        } else {
            snprintf(value_str, sizeof(value_str), "%.6g", value);
        }

        if (i > 0) {
            strcat(values_buffer, ",");
        }
        strcat(values_buffer, value_str);
    }

    // URL encode the values
    char* encoded_values = url_encode(values_buffer);
    if (!encoded_values) {
        free(url_buffer);
        return NULL;
    }

    // Create the URL string with the fx parameter
    snprintf(url_buffer, buffer_size, "?fx=%s", encoded_values);

    free(encoded_values);
    return url_buffer;
}

void pfxr_free_sound_config(pfxr_sound_t* config) {
    if (config) {
        free(config);
    }
}
