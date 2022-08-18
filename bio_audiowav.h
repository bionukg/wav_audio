// wav_audio.cpp : ���ļ����� "main" ����������ִ�н��ڴ˴���ʼ��������
//
#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
const auto PI = acos(-1.0);

class wav
{
#define wav_headerlength 0x26
private:
    uint8_t* header;
    uint32_t* filetag;//"RIFF"
    uint32_t* filedatasize;//+8==filesize
    uint32_t* format;//"WAVE"
    uint32_t* formatag;//"fmt "
    uint32_t* blocksize;//16/18/20/40//������18
    uint16_t* format_tag;//1
    uint16_t* soundtrack;//1/2����
    uint32_t* sampling_rate;//11025000/22050000/44100000������
    uint32_t* bitrate;//=soundtrack*sampling_rate*bitdepth/8 ��������
    uint16_t* dataunit;//=soundtrack*bitdepth/8 ����֡��С
    uint16_t* bitdepth;//4/8/12/16/24/32 λ���
    uint16_t* extrasize;//�����
public:
    uint8_t* extra = 0;
    uint32_t datastart = 0;
    uint8_t* data = 0;
    uint32_t datasize = 0;

    uint32_t inline _4chars(const char str[5])
    {
        return *(uint32_t*)str;
    }
    wav(void)
    {
        header = new uint8_t[wav_headerlength];

        filetag = (uint32_t*)&header[0];
        *filetag = _4chars("RIFF");
        filedatasize = (uint32_t*)&header[0x04];
        format = (uint32_t*)&header[0x08];
        *format = _4chars("WAVE");
        formatag = (uint32_t*)&header[0x0c];
        *formatag = _4chars("fmt ");
        blocksize = (uint32_t*)&header[0x10];
        format_tag = (uint16_t*)&header[0x14];
        soundtrack = (uint16_t*)&header[0x16];
        sampling_rate = (uint32_t*)&header[0x18];
        bitrate = (uint32_t*)&header[0x1c];
        dataunit = (uint16_t*)&header[0x20];
        bitdepth = (uint16_t*)&header[0x22];
        extrasize = (uint16_t*)&header[0x24];
        datastart = _4chars("data");
        data = 0;
        datasize = 0;
    }
    ~wav(void)
    {
        delete[] header;
        if (data != 0)
            delete[] data;
        if (extra != 0)
            delete[] extra;
    }
    wav(const char* path)
    {
        FILE* fp = fopen(path, "rb+");
        if (fp == 0)
        {
            std::cout << "unknown file" << path << "\n";
            system("pause");
            return;
        }
        header = new uint8_t[wav_headerlength];
        //std::cout << "header:" << 
        fread(header, 1, wav_headerlength - 2, fp);
        filetag = (uint32_t*)&header[0];
        //*filetag = _4chars("RIFF");
        filedatasize = (uint32_t*)&header[0x04];
        format = (uint32_t*)&header[0x08];
        //*format = _4chars("WAVE");
        formatag = (uint32_t*)&header[0x0c];
        //*formatag = _4chars("fmt ");
        blocksize = (uint32_t*)&header[0x10];
        extrasize = (uint16_t*)&header[0x24];
        switch (*blocksize)
        {
        case 16:
            *extrasize = 0;
            break;
        case 18:
            fread(extrasize, 1, 2, fp);

            extra = new uint8_t[*extrasize];
            //std::cout << "extra:" << 
            fread(extra, 1, *extrasize, fp);//<< "\n";

            break;
        default:
            std::cout << "unknown blocksize type";
            system("pause");
            delete[] header;

            return;
            break;
        }
        format_tag = (uint16_t*)&header[0x14];
        soundtrack = (uint16_t*)&header[0x16];
        sampling_rate = (uint32_t*)&header[0x18];
        bitrate = (uint32_t*)&header[0x1c];
        dataunit = (uint16_t*)&header[0x20];
        bitdepth = (uint16_t*)&header[0x22];


        fread(&datastart, 4, 1, fp);
        if (datastart - *(uint32_t*)&"data" != 0)
        {
            printf("data�ֶ�����");
            system("pause");
        }
        datasize = *filedatasize - wav_headerlength + 4 - *extrasize;
        data = (uint8_t*)new int8_t[datasize];
        //std::cout << "data:" << 
        fread(data, 1, datasize, fp);// << "\n";
    }
    wav(wav& in)
    {
        header = new uint8_t[wav_headerlength];
        memcpy(header, in.header, wav_headerlength);

        filetag = (uint32_t*)&header[0];
        *filetag = _4chars("RIFF");
        filedatasize = (uint32_t*)&header[0x04];
        format = (uint32_t*)&header[0x08];
        *format = _4chars("WAVE");
        formatag = (uint32_t*)&header[0x0c];
        *formatag = _4chars("fmt ");
        blocksize = (uint32_t*)&header[0x10];
        format_tag = (uint16_t*)&header[0x14];
        soundtrack = (uint16_t*)&header[0x16];
        sampling_rate = (uint32_t*)&header[0x18];
        bitrate = (uint32_t*)&header[0x1c];
        dataunit = (uint16_t*)&header[0x20];
        bitdepth = (uint16_t*)&header[0x22];
        extrasize = (uint16_t*)&header[0x24];

        if (in.extra != 0)
        {
            extra = new uint8_t[*extrasize];
            memcpy(extra, in.extra, *extrasize);
        }
        datastart = in.datastart;
        datasize = in.datasize;
        data = new uint8_t[datasize];
        memcpy(data, in.data, datasize);

    }
    void init_std_wav(void) //65536Hz,8bit
    {
        init_wav(1, 0x10000, 8);
        return;
    }
    void init_wav(uint16_t _soundtrack, uint32_t _sampling_rate, uint16_t _bitdepth)
    {
        *blocksize = 18;
        *format_tag = 1;
        *soundtrack = _soundtrack;
        *sampling_rate = _sampling_rate;
        *bitdepth = _bitdepth;
        *dataunit = _soundtrack * _bitdepth / 8;
        *bitrate = *sampling_rate * *dataunit;
        *extrasize = 0;
        return;
    }
    //void fill_wav32(int32_t(*f_t)(uint32_t), uint32_t sampleing_point_count)
    //{
    //    if (*bitdepth != 32)
    //    {
    //        printf("\n����32λ��Ƶ\n");
    //        system("pause");
    //    }
    //    data = (uint8_t*)new int32_t[sampleing_point_count];
    //    datasize = *bitdepth / 8;
    //    datasize *= sampleing_point_count;
    //    
    //    uint32_t i = 0;
    //    
    //    for (i = 0; i < sampleing_point_count; i++)
    //    {
    //        *(int32_t*)&data[i] = f_t(i);
    //    }
    //    return;
    //}
    //void fill_wav32_double(double(*f_t)(double), time_t t)
    //{
    //    if (*bitdepth != 32)
    //    {
    //        printf("\n����32λ��Ƶ\n");
    //        system("pause");
    //    }
    //    uint64_t sampleing_point_count = t * *sampling_rate;
    //    data = (uint8_t*)new int32_t[sampleing_point_count];
    //    datasize = *bitdepth / 8;
    //    datasize *= sampleing_point_count;
    //    double time = 0.0,value=0.0;
    //    uint32_t i = 0;
    //    for (i = 0; i < sampleing_point_count; i++)
    //    {
    //        time = ((double)i) / *sampling_rate;
    //        value = f_t(time)*INT32_MAX;
    //        *(int32_t*)&data[i]=(int32_t)value;
    //    }
    //    return;
    //}
    void fill_wav8(int8_t(*f_t)(uint32_t), uint32_t sampleing_point_count)
    {
        if (*bitdepth != 8)
        {
            printf("\n����8λ��Ƶ\n");
            system("pause");
        }
        data = (uint8_t*)new int8_t[sampleing_point_count];
        datasize = *bitdepth / 8;
        datasize *= sampleing_point_count;

        uint32_t i = 0;

        for (i = 0; i < sampleing_point_count; i++)
        {
            *(int8_t*)&data[i] = f_t(i);
        }
        return;
    }
    void fill_wav8_double(double(*f_t)(double), time_t t)
    {
        if (*bitdepth != 8)
        {
            printf("\n����8λ��Ƶ\n");
            system("pause");
        }
        uint64_t sampleing_point_count = t * *sampling_rate;
        data = (uint8_t*)new int8_t[sampleing_point_count];
        datasize = *bitdepth / 8;
        datasize *= sampleing_point_count;
        double time = 0.0, value = 0.0;

        uint32_t i = 0;

        for (i = 0; i < sampleing_point_count; i++)
        {
            time = ((double)i) / *sampling_rate;
            value = f_t(time) * INT8_MAX;
            *(int8_t*)&data[i] = (int8_t)value;
        }
        return;
    }
    void fill_wav16_double(double(*f_t)(double), time_t t)
    {
        if (*bitdepth != 16)
        {
            printf("\n����16λ��Ƶ\n");
            system("pause");
        }
        uint64_t sampleing_point_count = t * *sampling_rate;
        data = (uint8_t*)new int16_t[sampleing_point_count];
        datasize = *bitdepth / 8;
        datasize *= sampleing_point_count;
        double time = 0.0, value = 0.0;

        uint32_t i = 0;

        for (i = 0; i < sampleing_point_count; i++)
        {
            time = ((double)i) / *sampling_rate;
            value = f_t(time) * INT16_MAX;
            *(int16_t*)&data[2 * i] = (int16_t)value;
        }
        return;
    }

    //void fill_wavfloat_double(float(*f_t)(double), time_t t)
    //{
    //    if (*bitdepth != 32)
    //    {
    //        printf("\n����32λ������Ƶ\n");
    //        system("pause");
    //    }
    //    uint64_t sampleing_point_count = t * *sampling_rate;
    //    data = (uint8_t*)new int8_t[sampleing_point_count];
    //    datasize = *bitdepth / 8;
    //    datasize *= sampleing_point_count;
    //    double time = 0.0, value = 0.0;
    //    uint32_t i = 0;
    //    for (i = 0; i < sampleing_point_count; i++)
    //    {
    //        time = ((double)i) / *sampling_rate;
    //        value = f_t(time) * INT8_MAX;
    //        *(int8_t*)&data[i] = (int8_t)value;
    //    }
    //    return;
    //}
    void save_wav(const char* path)
    {
        int actual_header_length = wav_headerlength;
        FILE* fp = 0;
        fp = fopen(path, "wb+");
        if (fp == 0)
        {
            printf("��\"%s\"ʧ��", path);
            system("pause");
            return;
        }
        switch (*blocksize)
        {
        case 16:
            actual_header_length -= 2;
            break;

        case 18:
            break;

        default:
            std::cout << "unknown blocksize type";
            system("pause");
            return;
            break;
        }
        *filedatasize = actual_header_length + datasize;//����+8�����Ƕ����һ��'data'
        fseek(fp, 0, 0);
        *extrasize = 0;

        fwrite(header, 1, actual_header_length, fp);
        fwrite("data", 1, 4, fp);
        fwrite(&datasize, sizeof(datasize), 1, fp);
        fwrite(data, 1, datasize, fp);
        fclose(fp);
        return;
    }
};

//�ο�����(��)https://www.cnblogs.com/wangguchangqing/p/5970516.html
// 
// ���г���: Ctrl + F5 ����� >����ʼִ��(������)���˵�
// ���Գ���: F5 ����� >����ʼ���ԡ��˵�

// ����ʹ�ü���: 
//   1. ʹ�ý��������Դ�������������/�����ļ�
//   2. ʹ���Ŷ���Դ�������������ӵ�Դ�������
//   3. ʹ��������ڲ鿴���������������Ϣ
//   4. ʹ�ô����б��ڲ鿴����
//   5. ת������Ŀ��>���������Դ����µĴ����ļ�����ת������Ŀ��>�����������Խ����д����ļ���ӵ���Ŀ
//   6. ��������Ҫ�ٴδ򿪴���Ŀ����ת�����ļ���>���򿪡�>����Ŀ����ѡ�� .sln �ļ�
