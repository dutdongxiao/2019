package cn.luosh.rockertest;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Handler;
import android.os.Message;
import android.os.StrictMode;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;

import java.io.ByteArrayOutputStream;
import java.io.DataInput;
import java.io.DataInputStream;
import java.io.IOException;
import java.net.DatagramSocket;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;


public class MainActivity extends AppCompatActivity {

    private RockerSocketView rockersocket;
    private EditText carip,carport;
    Button button1;  //连接
    Button button2;  //断开
    ImageView view;  //图像view
    Bitmap bmp;

    MyHandler updateImageHandler;


    Button speedUp;    //加速
    Button speedDown;  //减速
    Button turnUp;
    Button turnDown;
    Button turnLeft;
    Button turnRight;

    private static int millis = 300;

    private volatile static  boolean isOnLongClick;



    Runnable runnableControl = new Runnable() {
        @Override
        public void run() {
            button1 = (Button) findViewById(R.id.btn_car_connect);
            button2 = (Button) findViewById(R.id.btn_car_close);
            rockersocket = (RockerSocketView) findViewById(R.id.rockersocket);   //摇杆view
            carip = (EditText) findViewById(R.id.carip);
            carport = (EditText) findViewById(R.id.carport);
            //连接， 断开的按钮
            View.OnClickListener listener = new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    int tag = (int) view.getTag();
                    switch (tag) {
                        case 1:
                            rockersocket.init(carip.getText().toString(),Integer.parseInt(carport.getText().toString()), updateImageHandler);
                            break;
                        case 2:
                            rockersocket.Destroy();
                            break;
                        case 3:
                            if (rockersocket.isControlConnected()) {
                                rockersocket.send("up");
                            }
                            break;
                        case 4:
                            if (rockersocket.isControlConnected()) {
                                rockersocket.send("down");
                            }
                            break;
                    }
                }
            };
            button1.setTag(1);
            button1.setOnClickListener(listener);
            button2.setTag(2);
            button2.setOnClickListener(listener);

            //添加加速减速
            View.OnTouchListener speedListener = new View.OnTouchListener() {
                @Override
                public boolean onTouch(View view, MotionEvent motionEvent) {
                    int action = motionEvent.getAction();
                    if (action == MotionEvent.ACTION_DOWN) {
                        //按下
                        int tag = (int) view.getTag();
                        switch (tag) {
                            case 3:  //up
                                if (rockersocket.isControlConnected()) {
                                    rockersocket.send("up");
                                }
                                break;
                            case 4:  //down
                                if (rockersocket.isControlConnected()) {
                                    rockersocket.send("down");
                                }
                                break;
                        }

                    } else if (action == MotionEvent.ACTION_UP) {
                        Log.e("ACTION_UP-------->", "按钮松开");
                        if (rockersocket.isControlConnected()) {
                            rockersocket.send("normal");
                        }
                    }
                    return false;
                }
            };

            speedUp.setTag(3);
            speedUp.setOnTouchListener(speedListener);
            speedDown.setTag(4);
            speedDown.setOnTouchListener(speedListener);



            //触摸监听事件
            View.OnTouchListener cameraListener = new View.OnTouchListener() {
                @Override
                public boolean onTouch(View view, MotionEvent motionEvent) {
                    int action = motionEvent.getAction();
                    if (action == MotionEvent.ACTION_DOWN) {
                        //按下
                        Log.e("BUTTON_ACTION--------->", "按钮按下");

                        int tag = (int) view.getTag();
                            switch (tag) {
                                case 1:  //up
                                    if (rockersocket.isControlConnected()) {
                                        rockersocket.send("t");
                                    }
                                    break;
                                case 2:  //down
                                    if (rockersocket.isControlConnected()) {
                                        rockersocket.send("b");
                                    }
                                    break;
                                case 3:  //left
                                    if (rockersocket.isControlConnected()) {
                                        rockersocket.send("l");
                                    }
                                    break;
                                case 4:  //right
                                    if (rockersocket.isControlConnected()) {
                                        rockersocket.send("r");
                                    }
                                    break;
                            }

                    } else if (action == MotionEvent.ACTION_UP) {
                        Log.e("BUTTON_ACTION--------->", "按钮松开");
                        if (rockersocket.isControlConnected()) {
                            rockersocket.send("normal1");
                        }

                    }
                    return false;
                }
            };
            turnUp.setTag(1);
            turnUp.setOnTouchListener(cameraListener);
            turnDown.setTag(2);
            turnDown.setOnTouchListener(cameraListener);
            turnLeft.setTag(3);
            turnLeft.setOnTouchListener(cameraListener);
            turnRight.setTag(4);
            turnRight.setOnTouchListener(cameraListener);

        }
    };








    @Override
    protected void onCreate(Bundle savedInstanceState) {
        //TODO
        if (android.os.Build.VERSION.SDK_INT > 9) {
            StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitAll().build();
            StrictMode.setThreadPolicy(policy);
        }
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        updateImageHandler = new MyHandler();
        view = (ImageView) findViewById(R.id.img_rec);
        speedUp = (Button) findViewById(R.id.btn_speedUp);
        speedDown = (Button) findViewById(R.id.btn_speedDown);

        //摄像头方向按钮
        turnUp = (Button) findViewById(R.id.btn_turnUp);
        turnDown = (Button) findViewById(R.id.btn_turnDown);
        turnLeft = (Button) findViewById(R.id.btn_turnLeft);
        turnRight = (Button) findViewById(R.id.btn_turnRight);

        //新建线程处理控制相关
        new Thread(runnableControl).start();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        rockersocket.Destroy();
        rockersocket = null;
    }

    class MyHandler extends Handler {
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            bmp = (Bitmap) msg.obj;
            view.setImageBitmap(bmp);
        }
    }
}
