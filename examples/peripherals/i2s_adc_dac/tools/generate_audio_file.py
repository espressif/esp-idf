import os
import wave
import struct

def get_wave_array_str(filename, target_bits):
    wave_read = wave.open(filename, "r")
    array_str = ""
    nchannels, sampwidth, framerate, nframes, comptype, compname = wave_read.getparams()
    sampwidth *= 8
    for i in range(wave_read.getnframes()):
        val, = struct.unpack("<H", wave_read.readframes(1))
        scale_val = (1 << target_bits) - 1
        cur_lim   = (1 << sampwidth) - 1
        #scale current data to 8-bit data
        val       = val * scale_val / cur_lim
        val       = (val + ((scale_val + 1) / 2)) & scale_val
        array_str += "0x%x, "%(val)
        if (i + 1) % 16 == 0:
            array_str += "\n"
    return array_str

def gen_wave_table(wav_file_list, target_file_name, scale_bits = 8):
    with open(target_file_name, "w") as audio_table:
        print >> audio_table, '#include <stdio.h>'
        print >> audio_table, 'const unsigned char audio_table[] = {'
        for wav in wav_file_list:
            print("processing: {}".format(wav))
            print >> audio_table, get_wave_array_str(filename = wav, target_bits = scale_bits)
        print >>audio_table,'};\n'
    print("Done...")

if __name__=='__main__':
    print("Generating audio array...")
    wav_list = []
    for filename in os.listdir("./"):
        if filename.endswith(".wav"):
            wav_list.append(filename)
    gen_wave_table(wav_file_list = wav_list, target_file_name = "audio_example_file.h")
