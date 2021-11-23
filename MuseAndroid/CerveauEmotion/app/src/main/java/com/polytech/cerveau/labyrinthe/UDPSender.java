package com.polytech.cerveau.labyrinthe;

import android.os.AsyncTask;
import android.util.Log;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.UnknownHostException;

public class UDPSender extends AsyncTask<String, Void, Void> {

    final static int port = 9436;
    InetAddress serveur;
    DatagramSocket socket;
    DatagramPacket donneesEmises;
    String ip;

    public UDPSender(String ip_value){
        ip = new String();
        ip = ip_value;
    }

    protected Void doInBackground(String... voids) {

        try {
            Log.d("ip_value in UDPSender", ip);
            serveur = InetAddress.getByName(ip);
            int length = voids[0].length();
            byte buffer[] = voids[0].getBytes();
            Log.d("log_value", voids[0]);
            socket = new DatagramSocket();
            donneesEmises = new DatagramPacket(buffer, length, serveur, port);
            socket.send(donneesEmises);



        } catch (Exception e) {
            e.printStackTrace();
        }

        return null;
    }

}
