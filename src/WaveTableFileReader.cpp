#include "WaveTableFileReader.h"
#include <cstring>

using namespace juce;

inline int WaveTableFileReader::getBytesPerSample(BitDepth bitDepth) const
{
    return static_cast<int>(bitDepth) / 8;
}

float WaveTableFileReader::convertSampleToFloat(const uint8* bytes, BitDepth bitDepth, bool isIEEEFloat)
{
    switch (bitDepth)
    {
        case BitDepth::Bit8:
        {
            int8 sample = static_cast<int8>(bytes[0]);

            return static_cast<float>(sample) / 128.f;
        }
        
        case BitDepth::Bit16:
        {
            int16 sample = static_cast<int16>(bytes[0] | (bytes[1] << 8));

            return static_cast<float>(sample) / 32768.f;
        }
        
        case BitDepth::Bit24:
        {
            int32 sample = bytes[0] | (bytes[1] << 8) | (bytes[2] << 16);
            if (sample & 0x800000)
                sample |= 0xFF000000;

            return static_cast<float>(sample) / 8388608.f;
        }
        
        case BitDepth::Bit32:
        {
            if (isIEEEFloat)
            {
                // 32-bit IEEE float - just reinterpret the bytes
                float sample;
                std::memcpy(&sample, bytes, 4);
                return sample;
            }
            else
            {
                // 32-bit integer PCM
                int32 sample = static_cast<int32>(
                    bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24)
                );
                return static_cast<float>(sample) / 2147483648.f;
            }
        }
    }
    
    return 0.f;
}

bool WaveTableFileReader::isWavFile(juce::FileInputStream& stream)
{
    stream.setPosition(0);
    char riffHeader[4];
    if (stream.read(riffHeader, 4) != 4)
        return false;
    
    return (riffHeader[0] == 'R' && riffHeader[1] == 'I' && 
            riffHeader[2] == 'F' && riffHeader[3] == 'F');
}

bool WaveTableFileReader::parseWavHeader(juce::FileInputStream& stream, WavHeader& header)
{
    stream.setPosition(0);
    
    char chunkId[4];
    if (stream.read(chunkId, 4) != 4 || 
        chunkId[0] != 'R' || chunkId[1] != 'I' || chunkId[2] != 'F' || chunkId[3] != 'F')
    {
        mLastError = "Invalid WAV file: missing RIFF header";
        return false;
    }
    
    uint32 fileSize;
    stream.read(&fileSize, 4);
    
    char format[4];
    if (stream.read(format, 4) != 4 || 
        format[0] != 'W' || format[1] != 'A' || format[2] != 'V' || format[3] != 'E')
    {
        mLastError = "Invalid WAV file: missing WAVE format";
        return false;
    }
    
    bool foundFmt = false;
    bool foundData = false;
    
    while (stream.getPosition() < stream.getTotalLength())
    {
        char subChunkId[4];
        if (stream.read(subChunkId, 4) != 4)
            break;
        
        uint32 subChunkSize;
        stream.read(&subChunkSize, 4);
        
        if (subChunkId[0] == 'f' && subChunkId[1] == 'm' && subChunkId[2] == 't' && subChunkId[3] == ' ')
        {
            stream.read(&header.audioFormat, 2);
            stream.read(&header.numChannels, 2);
            stream.read(&header.sampleRate, 4);
            
            uint32 byteRate;
            stream.read(&byteRate, 4);
            
            uint16 blockAlign;
            stream.read(&blockAlign, 2);
            stream.read(&header.bitsPerSample, 2);
            
            int64 remainingBytes = subChunkSize - 16;
            if (remainingBytes > 0)
                stream.setPosition(stream.getPosition() + remainingBytes);
            
            foundFmt = true;
        }
        else if (subChunkId[0] == 'd' && subChunkId[1] == 'a' && subChunkId[2] == 't' && subChunkId[3] == 'a')
        {
            header.dataChunkSize = subChunkSize;
            header.dataChunkOffset = stream.getPosition();
            foundData = true;
            break;
        }
        else
        {
            stream.setPosition(stream.getPosition() + subChunkSize);
        }
    }
    
    if (!foundFmt)
    {
        mLastError = "Invalid WAV file: missing fmt chunk";
        return false;
    }
    
    if (!foundData)
    {
        mLastError = "Invalid WAV file: missing data chunk";
        return false;
    }
    
    if (header.bitsPerSample != 8 && header.bitsPerSample != 16 && 
        header.bitsPerSample != 24 && header.bitsPerSample != 32)
    {
        mLastError = juce::String::formatted("Unsupported bit depth: %d", header.bitsPerSample);
        return false;
    }
    
    return true;
}

bool WaveTableFileReader::loadWavFile(juce::FileInputStream& stream, const WavHeader& header, const Config& config)
{
    const int bytesPerSample = header.bitsPerSample / 8;
    const int64 totalSamplesInFile = header.dataChunkSize / static_cast<uint32>(bytesPerSample * header.numChannels);
    
    mWaveTables.resize(static_cast<size_t>(config.numTables));
    for (int tableIdx = 0; tableIdx < config.numTables; ++tableIdx)
    {
        mWaveTables[static_cast<size_t>(tableIdx)].resize(static_cast<size_t>(config.tableSize));
    }
    
    stream.setPosition(header.dataChunkOffset);
    
    std::vector<uint8> sampleBuffer(static_cast<size_t>(bytesPerSample * header.numChannels));
    BitDepth wavBitDepth = static_cast<BitDepth>(header.bitsPerSample);
    bool isFloatWave32 = (header.audioFormat == 3);
    
    int64 sampleCount = 0;
    for (int tableIdx = 0; tableIdx < config.numTables; ++tableIdx)
    {
        for (int sampleIdx = 0; sampleIdx < config.tableSize; ++sampleIdx)
        {
            if (sampleCount > 0 && sampleCount % totalSamplesInFile == 0)
            {
                stream.setPosition(header.dataChunkOffset);
            }
            
            int bytesRead = stream.read(sampleBuffer.data(), bytesPerSample * header.numChannels);
            if (bytesRead != bytesPerSample * header.numChannels)
            {
                mLastError = "Unexpected end of WAV data";
                mWaveTables.clear();
                return false;
            }
            
            float monoSample = 0.0f;
            for (int ch = 0; ch < header.numChannels; ++ch)
            {
                const uint8* channelData = sampleBuffer.data() + (ch * bytesPerSample);
                monoSample += convertSampleToFloat(channelData, wavBitDepth, isFloatWave32);
            }
            // TODO: implement an equal power sum here instead of avg.
            monoSample /= static_cast<float>(header.numChannels);
            
            mWaveTables[static_cast<size_t>(tableIdx)][static_cast<size_t>(sampleIdx)] = monoSample;
            ++sampleCount;
        }
    }
    
    return true;
}

bool WaveTableFileReader::loadRawFile(juce::FileInputStream& stream, const Config& config)
{
    const int bytesPerSample = getBytesPerSample(config.bitDepth);
    const int64 totalBytesNeeded = static_cast<int64>(config.numTables) * 
                                    static_cast<int64>(config.tableSize) * 
                                    static_cast<int64>(bytesPerSample);
    
    const int64 fileSize = stream.getTotalLength();
    
    if (fileSize < totalBytesNeeded)
    {
        mLastError = juce::String::formatted("File too small: need %lld bytes, file has %lld bytes",
            totalBytesNeeded, fileSize
        );
        return false;
    }
    
    mWaveTables.resize(static_cast<size_t>(config.numTables));
    for (int tableIdx = 0; tableIdx < config.numTables; ++tableIdx)
    {
        mWaveTables[static_cast<size_t>(tableIdx)].resize(static_cast<size_t>(config.tableSize));
    }
    
    std::vector<uint8> buffer(static_cast<size_t>(bytesPerSample));
    stream.setPosition(0);

    constexpr float fadeAlpha = 0.02f; // 20 samples fade at each end
    const int fadeLength = static_cast<int>(fadeAlpha * static_cast<float>(config.tableSize) / 2.0f);
    const int endFadeStartIndex = config.tableSize - fadeLength;
    float gain = 1.0f;

    for (int tableIdx = 0; tableIdx < config.numTables; ++tableIdx)
    {
        for (int sampleIdx = 0; sampleIdx < config.tableSize; ++sampleIdx)
        {
            int bytesRead = stream.read(buffer.data(), bytesPerSample);
            
            if (bytesRead != bytesPerSample)
            {
                mLastError = "Unexpected end of file while reading";
                mWaveTables.clear();
                return false;
            }

            if (sampleIdx < fadeLength)
                gain = static_cast<float>(0.5 * (1.0 - std::cos(M_PI * sampleIdx / fadeLength)));
            else if (sampleIdx >= endFadeStartIndex)
                gain = static_cast<float>(0.5 * (1.0 + std::cos(M_PI * (sampleIdx - (config.tableSize - fadeLength)) / fadeLength)));
            
            const float sample = convertSampleToFloat(buffer.data(), config.bitDepth) * gain;
            mWaveTables[static_cast<size_t>(tableIdx)][static_cast<size_t>(sampleIdx)] = sample;
        }
    }
    
    return true;
} 

bool WaveTableFileReader::loadFile(const juce::File& file, const Config& config)
{
    //TODO: Not thread safe, implement a pointer swap and lock.
    mWaveTables.clear();
    mLastError.clear();

    if (!file.existsAsFile())
    {
        mLastError = "File does not exist: " + file.getFullPathName();
        return false;
    }
    
    juce::FileInputStream inputStream(file);
    if (inputStream.failedToOpen())
    {
        mLastError = "Failed to open file: " + file.getFullPathName();
        return false;
    }

    bool success = false;
    
    if (isWavFile(inputStream))
    {
        WavHeader header;
        if (!parseWavHeader(inputStream, header))
            return false;
        
        success = loadWavFile(inputStream, header, config);
    }
    else
    {
        success = loadRawFile(inputStream, config);
    }
    
    if (success)
        mLastConfig = config;
    
    return success;
}
