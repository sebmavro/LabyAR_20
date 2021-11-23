package model;

import android.util.Log;

import com.android.volley.Request;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.JsonArrayRequest;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

public class RequestEEG {

    private JsonArrayRequest req = null;
    private String url = "https://muse-labyrinthe.herokuapp.com/muse/test?progress=";

    public RequestEEG(int progress){
        Log.d("CREATE URL", url + progress);
        req = new JsonArrayRequest(
                Request.Method.GET,
                url + progress,
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
                            } catch (JSONException e) {
                                e.printStackTrace();
                            }
                        }

                    }
                },
                new Response.ErrorListener() {
                    @Override
                    public void onErrorResponse(VolleyError error) {
                        Log.d("ERROR", error.toString());
                    }
                });
    }

    public JsonArrayRequest getReq() {
        return req;
    }


}
