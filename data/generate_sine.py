#!/usr/bin/env python3
import numpy as np
import wave
import struct

num_samples = 2048
sample_rate = 44100
frequency = sample_rate / num_samples

t = np.linspace(0, 1, num_samples, endpoint=False)
sine_wave = np.sin(2 * np.pi * t)

sine_wave_int16 = np.int16(sine_wave * 32767)

output_file = "sineSingle.wav"
with wave.open(output_file, 'w') as wav_file:
    wav_file.setnchannels(1)
    wav_file.setsampwidth(2)
    wav_file.setframerate(sample_rate)
    wav_file.writeframes(sine_wave_int16.tobytes())

print(f"Generated {output_file} with {num_samples} samples (single cycle sine wave)")
print(f"Sample rate: {sample_rate} Hz")
print(f"Frequency: {frequency:.2f} Hz")
