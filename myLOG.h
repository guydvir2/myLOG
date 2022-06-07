/* This library creates log file daved on Flash memory on ESP8266/ ESP32 */
/* Written by guyDvir */
/* 01/2021 */

#ifndef myLOG_h
#define myLOG_h
#include <FS.h>

#if defined(ESP32)
#include <LITTLEFS.h>
#define isESP32 true
#define isESP8266 false
#define LITFS LITTLEFS
#elif defined(ARDUINO_ARCH_ESP8266)
#include <LittleFS.h>
#define isESP32 false
#define isESP8266 true
#define LITFS LittleFS
#endif

class flashLOG
{
private:
    const char _EOL = '\n';
    char *_logfilename = "/logfile.txt";
    uint8_t _logSize;
    bool _useDebug = false;
    bool _useDelayedSave = true;
    String _logBuff = "";
    
public:
    char *VeR = "flashLOG v2.1";
    unsigned long lastUpdate = 0;

private:
    bool _write2file();
    bool _chkFileOK(File &_file);
    bool _del_lines(uint8_t line_index);
    bool _delayed_save(uint8_t _savePeriod);
    void _emptyBuffer();
    void _printDebug(char *msg);
    void _insert_record_to_buffer(const char *inmsg);
    int _getBuffer_records();
    String _getBuffer_line(int requested_line);

public:
    flashLOG(char *filename = "/logfile.txt");
    bool start(uint8_t max_entries = 10, bool delyedSave = true, bool debugmode = false);
    void write(const char *message, bool NOW = false);
    bool readline(uint8_t r, char *retLog);
    void looper(uint8_t savePeriod = 10);
    void rawPrintfile();
    bool delog();
    bool del_line(uint8_t line_index);
    unsigned long sizelog();
    int getnumlines();
};
#endif
