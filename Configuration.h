#pragma once

#define SIZE_MEMORY_CODE (65000)
#define SIZE_MEMORY_STATE (65000)
#define SIZE_MEMORY_AUDIO (65000)
#define NAME_MEMORY_CODE ("/code")
#define NAME_MEMORY_STATE ("/state")
#define NAME_MEMORY_AUDIO ("/audio")
#define NAME_SEMAPHORE_COMPILER_A ("/compiler_a")
#define NAME_SEMAPHORE_COMPILER_B ("/compiler_b")
#define NAME_SEMAPHORE_EXECUTER_A ("/executer_a")
#define NAME_SEMAPHORE_EXECUTER_B ("/executer_b")
#define NAME_SEMAPHORE_SUBMIT ("/submit")
#define NAME_SEMAPHORE_COMPILE ("/compile")
#define NAME_SEMAPHORE_SWAP ("/swap")
#define AUDIO_BLOCK_SIZE (128)
#define AUDIO_CHANNEL_OUT (8)
#define AUDIO_CHANNEL_IN (32)
#define AUDIO_OFFSET_IN (AUDIO_BLOCK_SIZE * AUDIO_CHANNEL_OUT)
#define AUDIO_SAMPLERATE (48000.0)
#define AUDIO_SAMPLETIME (1.0/48000.0)


