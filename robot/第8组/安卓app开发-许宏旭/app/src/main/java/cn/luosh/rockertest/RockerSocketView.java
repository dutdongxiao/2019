package cn.luosh.rockertest;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.AsyncTask;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.UnsupportedEncodingException;
import java.net.ConnectException;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;


public class RockerSocketView extends RockerView{
    //树莓派ip地址
    private String carIp;
    //端口
    private int carPort;
    private Handler updateHandler;
    private Bitmap bitmap;

    private Context context;

    //socket连接
    Socket socket = null;
    Socket recImageSocket = null;
    //是否连接标志
    private boolean isConnect = false;
    private boolean isGetImageConnected = false;
    BufferedReader reader = null;
    BufferedWriter writer = null;

    private int rockerEvent = 0;
    private int oldrockerEvent = -1;

    //定义摇杆事件状态
    public static final int Movestop = 0;
    public static final int Moveforward = 1;
    public static final int Movebackward = 2;
    public static final int Turnleft = 3;
    public static final int Turnright = 4;





    //重新定义了构造方法，目的是为了得到使用该控件的Activity的context
    public RockerSocketView(Context context) {
        this(context, null);
    }

    public RockerSocketView(Context context, AttributeSet attrs) {
        super(context, attrs, 0);
        this.context = context;
    }

    private Runnable recImageRunnable = new Runnable() {
        @Override
        public void run() {
            try {
                recImageSocket = new Socket(carIp, 8807);
                Log.e("ReceiveAction------>", "创建一个socket, 正在监听" + (carPort + 1));
                isGetImageConnected = true;
                while (isGetImageConnected) {
                    long start = System.currentTimeMillis();

                    InputStream inputStream = recImageSocket.getInputStream();
                    byte[] picLenBuff = new byte[200];
                    int picLen = inputStream.read(picLenBuff);
                    if (picLen <= 0) break;
                    if (!isGetImageConnected) break;
                    String picLenString = new String(picLenBuff, 0, picLen);
                    int getPicLen = Integer.valueOf(picLenString);
                    //反馈
                    OutputStream outputStream = recImageSocket.getOutputStream();
                    outputStream.write("1".getBytes());
                    outputStream.flush();

                    //开始接收图片
                    int offset = 0;
                    byte[] bitmapBuff = new byte[getPicLen];
                    while (offset < getPicLen) {
                        int len = inputStream.read(bitmapBuff, offset, getPicLen - offset);
                        offset += len;
                    }
                    //图片解码
                    bitmap = BitmapFactory.decodeByteArray(bitmapBuff, 0, offset);
                    Message msg = updateHandler.obtainMessage();
                    msg.obj = bitmap;
                    Log.e("ReceiveAction------>", "解码图片" + (carIp + 1));
                    updateHandler.sendMessage(msg);

                    long end = System.currentTimeMillis();
                    Log.e("ReceiveAction------>", "接收图片耗时" + (end - start));

                }
            } catch (IOException e) {
                e.printStackTrace();
                Log.e("ReceiveAction------>", "接收图片失败" + (carIp + 1));
                isGetImageConnected = false;
            }
        }
    };


    //初始化函数，由使用该控件的Activity调用
    public void init(String carIp,int carPort, Handler updateImageHandler){
        this.updateHandler = updateImageHandler;
        this.carIp = carIp;
        this.carPort = carPort;
        connectAsyncTask();
        //启动图片接收线程
//        try {
//            Thread.sleep(1000);
//        } catch (InterruptedException e) {
//            e.printStackTrace();
//        }
        int connectTimes = 10;
        while (!isConnect && connectTimes > 0) {
            try {
                Thread.sleep(100);
                connectTimes--;
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        Log.e("EVENT_ACTION----->", "尝试次数:" + (10 - connectTimes));
        Log.e("EVENT_ACTION-------->", "8806连接状态" + isConnect);
        if (isConnect) {
            Log.e("EVENT_ACTION-------->", "新建图像接收线程");
            new Thread(recImageRunnable).start();
        } else {
            Toast.makeText(context,"8806连接失败",Toast.LENGTH_SHORT).show();
        }

        //添加摇杆事件监听
        this.setListener(new RockerView.RockerListener() {
            @Override
            public void callback(int eventType, int currentAngle, float currentDistance) {
                switch (eventType) {
                    case RockerView.EVENT_ACTION:
                        // 触摸事件回调
                        Log.e("EVENT_ACTION-------->", "angle="+currentAngle+" - distance"+currentDistance);
                        rockerEvent = judgeEvent(currentAngle);
                        if (isConnect) {
                            send();
                        }
                        break;
                    case RockerView.EVENT_CLOCK:
                        // 定时回调
                        Log.e("EVENT_CLOCK", "angle="+currentAngle+" - distance"+currentDistance);
                        rockerEvent = judgeEvent(currentAngle);
                        if (isConnect) {
                            send();
                        }
                        break;
                }
            }
        });
    }

    //根据角度判断摇杆方向
    public int judgeEvent(int currentAngle){
        if (currentAngle == -1){
            return Movestop;
        }else if (currentAngle > 45 && currentAngle <= 135){
            return Moveforward;
        }else if (currentAngle > 135 && currentAngle <= 225){
            return Turnleft;
        }else if (currentAngle > 225 && currentAngle <= 315){
            return Movebackward;
        }else{
            return Turnright;
        }
    }

    public void Destroy(){
        isConnect = false;
        isGetImageConnected = false;
        try {
            if (socket != null) {
                close(socket);
                Log.e("EVENT_FIN", "主动断开socket:8806");
                socket.close();
                Toast.makeText(context,"连接断开",Toast.LENGTH_SHORT).show();
            }
            if (recImageSocket != null) {
                close(recImageSocket);
                Log.e("EVENT_FIN", "主动断开socket:8807");
                recImageSocket.close();
            }

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void close(Socket s) {
        OutputStream outputStream = null;
        try {
            outputStream = s.getOutputStream();
            outputStream.write("close".getBytes());
            outputStream.close();
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    //使用异步任务建立socket连接
    public void connectAsyncTask(){
        AsyncTask<Void,String,Void> connect = new AsyncTask<Void, String, Void>() {
            @Override
            protected Void doInBackground(Void... params) {

                try {
                    Log.e("EVENT_CLOCK", "开始连接8806");
                    socket = new Socket(carIp,carPort);
                    writer = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()));
                    reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                    publishProgress("连接成功\n");
                    Log.e("EVENT_CLOCK", "8806连接成功");
                    isConnect = true;
                } catch (Exception e1) {
                    Log.e("EVENT_CLOCK", "8806连接失败");
                }

                if (isConnect) {
                    String Line;
                    try {
                        while ((Line = reader.readLine()) != null){
                            publishProgress(Line);
                        }
                    } catch (IOException e) {
                        e.printStackTrace();
                    }

                }

                return null;
            }

            //消息显示
            protected void onProgressUpdate(String...values){
                if(values[0].equals("连接成功\n")){
                    Toast.makeText(context,"连接成功",Toast.LENGTH_SHORT).show();
                }
                else{
                    Toast.makeText(context,"连接失败",Toast.LENGTH_SHORT).show();
                }

                super.onProgressUpdate(values[0]);
            }
        };
        connect.execute();

    }


    //不使用线程，事件变化时再发送状态
    public synchronized void send(){
        //先判断是否成功建立socket   连接再向树莓派发送数据，避免出现错误
        if(isConnect){
            if(rockerEvent != oldrockerEvent){
                oldrockerEvent = rockerEvent;
                try {
                    //将要发送的int数据转换成String再发送
                    writer.write(String.valueOf(rockerEvent));
                    writer.flush();
                    Log.e("EVENT_ACTION-------->", "rockerEvent=" + rockerEvent);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    public synchronized void send(String action) {
        if (isConnect) {
            try {
                writer.write(action);
                writer.flush();
                Log.e("EVENT_ACTION-------->", "turn " + action);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public boolean isControlConnected() {
        return isConnect;
    }
}
