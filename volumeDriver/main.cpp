#include <iostream>
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

void alterSystemVolume(int mode);

int main() {
    HANDLE serialPort = CreateFile(
            "COM6",             // Ändern Sie dies entsprechend Ihrem Port, z.B. "COM1", "COM2", usw.
            GENERIC_READ,       // Nur Leserechte
            0,                  // Keine gemeinsame Nutzung
            nullptr,            // Keine Sicherheitsattribute
            OPEN_EXISTING,      // Öffnen Sie nur einen vorhandenen Port
            FILE_ATTRIBUTE_NORMAL,
            nullptr             // Kein Template-Handle
    );

    if (serialPort == INVALID_HANDLE_VALUE) {
        std::cerr << "Fehler beim Öffnen des seriellen Ports." << std::endl;
        return -1;
    }

    // Konfigurieren Sie die serielle Schnittstelle
    DCB serialParams = { 0 };
    serialParams.DCBlength = sizeof(serialParams);
    if (!GetCommState(serialPort, &serialParams)) {
        std::cerr << "Fehler beim Abrufen des Zustands der seriellen Schnittstelle." << std::endl;
        CloseHandle(serialPort);
        return -1;
    }

    serialParams.BaudRate = CBR_9600;     // Baudrate auf 9600 setzen
    serialParams.ByteSize = 8;             // 8 Datenbits
    serialParams.StopBits = ONESTOPBIT;    // 1 Stoppbit
    serialParams.Parity = NOPARITY;        // Keine Parität

    if (!SetCommState(serialPort, &serialParams)) {
        std::cerr << "Fehler beim Einstellen des Zustands der seriellen Schnittstelle." << std::endl;
        CloseHandle(serialPort);
        return -1;
    }

    // Lesen Sie Daten vom seriellen Port
    char buffer[4];
    DWORD bytesRead;

    while (true) {
        if (ReadFile(serialPort, buffer, sizeof(buffer), &bytesRead, nullptr)) {
            std::string data = std::string(buffer, bytesRead);
            std::cout << "Gelesene Daten: " << data << std::endl;

            if (data == "POS+") {
                alterSystemVolume(1);
            }else if (data == "POS-") {
                alterSystemVolume(-1);
            }else if (data == "PUSH") {
                alterSystemVolume(0);
            }
        } else {
            std::cerr << "Fehler beim Lesen vom seriellen Port." << std::endl;
            break;
        }
    }

    // Schließen Sie den seriellen Port
    CloseHandle(serialPort);

    return 0;
}

void alterSystemVolume(int mode) {
    // Initialisieren Sie COM (Component Object Model)
    CoInitialize(nullptr);

    // Erhalten Sie den Audiogeräte-Manager
    IMMDeviceEnumerator* deviceEnumerator = nullptr;
    CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&deviceEnumerator);

    // Erhalten Sie das Standard-Audiogerät (Wiedergabe)
    IMMDevice* defaultDevice = nullptr;
    deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);

    // Erhalten Sie das Audio-Endpunkt-Volume-Objekt
    IAudioEndpointVolume* endpointVolume = nullptr;
    defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, (void**)&endpointVolume);

    // Aktuelle Lautstärke erhalten
    float currentVolume;
    BOOL isMute;
    endpointVolume->GetMute(&isMute);
    endpointVolume->GetMasterVolumeLevelScalar(&currentVolume);
    std::cout << "Current Volume: " << currentVolume << std::endl;

    // Modi abfragen und entsprechende Aktionen ausführen
    if (mode == 0) {
        if (isMute == FALSE) {
            endpointVolume->SetMute(TRUE, nullptr);
            isMute = TRUE;
        }else {
            endpointVolume->SetMute(FALSE, nullptr);
            isMute = FALSE;
        }
    }else {
        float newVolume;
        if (mode == 1) {
            newVolume = currentVolume + 0.01;
        }else if (mode == -1) {
            newVolume = currentVolume - 0.01;
        }
        endpointVolume->SetMasterVolumeLevelScalar(newVolume, nullptr);
    }


    // Warten Sie einen Moment, um die Änderung zu bemerken
    //Sleep(500);

    // Aufräumen
    defaultDevice->Release();
    deviceEnumerator->Release();
    endpointVolume->Release();

    // Beenden Sie COM
    CoUninitialize();
}
