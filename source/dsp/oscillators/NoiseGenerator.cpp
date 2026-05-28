#include "NoiseGenerator.h"
#include <cstdlib>

NoiseGenerator::NoiseGenerator(float amplitude) : amplitude(amplitude) {}

void NoiseGenerator::setAmplitude(float newAmplitude) {
    amplitude = newAmplitude;
}

void NoiseGenerator::processAudio(float* buffer, int numSamples) {
    // Step by 2 to handle Left/Right interleaved stereo channels
    for (int i = 0; i < numSamples; i += 2) {
        
        // Generate a random float between -1.0 and 1.0
        float randomValue = (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f;
        
        // Scale by amplitude
        float sample = randomValue * amplitude;
        
        // Write the same random sample to both Left and Right channels
        buffer[i]     = sample; 
        buffer[i + 1] = sample; 
    }
}