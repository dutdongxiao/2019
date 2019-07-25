package com.wow.carcontroller;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.content.res.Configuration;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.provider.Telephony;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;
import java.io.OutputStreamWriter;
import java.net.Socket;

public class MainActivity extends AppCompatActivity implements SensorEventListener {

    //要连接的IP地址
    private String ip = "192.168.137.218";
    //控制方向连接端口
    private static final int port = 7788;
    //发送指令连接端口
    private static final int portCommand = 8899;
    //连接按钮
    private Button buttonConnect;
    private Button buttonControl;
    private Button rockerControl;
    private EditText textIP;
    private Button buttonExecute;
    private EditText textCommand;

    //传感器
    private SensorManager sensorManager;
    private Sensor sensor;
    private float X = 0;
    private float Y = 0;
    private float Z = 0;
    //前后最低控制角度
    private static final float bbChangeAngle = 4;
    //左右最低控制角度
    private static final float lrChangeAngle = 3;
    //前后加速变换角度
    private static final float speedAngle = 2;
    //原始坐标长度
    private static final float staticXYZ = 10;
    //重力感应控制的方向
    private ControlNum gravityControlNum = ControlNum.NNNN;
    //记录当前前后方向在哪个加速区
    private int gravitySpeedTimes = 0;
    private Button gravityButton;
    private TextView textGravity;
    private boolean isGravityControl = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        buttonConnect = findViewById(R.id.buttonConnect);
        buttonControl = findViewById(R.id.buttonControl);
        rockerControl = findViewById(R.id.rockerControl);
        textIP =findViewById(R.id.textIP);
        buttonExecute = findViewById(R.id.buttonExecute);
        textCommand = findViewById(R.id.textCommand);

        sensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        sensor = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        sensorManager.registerListener(this, sensor, SensorManager.SENSOR_DELAY_NORMAL);
        gravityButton = findViewById(R.id.gravityControl);
        textGravity = findViewById(R.id.textGravity);

        buttonConnect.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(CarSocket.getSocket()==null) {
                    new ThreadConnect().start();
                } else {
                    new ThreadConnect().start();
                }
            }
        });
        buttonControl.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(!isGravityControl) {
                    if(CarSocket.getSocket()!=null) {
                        Intent intent1 = new Intent(MainActivity.this, ControlActivity.class);
                        startActivity(intent1);
                    }else {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                Toast.makeText(MainActivity.this,"未接入连接",Toast.LENGTH_SHORT).show();
                            }
                        });
                    }
                } else {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            Toast.makeText(MainActivity.this,"重力控制中",Toast.LENGTH_SHORT).show();
                        }
                    });
                }
            }
        });
        rockerControl.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(!isGravityControl) {
                    if(CarSocket.getSocket()!=null) {
                        Intent intent1 = new Intent(MainActivity.this, RockerActivity.class);
                        startActivity(intent1);
                    }else {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                Toast.makeText(MainActivity.this,"未接入连接",Toast.LENGTH_SHORT).show();
                            }
                        });
                    }
                } else {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            Toast.makeText(MainActivity.this,"重力控制中",Toast.LENGTH_SHORT).show();
                        }
                    });
                }
            }
        });
        buttonExecute.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String tip = textIP.getText().toString();
                if(!tip.equals("")) {
                    ip = tip;
                }
                new ThreadSendCommand(ip).start();
            }
        });
        gravityButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(CarSocket.getSocket()!=null) {
                    if(!isGravityControl) {
                        isGravityControl = true;
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                Toast.makeText(MainActivity.this,"开启重力感应",Toast.LENGTH_SHORT).show();
                            }
                        });
                        textGravity.setVisibility(View.VISIBLE);
                        gravityButton.setText("取消重力");
                    } else {
                        isGravityControl = false;
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                Toast.makeText(MainActivity.this,"取消重力感应",Toast.LENGTH_SHORT).show();
                            }
                        });
                        textGravity.setVisibility(View.INVISIBLE);
                        gravityButton.setText("重力感应");
                    }
                }else {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            Toast.makeText(MainActivity.this,"未接入连接",Toast.LENGTH_SHORT).show();
                        }
                    });
                }
            }
        });
    }

    @Override
    protected void onStart() {
        super.onStart();
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onRestart() {
        super.onRestart();
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    protected void onStop() {
        super.onStop();
    }

    @Override
    protected void onDestroy() {
        CarSocket.killSocket();
        buttonConnect.setText("连接");
        super.onDestroy();
    }

    @Override
    public void onSensorChanged(SensorEvent sensorEvent) {
        if(isGravityControl) {
            X = sensorEvent.values[SensorManager.DATA_X];
            Y = sensorEvent.values[SensorManager.DATA_Y];
            Z = sensorEvent.values[SensorManager.DATA_Z];
            if(Z<bbChangeAngle&&Z>-bbChangeAngle) {
                //不前后移动
                if(X>lrChangeAngle&&X<staticXYZ) {
                    //向左
                    if(gravityControlNum!=ControlNum.Left) {
                        gravityControlNum = ControlNum.Left;
                        new Thread(new SendControl(gravityControlNum.getCom())).start();
                    }
                } else if(X<-lrChangeAngle&&X>-staticXYZ) {
                    //向右
                    if(gravityControlNum!=ControlNum.Right) {
                        gravityControlNum = ControlNum.Right;
                        new Thread(new SendControl(gravityControlNum.getCom())).start();
                    }
                } else {
                    //其他
                    if(gravityControlNum!=ControlNum.Stop) {
                        gravityControlNum = ControlNum.Stop;
                        new Thread(new SendControl(gravityControlNum.getCom())).start();
                    }
                }
            } else {
                //前后移动
                if(Z>0&&Z<staticXYZ) {
                    //向前
                    if(gravityControlNum!=ControlNum.Forward) {
                        gravityControlNum = ControlNum.Forward;
                        gravitySpeedTimes = 0;
                        new Thread(new SendControl(gravityControlNum.getCom())).start();
                    } else {
                        if(Z>bbChangeAngle&&Z<(bbChangeAngle+speedAngle)&&gravitySpeedTimes!=0) {
                            //减速
                            new Thread(new SendControl(ControlNum.Down.getCom())).start();
                            gravitySpeedTimes = 0;
                        } else if(Z>(bbChangeAngle+speedAngle)&&Z<(bbChangeAngle+2*speedAngle)&&gravitySpeedTimes!=1) {
                            if(gravitySpeedTimes<1) {
                                //加速
                                new Thread(new SendControl(ControlNum.Up.getCom())).start();
                            } else {
                                //减速
                                new Thread(new SendControl(ControlNum.Down.getCom())).start();
                            }
                            gravitySpeedTimes = 1;
                        } else if(Z>(bbChangeAngle+2*speedAngle)&&Z<(bbChangeAngle+3*speedAngle)&&gravitySpeedTimes!=2) {
                            if(gravitySpeedTimes<2) {
                                new Thread(new SendControl(ControlNum.Up.getCom())).start();
                            } else {
                                new Thread(new SendControl(ControlNum.Down.getCom())).start();
                            }
                            gravitySpeedTimes = 2;
                        } else if(Z>(bbChangeAngle+3*speedAngle)&&gravitySpeedTimes!=3) {
                            gravitySpeedTimes = 3;
                            new Thread(new SendControl(ControlNum.Up.getCom())).start();
                        }
                    }
                } else if (Z<0&&Z>-staticXYZ) {
                    //向后
                    if(gravityControlNum!=ControlNum.Back) {
                        gravityControlNum = ControlNum.Back;
                        gravitySpeedTimes = 0;
                        new Thread(new SendControl(gravityControlNum.getCom())).start();
                    } else {
                        if(Z<-bbChangeAngle&&Z>-(bbChangeAngle+speedAngle)&&gravitySpeedTimes!=0) {
                            //减速
                            new Thread(new SendControl(ControlNum.Down.getCom())).start();
                            gravitySpeedTimes = 0;
                        } else if(Z<-(bbChangeAngle+speedAngle)&&Z>-(bbChangeAngle+2*speedAngle)&&gravitySpeedTimes!=1) {
                            if(gravitySpeedTimes<1) {
                                //加速
                                new Thread(new SendControl(ControlNum.Up.getCom())).start();
                            } else {
                                //减速
                                new Thread(new SendControl(ControlNum.Down.getCom())).start();
                            }
                            gravitySpeedTimes = 1;
                        } else if(Z<-(bbChangeAngle+2*speedAngle)&&Z>-(bbChangeAngle+3*speedAngle)&&gravitySpeedTimes!=2) {
                            if(gravitySpeedTimes<2) {
                                new Thread(new SendControl(ControlNum.Up.getCom())).start();
                            } else {
                                new Thread(new SendControl(ControlNum.Down.getCom())).start();
                            }
                            gravitySpeedTimes = 2;
                        } else if(Z<-(bbChangeAngle+3*speedAngle)&&gravitySpeedTimes!=3) {
                            gravitySpeedTimes = 3;
                            new Thread(new SendControl(ControlNum.Up.getCom())).start();
                        }
                    }
                } else {
                    //其他
                    gravityControlNum = ControlNum.Stop;
                    gravitySpeedTimes = 0;
                }
            }
//            Log.v("activity", "\n\nx="+X+"\ny="+Y+"\nz="+Z);
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int i) { }

    class ThreadConnect extends Thread{
        public void run(){
            String tip = textIP.getText().toString();
            if(!tip.equals(""))
                ip = tip;
            if(CarSocket.setSocket(ip, port)) {
                buttonConnect.setText("已连接");
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(MainActivity.this,"连接成功",Toast.LENGTH_SHORT).show();
                    }
                });
            } else {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(MainActivity.this,"连接失败",Toast.LENGTH_SHORT).show();
                    }
                });
            }
        }
    }

    class ThreadSendCommand extends Thread {
        private String toIp;
        public ThreadSendCommand(String toIp) {
            this.toIp = toIp;
        }
        public void run() {
            try {
                Socket socket = new Socket(toIp, portCommand);
                OutputStreamWriter oos = new OutputStreamWriter(socket.getOutputStream(),"utf-8");
                oos.write(textCommand.getText().toString());
                oos.flush();
                oos.close();
                socket.close();
            } catch (IOException e) {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(MainActivity.this,"send failure",Toast.LENGTH_SHORT).show();
                    }
                });
            }
        }
    }



}
