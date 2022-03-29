// wav_audio.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#define PI 3.14159
uint32_t _4chars(const char str[5])
{
    return *(uint32_t*)str;
}



double deltat = 0,tl=0;
int cont = 0;

void _10000print(double p)
{
    if (cont < 10000)
{
    cont++;
}
else
{
    cont = 0;
    printf("%f\t", p);
}
}
class wav
{
#define wav_header_length 0x26
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
    

    
    uint8_t* data;
    uint32_t datasize;
public:

    wav(void)
    {
        header = new uint8_t[wav_header_length];

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
        data = 0;
        datasize = 0;
    }
    ~wav(void)
    {
        delete header;
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
        *dataunit = _soundtrack* _bitdepth/8;
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
            *(int16_t*)&data[2*i] = (int16_t)value;
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
        FILE* fp=0;
        fp = fopen(path, "wb+");
        if (fp == 0)
        {
            printf("打开\"%s\"失败", path);
            system("pause");
            return;
        }
        *filedatasize = wav_header_length + datasize;
        fseek(fp, 0, 0);
        fwrite(header, 1, wav_header_length, fp);
        fwrite("data", 1, 4, fp);
        fwrite(&datasize, sizeof(datasize), 1, fp);
        fwrite(data, 1, datasize, fp);
        fclose(fp);
        return;
    }
};
double mic(double time, double hei)//define 256Hz==Do音==hei=0
{
    if (hei > 37)
        system("pause");
    return sin(256 * PI * time  * pow(2, hei / 12));
}
double poow(double l, double r)
{
    double re = pow(l, r);
    _10000print(re);
    return re;
}
double snd(double time)//你的音频的函数
{
    return sin(pow(1.2, time) * 512);
}
int main()
{
    wav* w = new wav;
    w->init_wav(1, 0x10000, 16);
    w->fill_wav16_double(snd, 60);
    w->save_wav("0.wav");
    system("0.wav");
}
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
