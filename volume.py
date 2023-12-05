import serial
from ctypes import cast, POINTER
from comtypes import CLSCTX_ALL
from pycaw.pycaw import AudioUtilities, IAudioEndpointVolume


def get_system_volume():
    devices = AudioUtilities.GetSpeakers()
    interface = devices.Activate(
        IAudioEndpointVolume._iid_, CLSCTX_ALL, None)
    volume_interface = cast(interface, POINTER(IAudioEndpointVolume))
    return volume_interface.GetMasterVolumeLevelScalar()

def set_system_volume(volume):
    devices = AudioUtilities.GetSpeakers()
    interface = devices.Activate(
        IAudioEndpointVolume._iid_, CLSCTX_ALL, None)
    volume_interface = cast(interface, POINTER(IAudioEndpointVolume))
    volume_interface.SetMasterVolumeLevelScalar(volume, None)


def mute_system_volume():
    devices = AudioUtilities.GetSpeakers()
    interface = devices.Activate(
        IAudioEndpointVolume._iid_, CLSCTX_ALL, None)
    volume_interface = cast(interface, POINTER(IAudioEndpointVolume))
    volume_interface.SetMute(1, None)

def unmute_system_volume():
    devices = AudioUtilities.GetSpeakers()
    interface = devices.Activate(
        IAudioEndpointVolume._iid_, CLSCTX_ALL, None)
    volume_interface = cast(interface, POINTER(IAudioEndpointVolume))
    volume_interface.SetMute(0, None)


ser = serial.Serial('COM6', 9600);
mute = False;

while True:
    if ser.in_waiting > 0:
        data = ser.readline().decode('utf-8').rstrip()
        print("Received from Arduino:", data)
        if data == "clicked":
            if mute == False:
                mute_system_volume();
                mute = True;
            else:
                unmute_system_volume();
                mute = False;
        
        if data == "POSPLUS":
            try:
                set_system_volume(get_system_volume() + 0.01);
            except:
                print("Volume at max");
        elif data == "POSMINUS":
            try:
                set_system_volume(get_system_volume() - 0.01);
            except:
                print("Volume at min");