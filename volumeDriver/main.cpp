#include <iostream>
#include <windows.h>

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
        } else {
            std::cerr << "Fehler beim Lesen vom seriellen Port." << std::endl;
            break;
        }
    }

    // Schließen Sie den seriellen Port
    CloseHandle(serialPort);

    return 0;
}
