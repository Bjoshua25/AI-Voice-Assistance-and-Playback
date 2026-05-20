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


# ------ CONFIGURATION ------
FS = 16000
CHANNELS = 1
SESSION_DURATION = 300     # 5 minutes


# RECORDING FUNCTION
def record_sample (label):

    # create folder to save the audios
    label_dir = data_dir / label
    label_dir.mkdir(parents=True, exist_ok=True)

    # start recording count-down
    for i in range(3, 0, -1):
        print(f"{i}...", end=" ", flush=True)
        time.sleep(0.7)
        
    print(f"\nRECORDING...")


    # Start Recording using Sounddevice
    recording = sd.rec(int(FS * SESSION_DURATION), samplerate= FS, dtype='int16', channels=CHANNELS)

    # Simple progress bar so you don't get bored or stop early
    for second in range(SESSION_DURATION):
        time.sleep(1)
        if second % 10 == 0: # Print every 10 seconds
            print(f"Time elapsed: {second}s / {SESSION_DURATION}s")

            
    sd.wait()
    print("Done....")

    # filename in timestamp
    filename = f"{label}_{int(time.time() * 1000)}.wav"
    filepath = label_dir / filename

    # write the recorded array into the file
    write(filepath, FS, recording)
    print(f"Saved to: {filepath}") 



if __name__ == "__main__":
    current_label = input("Enter the Label name. eg.('hi', 'momas', 'noise', 'unknown'): ").strip().lower()
    
    try:
        while True:
            record_sample(current_label)
            cont = input("\nPress Enter to record another, or 'new' to enter a new label, or 'exit' to exit the program: ").strip().lower()

            if cont == 'exit':
                break
            elif cont == 'new':
                current_label= input("Enter Label name:  ").strip().lower()
    
    except KeyboardInterrupt:
        print("\nStopping...")

