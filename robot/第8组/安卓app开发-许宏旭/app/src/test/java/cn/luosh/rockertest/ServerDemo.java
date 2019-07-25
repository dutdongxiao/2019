package cn.luosh.rockertest;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.InetAddress;
import java.net.Socket;
import java.net.ServerSocket;
public class ServerDemo {

    private static ServerSocket ss;
    private static  Socket  s;
    private static String ip;



    public static void main(String[] args) throws IOException {
        //创建接收端的Socket对象
        //ServerSocket(int port)

        ss= new ServerSocket(8806);
        //监听客户端连接，返回一个对应的Socket对象
        //public Socket accept();
        s = ss.accept();   //在连接传入前一直阻塞
        InetAddress inetAddress = s.getInetAddress();
        ip = inetAddress.getHostAddress();
        //新建线程，发送图片
        new Thread(sendRunnable).start();
        //获取输入流，读取数据显示在控制台
        while (true) {
            InputStream is = s.getInputStream();

            byte[] bys = new byte[1024];
            int len = is.read(bys); //阻塞
            String str = new String(bys, 0, len);
            System.out.println(str);
            System.out.println(s.getInetAddress().getHostAddress());

            // //获取输出流
            // OutputStream os = s.getOutputStream();
            // os.write("数据已收到".getBytes();
        }
        //释放资源
        //  s.close();
        //ss.close();   服务器不应该关闭
    }

    private static Runnable sendRunnable = new Runnable() {
        @Override
        public void run() {
            Socket send = null;
            BufferedReader br = null;
            BufferedWriter bw = null;
            try {
                send = new Socket(ip, 8807);
                br = new BufferedReader(new FileReader(
                        "girl.png"
                ));

                //封装通道内的流
                bw = new BufferedWriter(new OutputStreamWriter(
                        send.getOutputStream()
                ));

                String line = null;
                while ((line = br.readLine()) != null) {   //阻塞
                    bw.write(line);
                    bw.newLine();
                    bw.flush();
                }
            } catch (IOException e) {
                e.printStackTrace();
            } finally {
                try {
                    br.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
                try {
                    s.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }

            //
            // //Socket提供了一个终止功能，它会通知服务器你别等了，我没有数据要过来。
            // s.shutdownOutput();
            //
            // //接收反馈
            // BufferedReader brClient = new BufferedReader(new InputStreamReader(
            //         s.getInputStream()
            // ));
            // String client = brClient.readLine(); //阻塞
            // System.out.println(client);

            //释放资源

        }
    };
}