#pragma once
#include <QThread>
#include <mutex>
class FQFDemux;
class FQFAudioThread;
class FQFVideoThread;

class FQFDemuxThread : public QThread
{
public:
    //文件播放状态
    typedef enum{
        Play,   //播放
        Stop,   //停止
        Pause,  //暂停
        End     //结束
    }MusicType;
    FQFDemuxThread();
    virtual ~FQFDemuxThread();
    //打开文件，该函数为顶层操作，函数内部会执行上下文打开、解码器打开、播放打开等一系列操作
    //@param path 要打开文件的路径
    //@return bool 打开结果
    virtual bool openFile(const char *path);
    //关闭文件，该函数为顶层操作，函数内部会关闭上下文，关闭解码器，退出当前线程，退出解码线程
    virtual void closeFile();
    //清理，该函数为顶层操作，同时会清理解码所有涉及对象，只是清理缓冲不影响继续播放
    virtual void clear();
    //暂停/播放
    //@param isPause 是否暂停
    virtual void setPause(bool isPause);
    //媒体跳转
    //@param pos 位置 0.0~1.0
    //@return bool 跳转结果
    virtual void seek(double pos);
    //获取当前媒体的播放状态
    //@return MusicType 状态
    virtual MusicType getMusicStatus();
    //获取当前正在播放的进度
    //@return long long 当前的时间 ms
    virtual long long getNowTimeMs();
    //获取当前文件的总时长
    //@return long long 媒体总时长 ms
    virtual long long getFileTimeMs();
	void run();

protected:
    virtual void startTh();
    FQFDemux *demux = nullptr;
    FQFAudioThread *at = nullptr;
	std::mutex mux;
	bool isExit = false;
    MusicType status = Stop;
    long long pts = 0;
    long long totalMs = 0;
};

