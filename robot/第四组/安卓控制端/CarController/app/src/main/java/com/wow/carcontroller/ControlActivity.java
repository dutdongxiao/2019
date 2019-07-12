package com.wow.carcontroller;

import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.widget.ImageButton;

import androidx.appcompat.app.AppCompatActivity;

import java.io.DataOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.Socket;

public class ControlActivity extends AppCompatActivity implements View.OnTouchListener {

    private static ControlNum control = ControlNum.NNNN;
    private ImageButton imageButtonForward;
    private ImageButton imageButtonBack;
    private ImageButton imageButtonRight;
    private ImageButton imageButtonLeft;
    private ImageButton imageButtonUp;
    private ImageButton imageButtonDown;
    private ImageButton imageButtonStop;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_control);
        imageButtonForward = findViewById(R.id.imageButtonForward);
        imageButtonBack = findViewById(R.id.imageButtonBack);
        imageButtonRight = findViewById(R.id.imageButtonRight);
        imageButtonLeft = findViewById(R.id.imageButtonLeft);
        imageButtonUp = findViewById(R.id.imageButtonUp);
        imageButtonDown = findViewById(R.id.imageButtonDown);
        imageButtonStop = findViewById(R.id.imageButtonStop);
        imageButtonForward.setOnTouchListener(this);
        imageButtonBack.setOnTouchListener(this);
        imageButtonRight.setOnTouchListener(this);
        imageButtonLeft.setOnTouchListener(this);
        imageButtonRight.setOnTouchListener(this);
        imageButtonUp.setOnTouchListener(this);
        imageButtonDown.setOnTouchListener(this);
        imageButtonStop.setOnTouchListener(this);
    }

    @Override
    public boolean onTouch(View view, MotionEvent motionEvent) {
        view.performClick();
        if (motionEvent.getAction() == MotionEvent.ACTION_DOWN) {
            switch (view.getId()) {
                case R.id.imageButtonForward:
                    control = ControlNum.Forward;
                    break;
                case R.id.imageButtonBack:
                    control = ControlNum.Back;
                    break;
                case R.id.imageButtonRight:
                    control = ControlNum.Right;
                    break;
                case R.id.imageButtonLeft:
                    control = ControlNum.Left;
                    break;
                case R.id.imageButtonUp:
                    control = ControlNum.Up;
                    break;
                case R.id.imageButtonDown:
                    control = ControlNum.Down;
                    break;
                case R.id.imageButtonStop:
                    control = ControlNum.Stop;
                    break;
            }
            new Thread(new SendControl(control.getCom())).start();
            return true;
        } else if (motionEvent.getAction() == MotionEvent.ACTION_UP) {
            return true;
        }
        return false;
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
        super.onDestroy();
    }

}
