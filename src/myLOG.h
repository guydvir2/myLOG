/* This library creates log file daved on Flash memory on ESP8266/ ESP32 */
/* Written by guyDvir */
/* 01/2021 */

#ifndef myLOG_h
#define myLOG_h

#include <Arduino.h>
#include <FS.h>

#define isESP32 false
#define isESP8266 false

#if defined(ESP32)
#include <LITTLEFS.h>
#define isESP32 true
#define LITFS LITTLEFS
#elif defined(ARDUINO_ARCH_ESP8266)
#include <LittleFS.h>
#define isESP8266 true
#define LITFS LittleFS
#endif

class flashLOG
{
private:
    int _maxLOG_entries;
    String _logBuff = "";
    const char _EOL = '\n';
    bool _useDebug = false;
    bool _useDelayedSave = true;
    char *_logfilename = "/logfile.txt";

public:
    char *VeR = "flashLOG v2.2";
    unsigned long lastUpdate = 0;

private:
    bool _write2file();
    bool _chkFileOK(File &_file);
    bool _del_lines(uint8_t line_index);
    bool _delayed_save(uint8_t _savePeriod);
    void _clearBuffer();
    void _printDebug(char *msg);
    void _insert_record_to_buffer(const char *inmsg);
    int _getBuffer_records();

public:
    flashLOG(char *filename = "/logfile.txt");
    void rawPrintfile();
    void looper(uint8_t savePeriod = 10);
    bool delog();
    bool del_line(uint8_t line_index);
    bool readline(uint8_t r, char *retLog);
    void write(const char *message, bool NOW = false);
    bool start(uint8_t max_entries = 10, bool delyedSave = true, bool debugmode = false);
    unsigned long sizelog();
    int get_num_saved_records();
};
#endif
