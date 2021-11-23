package com.polytech.cerveau.labyrinthe;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.TextView;

import com.choosemuse.libmuse.Muse;
import com.choosemuse.libmuse.MuseManagerAndroid;
import com.github.lzyzsd.circleprogress.CircleProgress;
import com.jjoe64.graphview.GraphView;

import org.w3c.dom.Text;

import java.util.List;

import controllers.MuseControllers.MuseConnectionHelper;
import controllers.SVMController.SVM_Helper;

import static constants.AppConstants.SVM_MODEL_FN;
import static constants.AppConstants.USE_MUSE;


public class ActivityRealTimeEEGClassifier extends Activity implements View.OnClickListener {

    private final String TAG = "RealTimeEEGClassifier";
    private final Handler handler = new Handler();

    private MuseConnectionHelper museConnectionHelper;
    private MuseManagerAndroid manager;

    private String muse_status;

    private Context context;
    private String name;
    private SVM_Helper sh;

    String ip_string;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        context = this;

        // Load the Muse Library
        manager = MuseManagerAndroid.getInstance();
        manager.setContext(this);

        //Load SVM
        ip_string = getIntent().getStringExtra("IP_ADDRESS");

        sh = new SVM_Helper(context, SVM_MODEL_FN, ip_string);
        museConnectionHelper = new MuseConnectionHelper(sh);

        // Connect Muse Activity
        Log.d("USE_MUSE", USE_MUSE ? "VRAI" : "FAUX");
        if (USE_MUSE) {
            Intent i = new Intent(this, ActivityConnectMuse.class);
            startActivityForResult(i, R.integer.SELECT_MUSE_REQUEST);
        } else {
            init();
        }

    }

    private void init() {
        setContentView(R.layout.activity_eeg_realtime_classifier);

        TextView hsi1 = findViewById(R.id.hsi1);
        TextView hsi2 = findViewById(R.id.hsi2);
        TextView hsi3 = findViewById(R.id.hsi3);
        TextView hsi4 = findViewById(R.id.hsi4);

        // Set up Graph
        GraphView graph = (GraphView) findViewById(R.id.graph);
        graph.getViewport().setYAxisBoundsManual(true);
        graph.getViewport().setMinY(0);
        graph.getViewport().setMaxY(1);

        graph.getViewport().setXAxisBoundsManual(true);
        graph.getViewport().setMinX(0);
        graph.getViewport().setMaxX(45);
        sh.setupGraph(graph);

        TextView tv_muse_status = findViewById(R.id.tv_muse_status);

        TextView ip_textview = findViewById(R.id.ip_value);
        ip_textview.setText(ip_string);

        CircleProgress pbMeditionMeter = findViewById(R.id.pb_meditation_meter);
        sh.setPb_meditation_meter(pbMeditionMeter);

        museConnectionHelper.setHSITextView(hsi1, hsi2, hsi3, hsi4);

        museConnectionHelper.setTv_muse_status(tv_muse_status);
        museConnectionHelper.updateGUI.run();

        handler.post(sh.processEEG);




    }

    @Override
    public void onClick(View v) {

    }





    /*
     *  -------------- Return from startActivityForResult ------------------
     */


    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {

        if (requestCode == R.integer.SELECT_MUSE_REQUEST) {
            if (resultCode == RESULT_OK) {

                int position = data.getIntExtra("pos", 0);
                List<Muse> availableMuse = manager.getMuses();
                connect_to_muse(availableMuse.get(position));
                init();
            } else {
                finish();
            }
        }
    }

    private void connect_to_muse(Muse muse) {
        museConnectionHelper.setMuse(muse);
        museConnectionHelper.connect_to_muse();
    }

//    @Override
public void onBackPressed() {

//        museConnectionHelper
}

}
