package com.polytech.cerveau.labyrinthe;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.StrictMode;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;


public class MainActivity extends Activity implements OnClickListener {

    private final String TAG = "Main_Activity!";
    private EditText editText;
    private TextView tv_feedback;
    String ip;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        if (android.os.Build.VERSION.SDK_INT > 9)
        {
            StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitAll().build();
            StrictMode.setThreadPolicy(policy);
        }

        Button recordData = findViewById(R.id.start);
        recordData.setOnClickListener(this);

        //et_name = findViewById(R.id.nameField);
        tv_feedback = findViewById(R.id.tv_name_feedback);

    }


    @Override
    public void onClick(View v) {

        if (v.getId() == R.id.start) {

            editText = findViewById(R.id.editText);
            ip = editText.getText().toString();
            Log.d("log_value", ip);

            Intent myIntent = new Intent(MainActivity.this,
                    ActivityRealTimeEEGClassifier.class);
            myIntent.putExtra("IP_ADDRESS", ip);
            startActivity(myIntent);

        }

    }
}
