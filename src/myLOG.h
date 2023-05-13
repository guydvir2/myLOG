/* This library creates log file daved on Flash memory on ESP8266/ ESP32 */
/* Written by guyDvir */
/* 01/2021 */

#ifndef myLOG_h
#define myLOG_h

#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>

class flashLOG
{
    bool _useDebug = false;

#ifndef PRNt
#define PRNt(a) \
    if (_useDebug)      \
    Serial.print(a)
#endif
#ifndef PRNtl
#define PRNtl(a) \
    if (_useDebug)       \
    Serial.println(a)
#endif

private:
    int _maxLOG_entries;
    String _logBuff = "";
    const char _EOL = '\n';
    bool _useDelayedSave = true;
    const char *_logfilename = "/logfile1234567.txt";

public:
    const char *VeR = "flashLOG v2.5a";
    unsigned long lastUpdate = 0;

private:
    bool _write2file();
    bool _chkFileOK(File &_file);
    bool _del_lines(int line_index);
    bool _delayed_save(uint8_t _savePeriod);
    void _clearBuffer();
    void _printDebug(char *msg);
    void _insert_record_to_buffer(const char *inmsg);
    int _getBuffer_records();

public:
    flashLOG(const char *filename = "/logfile.txt");
    void rawPrintfile();
    void looper(uint8_t savePeriod = 10);
    bool delog();
    bool del_line(int line_index);
    bool readline(int r, char *retLog);
    void write(const char *message, bool NOW = false);
    bool start(int max_entries = 10, bool delyedSave = true, bool debugmode = false);
    unsigned long sizelog();
    int get_num_saved_records();
};
#endif
