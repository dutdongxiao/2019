package com.example.myapplication3;

import android.Manifest;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.Switch;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import com.baidu.speech.EventListener;
import com.baidu.speech.EventManager;
import com.baidu.speech.EventManagerFactory;

import com.baidu.speech.asr.SpeechConstant;
import com.example.myapplication3.UDPSocket.UDPFrameRecService;
import com.example.myapplication3.UDPSocket.UDPSocket;


import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.TreeMap;

public class UDPSendMessageActivity extends AppCompatActivity {

    String ipAddress = null;
    int port = 0;
    private UDPSocket socket;
    public boolean TG = false;
    public boolean VR = false;

    public SensorManager sensorManager;
    public Sensor accelerometer;
    public Sensor magnetic;
    private Sensor mSensor;
    public float[] accelerometerValues = new float[3];
    public float[] magneticFieldValues = new float[3];

    private float timestamp = 0;
    private float angle[] = new float[3];
    private static final float NS2S = 1.0f / 1000000000.0f;
    private float gx = 0,gy = 0,gz = 0;

    protected boolean enableOffline = false; // 测试离线命令词，需要改成true
    private int backTrackInMs = 1500;
    View.OnTouchListener myOnTouchListener;
    View.OnTouchListener myCameraOnTouchListener;
    CompoundButton.OnCheckedChangeListener myOnCheckedChangeListener;
    EventManager wp;
    EventManager asr;
    EventListener wpListener;
    EventListener asrListener;

    Button btnCarUp;
    Button btnCarDown;
    Button btnCarLeft;
    Button btnCarRight;

    Button btnCameraUp;
    Button btnCameraDown;
    Button btnCameraLeft;
    Button btnCameraRight;

    ImageView imageView;

    Switch switchRecord;
    Switch switchVideo;
    Switch switchVR;
    Switch switchTG;

    EditText azimuthAngle;
    EditText pitchAngle;
    EditText rollAngle;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_other);
        //获得上个页面传来的参数
        Intent intent = getIntent();
        ipAddress = intent.getStringExtra("ipAddress");
        port = intent.getIntExtra("port", port);
        System.out.println(ipAddress + port);
        //获得传感器
        sensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        accelerometer = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        magnetic = sensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD);
        mSensor = sensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE);

        InitView();
        initPermission();//动态获取权限
        wp = EventManagerFactory.create(this,"wp");
        asr = EventManagerFactory.create(this,"asr");


        socket = new UDPSocket(this,ipAddress,port);
        socket.startUDPSocket();



        final Handler handler = new Handler(){
            @Override
            public void handleMessage(@NonNull Message msg) {//处理子进程发来的图片显示在view上
                super.handleMessage(msg);
                if(msg.what == 1101){
                    Bitmap bitmap = (Bitmap) msg.obj;
                    imageView.setImageBitmap(bitmap);
                }
            }
        };

        UDPFrameRecService udpFrameRecService = new UDPFrameRecService(handler);
        udpFrameRecService.startReceiveThread();

            //设置监听，多个按钮对应一个监听  --->长按监听
        myOnTouchListener = new View.OnTouchListener() {

            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                String message = null;
                if (view.getId() == R.id.btnCarUp) {
                        switch (motionEvent.getAction()) {
                            case MotionEvent.ACTION_DOWN:

                                socket.sendMessage("F$");
                                break;
                            case MotionEvent.ACTION_UP:

                                socket.sendMessage("S$");
                                break;
                        }
                    } else if (view.getId() == R.id.btnCarDown) {
                        switch (motionEvent.getAction()) {
                            case MotionEvent.ACTION_DOWN:
                                //SendMessage("B");
                                socket.sendMessage("B$");
                                break;
                            case MotionEvent.ACTION_UP:
                                //SendMessage("S");
                                socket.sendMessage("S$");
                                break;
                        }
                    } else if (view.getId() == R.id.btnCarLeft) {
                    switch (motionEvent.getAction()) {
                            case MotionEvent.ACTION_DOWN:
                                //SendMessage("L");
                                socket.sendMessage("L$");
                                break;
                            case MotionEvent.ACTION_UP:
                                //SendMessage("S");
                                socket.sendMessage("S$");
                                break;
                        }
                } else if (view.getId() == R.id.btnCarRight) {
                        switch (motionEvent.getAction()) {
                            case MotionEvent.ACTION_DOWN:
                                //SendMessage("R");
                                socket.sendMessage("R$");
                                break;
                            case MotionEvent.ACTION_UP:
                                //SendMessage("S");
                                socket.sendMessage("S$");
                                break;
                        }
                    }
                return false;
            }
        };

        myCameraOnTouchListener = new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                String message = null;
                if (view.getId() == R.id.btnCameraUp){
                    switch (motionEvent.getAction()){
                        case MotionEvent.ACTION_DOWN:
                            //SendMessage("u");
                            socket.sendMessage("u$");
                            break;
                        case MotionEvent.ACTION_UP:
                            //SendMessage("s");
                            socket.sendMessage("s$");
                            break;
                    }
                }else if (view.getId() == R.id.btnCameraDown){
                    switch (motionEvent.getAction()){
                        case MotionEvent.ACTION_DOWN:
                            //SendMessage("d");
                            socket.sendMessage("d$");
                            break;
                        case MotionEvent.ACTION_UP:
                            //SendMessage("s");
                            socket.sendMessage("s$");
                            break;
                    }
                }else if (view.getId() == R.id.btnCameraLeft){
                    switch (motionEvent.getAction()){
                        case MotionEvent.ACTION_DOWN:
                            //SendMessage("l");
                            socket.sendMessage("l$");
                            break;
                        case MotionEvent.ACTION_UP:
                            //SendMessage("s");
                            socket.sendMessage("s$");
                            break;
                    }
                }else if (view.getId() == R.id.btnCameraRight){
                    switch (motionEvent.getAction()){
                        case MotionEvent.ACTION_DOWN:
                            //SendMessage("r");
                            socket.sendMessage("r$");
                            break;
                        case MotionEvent.ACTION_UP:
                            //SendMessage("s");
                            socket.sendMessage("s$");
                            break;
                    }
                }

                return false;
            }

        };

        myOnCheckedChangeListener = new CompoundButton.OnCheckedChangeListener() {
            boolean flag = true;
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                switch (compoundButton.getId()){
                    case R.id.switchRecord:
                        if (b){
                            start();
                        }else {
                            stop();
                        }
                        break;
                    case R.id.switchVideo:
                        if (b){
                            if (flag){
                                socket.sendMessage("startVideo");
                                flag = false;
                            }
                            imageView.setVisibility(View.VISIBLE);
                        }else {
                            socket.sendMessage("endVideo");
                            imageView.setVisibility(View.INVISIBLE);
                        }
                        break;
                    case R.id.switchVR:
                        if (b){
                            switchTG.setChecked(false);
                            HiddenUI(View.INVISIBLE);
                            VR = true;
                            gx = 0; gy = 0; gz = 0;
                            azimuthAngle.setText("gx:"+ Math.round(gx) + "");
                            pitchAngle.setText("gz:" + Math.round(gz) + "");
                            //发送初始位置
                            if ((Math.round(gx) + 50)< 255){
                                socket.sendMessage("a" + "%"+ (Math.round(gx) + 50) + "%" + 0 + "%" + "$");
                            }/*else {
                                socket.sendMessage("a" + 250 + "%" +((Math.round(gx) + 50) - 250) + "%" + "$");
                            }*/
                            socket.sendMessage("A" + "%"+122 + "%" + "$");

                        }else {
                            VR = false;
                            socket.sendMessage("s$");
                            socket.sendMessage("S$");
                            HiddenUI(View.VISIBLE);
                        }
                        break;
                    case  R.id.switchTG:
                        if (b){
                            switchVR.setChecked(false);
                            HiddenUI(View.INVISIBLE);
                            TG = true;
                        }else {
                            socket.sendMessage("s$");
                            socket.sendMessage("S$");
                            HiddenUI(View.VISIBLE);
                            TG = false;
                        }
                        break;
                }

            }
        };

        wpListener = new EventListener() {
            @Override
            public void onEvent(String name, String params, byte [] data, int
                    offset, int length) {
                Log.d("语音唤醒功能：", String.format("event: name=%s, params=%s", name, params));
                pitchAngle.setText("开启唤醒");
                //唤醒事件
                if(name.equals("wp.data")){
                    try {
                        JSONObject json = new JSONObject(params);
                        int errorCode = json.getInt("errorCode");
                        if(errorCode == 0){
                            //唤醒成功
                            //txtLog.setText("");
                            pitchAngle.setText("唤醒成功！！");

                            Map<String, Object> voiceParams = new LinkedHashMap<String, Object>();
                            String event = SpeechConstant.ASR_START; // 替换成测试的event
                            //event = SpeechConstant.ASR_START; // 替换成测试的event

                            if (enableOffline) {
                                voiceParams.put(SpeechConstant.DECODER, 2);
                            }

                            voiceParams.put(SpeechConstant.ACCEPT_AUDIO_VOLUME, false);

                            voiceParams.put(SpeechConstant.VAD, SpeechConstant.VAD_DNN);
                            // 如识别短句，不需要需要逗号，使用1536搜索模型。其它PID参数请看文档
                            voiceParams.put(SpeechConstant.PID, 1536);
                            if (backTrackInMs > 0) {
                                // 方案1  唤醒词说完后，直接接句子，中间没有停顿。开启回溯，连同唤醒词一起整句识别。
                                // System.currentTimeMillis() - backTrackInMs ,  表示识别从backTrackInMs毫秒前开始
                                voiceParams.put(SpeechConstant.AUDIO_MILLS, System.currentTimeMillis() - backTrackInMs);
                            }
                            asr.send(SpeechConstant.ASR_CANCEL, "{}", null, 0, 0);//取消识别
                            String jsonMessage = new JSONObject(voiceParams).toString();// 这里可以替换成你需要测试的json
                            asr.send(event, jsonMessage, null, 0, 0);
                        } else {
                            //唤醒失败
                            pitchAngle.setText("唤醒失败");
                            //start();
                        }
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }
                } else if("wp.exit".equals(name)){
                        //唤醒已停止
                    pitchAngle.setText("唤醒停止");
                }
            }
        };
        asrListener = new EventListener() {
            @Override
            public void onEvent(String name, String params, byte[] bytes, int i, int i1) {
                if(name.equals(SpeechConstant.CALLBACK_EVENT_ASR_READY)){
                    // 引擎就绪，可以说话，一般在收到此事件后通过UI通知用户可以说话了
                    pitchAngle.setText("请开始说话");

                }
                if(name.equals(SpeechConstant.CALLBACK_EVENT_ASR_PARTIAL)){
                    // 识别结束
                    try {
                        JSONObject result = new JSONObject(params);
                        String compare = result.getString("result_type");
                        if (compare.equals("final_result") ){
                            String recResult = result.getString("best_result");
                            pitchAngle.setText(recResult);
                            recAfterSend(recResult);
                        }
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }

                }if (name.equals(SpeechConstant.CALLBACK_EVENT_ASR_END)){
                    pitchAngle.setText("说话结束");
                }
                    // ... 支持的输出事件和事件支持的事件参数见“输入和输出参数”一节
            }

        };

        setListeners();



    }//OnCreate结束

    public void InitView(){//初始化view

        btnCarUp = (Button) findViewById(R.id.btnCarUp);
        btnCarDown = (Button) findViewById(R.id.btnCarDown);
        btnCarLeft = (Button) findViewById(R.id.btnCarLeft);
        btnCarRight = (Button) findViewById(R.id.btnCarRight);

        btnCameraUp = (Button) findViewById(R.id.btnCameraUp);
        btnCameraDown = (Button) findViewById(R.id.btnCameraDown);
        btnCameraLeft = (Button) findViewById(R.id.btnCameraLeft);
        btnCameraRight = (Button) findViewById(R.id.btnCameraRight);

        imageView = (ImageView) findViewById(R.id.imageView);
        switchRecord = (Switch) findViewById(R.id.switchRecord);
        switchVideo = (Switch) findViewById(R.id.switchVideo);
        switchVR = (Switch) findViewById(R.id.switchVR);
        switchTG = (Switch) findViewById(R.id.switchTG);

        azimuthAngle = (EditText) findViewById(R.id.azimuthAngle);
        pitchAngle = (EditText) findViewById(R.id.pitchAngle);
        rollAngle = (EditText) findViewById(R.id.rollAngle);

    }

    public void setListeners(){

        btnCarUp.setOnTouchListener(myOnTouchListener);
        btnCarDown.setOnTouchListener(myOnTouchListener);
        btnCarLeft.setOnTouchListener(myOnTouchListener);
        btnCarRight.setOnTouchListener(myOnTouchListener);

        btnCameraUp.setOnTouchListener(myCameraOnTouchListener);
        btnCameraDown.setOnTouchListener(myCameraOnTouchListener);
        btnCameraLeft.setOnTouchListener(myCameraOnTouchListener);
        btnCameraRight.setOnTouchListener(myCameraOnTouchListener);

        switchRecord.setOnCheckedChangeListener(myOnCheckedChangeListener);
        switchVideo.setOnCheckedChangeListener(myOnCheckedChangeListener);
        switchVR.setOnCheckedChangeListener(myOnCheckedChangeListener);
        switchTG.setOnCheckedChangeListener(myOnCheckedChangeListener);

        sensorManager.registerListener(new MySensorEventListener(),accelerometer,Sensor.TYPE_ACCELEROMETER);
        sensorManager.registerListener(new MySensorEventListener(),magnetic,Sensor.TYPE_MAGNETIC_FIELD);
        sensorManager.registerListener(new MySensorEventListener(), mSensor, SensorManager.SENSOR_DELAY_GAME);

        wp.registerListener(wpListener);
        asr.registerListener(asrListener);

    }

    class MySensorEventListener implements SensorEventListener {
        float memoreyx = gx;
        float memoreyz = gz;
        @Override
        public void onSensorChanged(SensorEvent sensorEvent) {
            //System.out.println("进入传感器!!!!!!!!!");
            if (sensorEvent.sensor.getType() == Sensor.TYPE_ACCELEROMETER){
                accelerometerValues = sensorEvent.values;
            }else if (sensorEvent.sensor.getType() == Sensor.TYPE_MAGNETIC_FIELD){
                magneticFieldValues = sensorEvent.values;
            }else if (sensorEvent.sensor.getType() == Sensor.TYPE_GYROSCOPE) {
                if (timestamp != 0) {
                    final float dT = (sensorEvent.timestamp - timestamp) * NS2S;
                    angle[0] += sensorEvent.values[0] * dT;
                    angle[1] += sensorEvent.values[1] * dT;
                    angle[2] += sensorEvent.values[2] * dT;

                    float anglex = (float) Math.toDegrees(angle[0]);
                    float angley = (float) Math.toDegrees(angle[1]);
                    float anglez = (float) Math.toDegrees(angle[2]);



                    if (gx != 0) {
                        float c = gx - anglex;
                        if (Math.abs(c) >= 2) {

                            gx = anglex % 360;
                        }

                    } else {
                        gx = anglex % 360;
                    }
                    if (gy != 0) {
                        float c = gy - angley;
                        if (Math.abs(c) >= 2) {

                            gy = angley % 360;
                        }
                    } else {
                        gy = angley % 360;
                    }
//                            if(gz != 0){
//                                Log.d("================", "anglex------------>" + (gz - anglez));
//                            }


                    gz = anglez % 360;

                }
                timestamp = sensorEvent.timestamp;


                if (VR){
                    if ( memoreyx < gx){
                        socket.sendMessage("r$");
                        //socket.sendMessage("a" + (Math.round(gx) + 172) +"%" + 0 +"%"+ "$");
                    }else if (memoreyx > gx){
                        socket.sendMessage("l$");
                        //socket.sendMessage("a" + 250 +"%" +((Math.round(gx) + 172) - 250)+ "%" + "$");
                    }/*else if (memoreyz < gz){
                        socket.sendMessage("l$");
                    }else if (memoreyz > gz){
                        socket.sendMessage("r$");
                    }*/
                    memoreyx = gx;
                    memoreyz = gz;

                    //sendMessage
                    azimuthAngle.setText("gx:"+ Math.round(gx) + "");
                    pitchAngle.setText("gz:" + Math.round(gz) + "");
                }
            }
                calculateOrientation();
            }



        @Override
        public void onAccuracyChanged(Sensor sensor, int i) {

        }
    }
    public void calculateOrientation(){
        float[] values = new float[3];
        float[] R = new float[9];
        int[] result = new int[3];

        SensorManager.getRotationMatrix(R,null,accelerometerValues,magneticFieldValues);
        SensorManager.getOrientation(R,values);

        for (int i = 0; i < values.length; i ++){
            values[i] = (float) Math.toDegrees(values[i]);
            result[i] = Math.round(values[i]);
            //System.out.println(result[i]+"||||");

        }
        //得到俯仰角



        if (VR){
            rollAngle.setText("俯仰角：" + result[2] + "");
            //socket.sendMessage("A" + result[2] + "$");//上下角度1
            if (result[2] > 30){
                socket.sendMessage("d$");
            }else if (result[2] < -30){
                socket.sendMessage("u$");
            }
        }

        if(TG){
            if (result[2] >= 0 && Math.abs(result[2]) > 25){
                socket.sendMessage("B$");
            }else if (result[2] < 0 && Math.abs(result[2]) > 25){
                socket.sendMessage("F$");
            }else if (result[1] >= 0 && Math.abs(result[1]) > 10){
                socket.sendMessage("L$");
            }else if (result[1] < 0 && Math.abs(result[1]) > 10){
                socket.sendMessage("R$");
            }

        }


    }
    public void HiddenUI(final int INVISIBLEorVISIBLE){

        btnCameraDown.setVisibility(INVISIBLEorVISIBLE);
        btnCameraLeft.setVisibility(INVISIBLEorVISIBLE);
        btnCameraRight.setVisibility(INVISIBLEorVISIBLE);
        btnCameraUp.setVisibility(INVISIBLEorVISIBLE);
        btnCarDown.setVisibility(INVISIBLEorVISIBLE);
        btnCarLeft.setVisibility(INVISIBLEorVISIBLE);
        btnCarRight.setVisibility(INVISIBLEorVISIBLE);
        btnCarUp.setVisibility(INVISIBLEorVISIBLE);

    }
    /**
     * android 6.0 以上需要动态申请权限
     */
    private void initPermission() {
        String permissions[] = {Manifest.permission.RECORD_AUDIO,
                Manifest.permission.ACCESS_NETWORK_STATE,
                Manifest.permission.INTERNET,
                Manifest.permission.READ_PHONE_STATE,
                Manifest.permission.WRITE_EXTERNAL_STORAGE
        };

        ArrayList<String> toApplyList = new ArrayList<String>();

        for (String perm : permissions) {
            if (PackageManager.PERMISSION_GRANTED != ContextCompat.checkSelfPermission(this, perm)) {
                toApplyList.add(perm);
                // 进入到这里代表没有权限.

            }
        }
        String tmpList[] = new String[toApplyList.size()];
        if (!toApplyList.isEmpty()) {
            ActivityCompat.requestPermissions(this, toApplyList.toArray(tmpList), 123);
        }

    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        // 此处为android 6.0以上动态授权的回调，用户自行实现。
    }
    /**
     * 测试参数填在这里
     * 基于SDK唤醒词集成第2.1 设置唤醒的输入参数
     */
    private void start() {
        //txtLog.setText("");
        // 基于SDK唤醒词集成第2.1 设置唤醒的输入参数
        Map<String, Object> params = new TreeMap<String, Object>();
        params.put(SpeechConstant.ACCEPT_AUDIO_VOLUME, false);
        params.put(SpeechConstant.WP_WORDS_FILE, "assets:///WakeUp.bin");
        // "assets:///WakeUp.bin" 表示WakeUp.bin文件定义在assets目录下

        // 这里可以替换成你需要测试的json
        String json = new JSONObject(params).toString();
        wp.send(SpeechConstant.WAKEUP_START, json, null, 0, 0);
        //printLog("输入参数：" + json);
    }

    // 基于SDK唤醒词集成第4.1 发送停止事件
    private void stop() {
        asr.send(SpeechConstant.ASR_STOP, "{}", null, 0, 0); //
        wp.send(SpeechConstant.WAKEUP_STOP, "{}", null, 0, 0);
    }

   /* @Override
    protected void onDestroy() {
        super.onDestroy();
        wp.send(SpeechConstant.WAKEUP_STOP, "{}", null, 0, 0);
    }*/
   public void recAfterSend(String recResult){
       if (recResult.indexOf("小车小车前进") >= 0){
           socket.sendMessage("F$");
       }else if (recResult.indexOf("小车小车左转") >= 0){
           socket.sendMessage("L$");
       }else if (recResult.indexOf("小车小车右转") >= 0){
           socket.sendMessage("R$");
       }else if (recResult.indexOf("小车小车后退") >= 0 || recResult.indexOf("小车小车到") >= 0 ||
               recResult.indexOf("小车小车道") >=0 || recResult.indexOf("小车小车倒") >= 0){
           socket.sendMessage("B$");
       }else if (recResult.indexOf("小车小车停") >= 0){
           socket.sendMessage("S$");
       }
   }
}
