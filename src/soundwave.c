/**
 * @file soundwave.c
 * @author Rafael Dioaltzis
 * @brief The main core code of soundwave
 * @version 0.1
 * @date 2025-12-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include"soundman.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<inttypes.h>

void print_help_message(){
    printf("\nSoundWave - A simple WAV audio utility\n\n");
    printf("Usage ./soundwave <command> [parameters]\n\n"
           "Commands:\n");
    printf("  %-30s%-60s\n", "--help or -h", "displays this help message");
    printf("  %-30s%-60s\n", "info", "display the properties of the wav file");
    printf("  %-30s%-60s\n", "rate <value>", "changes the rate of the wav file");
    printf("  %-30s%-60s\n", "channel <left|right>", "keeps the data from one channel if wav is stereo");
    printf("  %-30s%-60s\n", "volume <value>", "changes the volume of the wav data");
    printf("  %-30s%-60s\n", "generate [options]", "Generate a WAV file with the specified options\n");

    printf("Generate command options:\n");
    printf("  %-30s%-60s\n", "--dur <seconds>", "Duration of the sound (Default: 3)");
    printf("  %-30s%-60s\n", "--sr <rate>", "Sample rate in Hz (Default: 44100)");
    printf("  %-30s%-60s\n", "--fm <modulation>", "Frequency modulation (Default: 2.0)");
    printf("  %-30s%-60s\n", "--fc <carrier>", "Frequency carrier (Default: 1500.0)");
    printf("  %-30s%-60s\n", "--mi <index>", "Modulation index (Default: 100.0)");
    printf("  %-30s%-60s\n", "--amp <amplitude>", "Amplitude (Default: 30000.0)");

}

void parse_args(int argc, char* argv[], short* flag){
    *flag = 0;
    
    if(argc <= 1){
        print_help_message();
        return;
    }

    if(strcmp(argv[1], "info") == 0){
        *flag = 1;
    } 
    else if(strcmp(argv[1], "rate") == 0){
        if(argc < 3){
            printf("Usage: ./soundwave rate <value>\n");
            return;
        }
        *flag = 2;
    } 
    else if(strcmp(argv[1], "channel") == 0){
        if(argc < 3){
            printf("Usage: ./soundwave channel <left|right>\n");
            return;
        }
        *flag = 3;
    } 
    else if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0){
        print_help_message();
    }
    else if(strcmp(argv[1], "volume") == 0){
        if(argc < 3){
            printf("Usage: ./soundwave volume <value>\n");
            return;
        }
        *flag = 4;
    } 
    else if(strcmp(argv[1], "generate") == 0){
        *flag = 5;
    }
    else if(strcmp(argv[1], "dj") == 0){
        *flag = 6;
    }
}

int main(int argc, char* argv[]){
    /*
        0 = N/A
        1 = info
        2 = rate
        3 = channel
        4 = volume
        5 = generate
        6 = dj
    */
    short args_flag = 0;
    short flag = 0; 

    parse_args(argc, argv, &args_flag);

    if(args_flag == 0){
        flag = 1;
    } 
    else if(args_flag == 1){
        info_command(&flag);
    } 
    else if(args_flag == 2){
        char* endptr;
        double rate = strtod(argv[2], &endptr);
        if(*endptr == *argv[2] || errno == ERANGE || *endptr != '\0'){
            fprintf(stderr, "Warning: Something unexpected occured while parsing the value of the rate argument. This might lead to unexpected behavior\n");
            errno = 0;
        }
        srate_command(rate, &flag);
    }
    else if(args_flag == 3){
        short channel;
        if(strcmp(argv[2], "left") == 0){
            channel = 0;
        } else if(strcmp(argv[2], "right") == 0){
            channel = 1;
        } else{
            print_help_message();
            return 1;
        }
        schannel_command(channel, &flag);
    }
    else if(args_flag == 4){
        double volume = safe_StrToDouble(argv[2]);
        svolume_command(volume, &flag);
    }
    else if(args_flag == 5){
        int duration = 3;
        int sample_rate = 44100;
        double frequency_modulation = 2.0;
        double carrier_frequency = 1500.0;
        double modulation_index = 100.0;
        double amplitude = 30000.0;

        for(int i = 2; i < argc; i++){
            if(strcmp(argv[i], "--dur") == 0){
                if(i+1 >= argc){
                    fprintf(stderr, "Error: in command generate the parameter %s has no value\n", argv[i]);
                    return 1;
                }
                i++;
                duration = (int)safe_StrToDouble(argv[i]);
            }
            else if(strcmp(argv[i], "--sr") == 0){
                if(i+1 >= argc){
                    fprintf(stderr, "Error: in command generate the parameter %s has no value\n", argv[i]);
                    return 1;
                }
                i++;
                sample_rate = (int)safe_StrToDouble(argv[i]);
            }
            else if(strcmp(argv[i], "--fm") == 0){
                if(i+1 >= argc){
                    fprintf(stderr, "Error: in command generate the parameter %s has no value\n", argv[i]);
                    return 1;
                }
                i++;
                frequency_modulation = safe_StrToDouble(argv[i]);
            }
            else if(strcmp(argv[i], "--fc") == 0){
                if(i+1 >= argc){
                    fprintf(stderr, "Error: in command generate the parameter %s has no value\n", argv[i]);
                    return 1;
                }
                i++;
                carrier_frequency = safe_StrToDouble(argv[i]);
            }
            else if(strcmp(argv[i], "--mi") == 0){
                if(i+1 >= argc){
                    fprintf(stderr, "Error: in command generate the parameter %s has no value\n", argv[i]);
                    return 1;
                }
                i++;
                modulation_index = safe_StrToDouble(argv[i]);
            }
            else if(strcmp(argv[i], "--amp") == 0){
                if(i+1 >= argc){
                    fprintf(stderr, "Error: in command generate the parameter %s has no value\n", argv[i]);
                    return 1;
                }
                i++;
                amplitude = safe_StrToDouble(argv[i]);
            } else{
                fprintf(stderr, "Warning: undefined parameter %s in the generate command\n", argv[i]);
            }
        }
        mysound(duration, sample_rate, frequency_modulation, carrier_frequency, modulation_index, amplitude);
    }
    else if(args_flag == 6){
        flag = play_sound() == 0 ? 0u : 1u;
    }

    if(flag == 1){
        return 1;
    }

    return 0;
}