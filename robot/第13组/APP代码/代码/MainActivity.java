package com.example.myapp;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.Socket;

public class MainActivity extends AppCompatActivity implements View.OnClickListener{

    public static Context context;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        context=this;
        Button b1 = (Button) findViewById(R.id.button1);
        b1.setOnClickListener(this);
        Button b2 = (Button) findViewById(R.id.button2);
        b2.setOnClickListener(this);
        Button b3 = (Button) findViewById(R.id.button3);
        b3.setOnClickListener(this);
        Button b4 = (Button) findViewById(R.id.button4);
        b4.setOnClickListener(this);
        Button b5 = (Button) findViewById(R.id.button5);
        b5.setOnClickListener(this);
        Button b6 = (Button) findViewById(R.id.button6);
        b6.setOnClickListener(this);
        Button b7 = (Button) findViewById(R.id.button7);
        b7.setOnClickListener(this);
        Button b8 = (Button) findViewById(R.id.button8);
        b8.setOnClickListener(this);
        Button b9 = (Button) findViewById(R.id.button9);
        b9.setOnClickListener(this);
        Button b10 = (Button) findViewById(R.id.button10);
        b10.setOnClickListener(this);
        Button b11 = (Button) findViewById(R.id.button11);
        b11.setOnClickListener(this);
    }
    @Override
    public void onClick(final View v) {
        new Thread() {
            @Override
            public void run() {
                try {
                    if(v.getId() == R.id.button1)acceptServer("w");

                    else if(v.getId() == R.id.button2)acceptServer("s");

                    else if(v.getId() == R.id.button3)acceptServer("a");

                    else if(v.getId() == R.id.button4)acceptServer("d");

                    else if(v.getId() == R.id.button5)acceptServer("p");

                    else if(v.getId() == R.id.button6)acceptServer("2");

                    else if(v.getId() == R.id.button7)acceptServer("1");

                    else if(v.getId() == R.id.button8)acceptServer("3");

                    else if(v.getId() == R.id.button9)acceptServer("5");

                    else if(v.getId() == R.id.button10)acceptServer_1("yuyin");

                    else if(v.getId() == R.id.button11)acceptServer_1("ssd");

                    //Log.i("test", "run:... ");
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }.start();
    }

    private void acceptServer(String mes) throws IOException {
        //1.创建客户端Socket，指定服务器地址和端口
        Socket socket = new Socket("192.168.137.100", 55500);
        //2.获取输出流，向服务器端发送信息
        OutputStream os = socket.getOutputStream();//字节输出流
        PrintWriter pw = new PrintWriter(os);//将输出流包装为打印流
        //获取客户端的IP地址
        //InetAddress address = InetAddress.getLocalHost();
        //String ip = address.getHostAddress();
        pw.write(mes);
        pw.flush();
        socket.shutdownOutput();
        socket.close();
    }
    private void acceptServer_1(String mes) throws IOException {
        //1.创建客户端Socket，指定服务器地址和端口
        Socket socket = new Socket("192.168.137.128", 55501);
        //2.获取输出流，向服务器端发送信息
        OutputStream os = socket.getOutputStream();//字节输出流
        PrintWriter pw = new PrintWriter(os);//将输出流包装为打印流
        //获取客户端的IP地址
        //InetAddress address = InetAddress.getLocalHost();
        //String ip = address.getHostAddress();
        pw.write(mes);
        pw.flush();
        socket.shutdownOutput();
        socket.close();
    }
}
