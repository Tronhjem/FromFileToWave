#include "WaveTableFileReader.h"

int WaveTableFileReader::getBytesPerSample(BitDepth bitDepth) const
{
    return static_cast<int>(bitDepth) / 8;
}

float WaveTableFileReader::convertSampleToFloat(const uint8* bytes, BitDepth bitDepth)
{
    switch (bitDepth)
    {
        case BitDepth::Bit8:
        {
            int8 sample = static_cast<int8>(bytes[0]);
            return sample / 128.0f;
        }
        
        case BitDepth::Bit16:
        {
            int16 sample = static_cast<int16>(bytes[0] | (bytes[1] << 8));
            return sample / 32768.0f;
        }
        
        case BitDepth::Bit24:
        {
            int32 sample = bytes[0] | (bytes[1] << 8) | (bytes[2] << 16);
            if (sample & 0x800000)
                sample |= 0xFF000000;
            return sample / 8388608.0f;
        }
        
        case BitDepth::Bit32:
        {
            int32 sample = static_cast<int32>(
                bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24)
            );
            return sample / 2147483648.0f;
        }
    }
    
    return 0.0f;
}

bool WaveTableFileReader::loadFile(const juce::File& file, const Config& config)
{
    waveTables.clear();
    lastError.clear();
    
    if (!file.existsAsFile())
    {
        lastError = "File does not exist: " + file.getFullPathName();
        return false;
    }
    
    juce::FileInputStream inputStream(file);
    if (inputStream.failedToOpen())
    {
        lastError = "Failed to open file: " + file.getFullPathName();
        return false;
    }
    
    const int bytesPerSample = getBytesPerSample(config.bitDepth);
    const int64 totalBytesNeeded = static_cast<int64>(config.numTables) * 
                                    static_cast<int64>(config.tableSize) * 
                                    static_cast<int64>(bytesPerSample);
    
    const int64 fileSize = inputStream.getTotalLength();
    
    if (fileSize < totalBytesNeeded)
    {
        lastError = juce::String::formatted(
            "File too small: need %lld bytes, file has %lld bytes",
            totalBytesNeeded, fileSize
        );

        return false;
    }
    
    waveTables.resize(static_cast<size_t>(config.numTables));
    for (int tableIdx = 0; tableIdx < config.numTables; ++tableIdx)
    {
        waveTables[static_cast<unsigned long>(tableIdx)].resize(static_cast<size_t>(config.tableSize));
    }
    
    std::vector<uint8> buffer(bytesPerSample);
    
    for (int tableIdx = 0; tableIdx < config.numTables; ++tableIdx)
    {
        for (int sampleIdx = 0; sampleIdx < config.tableSize; ++sampleIdx)
        {
            int bytesRead = inputStream.read(buffer.data(), bytesPerSample);
            
            if (bytesRead != bytesPerSample)
            {
                lastError = "Unexpected end of file while reading";
                waveTables.clear();
                return false;
            }
            
            waveTables[tableIdx][sampleIdx] = convertSampleToFloat(buffer.data(), config.bitDepth);
        }
    }
    
    return true;
}
