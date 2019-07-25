package com.wow.carcontroller;



import android.util.Log;

import java.io.IOException;
import java.io.OutputStreamWriter;
import java.net.Socket;

public class CarSocket {

    private static Socket socket = null;
    public static Socket getSocket(){
        return socket;
    }
    public static boolean setSocket(String ip, int port){
        try {
            socket = new Socket(ip, port);
            return true;
        } catch (IOException e) {
            return false;
        }
    }
    public static void killSocket(){
        if(socket!=null) {
            new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        socket.close();
                        socket = null;
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }).start();
        }
    }

}

class SendControl implements Runnable {
    private int num;
    public SendControl(int num) {
        this.num = num;
    }
    @Override
    public void run() {
        try {
            Socket socket = CarSocket.getSocket();
            if(socket!=null) {
                OutputStreamWriter oos = new OutputStreamWriter(socket.getOutputStream(),"utf-8");
                oos.write(num);
                oos.flush();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
