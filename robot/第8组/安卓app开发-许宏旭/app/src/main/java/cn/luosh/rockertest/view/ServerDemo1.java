package cn.luosh.rockertest.view;

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
import java.util.Arrays;

public class ServerDemo1 {
    private static Socket recImageSocket = null;
    private static Socket contrlSocket = null;
    public static void main(String[] args) throws IOException {
        contrlSocket = new Socket("127.0.0.1",8806);
        System.out.println("8806连接成功");
        new Thread(recRunnable).start();
    }


    private static Runnable recRunnable = new Runnable() {
        @Override
        public void run() {
            try {
                recImageSocket = new Socket("10.5.118.120", 8807);
                System.out.println("监听8807");
                while (recImageSocket != null) {
                    InputStream inputStream = recImageSocket.getInputStream();
                    byte[] picLenBuff = new byte[200];
                    int picLen = inputStream.read(picLenBuff);
                    String picLenString = new String(picLenBuff, 0, picLen);
                    System.out.println("长度" + picLenString);
                    int getPicLen = Integer.valueOf(picLenString);
                    //反馈
                    OutputStream outputStream = recImageSocket.getOutputStream();
                    outputStream.write("1".getBytes());
                    outputStream.flush();
                    System.out.println("发送反馈信号1");

                    //开始接收图片
                    int offset = 0;
                    byte[] bitmapBuff = new byte[getPicLen];
                    while (offset < getPicLen) {
                        int len = inputStream.read(bitmapBuff, offset, getPicLen - offset);
                        offset += len;
                        System.out.println("接收图片" + Arrays.toString(bitmapBuff));
                    }
                }
            } catch (IOException e) {
                e.printStackTrace();
            } finally {
                //  recImageSocket.close();
            }
        }
    };
}
