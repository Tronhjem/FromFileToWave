# FromFileToWave

Still very much a work in progress and still it's early stages.

The plan is to create a wave table synth that reads in any file. The reader now supports:

## File Format Support

### WAV Files
- Automatically detects and parses WAV file headers
- Supports 8, 16, 24, and 32-bit PCM formats
- Multi-channel files are automatically mixed to mono (averaging all channels)
- Audio data is repeated or truncated to fit the requested table size
- Bit depth is automatically detected from the WAV header

### Raw Binary Files
- Reads any file as raw binary data with user-specified bit depth (8, 16, 24, or 32-bit)
- Maintains backward compatibility with existing functionality

## Usage
The reader accepts a configuration specifying:
- `tableSize`: Number of samples per wavetable
- `numTables`: Number of wavetables to read from the file
- `bitDepth`: Bit depth for raw binary files (ignored for WAV files)

This allows users to morph through a file and explore new timbres. These could be carefully curated wavetables or any arbitrary data.

## Future Plans
The idea is to later have more files, in order to have a 2D map of morphing wave tables.

Disclaimer: AI may have been used in this project but mostly for tedious tasks and code reviews. It's by no means a vibe coded project.
