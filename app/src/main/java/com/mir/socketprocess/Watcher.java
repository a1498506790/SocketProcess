package com.mir.socketprocess;

/**
 * @author by lx
 * @github https://github.com/a1498506790
 * @data 2018/3/8
 * @desc
 */

public class Watcher {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    public native void createWatcher(String userId);

    public native void connectMonitor();

}
