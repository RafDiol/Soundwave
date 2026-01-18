/**
 * @file soundman.h
 * @author Rafael Diolatzis
 * @brief Contains the soundwave commands
 * @version 0.1
 * @date 2025-11-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include"utils.h"
#define _USE_MATH_DEFINES
#include<math.h>
#include"caudio.h"

/**
 * @brief Displays the information of the WAV file provided through STDIN
 * 
 * @param flag Upon successfull completion the value is set to 0. Otherwise a non-zero value is stored.
 */
void info_command(short* flag){
    *flag = 0;

    char* RIFF = get_RIFF();
    if(RIFF == NULL || memcmp(RIFF, "RIFF", 4) != 0){
        fprintf(stderr, "Error! \"RIFF\" not found\n");
        *flag = 1;
        free(RIFF);
        return;
    }

    uint32_t SizeOfFile = get_SizeOfFile();

    char* WAVE = get_WAVE();
    if(WAVE == NULL || memcmp(WAVE, "WAVE", 4) != 0){
        fprintf(stderr, "Error! \"WAVE\" not found\n");
        *flag = 1;
        free(RIFF);
        free(WAVE);
        return;
    }

    char* FMT = get_FMT();
    if(FMT == NULL || memcmp(FMT, "fmt ", 4) != 0){
        fprintf(stderr, "Error! \"fmt \" not found\n");
        *flag = 1;
        free(RIFF);
        free(WAVE);
        free(FMT);
        return;
    }

    uint32_t format_chunk = get_FormatChunk();
    if(format_chunk != 16){
        fprintf(stderr, "Error! size of format chunk should be 16\n");
        *flag = 1;
        free(RIFF);
        free(WAVE);
        free(FMT);
        return;
    }

    uint16_t wave_format = get_WaveFormat();
    if(wave_format != 1){
        fprintf(stderr, "Error! WAVE type format should be 1\n");
        *flag = 1;
        free(RIFF);
        free(WAVE);
        free(FMT);
        return;
    }

    uint16_t mono_stereo = get_MonoStereo();
    if(mono_stereo != 1 && mono_stereo != 2){
        fprintf(stderr, "Error! mono/stereo should be 1 or 2\n");
        *flag = 1;
        free(RIFF);
        free(WAVE);
        free(FMT);
        return;
    }
    
    uint32_t sample_rate = get_SampleRate();
    uint32_t byte_per_sec = get_BytePerSec();
    uint16_t block_align = get_BlockAlign();
    if(byte_per_sec != sample_rate * block_align){
        fprintf(stderr, "Error! bytes/second should be sample rate x block alignment\n");
        *flag = 1;
        free(RIFF);
        free(WAVE);
        free(FMT);
        return;
    }

    int16_t bits_per_sample = get_BitsPerSample();
    if(bits_per_sample != 8 && bits_per_sample != 16){
        fprintf(stderr, "Error! bits/sample should be 8 or 16\n");
        *flag = 1;
        free(RIFF);
        free(WAVE);
        free(FMT);
        return;
    }
    if(block_align != (bits_per_sample / 8) * mono_stereo){
        fprintf(stderr, "Error! block alignment should be bits per sample / 8 x mono/stereo\n");
        *flag = 1;
        free(RIFF);
        free(WAVE);
        free(FMT);
        return;
    }

    char* data_start_segment = get_DataSegmentStart();
    if(data_start_segment == NULL || memcmp(data_start_segment, "data", 4) != 0){
        fprintf(stderr, "Error! \"data\" not found\n");
        *flag = 1;
        free(RIFF);
        free(WAVE);
        free(FMT);
        free(data_start_segment);
        return;
    }
    uint32_t data_segment_size = get_DataSegmentSize();

    // read DATA segment
    uint32_t index = 0;
    short error = 0;
    while(index < data_segment_size && !error){
        error = getchar() == EOF ? 1 : error;
        index++;
    }
    if(error){
        fprintf(stderr, "Error! insufficient data\n");
        *flag = 1;
        free(RIFF);
        free(WAVE);
        free(FMT);
        free(data_start_segment);
        return;
    }

    uint32_t total_bytes_traversed = SIZE_OF_WAVE_HEADER + index;

    while(total_bytes_traversed < SizeOfFile){
        getchar();
        total_bytes_traversed++;
    }

    if(getchar() != EOF){
        fprintf(stderr, "Error! bad file size (found data past the expected end of file)\n");
        *flag = 1;
        free(RIFF);
        free(WAVE);
        free(FMT);
        free(data_start_segment);
        return;
    }
    
    printf("size of file: %" PRIu32 "\n", SizeOfFile);
    printf("size of format chunk: %" PRIu32 "\n", format_chunk);
    printf("WAVE type format: %" PRIu16 "\n", wave_format);
    printf("mono/stereo: %" PRIu16 "\n", mono_stereo);
    printf("sample rate: %" PRIu32 "\n", sample_rate);
    printf("byte/sec: %" PRIu32 "\n", byte_per_sec);
    printf("block align: %" PRIu16 "\n", block_align);
    printf("bits/sample: %" PRId16 "\n", bits_per_sample);
    printf("size of data chunk: %" PRIu32 "\n", data_segment_size);

    free(RIFF);
    free(WAVE);
    free(FMT);
    free(data_start_segment);

    return;
}

/**
 * @brief Reads a WAV file from standard input and writes the file to standard output with an adjusted playback rate
 * 
 * @param rate Playback-rate multiplier applied to the input audio 
 * @param flag Upon successfull completion the value is set to 0. Otherwise a non-zero value is stored
 */
void srate_command(double rate, short* flag){
    char* RIFF = get_RIFF();
    if(RIFF == NULL || memcmp(RIFF, "RIFF", 4) != 0){
        fprintf(stderr, "Error! \"RIFF\" not found\n");
        free(RIFF);
        *flag = 1;
        return;
    }

    uint32_t SizeOfFile = get_SizeOfFile();

    char* WAVE = get_WAVE();
    if(WAVE == NULL || memcmp(WAVE, "WAVE", 4) != 0){
        fprintf(stderr, "Error! \"WAVE\" not found\n");
        free(RIFF);
        free(WAVE);
        *flag = 1;
        return;
    }

    char* FMT = get_FMT();
    if(FMT == NULL || memcmp(FMT, "fmt ", 4) != 0){
        fprintf(stderr, "Error! \"fmt \" not found\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        *flag = 1;
        return;
    }

    uint32_t format_chunk = get_FormatChunk();
    if(format_chunk != 16){
        fprintf(stderr, "Error! size of format chunk should be 16\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        *flag = 1;
        return;
    }

    uint16_t wave_format = get_WaveFormat();
    if(wave_format != 1){
        fprintf(stderr, "Error! WAVE type format should be 1\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        *flag = 1;
        return;
    }

    uint16_t mono_stereo = get_MonoStereo();
    if(mono_stereo != 1 && mono_stereo != 2){
        fprintf(stderr, "Error! mono/stereo should be 1 or 2\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        *flag = 1;
        return;
    }
    
    uint32_t sample_rate = get_SampleRate();

    uint32_t bytes_per_sec = get_BytePerSec();

    uint16_t block_align = get_BlockAlign();
    if(bytes_per_sec != sample_rate * block_align){
        fprintf(stderr, "Error! bytes/second should be sample rate x block alignment\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        *flag = 1;
        return;
    }

    int16_t bits_per_sample = get_BitsPerSample();
    if(bits_per_sample != 8 && bits_per_sample != 16){
        fprintf(stderr, "Error! bits/sample should be 8 or 16\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        *flag = 1;
        return;
    }
    if(block_align != (bits_per_sample / 8) * mono_stereo){
        fprintf(stderr, "Error! block alignment should be bits per sample / 8 x mono/stereo\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        *flag = 1;
        return;
    }

    char* data_start_segment = get_DataSegmentStart();
    if(data_start_segment == NULL || memcmp(data_start_segment, "data", 4) != 0){
        fprintf(stderr, "Error! \"data\" not found\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        free(data_start_segment);
        *flag = 1;
        return;
    }
    uint32_t data_segment_size = get_DataSegmentSize();

    
    // read DATA segment
    short error = 0;
    char* data = read_DataSegment(data_segment_size, &error);
    if(error || data == NULL){
        fprintf(stderr, "Error! insufficient data\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        free(data_start_segment);
        free(data);
        *flag = 1;
        return;
    }

    char* other_data_buffer = get_OtherData(SizeOfFile, data_segment_size);

    if(getchar() != EOF){
        fprintf(stderr, "Error! bad file size (found data past the expected end of file)\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        free(data_start_segment);
        free(data);
        free(other_data_buffer);
        *flag = 1;
        return;
    }

    // Manipulate data
    sample_rate = (uint32_t)(sample_rate * rate);
    bytes_per_sec = (uint32_t)(bytes_per_sec * rate);

    swrite_ch(RIFF, 4);
    write_u32(SizeOfFile);
    swrite_ch(WAVE, 4);
    swrite_ch(FMT, 4);
    write_u32(format_chunk);
    write_u16(wave_format);
    write_u16(mono_stereo);
    write_u32(sample_rate);
    write_u32(bytes_per_sec);
    write_u16(block_align);
    write_d16(bits_per_sample);
    swrite_ch(data_start_segment, 4);
    write_u32(data_segment_size);
    swrite_ch(data, data_segment_size);
    swrite_ch(other_data_buffer, SizeOfFile -(SIZE_OF_WAVE_HEADER + data_segment_size));

    free(RIFF);
    free(WAVE);
    free(FMT);
    free(data_start_segment);
    free(data);
    free(other_data_buffer);
}

/**
 * @brief Reads a WAV file from standard input and writes the file to standard output with only the selected audio channel preserved
 * 
 * @param channel The channel to preserve. 0 for left channel and any non-zero value for right channel.
 * @param flag Upon successfull completion the value is set to 0. Otherwise a non-zero value is stored
 */
void schannel_command(short channel, short* flag){
    char* RIFF = get_RIFF();
    if(RIFF == NULL || memcmp(RIFF, "RIFF", 4) != 0){
        fprintf(stderr, "Error! \"RIFF\" not found\n");
        free(RIFF);
        *flag = 1;
        return;
    }

    uint32_t SizeOfFile = get_SizeOfFile();

    char* WAVE = get_WAVE();
    if(WAVE == NULL || memcmp(WAVE, "WAVE", 4) != 0){
        fprintf(stderr, "Error! \"WAVE\" not found\n");
        free(RIFF);
        free(WAVE);
        *flag = 1;
        return;
    }

    char* FMT = get_FMT();
    if(FMT == NULL || memcmp(FMT, "fmt ", 4) != 0){
        fprintf(stderr, "Error! \"fmt \" not found\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        *flag = 1;
        return;
    }

    uint32_t format_chunk = get_FormatChunk();
    if(format_chunk != 16){
        fprintf(stderr, "Error! size of format chunk should be 16\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        *flag = 1;
        return;
    }

    uint16_t wave_format = get_WaveFormat();
    if(wave_format != 1){
        fprintf(stderr, "Error! WAVE type format should be 1\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        *flag = 1;
        return;
    }

    uint16_t mono_stereo = get_MonoStereo();
    if(mono_stereo != 1 && mono_stereo != 2){
        fprintf(stderr, "Error! mono/stereo should be 1 or 2\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        *flag = 1;
        return;
    }

    if(mono_stereo == 1){ 
        channel = 0;
    }
    
    uint32_t sample_rate = get_SampleRate();

    uint32_t bytes_per_sec = get_BytePerSec();

    uint16_t block_align = get_BlockAlign();
    if(bytes_per_sec != sample_rate * block_align){
        fprintf(stderr, "Error! bytes/second should be sample rate x block alignment\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        *flag = 1;
        return;
    }

    uint16_t bits_per_sample = get_BitsPerSample();
    if(bits_per_sample != 8 && bits_per_sample != 16){
        fprintf(stderr, "Error! bits/sample should be 8 or 16\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        *flag = 1;
        return;
    }
    if(block_align != (bits_per_sample / 8) * mono_stereo){
        fprintf(stderr, "Error! block alignment should be bits per sample / 8 x mono/stereo\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        *flag = 1;
        return;
    }

    char* data_start_segment = get_DataSegmentStart();
    if(data_start_segment == NULL || memcmp(data_start_segment, "data", 4) != 0){
        fprintf(stderr, "Error! \"data\" not found\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        free(data_start_segment);
        *flag = 1;
        return;
    }
    uint32_t data_segment_size = get_DataSegmentSize();

    
    // read DATA segment
    short error = 0;
    char* data = read_DataSegment(data_segment_size, &error);
    if(error){
        fprintf(stderr, "Error! insufficient data\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        free(data_start_segment);
        free(data);
        *flag = 1;
        return;
    }

    char* other_data_buffer = get_OtherData(SizeOfFile, data_segment_size);

    if(getchar() != EOF){
        fprintf(stderr, "Error! bad file size (found data past the expected end of file)\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        free(data_start_segment);
        free(data);
        free(other_data_buffer);
        *flag = 1;
        return;
    }

    // According to the exercise bits per sample is always either 8 or 16
    void* channel_data;
    if(bits_per_sample == 8){
        channel_data = read_Channel_8bit(data, data_segment_size, channel);
    } 
    else { 
        channel_data = read_Channel_16bit(data, data_segment_size, channel);
    }
    mono_stereo = 1; // one channel only now
    bytes_per_sec = bytes_per_sec / 2;
    block_align = block_align / 2;
    data_segment_size = data_segment_size / 2; // half the data now
    SizeOfFile = SIZE_OF_WAVE_HEADER + data_segment_size; // re-compute

    swrite_ch(RIFF, 4);
    write_u32(SizeOfFile);
    swrite_ch(WAVE, 4);
    swrite_ch(FMT, 4);
    write_u32(format_chunk);
    write_u16(wave_format);
    write_u16(mono_stereo);
    write_u32(sample_rate);
    write_u32(bytes_per_sec);
    write_u16(block_align);
    write_d16(bits_per_sample);
    swrite_ch(data_start_segment, 4);
    write_u32(data_segment_size);
    swrite_ch(channel_data, data_segment_size);
    swrite_ch(other_data_buffer, SizeOfFile -(SIZE_OF_WAVE_HEADER + data_segment_size));

    free(RIFF);
    free(WAVE);
    free(FMT);
    free(data_start_segment);
    free(data);
    free(other_data_buffer);
    free(channel_data);
}

/**
 * @brief Reads a WAV file from standard input and writes the file to standard output with its volume adjusted
 * 
 * @param volume The volume multiplier applied to the inputted audio file
 * @param flag Upon successfull completion the value is set to 0. Otherwise a non-zero value is stored
 */
void svolume_command(double volume, short* flag){
    char* RIFF = get_RIFF();
    if(RIFF == NULL || memcmp(RIFF, "RIFF", 4) != 0){
        fprintf(stderr, "Error! \"RIFF\" not found\n");
        free(RIFF);
        *flag = 1;
        return;
    }

    uint32_t SizeOfFile = get_SizeOfFile();

    char* WAVE = get_WAVE();
    if(WAVE == NULL || memcmp(WAVE, "WAVE", 4) != 0){
        fprintf(stderr, "Error! \"WAVE\" not found\n");
        free(RIFF);
        free(WAVE);
        *flag = 1;
        return;
    }

    char* FMT = get_FMT();
    if(FMT == NULL || memcmp(FMT, "fmt ", 4) != 0){
        fprintf(stderr, "Error! \"fmt \" not found\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        *flag = 1;
        return;
    }

    uint32_t format_chunk = get_FormatChunk();
    if(format_chunk != 16){
        fprintf(stderr, "Error! size of format chunk should be 16\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        *flag = 1;
        return;
    }

    uint16_t wave_format = get_WaveFormat();
    if(wave_format != 1){
        fprintf(stderr, "Error! WAVE type format should be 1\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        *flag = 1;
        return;
    }

    uint16_t mono_stereo = get_MonoStereo();
    if(mono_stereo != 1 && mono_stereo != 2){
        fprintf(stderr, "Error! mono/stereo should be 1 or 2\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        *flag = 1;
        return;
    }
    
    uint32_t sample_rate = get_SampleRate();

    uint32_t bytes_per_sec = get_BytePerSec();

    uint16_t block_align = get_BlockAlign();
    if(bytes_per_sec != sample_rate * block_align){
        fprintf(stderr, "Error! bytes/second should be sample rate x block alignment\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        *flag = 1;
        return;
    }

    uint16_t bits_per_sample = get_BitsPerSample();
    if(bits_per_sample != 8 && bits_per_sample != 16){
        fprintf(stderr, "Error! bits/sample should be 8 or 16\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        *flag = 1;
        return;
    }
    if(block_align != (bits_per_sample / 8) * mono_stereo){
        fprintf(stderr, "Error! block alignment should be bits per sample / 8 x mono/stereo\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        *flag = 1;
        return;
    }

    char* data_start_segment = get_DataSegmentStart();
    if(data_start_segment == NULL || memcmp(data_start_segment, "data", 4) != 0){
        fprintf(stderr, "Error! \"data\" not found\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        free(data_start_segment);
        *flag = 1;
        return;
    }
    uint32_t data_segment_size = get_DataSegmentSize();

    
    // read DATA segment
    short error = 0;
    char* data = read_DataSegment(data_segment_size, &error);
    if(error){
        fprintf(stderr, "Error! insufficient data\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        free(data_start_segment);
        free(data);
        *flag = 1;
        return;
    }
    
    char* newData = set_Volume(data, data_segment_size, bits_per_sample, volume);
    free(data);

    char* other_data_buffer = get_OtherData(SizeOfFile, data_segment_size);

    if(getchar() != EOF){
        fprintf(stderr, "Error! bad file size (found data past the expected end of file)\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        free(data_start_segment);
        free(newData);
        free(other_data_buffer);
        *flag = 1;
        return;
    }

    swrite_ch(RIFF, 4);
    write_u32(SizeOfFile);
    swrite_ch(WAVE, 4);
    swrite_ch(FMT, 4);
    write_u32(format_chunk);
    write_u16(wave_format);
    write_u16(mono_stereo);
    write_u32(sample_rate);
    write_u32(bytes_per_sec);
    write_u16(block_align);
    write_d16(bits_per_sample);
    swrite_ch(data_start_segment, 4);
    write_u32(data_segment_size);
    swrite_ch(newData, data_segment_size);
    swrite_ch(other_data_buffer, SizeOfFile -(SIZE_OF_WAVE_HEADER + data_segment_size));

    free(RIFF);
    free(WAVE);
    free(FMT);
    free(data_start_segment);
    free(newData);
    free(other_data_buffer);
}

/**
 * @brief Generates a WAV file that is written to standard output
 * 
 * @param dur Duration in seconds
 * @param sr Sample rate in Hz
 * @param fm Frequency modulation
 * @param fc Frequency carrier
 * @param mi Modulation index
 * @param amp Amplitude
 */
void mysound(int dur, int sr, double fm, double fc, double mi, double amp){
    uint16_t mono_stereo = 1;
    int16_t bits_per_sample = 16;
    uint32_t bytes_per_sec = sr * mono_stereo * (bits_per_sample / 8);
    uint16_t block_align = mono_stereo * (bits_per_sample / 8);
    uint32_t data_segment_size = dur * sr * block_align;

    uint32_t format_chunk = 16; // Fixed size by exercise
    uint32_t SizeOfFIle = SIZE_OF_WAVE_HEADER + data_segment_size;
    uint16_t wave_format = 1; // Fixed value by exercise

    swrite_ch("RIFF", 4);
    write_u32(SizeOfFIle);
    swrite_ch("WAVE", 4);
    swrite_ch("fmt ", 4);
    write_u32(format_chunk);
    write_u16(wave_format);
    write_u16(mono_stereo);
    write_u32((uint32_t)sr);
    write_u32(bytes_per_sec);
    write_u16(block_align);
    write_d16(bits_per_sample);
    swrite_ch("data", 4);
    write_u32(data_segment_size);
    
    // write data
    uint32_t total_samples = dur * sr;

    for(uint32_t i = 0; i < total_samples; i++){
        double t = (double)i / sr;
        double tmp = amp * sin(2 * M_PI * fc * t - mi * sin(2 * M_PI * fm * t));
        int16_t sample = trunc(tmp);
        write_d16(sample);
    }
}

/**
 * @brief Plays the WAV file provided from standard input
 * 
 * @return Zero on success.
 * Negative values are propagated from functions defined in caudio.h. See the header file documentation for more details. Positive values indicate the following function-specific errors
 *      - 1: The WAV file provided is corrupted
 *      - 2: An unexpected error occured while playing the WAV file
 * 
 */
int play_sound(){
    char* RIFF = get_RIFF();
    if(RIFF == NULL || memcmp(RIFF, "RIFF", 4) != 0){
        fprintf(stderr, "Error! \"RIFF\" not found\n");
        free(RIFF);
        return 1;
    }

    //uint32_t SizeOfFile = get_SizeOfFile();
    getchar(); getchar(); getchar(); getchar();

    char* WAVE = get_WAVE();
    if(WAVE == NULL || memcmp(WAVE, "WAVE", 4) != 0){
        fprintf(stderr, "Error! \"WAVE\" not found\n");
        free(RIFF);
        free(WAVE);
        return 1;
    }

    char* FMT = get_FMT();
    if(FMT == NULL || memcmp(FMT, "fmt ", 4) != 0){
        fprintf(stderr, "Error! \"fmt \" not found\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        return 1;
    }

    uint32_t format_chunk = get_FormatChunk();
    if(format_chunk != 16){
        fprintf(stderr, "Error! size of format chunk should be 16\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        return 1;
    }

    uint16_t wave_format = get_WaveFormat();
    if(wave_format != 1){
        fprintf(stderr, "Error! WAVE type format should be 1\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        return 1;
    }

    uint16_t mono_stereo = get_MonoStereo();
    if(mono_stereo != 1 && mono_stereo != 2){
        fprintf(stderr, "Error! mono/stereo should be 1 or 2\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        return 1;
    }
    
    uint32_t sample_rate = get_SampleRate();
    uint32_t byte_per_sec = get_BytePerSec();
    uint16_t block_align = get_BlockAlign();
    if(byte_per_sec != sample_rate * block_align){
        fprintf(stderr, "Error! bytes/second should be sample rate x block alignment\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        return 1;
    }

    int16_t bits_per_sample = get_BitsPerSample();
    if(bits_per_sample != 8 && bits_per_sample != 16){
        fprintf(stderr, "Error! bits/sample should be 8 or 16\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        return 1;
    }
    if(block_align != (bits_per_sample / 8) * mono_stereo){
        fprintf(stderr, "Error! block alignment should be bits per sample / 8 x mono/stereo\n");
        free(RIFF);
        free(WAVE);
        free(FMT);
        return 1;
    }

    char* data_start_segment = get_DataSegmentStart();
    if(data_start_segment == NULL || memcmp(data_start_segment, "data", 4) != 0){
        fprintf(stderr, "Error! \"data\" not found\n");
        free(data_start_segment);
        free(RIFF);
        free(WAVE);
        free(FMT);
        return 1;
    }

    uint32_t data_segment_size = get_DataSegmentSize();

    short eof;
    int err = 0;
    void* buffer = read_DataSegment(data_segment_size, &eof);

    int fd = caudio_open_device();
    if(fd < 0){
        fprintf(stderr, "Error: Unable to detect a valid audio device to use\n");
        free(buffer);
        free(data_start_segment);
        free(RIFF);
        free(WAVE);
        free(FMT);
        return 1;
    }

    uint32_t segmentSize = 1024; // Default buffer size
    struct snd_pcm_hw_params hw;
    struct snd_pcm_sw_params sw;
    err = caudio_setup_params(fd, &hw, &sw, (int)mono_stereo, bits_per_sample, (unsigned int)sample_rate, (unsigned int)segmentSize);
    if(err != 0){
        fprintf(stderr, "Error: Unable to configure audio device (Error code: %d)\n", err);
        free(buffer);
        free(data_start_segment);
        free(RIFF);
        free(WAVE);
        free(FMT);
        return err;
    }

    uint32_t steps = 0;
    char* ptr = buffer;
    void* segment = malloc(segmentSize * sizeof(uint8_t));
    caudio_start_playback(fd);
    while(steps < data_segment_size){
        if(steps + segmentSize < data_segment_size){
            memcpy(segment, ptr, segmentSize);
            ptr += segmentSize;
            steps += segmentSize;
        } else{
            uint32_t N = data_segment_size - steps;
            memcpy(segment, ptr, N);
            ptr += N;
            steps += N;
        }

        if(caudio_write_data_to_device(fd, segment, segmentSize) == -1){
            fprintf(stderr, "Error: An unexpected error occured while playing your WAV file\n");
            caudio_stop_playback(fd);
            caudio_close_audio_devide(fd);

            free(segment);
            free(buffer);
            free(data_start_segment);
            free(RIFF);
            free(WAVE);
            free(FMT);
            return 2;
        }
    }

    caudio_stop_playback(fd);
    
    caudio_close_audio_devide(fd);

    free(segment);
    free(buffer);
    free(data_start_segment);
    free(RIFF);
    free(WAVE);
    free(FMT);
    return 0;
}