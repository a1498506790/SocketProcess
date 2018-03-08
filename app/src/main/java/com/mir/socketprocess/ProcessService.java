package com.mir.socketprocess;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.Process;
import android.support.annotation.Nullable;
import android.util.Log;

import java.util.Timer;
import java.util.TimerTask;

/**
 * @author by lx
 * @github https://github.com/a1498506790
 * @data 2018/3/8
 * @desc 需要守护的app服务
 */

public class ProcessService extends Service {

    private int mCount = 0;

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        Watcher watcher = new Watcher();
        watcher.createWatcher(String.valueOf(Process.myUid()));
        watcher.connectMonitor();
        Timer timer = new Timer();
        timer.scheduleAtFixedRate(new TimerTask() {
            @Override
            public void run() {
                Log.e("test", "服务开启中 : " + mCount);
                mCount ++;
            }
        },0, 1000);
    }
}
