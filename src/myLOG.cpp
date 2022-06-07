#include "myLOG.h"

flashLOG::flashLOG(char *filename)
{
    _logfilename = filename;
}
bool flashLOG::start(uint8_t max_entries, bool delyedSave, bool debugmode)
{
    _maxLOG_entries = max_entries;
    _useDelayedSave = delyedSave;
    _useDebug = debugmode;

#if isESP32
    bool a = LITFS.begin(true);
#elif isESP8266
    bool a = LITFS.begin();
#endif

    if (!a)
    {
        if (_useDebug)
        {
            Serial.println("LittleFS mount failed");
        }
    }
    return a;
}
void flashLOG::looper(uint8_t savePeriod)
{
    if (_delayed_save(savePeriod))
    {
        _write2file();
    }
}
void flashLOG::write(const char *message, bool NOW)
{
    _insert_record_to_buffer(message);

    if (NOW == true || !_useDelayedSave)
    {
        _write2file();
    }
    else
    {
        lastUpdate = millis(); // Making sure update will be in delay to request.
    }
}
void flashLOG::rawPrintfile()
{
    int row_counter = 0;
    File file = LITFS.open(_logfilename, "r");
    if (!file)
    {
        if (_useDebug)
        {
            Serial.println("Failed to open file for reading");
        }
    }

    Serial.print("~~~ Saved in ");
    Serial.print(_logfilename);
    Serial.println(" ~~~");

    while (file.available())
    {
        String line = file.readStringUntil(_EOL);
        String lineFormat = "row #" + String(row_counter) + " {" + line + "}";
        Serial.println(lineFormat);
        row_counter++;
    }

    Serial.println("~~~ EOF ~~~");
    file.close();
}
bool flashLOG::del_line(uint8_t line_index)
{
    uint8_t row_counter = 0;
    char *tfile = "/tempfile.txt";
    bool line_deleted = false;
    File file1 = LITFS.open(_logfilename, "r");
    File file2 = LITFS.open(tfile, "w");

    if (file1 && file2)
    {
        while (file1.available())
        {
            String line = file1.readStringUntil(_EOL);
            if (line_index != row_counter)
            {
                file2.println(line);
            }
            else
            {
                line_deleted = true;
            }
            row_counter++;
        }
    }
    else
    {
        if (_useDebug)
        {
            Serial.println("Fail open files");
        }
    }
    file1.close();
    file2.close();
    LITFS.remove(_logfilename);
    LITFS.rename(tfile, _logfilename);
    return line_deleted;
}
bool flashLOG::readline(uint8_t r, char retLog[])
{
    uint8_t row_counter = 0;
    File file = LITFS.open(_logfilename, "r");

    if (file)
    {
        while (file.available() || row_counter != r)
        {
            String line = file.readStringUntil(_EOL);
            if (row_counter == r)
            {
                int len = line.length() + 1;
                line.toCharArray(retLog, len);
                file.close();
                return 1;
            }
            row_counter++;
        }
    }
    else
    {
        if (_useDebug)
        {
            Serial.println("Fail open file");
        }
        file.close();
        return 0;
    }
}
bool flashLOG::delog()
{
    return LITFS.remove(_logfilename);
}
int flashLOG::get_num_saved_records()
{
    int row_counter = 0;
    File file = LITFS.open(_logfilename, "r");

    if (file)
    {
        while (file.available())
        {
            char ch = file.read();
            if (ch == _EOL)
            {
                row_counter++;
            }
            // file.readStringUntil(_EOL);
            // row_counter++;
        }
    }
    file.close();
    return row_counter;
}
unsigned long flashLOG::sizelog()
{
    File file = LITFS.open(_logfilename, "r");

    unsigned long f = file.size();
    file.close();
    return f;
}
bool flashLOG::_delayed_save(uint8_t _savePeriod)
{
    bool timeCondition = lastUpdate > 0 && millis() > _savePeriod * 1000UL + lastUpdate;
    bool overSize_Condition = _logBuff.length() > 800; // About 6 entries in buffer
    return timeCondition || overSize_Condition;
}
bool flashLOG::_chkFileOK(File &_file)
{
    if (!_file)
    {
        if (_useDebug)
        {
            Serial.println("Failed to open file for appending");
        }
        return 0;
    }
    else
    {
        return 1;
    }
}
void flashLOG::_clearBuffer()
{
    _logBuff = "";
}
void flashLOG::_insert_record_to_buffer(const char *inmsg)
{
    _logBuff += String(inmsg) + _EOL;
}
int flashLOG::_getBuffer_records()
{
    int _end = 0;
    int _start = 0;
    int lineCounter = 0;

    while (_end < _logBuff.length() && _logBuff.length() > 0)
    {
        _end = _logBuff.indexOf(_EOL, _start);
        if (_end > _start)
        {
            lineCounter++;
            _start = _end + 1;
        }
    }
    return lineCounter;
}
bool flashLOG::_write2file()
{
    bool _line_added = false;
    int _m = _getBuffer_records();           // Lines stored in buffer
    int num_lines = get_num_saved_records(); // entries stored
    if (_maxLOG_entries - 1 < num_lines + _m)
    {
        _del_lines(num_lines + _m + 1 - _maxLOG_entries);
    }

    File file1 = LITFS.open(_logfilename, "a+");

    int _start = 0;

    if (_chkFileOK(file1))
    {
        for (int x = 0; x < _m; x++)
        {
            int _end = _logBuff.indexOf(_EOL, _start);
            file1.println(_logBuff.substring(_start, _end));
            _start = _end + 1;
            _line_added = true;
        }
        _clearBuffer();
        lastUpdate = 0;
    }
    file1.close();
    if (_useDebug)
    {
        rawPrintfile();
    }
    return _line_added;
}
bool flashLOG::_del_lines(uint8_t line_index)
{
    uint8_t row_counter = 0;
    char *tfile = "/tempfile.txt";
    bool _delted_lines = false;
    File file1 = LITFS.open(_logfilename, "r");
    File file2 = LITFS.open(tfile, "w");

    if (_chkFileOK(file1) && _chkFileOK(file2))
    {
        while (file1.available())
        {
            String line = file1.readStringUntil(_EOL);
            row_counter++;
            if (row_counter >= line_index) /* copying non-deleted lines */
            {
                file2.println(line);
                _delted_lines = true;
            }
        }
    }
    file1.close();
    file2.close();
    LITFS.remove(_logfilename);
    LITFS.rename(tfile, _logfilename);
    return _delted_lines;
}
void flashLOG::_printDebug(char *msg)
{
    if (_useDebug)
    {
        Serial.print(_logfilename);
        Serial.print(": ");
        Serial.println(msg);
    }
}