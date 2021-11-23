package com.polytech.cerveau.labyrinthe;

import android.Manifest;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;

import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.JsonArrayRequest;
import com.android.volley.toolbox.JsonObjectRequest;
import com.android.volley.toolbox.Volley;
import com.choosemuse.libmuse.Muse;
import com.choosemuse.libmuse.MuseListener;
import com.choosemuse.libmuse.MuseManagerAndroid;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.lang.ref.WeakReference;
import java.util.List;

public class ActivityConnectMuse extends Activity {

    String TAG = "ActivityConnectMuse";

    MuseManagerAndroid manager;

    private ArrayAdapter<String> listviewAdapter;

    //private ActivityConnectMuse activityConnectMuse;
    //RequestQueue requestQueue;

    String url = "https://muse-labyrinthe.herokuapp.com/muse/test";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_connect_muse);

        //this.activityConnectMuse = this;

        manager = MuseManagerAndroid.getInstance();
        manager.setContext(this);

        WeakReference<ActivityConnectMuse> weakActivity =
                new WeakReference<>(this);
        manager.setMuseListener(new ActivityConnectMuse.MuseL(weakActivity));

        manager.stopListening();
        manager.startListening();

        requestPermission();
        initUI();

    }

    private void initUI() {
        Button refresh = findViewById(R.id.start);
        //final AlertDialog.Builder popUp = new AlertDialog.Builder(activityConnectMuse);
        //popUp.setTitle("Response GET");

        //requestQueue = Volley.newRequestQueue(activityConnectMuse);


        /*final JsonArrayRequest json = new JsonArrayRequest(
                Request.Method.GET,
                url,
                null,
                new Response.Listener<JSONArray>() {
                    @Override
                    public void onResponse(JSONArray response) {
                        Log.d("SUCCESS", response.toString());

                        for(int i = 0; i < response.length(); i++){
                            try {
                                JSONObject value = response.getJSONObject(i);
                                JSONObject data = value.getJSONObject("data");

                                String firstName = data.getString("firstName");
                                String lastName = data.getString("lastName");
                                String beauty = data.getString("beauty");

                                String message = firstName +"\n"+ lastName +"\n"+ beauty;
                                popUp.setMessage(message);
                            } catch (JSONException e) {
                                e.printStackTrace();
                            }
                        }

                        popUp.show();

                    }
                },
                new Response.ErrorListener() {
                    @Override
                    public void onErrorResponse(VolleyError error) {
                        Log.d("ERROR", error.toString());
                    }
                });
*/



        refresh.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                listviewAdapter.clear();
                manager.stopListening();
                manager.startListening();

                //requestQueue.add(json);
            }
        });

        ListView lv = findViewById(R.id.museList);
        listviewAdapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1);
        lv.setAdapter(listviewAdapter);

        lv.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view,
                                    int position, long id) {

                manager.stopListening(); // Stop listening
                List<Muse> availableMuse = manager.getMuses();
                Intent returnIntent = new Intent();

                if (availableMuse.size() < 1) {
                    Log.d(TAG, "No available muse to connect to!");
                    setResult(RESULT_CANCELED);
                } else {
                    returnIntent.putExtra("pos", position);
                    Log.d(TAG, "pos:" + position);
                    setResult(RESULT_OK, returnIntent);

                }
                finish();
            }
        });

    }

    @Override
    public void onBackPressed() {
        manager.stopListening();
        finish();
    }

    private void requestPermission() {


        if (ContextCompat.checkSelfPermission(this,
                Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {


            DialogInterface.OnClickListener buttonListener =
                    new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int which) {
                            dialog.dismiss();
                            ActivityCompat.requestPermissions(ActivityConnectMuse.this,
                                    new String[]{Manifest.permission.ACCESS_COARSE_LOCATION},
                                    0);
                        }
                    };


            AlertDialog introDialog = new AlertDialog.Builder(this)
                    .setTitle(R.string.permission_dialog_title)
                    .setMessage(R.string.permission_dialog_description)
                    .setPositiveButton(R.string.permission_dialog_understand, buttonListener)
                    .create();
            introDialog.show();
        }
    }

    public void museListChanged() {
        final List<Muse> list = manager.getMuses();
        listviewAdapter.clear();
        for (Muse m : list) {
            listviewAdapter.add(m.getName() + " - " + m.getMacAddress());
        }
    }


    class MuseL extends MuseListener {
        final WeakReference<ActivityConnectMuse> activityRef;

        MuseL(final WeakReference<ActivityConnectMuse> activityRef) {
            this.activityRef = activityRef;
        }

        @Override
        public void museListChanged() {
            activityRef.get().museListChanged();
        }

    }

}
