﻿#ifndef ist_isd_Stream_h
#define ist_isd_Stream_h

#include "isdUtil.h"

namespace ist {
namespace isd {


class istAPI Stream : public SharedObject
{
public:
    typedef ist::raw_vector<char> DataCont;

public:
    Stream();
    virtual ~Stream();

    virtual size_t getChannels() const=0;
    virtual size_t getSampleRate() const=0;
    virtual size_t getBitsPerSample() const=0;

    virtual size_t size()=0;
    virtual size_t tell()=0;
    virtual void seek(size_t size)=0;
    virtual bool eof()=0;

    virtual ALenum getALFormat() const;

    /// require_size byte 分読み込む 
    virtual DataCont& readByte(size_t require_size)=0;

    /// require_millisecミリ秒分読み込む 
    DataCont& readMillisec(size_t require_millisec);

    /// require_sampleサンプル分読み込む 
    DataCont& readSample(size_t require_sample);
};




class WaveStream : public Stream
{
public:
    struct riff_header {
        char riff[4];
        int size;
        char wave[4];
    };

    struct format_header {
        char fmt[4];
        int size;
        short format_id;
        short channels;
        int sample_rate;
        int bytes_par_second;
        short block_size;
        short bits_per_sample;
    };

    struct data_header {
        char data[4];
        int size;
    };

private:
    FILE *m_file;
    riff_header m_riff;
    format_header m_format;
    data_header m_data;
    DataCont m_tmp;

public:
    WaveStream();
    ~WaveStream();

    bool openStream(const char* path);
    void closeStream();

    size_t getHeaderSize() const;

    size_t size();
    size_t tell();
    void seek(size_t v);
    bool eof();

    size_t getChannels() const;
    size_t getSampleRate() const;
    size_t getBitsPerSample() const;

    DataCont& readByte(size_t require_size);
};

} // namespace isd
} // namespace ist

#endif // ist_isd_Stream_h
