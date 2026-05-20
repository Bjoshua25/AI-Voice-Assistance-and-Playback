# IMPORTS
import sounddevice as sd
import numpy as np
from scipy.io.wavfile import write
from pathlib import Path
import os
import time


# root directory
root_dir = Path().cwd()

# data directory
data_dir = root_dir / "my_robot_dataset"
print(data_dir)

# ----------- CONFIGURATION -------------
FS = 16000          # Sample rate
DURATION = 1.0      
CHANNELS = 1        # Mono
DATASET_DIR = data_dir


# ----------- RECORD AUDIO FUNCTION ----------
def record_audio(label):

    # create folder for each audio class
    path = DATASET_DIR / label
    path.mkdir(parents= True, exist_ok = True)
    print(F"Ready to record for label: {label}")

    # Create Countdown to show when to start recording
    for i in range(3, 0, -1):
        print(f"{i}...", end= " ", flush= True)
        time.sleep(0.7)

    print("\nRECORDING... ")

    # record the audio using sounddevice library
    recording = sd.rec(int(DURATION * FS), samplerate= FS, channels=1, dtype= 'int16')
    sd.wait()
    print("DONE")

    # create audio file based on timestamp
    filename = path / f"{label}_{int(time.time() * 1000)}.wav"
    filepath = path / filename

    # write the audio recording to the file
    write(filepath, FS, recording)
    print(f"Saved to: {filepath}")


if __name__ == "__main__":
    current_label = input("Enter label to record (eg. hi, momas, noise): ").strip().lower()

    try:
        while True:
            record_audio(current_label)
            cont = input("\nPress Enter to Record Again, or type 'new' to change label, or 'exit':  ").strip().lower()
            
            if cont == 'exit':
                break
            elif cont == 'new':
                current_label = input("Enter new label:  ").strip().lower()

    except KeyboardInterrupt:
        print("\nStopping...")
        