#pragma once
#include <mutex>
struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;

//释放pkt空间
//@param pkt 要被释放的pkt
extern void FQFFreePacket(AVPacket **pkt);

class FQFDemux
{
public:
    //pkt的格式
    typedef enum {
        AudioPacket,    //音频数据
        OtherPacket,    //其他数据
        NullPacket      //空
    }PacketType;
    FQFDemux();
    virtual ~FQFDemux();
    //打开媒体文件，打开前会执行关闭操作
    //@param path 要打开的文件的路径
    //@return bool 打开结果
    virtual bool openFile(const char *path);
    //关闭文件，会关闭输入流，同时会将视频参数恢复初始值
    virtual void closeFile();
    //清理空间，清理读取缓冲
    virtual void clear();
    //读取文件
    //@return AVPacket * 读取到的内容，空间需由调用者释放，包括AVPacket对象空间，和数据空间
    //释放参见FQFFreePacket()
    virtual AVPacket *readFile();
    //获取pkt的类型，不会清理pkt空间
    //@param pkt 需要判断类型的pkt
    //@return PacketType pkt类型
    virtual PacketType getPktType(AVPacket *pkt);
    //复制得到Audio的媒体参数，
    //@return AVCodecParameters * 媒体参数，空间需要在外部清理 avcodec_parameters_free()
    virtual AVCodecParameters *copyAudioPara();
    //媒体跳转
    //@param pos 位置 0.0~1.0
    //@return bool 跳转结果
    virtual bool seek(double pos);
    //获取文件的媒体时长
    //@return long long 当前媒体时长 ms
    virtual long long getFileTimeMs();
    //获取当前打开文件的采样率
    //@return int 采样率
    virtual int getFileSampleRate();
    //获取当前打开文件的声道数
    //@return int 声道
    virtual int getFileChannels();

protected:
    AVFormatContext *ic = nullptr;
	std::mutex mux;
    int audioStream = -1;
    long long totalMs = 0;
    int sampleRate = 0;
    int channels = 0;
};

