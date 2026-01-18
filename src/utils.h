/**
 * @file utils.h
 * @author Rafael Diolatzis
 * @brief Low level utility functions for the soundwave program
 * @version 0.1
 * @date 2025-11-30
 * 
 * @copyright Copyright (c) 2025
*/

#pragma once

#include<stdio.h>
#include<stdlib.h>
#include<inttypes.h>
#include<string.h>
#include<errno.h>

#define SIZE_OF_WAVE_HEADER 36

/**
 * @brief Writes characters to STDOUT untill null terminator is found
 * 
 * @param value a pointer to a char array
 */
void write_ch(char* value){
    char* ptr = value;
    while(*ptr){
        putchar(*ptr);
        ptr++;
    }
}

/**
 * @brief Writes a specific amount of characters to STDOUT
 * 
 * @param value a pointer to a char array
 * @param lenght how many characters to write
 */
void swrite_ch(char* value, uint32_t lenght){
    char* ptr = value;
    for(uint32_t i = 0; i < lenght; i++){
        putchar(*ptr);
        ptr++;
    }
}

/**
 * @brief Writes to STDOUT a uint32_t in little-endian order
 * 
 * @param value
 */
void write_u32(uint32_t value){
    putchar((value)  & 0xFF);
    putchar((value >> 8)  & 0xFF);
    putchar((value >> 16) & 0xFF);
    putchar((value >> 24) & 0xFF);
}

/**
 * @brief Writes to STDOUT a uint16_t in little-endian order
 * 
 * @param value
 */
void write_u16(uint16_t value){
    putchar((value)  & 0xFF);
    putchar((value >> 8)  & 0xFF);
}

/**
 * @brief Writes to STDOUT an int16_t that is made to be unsigned in little-endian order
 * 
 * @param value
 */
void write_d16(int16_t value){
    uint16_t uvalue = (uint16_t)value;
    write_u16(uvalue);
}

/**
 * @brief Returns 4bytes from STDIN
 * 
 * Warning: It is the programmers duty to ensure that the data read from the header with `getchar()` match the expected field of the WAV file
 */
char* get_RIFF(){
    
    char* RIFF = malloc(4 * sizeof(char));
    if(RIFF == NULL) return NULL;

    for(int i = 0; i < 4; i++){
        RIFF[i] = getchar();
    }
    return RIFF;
}

/**
 * @brief Returns a uint32_t from STDIN
 * 
 * Warning: It is the programmers duty to ensure that the data read from the header with `getchar()` match the expected field of the WAV file
 * It reads in little endian order and from that constructs the uint32_t
 */
uint32_t get_SizeOfFile(){
    uint32_t field;

    field = (getchar()) + (getchar() << 8) + (getchar() << 16) + (getchar() << 24);
    return field;
}

/**
 * @brief Returns 4bytes from STDIN
 * 
 * Warning: It is the programmers duty to ensure that the data read from the header with `getchar()` match the expected field of the WAV file
 */
char* get_WAVE(){
    return get_RIFF();
}

/**
 * @brief Returns 4bytes from STDIN
 * 
 * Warning: It is the programmers duty to ensure that the data read from the header with `getchar()` match the expected field of the WAV file
 */
char* get_FMT(){
    return get_RIFF();
}

/**
 * @brief Returns a uint32_t from STDIN
 * 
 * Warning: It is the programmers duty to ensure that the data read from the header with `getchar()` match the expected field of the WAV file
 * It reads in little endian order and from that constructs the uint32_t
 */
uint32_t get_FormatChunk(){
    return get_SizeOfFile();
}

/**
 * @brief Returns a uint16_t from STDIN
 * 
 * Warning: It is the programmers duty to ensure that the data read from the header with `getchar()` match the expected field of the WAV file
 * It reads in little endian order and from that constructs the uint16_t
 */
uint16_t get_WaveFormat(){
    uint16_t field;

    field = (getchar()) + (getchar() << 8);
    return field;
}

/**
 * @brief Returns a uint16_t from STDIN
 * 
 * Warning: It is the programmers duty to ensure that the data read from the header with `getchar()` match the expected field of the WAV file
 * It reads in little endian order and from that constructs the uint16_t
 */
uint16_t get_MonoStereo(){
    return get_WaveFormat();
}

/**
 * @brief Returns a uint32_t from STDIN
 * 
 * Warning: It is the programmers duty to ensure that the data read from the header with `getchar()` match the expected field of the WAV file
 * It reads in little endian order and from that constructs the uint32_t
 */
uint32_t get_SampleRate(){
    return get_SizeOfFile();
}

/**
 * @brief Returns a uint32_t from STDIN
 * 
 * Warning: It is the programmers duty to ensure that the data read from the header with `getchar()` match the expected field of the WAV file
 * It reads in little endian order and from that constructs the uint32_t
 */
uint32_t get_BytePerSec(){
    return get_SizeOfFile();
}

/**
 * @brief Returns a uint16_t from STDIN
 * 
 * Warning: It is the programmers duty to ensure that the data read from the header with `getchar()` match the expected field of the WAV file
 * It reads in little endian order and from that constructs the uint16_t
 */
uint16_t get_BlockAlign(){
    return get_WaveFormat();
}

/**
 * @brief Returns a uint16_t from STDIN
 * 
 * Warning: It is the programmers duty to ensure that the data read from the header with `getchar()` match the expected field of the WAV file
 * It reads in little endian order and from that constructs the uint16_t
 */
uint16_t get_BitsPerSample(){
    return get_WaveFormat();
}

/**
 * @brief Returns 4bytes from STDIN
 * 
 * Warning: It is the programmers duty to ensure that the data read from the header with `getchar()` match the expected field of the WAV file
 */
char* get_DataSegmentStart(){
    return get_RIFF();
}

/**
 * @brief Returns a uint32_t from STDIN
 * 
 * Warning: It is the programmers duty to ensure that the data read from the header with `getchar()` match the expected field of the WAV file
 * It reads in little endian order and from that constructs the uint32_t
 */
uint32_t get_DataSegmentSize(){
    return get_SizeOfFile();
}

/**
 * @brief Reads the data segment of the WAV file
 * 
 * Warning: It is the programmers duty to ensure that the data read from the WAV file with `getchar()` match the expected field of the file
 * 
 * @param size the size of the data segment
 * @param eof a flag that is set to 1 if EOF is reached while reading the data of the WAV file. Otherwise the value is set to 0.
 * 
 * @returns An array containing the data of the WAV file. 
 */
char* read_DataSegment(uint32_t size, short* eof){
    *eof = 0;
    char* buffer = malloc(size * sizeof(char));
    if(buffer == NULL) return NULL;

    for(uint32_t i = 0; i < size; i++){
        int c = getchar();
        if(c == EOF){
            *eof = 1;
        }
        buffer[i] = c;
    }
    return buffer;
}

/**
 * @brief Reads the specified 8bit width channel of the WAV file
 * 
 * @param data the data segment of the file
 * @param size the size of the data segment
 * @param channel 0 for left channel and any other value for right channel.
 * 
 * @return a void pointer pointing to a buffer containing all the data of the specified channel
 */
void* read_Channel_8bit(char* data, uint32_t size, short channel){
    size = size / 2; // size of buffer is half the original data
    char* buffer = malloc(size * sizeof(char));
    if(buffer == NULL) return NULL;

    uint32_t i = channel == 0 ? 0 : 1; // 0 is left channel, 1 is right channel

    for(; i < size; i += 2){
        buffer[i] = data[i];
    }
    return buffer;
}

/**
 * @brief Reads the specified 16bit width channel of the WAV file
 * 
 * @param data the data segment of the file
 * @param size the size of the data segment
 * @param channel 0 for left channel and any other value for right channel.
 * 
 * @return a void pointer pointing to a buffer containing all the data of the specified channel
 */
void* read_Channel_16bit(char* data, uint32_t size, short channel){
    size = size / 2; // size of buffer is half the original data
    char* buffer = malloc(size * sizeof(char));
    if(buffer == NULL) return NULL;

    uint32_t i = channel == 0 ? 0 : 1; // 0 is left channel, 1 is right channel

    for(; i < size; i += 2){
        buffer[i] = data[i];
        buffer[i+1] = data[i+1];
    }
    return buffer;
}

/**
 * @brief Clamps a value up to 255
 * 
 * @param value
 * 
 * @returns a uint8_t containing the clamped value
 */
uint8_t clamp_8bit(uint32_t value){
    if(value > 255) return 255;
    return (uint8_t)value;
}

/**
 * @brief Clamps a withing the range [INT16_MIN, INT16_MAX]
 * 
 * @param value
 * 
 * @returns an int16_t containing the clamped value
 */
int16_t clamp_16bit(int32_t value){
    if(value < INT16_MIN) return INT16_MIN;
    if(value > INT16_MAX) return INT16_MAX;
    return (int16_t)value;
}

char* set_Volume16bit(char* data, uint32_t size, double volume){
    char* buffer = malloc(size * sizeof(char));
    if(buffer == NULL) return NULL;
    
    for(uint32_t i = 0; i < size; i += 2){
        int32_t tmp = (int16_t)((uint8_t)data[i] | ((uint8_t)data[i+1] << 8));
        tmp = (int32_t)(tmp * volume);
        int16_t out = (int16_t)clamp_16bit(tmp);

        buffer[i] = (char)(out & 0xFF);
        buffer[i + 1] = (char)((out >> 8) & 0xFF);
    }
    return buffer;
}

char* set_Volume8bit(char* data, uint32_t size, double volume){
    char* buffer = malloc(size * sizeof(char));
    if(buffer == NULL) return NULL;
    
    for(uint32_t i = 0; i < size; i++){
        uint32_t tmp = (uint32_t)(data[i] * volume);
        buffer[i] = (uint8_t)clamp_8bit(tmp);
    }
    return buffer;
}

char* set_Volume(char* data, uint32_t size, int16_t bits_pet_sample, double volume){
    if(bits_pet_sample == 8){
        return set_Volume8bit(data, size, volume);
    } else{
        return set_Volume16bit(data, size, volume);
    }
}

char* get_OtherData(uint32_t total_size, uint32_t data_segment_size){
    uint32_t total_bytes_traversed = SIZE_OF_WAVE_HEADER + data_segment_size;
    uint32_t remaining = total_size - total_bytes_traversed;

    char* buffer = malloc(remaining * sizeof(char));
    if(buffer == NULL) return NULL;

    for(uint32_t i = 0; i < remaining; i++){
        buffer[i] = getchar();
    }
    return buffer;
}

double fsafe_StrToint(char* str, short* flag){
    errno = 0;
    char* endptr;
    double value = strtod(str, &endptr);
    if(endptr == str || errno == ERANGE || *endptr != '\0'){
        fprintf(stderr, "Warning: Something unexpected occured while parsing the value of an argument. This might lead to unexpected behavior\n");
        *flag = 1;
        errno = 0;
    }
    return value;
}

double safe_StrToDouble(char* str){
    short flag;
    return fsafe_StrToint(str, &flag);
}