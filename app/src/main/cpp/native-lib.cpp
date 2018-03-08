#include <jni.h>
#include <string>
#include <string.h>
#include "native-lib.h"

extern "C"
JNIEXPORT jstring

JNICALL
Java_com_mir_socketprocess_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

const char *userId;
const char * PATH = "/data/data/com.mir.socketprocess/my.sock";
int m_child;
void child_do_work() {
    //开启 socket 服务端
    if (child_create_channel()){
        child_listen_msg();
    }
}

//创建服务端socket
void child_listen_msg() {
    LOGE("服务段监听消息");
    fd_set rfds;
    struct timeval timeout = {3, 0};
    while (1){
        //清空内容
        FD_ZERO(&rfds);
        FD_SET(m_child, &rfds);
        //4个客户端
        int r = select(m_child + 1, &rfds, NULL, NULL, &timeout);
        if (r > 0){
            //缓冲区
            char pkg[256] = {0};
            //保证所读到的信息是指定apk的客户端的
            if(FD_ISSET(m_child, &rfds)){
                //阻塞式函数
                int result = read(m_child, pkg, sizeof(pkg));
                LOGE("userid %s ",  userId);
                //开启服务 子进程
                execle("am", "am", "startservice", "--user", userId,
                       "com.mir.socketprocess/com.mir.socketprocess.ProcessService", (char*)NULL);
                break;
            }
        }else if (r == -1){
            LOGE("服务端断开连接");
        }
    }
}

/**
 * 服务端读取信息
 * @return
 */
int child_create_channel() {
    int listenfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    unlink(PATH);
    //addr -->内存地址
    struct sockaddr_un addr;
    //清空内存
    memset(&addr, 0, sizeof(sockaddr_un));
    addr.sun_family = AF_LOCAL;

    int connfd = 0;
    //复制
    strcpy(addr.sun_path, PATH);
    if (bind(listenfd, (const sockaddr *) &addr, sizeof(sockaddr_un)) < 0){
        LOGE("绑定错误");
        return 0;
    }
    listen(listenfd, 5);
    //保证宿主进程连接成功
    while (1){
        //返回值 客服端的地址 阻塞式函数
        if ((connfd = accept(listenfd, NULL, NULL)) < 0){
            if (errno == EINTR){
                continue;
            }else{
                LOGE("读取错误");
                return 0;
            }
        }
        m_child = connfd;
        LOGE("apk 父进程连接上了 %d " , m_child);
        break;
    }
    return 1;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_mir_socketprocess_Watcher_createWatcher(JNIEnv *env, jobject instance, jstring userId_) {
    userId = env->GetStringUTFChars(userId_, 0);

    //开启双进程
    pid_t pid = fork();
    if (pid < 0){ //
    }else if(pid == 0){ //子进程 守护进程
        child_do_work();
    }else if (pid > 0){ //父进程
    }
    // TODO
    env->ReleaseStringUTFChars(userId_, userId);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_mir_socketprocess_Watcher_connectMonitor(JNIEnv *env, jobject instance) {

    int socked;
    struct sockaddr_un addr;
    //apk 进程
    while (1){
        LOGE("客户端 父进程开始连接了");
        socked = socket(AF_LOCAL, SOCK_STREAM, 0);
        if (socked < 0){
            LOGE("连接失败");
            return;
        }
        memset(&addr, 0, sizeof(sockaddr_un));
        addr.sun_family = AF_LOCAL;
        strcpy(addr.sun_path, PATH);
        if (connect(socked, (const sockaddr *) &addr, sizeof(sockaddr_un)) < 0){
            LOGE("连接失败");
            close(socked);
            sleep(1);
            //再来下一次尝试连接
            continue;
        }
        LOGE("连接成功");
        break;
    }

}