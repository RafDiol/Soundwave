/**
 * @file caudio.h
 * @author Rafael Diolatzis
 * @brief Low-level code to communicate with sound I/O devices
 * @version 0.1
 * @date 2025-12-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include <dirent.h>
#include <fcntl.h>
#include <sound/asound.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

/**
 * @brief Opens connection to an available kernel audio device
 *
 * @return If successfull returns the file descriptor of the device. Otherwise it returns a negative value
 */
int caudio_open_device() {
    DIR *directory = opendir("/dev/snd");
    struct dirent *ent;

    char audioDevicePath[264]; // 256 + 8 for the /dev/snd
    int foundDevice = -1;
    while ((ent = readdir(directory)) != NULL && foundDevice == -1) {
        if (strncmp(ent->d_name, "pcmC", 4) == 0 && // check for pcmC
            ent->d_name[strlen(ent->d_name) - 1] == 'p') { // check if device is playback

            strcpy(audioDevicePath, "/dev/snd/");
            strcat(audioDevicePath, ent->d_name);
            foundDevice = 1;
        }
    }

    closedir(directory);

    if (foundDevice != 1)
        return foundDevice;

    int fd = open(audioDevicePath, O_WRONLY | O_NONBLOCK);
    return fd;
}

#define MASK_COUNT (SNDRV_PCM_HW_PARAM_LAST_MASK - SNDRV_PCM_HW_PARAM_FIRST_MASK + 1)
#define INTR_COUNT (SNDRV_PCM_HW_PARAM_LAST_INTERVAL - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL + 1)
/**
 * @brief Setups the specified audio device
 * 
 * @param fd file descriptor
 * @param hw 
 * @param sw 
 * @param channels
 * @param bits_per_sample 
 * @param sample_rate 
 * @param segment_size 
 * @return int upon success zero is returned. Otherwise a negative value is returned.
 */
int caudio_setup_params(int fd,
                        struct snd_pcm_hw_params *hw,
                        struct snd_pcm_sw_params *sw,
                        unsigned channels,
                        int16_t bits_per_sample,
                        unsigned int sample_rate,
                        unsigned int segment_size) {
    int ret;

    /* ------------------ HARDWARE PARAM STRUCT INIT --------------------- */

    memset(hw, 0, sizeof(*hw));

    // rmask must include all parameters up to last
    hw->rmask = (1ULL << (SNDRV_PCM_HW_PARAM_LAST_INTERVAL + 1)) - 1;
    hw->cmask = 0;
    hw->flags = 0;

    // Initialize all masks to full bitmask
    for (int i = 0; i < MASK_COUNT; i++)
        memset(hw->masks[i].bits, 0xFF, sizeof(hw->masks[i].bits));

    // initialize all intervals to very wide range
    for (int i = 0; i < INTR_COUNT; i++) {
        hw->intervals[i].min = 0;
        hw->intervals[i].max = UINT32_MAX;
        hw->intervals[i].openmin = 0;
        hw->intervals[i].openmax = 0;
        hw->intervals[i].integer = 0;
    }

    /* ------------------- FIRST: BASIC HW_REFINE ----------------------- */
    ret = ioctl(fd, SNDRV_PCM_IOCTL_HW_REFINE, hw);
    if (ret < 0) {
        return -1;
    }

    // Ensure parameters are withing valid range

    struct snd_interval *ch = &hw->intervals[SNDRV_PCM_HW_PARAM_CHANNELS - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL];
    if (channels < ch->min || channels > ch->max)
        channels = ch->max;

    struct snd_interval *rate = &hw->intervals[SNDRV_PCM_HW_PARAM_RATE - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL];
    if (sample_rate < rate->min)
        sample_rate = rate->min;
    if (sample_rate > rate->max)
        sample_rate = rate->max;

    struct snd_interval *ps = &hw->intervals[SNDRV_PCM_HW_PARAM_PERIOD_SIZE - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL];
    if (segment_size < ps->min)
        segment_size = ps->min;
    if (segment_size > ps->max)
        segment_size = ps->max;
    ps->min = ps->max = segment_size;
    ps->integer = 1;

    struct snd_interval *bs = &hw->intervals[SNDRV_PCM_HW_PARAM_BUFFER_SIZE - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL];
    unsigned int buffer_size = segment_size * 4;
    if (buffer_size < bs->min)
        buffer_size = bs->min;
    if (buffer_size > bs->max)
        buffer_size = bs->max;
    bs->min = bs->max = buffer_size;
    bs->integer = 1;

    /* ---------------------- APPLY USER PARAMETERS --------------------- */

    /* format */
    snd_pcm_format_t fmt = (bits_per_sample == 8) ? SNDRV_PCM_FORMAT_U8 : SNDRV_PCM_FORMAT_S16_LE;

    hw->masks[SNDRV_PCM_HW_PARAM_FORMAT - SNDRV_PCM_HW_PARAM_FIRST_MASK]
        .bits[0] = 1ULL << fmt;

    /* access: interleaved */
    hw->masks[SNDRV_PCM_HW_PARAM_ACCESS - SNDRV_PCM_HW_PARAM_FIRST_MASK]
        .bits[0] = 1ULL << SNDRV_PCM_ACCESS_RW_INTERLEAVED;

    /* channels */
    ch = &hw->intervals[SNDRV_PCM_HW_PARAM_CHANNELS - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL];
    ch->min = ch->max = channels;
    ch->integer = 1;

    /* rate */
    rate = &hw->intervals[SNDRV_PCM_HW_PARAM_RATE - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL];
    rate->min = rate->max = sample_rate;
    rate->integer = 1;

    /* period size */
    ps = &hw->intervals[SNDRV_PCM_HW_PARAM_PERIOD_SIZE - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL];
    ps->min = ps->max = segment_size;
    ps->integer = 1;

    /* buffer size is set to 4× period */
    bs = &hw->intervals[SNDRV_PCM_HW_PARAM_BUFFER_SIZE - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL];
    bs->min = bs->max = segment_size * 4;
    bs->integer = 1;

    /* -------------------------- APPLY HW PARAMS ------------------------ */

    ret = ioctl(fd, SNDRV_PCM_IOCTL_HW_PARAMS, hw);
    if (ret < 0) {
        return -2;
    }

    /* ------------------------- SOFTWARE PARAMS ------------------------ */

    memset(sw, 0, sizeof(*sw));

    sw->period_step = 1;
    sw->start_threshold = 1;
    sw->stop_threshold = ~0U;
    sw->silence_threshold = 0;
    sw->silence_size = 0;
    sw->boundary = 0x7FFFFFFF;
    sw->avail_min = segment_size;

    ret = ioctl(fd, SNDRV_PCM_IOCTL_SW_PARAMS, sw);
    if (ret < 0) {
        return -3;
    }

    /* ---------------------------- PREPARE ------------------------------ */

    ret = ioctl(fd, SNDRV_PCM_IOCTL_PREPARE, NULL);
    if (ret < 0) {
        return -4;
    }

    return 0;
}

/**
 * @brief Starts playing the data provided to the specified audio device
 * 
 * @param fd file descriptor
 */
void caudio_start_playback(int fd) {
    ioctl(fd, SNDRV_PCM_IOCTL_START);
}

/**
 * @brief Stops playing audio from the specified audio device when all the data provided to it has been played
 * 
 * @param fd file descriptor
 * @return int Upon success zero is returned
 */
int caudio_stop_playback(int fd) {
    return ioctl(fd, SNDRV_PCM_IOCTL_DRAIN);
}

/**
 * @brief Writes audio data to the specified audio device
 * 
 * @param fd file descriptor
 * @param buffer contains the data
 * @param n size of the buffer
 * @return int Upon success zero is returned. Otherwise -1 is returned.
 */
int caudio_write_data_to_device(int fd, void *buffer, uint32_t n) {
    uint8_t *ptr = buffer;
    uint32_t left = n;
    int ret;
    while (left > 0) {
        ret = write(fd, ptr, left);
        if (ret < 0) {
            if (errno == EAGAIN)
                continue;
            return -1;
        }
        ptr += ret;
        left -= ret;
    }
    return 0;
}

/**
 * @brief Terminates an established connection with an audio device
 * 
 * @param fd file descriptor
 */
void caudio_close_audio_devide(int fd) {
    close(fd);
}