package com.example.myapplication3.UDPSocket;

import android.content.Context;
import android.util.Log;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketAddress;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class UDPSocket {
    private static final String TAG = "UDPSocket";

    // 单个CPU线程池大小
    private static final int POOL_SIZE = 5;

    private static final int BUFFER_LENGTH = 1024;
    private byte[] receiveByte = new byte[BUFFER_LENGTH];

   // private static final String BROADCAST_IP = "192.168.43.255";
    String BROADCAST_IP = null;
    // 端口号，飞鸽协议默认端口2425
    //public static final int CLIENT_PORT = 0x0971;
    int CLIENT_PORT = 0;
    private boolean isThreadRunning = false;

    private Context mContext;
    private DatagramSocket client;
    private DatagramPacket receivePacket;

    private long lastReceiveTime = 0;
    private static final long TIME_OUT = 120 * 1000;
    private static final long HEARTBEAT_MESSAGE_DURATION = 10 * 1000;

    private ExecutorService mThreadPool;
    private Thread clientThread;

    public UDPSocket(Context context,String ip,int port) {

        this.mContext = context;

        int cpuNumbers = Runtime.getRuntime().availableProcessors();
        // 根据CPU数目初始化线程池
        mThreadPool = Executors.newFixedThreadPool(cpuNumbers * POOL_SIZE);
        // 记录创建对象时的时间
        lastReceiveTime = System.currentTimeMillis();

        BROADCAST_IP = ip;
        CLIENT_PORT = port;

    }


    public void startUDPSocket() {
        if (client != null) return;
        try {
            // 表明这个 Socket 在设置的端口上监听数据。
            client = new DatagramSocket(6700);

            if (receivePacket == null) {
                // 创建接受数据的 packet
               // receivePacket = new DatagramPacket(receiveByte, BUFFER_LENGTH);
            }

            startSocketThread();
        } catch (SocketException e) {
            e.printStackTrace();
        }
    }

    /**
     * 开启发送数据的线程
     */
    private void startSocketThread() {
        clientThread = new Thread(new Runnable() {
            @Override
            public void run() {

                Log.d(TAG, "clientThread is running...");
                //receiveMessage();
            }
        });
        isThreadRunning = true;
        clientThread.start();

        //startHeartbeatTimer();
    }
    /**
     * 处理接受到的消息
     */
    private void receiveMessage() {
        while (isThreadRunning) {
            try {
                if (client != null) {
                    client.receive(receivePacket);
                    String string = new String(receiveByte,0,receiveByte.length);
                    System.out.println(string);
                }
                lastReceiveTime = System.currentTimeMillis();
                Log.d(TAG, "receive packet success...");
            } catch (IOException e) {
                Log.e(TAG, "UDP数据包接收失败！线程停止");
                stopUDPSocket();
                e.printStackTrace();
                return;
            }

            if (receivePacket == null || receivePacket.getLength() == 0) {
                Log.e(TAG, "无法接收UDP数据或者接收到的UDP数据为空");
                continue;
            }

            String strReceive = new String(receivePacket.getData(), 0, receivePacket.getLength());
            Log.d(TAG, strReceive + " from " + receivePacket.getAddress().getHostAddress() + ":" + receivePacket.getPort());

            //解析接收到的 json 信息

            // 每次接收完UDP数据后，重置长度。否则可能会导致下次收到数据包被截断。
            if (receivePacket != null) {
                receivePacket.setLength(BUFFER_LENGTH);
            }
        }
    }


    public void stopUDPSocket() {
        isThreadRunning = false;
        receivePacket = null;
        if (clientThread != null) {
            clientThread.interrupt();
        }
        if (client != null) {
            client.close();
            client = null;
        }
        /*if (timer != null) {
            timer.exit();
        }*/
    }


    /**
     * 发送心跳包
     *
     * @param message
     */
    public void sendMessage(final String message) {
        mThreadPool.execute(new Runnable() {
            @Override
            public void run() {
                try {

                    InetAddress targetAddress = InetAddress.getByName(BROADCAST_IP);

                    DatagramPacket packet = new DatagramPacket(message.getBytes(), message.length(), targetAddress, CLIENT_PORT);

                    client.send(packet);

                    // 数据发送事件
                    Log.d(TAG, "数据发送成功");

                } catch (UnknownHostException e) {
                    e.printStackTrace();
                } catch (IOException e) {
                    e.printStackTrace();
                }

            }
        });
    }

}
