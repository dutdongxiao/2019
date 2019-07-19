package com.example.myapplication3;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

public class MainActivity extends AppCompatActivity {

    String ipAddress = null;
    int port = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button linkStart = (Button) findViewById(R.id.linkStart);
        linkStart.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                EditText inputIpAddr = (EditText) findViewById(R.id.ipAddress);
                EditText inputPort = (EditText) findViewById(R.id.port);
                ipAddress = inputIpAddr.getText().toString();
                port = Integer.parseInt(inputPort.getText().toString());
                System.out.println(ipAddress+port);

                Intent intent = new Intent(MainActivity.this,UDPSendMessageActivity.class);
                intent.putExtra("ipAddress",ipAddress);
                intent.putExtra("port",port);
                startActivity(intent);
            }
        });
    }

}
