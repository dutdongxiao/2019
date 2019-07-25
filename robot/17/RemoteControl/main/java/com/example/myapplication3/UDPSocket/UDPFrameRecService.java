package com.example.myapplication3.UDPSocket;

import android.app.Service;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.wifi.WifiManager;
import android.os.Binder;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.util.Log;

import com.example.myapplication3.UDPSendMessageActivity;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.net.SocketTimeoutException;

public class UDPFrameRecService extends Service {

    private boolean stopReceive=false;
    private int port=5051;

    public String Length;   //用来放客户端发过来图像的长度
    private Socket socket;
    private int offset=0;
    private BufferedOutputStream output;    //输出流
    private BufferedInputStream input;      //输入流
    private Handler handler;
    public UDPFrameRecService(Handler handler) {

        this.handler = handler;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        startReceiveThread();
        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public IBinder onBind(Intent intent) {
        // TODO: Return the communication channel to the service.
        return mBinder;
    }

    /**
     * Start the frame receive thread
     */
    public void startReceiveThread(){
        System.out.println("开始接受视频帧");

         new Thread(){

            @Override
            public void run() {
                super.run();

                try {
                    ServerSocket serverSocket = new ServerSocket(9000);//服务器端端口，数据就是在这个端口上
                    socket = serverSocket.accept();//他就是阻塞，没电话来就一直等，下面都执行不了
                    String string = null;
                    while (true) {

                                                                        //电话一来就建立连接，开始执行下面代码
                        output=new BufferedOutputStream(socket.getOutputStream());
                        input=new BufferedInputStream(socket.getInputStream());

                        Length=readMsgFromSocket(input);                //对面发来了图像大小，我好准备空间
                        /*send_msg.post(new Runnable() {
                            @Override
                            public void run() {
                                send_msg.setText(Length);              //这是为了看到图像大小，线程中必须这样才能显示到UI界面
                            }
                        });*/
                        SendMsg("123");
                        if (Length == ""){
                            Length = "0";//随便发送的，让对面知道我收到了长度
                        }
                        byte[] bitmapBuff=new byte[Integer.valueOf(Length)];      //分配放图片的空间
                        while (offset<Integer.valueOf(Length))
                        {
                            int len=input.read(bitmapBuff,offset,Integer.valueOf(Length)-offset);//读取图像数据放空间里
                            offset+=len;
                        }
                        Length="";
                        final Bitmap bitmap=BitmapFactory.decodeByteArray(bitmapBuff,0,offset);

                        sendFrameToActivity(bitmap);//解压图像文件
                        /*image.post(new Runnable() {
                            @Override
                            public void run() {
                                image.setImageBitmap(bitmap);                                 //将图片在UI上显示出来
                            }
                        });*/
                        SendMsg("12");                                   //告诉客户端这边图像已经接收完

                        offset=0;                                        //这点比较坑，参考的博客都没有写，让我调了几天，为什么第一张传了第二张就出问题了呢，原因是offset没复位为 0 ，第二个开始还是前面的值，一张完了就开始从新建立连接接收下一张图片了
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }finally {
                    try {
                        socket.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }


            }
            /**
             *
             * @param in
             * @return
             */
            public String readMsgFromSocket(InputStream in) {                   //读取客户端发过来的文本数据
                String msg = "";
                byte[] tempbuffer = new byte[1024];
                try {
                    int numReadedBytes = in.read(tempbuffer, 0, tempbuffer.length);
                    msg = new String(tempbuffer, 0, numReadedBytes, "utf-8");
                } catch (Exception e) {
                    e.printStackTrace();
                }
                return msg;
            }
            public void SendMsg(String msg) {                                  //发送文本数据给客户端
                String msg_1 = msg;
                try {
                    output.write(msg_1.getBytes("UTF-8"));
                    output.flush();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }.start();
    }

    /**
     * Notify the activity with specific message
     * @param "message.What"
     * @param "message.obj"
     */
    protected void sendMessageToActivity(int id,String content){
        Message message=new Message();
        message.what=id;
        message.obj=content;
        handler.sendMessage(message);
    }

    protected void sendFrameToActivity(Bitmap bitmap){
        Message message=new Message();
        message.what= 1101;
        message.obj=bitmap;
        handler.sendMessage(message);
    }

    private NoticeBinder mBinder=new NoticeBinder();
    class NoticeBinder extends Binder {
        /**
         * Notice the Frame receive thread to stop
         */
        void stopFrameRecThread(){
            stopReceive=true;
        }

    }


}
