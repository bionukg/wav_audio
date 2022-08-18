// wav_audio.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
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
    uint32_t* blocksize;//16/18/20/40//这里是18
    uint16_t* format_tag;//1
    uint16_t* soundtrack;//1/2声道
    uint32_t* sampling_rate;//11025000/22050000/44100000采样率
    uint32_t* bitrate;//=soundtrack*sampling_rate*bitdepth/8 数据速率
    uint16_t* dataunit;//=soundtrack*bitdepth/8 采样帧大小
    uint16_t* bitdepth;//4/8/12/16/24/32 位深度
    uint16_t* extrasize;//额外块
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
            printf("data字段有误");
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
    //        printf("\n不是32位音频\n");
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
    //        printf("\n不是32位音频\n");
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
            printf("\n不是8位音频\n");
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
            printf("\n不是8位音频\n");
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
            printf("\n不是16位音频\n");
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
    //        printf("\n不是32位浮点音频\n");
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
            printf("打开\"%s\"失败", path);
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
        *filedatasize = actual_header_length + datasize;//本来+8，但是多出来一个'data'
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

//参考文献(雾)https://www.cnblogs.com/wangguchangqing/p/5970516.html
// 
// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
