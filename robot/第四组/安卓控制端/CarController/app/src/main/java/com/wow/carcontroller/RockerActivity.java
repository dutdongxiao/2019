package com.wow.carcontroller;

import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.widget.ImageButton;

import androidx.appcompat.app.AppCompatActivity;



public class RockerActivity extends AppCompatActivity {

    private static ControlNum control = ControlNum.NNNN;
    private ImageButton imageButtonUp;
    private ImageButton imageButtonDown;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_rocker);

        imageButtonUp = findViewById(R.id.imageButtonUp);
        imageButtonDown = findViewById(R.id.imageButtonDown);

        imageButtonUp.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                switch (motionEvent.getAction()){
                    case MotionEvent.ACTION_DOWN:
                        new Thread(new SendControl(ControlNum.Up.getCom())).start();
                        break;
                    case MotionEvent.ACTION_UP:
                        break;
                    default:
                        return false;
                }
                return true;
            }
        });
        imageButtonDown.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                switch (motionEvent.getAction()){
                    case MotionEvent.ACTION_DOWN:
                        new Thread(new SendControl(ControlNum.Down.getCom())).start();
                        break;
                    case MotionEvent.ACTION_UP:
                        break;
                    default:
                        return false;
                }
                return true;
            }
        });

        //摇杆控制
        RockerView rockerView = findViewById(R.id.rockerView);
        if (rockerView != null) {
            rockerView.setOnShakeListener(new RockerView.OnShakeListener() {
                @Override
                public void onStart() { }

                @Override
                public void direction(RockerView.Direction direction) {
                    switch(direction) {
                        case DIRECTION_UP:
                            control = ControlNum.Forward;
                            break;
                        case DIRECTION_DOWN:
                            control = ControlNum.Back;
                            break;
                        case DIRECTION_RIGHT:
                            control = ControlNum.Right;
                            break;
                        case DIRECTION_LEFT:
                            control = ControlNum.Left;
                            break;
                        default:
                            break;
                    }
                    new Thread(new SendControl(control.getCom())).start();
                }

                @Override
                public void onFinish() {
                    control = ControlNum.Stop;
                    new Thread(new SendControl(control.getCom())).start();
                }
            });
        }
    }

}
