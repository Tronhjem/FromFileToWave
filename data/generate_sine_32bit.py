#!/usr/bin/env python3
import numpy as np
import wave
import struct

# Parameters
num_samples = 2048
sample_rate = 44100
frequency = sample_rate / num_samples  # One complete cycle over 2048 samples

# Generate sine wave (single cycle)
t = np.linspace(0, 1, num_samples, endpoint=False)
sine_wave = np.sin(2 * np.pi * t)

# Save as 32-bit float WAV
output_file = "sine_wave_32bit_float.wav"
with wave.open(output_file, 'w') as wav_file:
    wav_file.setnchannels(1)  # Mono
    wav_file.setsampwidth(4)  # 4 bytes per sample (32-bit)
    wav_file.setframerate(sample_rate)
    wav_file.setparams((1, 4, sample_rate, num_samples, 'NONE', 'not compressed'))
    
    # Pack as 32-bit floats
    for sample in sine_wave:
        wav_file.writeframes(struct.pack('<f', sample))

print(f"Generated {output_file} with {num_samples} samples (32-bit float format)")
print(f"Sample rate: {sample_rate} Hz")
print(f"Frequency: {frequency:.2f} Hz")
print(f"Values range: [{sine_wave.min():.6f}, {sine_wave.max():.6f}]")
