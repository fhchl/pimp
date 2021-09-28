#!/bin/bash

scp -B ../build/test_AudioBuf root@bela.local:/root/pimp/build/
scp -B ../build/test_BlockLMSFilter root@bela.local:/root/pimp/build/
scp -B ../build/test_LMSFilter root@bela.local:/root/pimp/build/
scp -B ../build/test_RLSFilter root@bela.local:/root/pimp/build/
scp -B ../build/test_fft root@bela.local:/root/pimp/build/

ssh bela.local


