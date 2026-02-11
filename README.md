# FromFileToWave

Still very much a work in progress and still it's early stages.
At it's core it's a wavetable synth. The idea is to be able to feed it anything, not just wave files thoough it works with that too. 
Any file should be able to be read with a selected bit depth, table size and number of tables to be able to listen to raw files as a wavetable. 
For now it's just the basics, reading data, interpolating the table with linear interpolation, no oversampling, anti-aliasing or anything fancy yet.

## Usage
- Select the bit depth, table size and number of tables from the menu. 
- Click the load button to pick a file.
- If it's a wave file, the bit depth will be ignored and the one in the file will be used.
- If it fails due to not enough data, play around with the values and press reload.
- Use the frequency slider to change pitch and the scan slider to scan through the wavetables.

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
