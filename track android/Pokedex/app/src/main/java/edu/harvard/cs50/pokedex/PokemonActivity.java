package edu.harvard.cs50.pokedex;

import androidx.appcompat.app.AppCompatActivity;

import android.content.SharedPreferences;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.JsonObjectRequest;
import com.android.volley.toolbox.Volley;
import com.squareup.picasso.Picasso;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.net.URL;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

public class PokemonActivity extends AppCompatActivity {
    private TextView nameTextView;
    private TextView numberTextView;
    private TextView type1TextView;
    private TextView type2TextView;
    private TextView pokemon_description;
    private String url;
    private Integer pokemon_id;
    private RequestQueue requestQueue;
    private Button catch_button;
    private ImageView pokemon_avatar;

    public static final String SHARED_PREFS = "sharedPrefs";
    public static final String CAUGHT_POKEMONS = "";

    private Map<String,Boolean> currentMap = new HashMap<>();
    private Boolean caughtOnOff;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pokemon);

        requestQueue = Volley.newRequestQueue(getApplicationContext());
        url = getIntent().getStringExtra("url");
        nameTextView = findViewById(R.id.pokemon_name);
        numberTextView = findViewById(R.id.pokemon_number);
        type1TextView = findViewById(R.id.pokemon_type1);
        type2TextView = findViewById(R.id.pokemon_type2);
        pokemon_avatar = findViewById(R.id.pokemon_avatar);
        pokemon_description = findViewById(R.id.pokemon_description);

        load();
    }

    public void load() {
        type1TextView.setText("");
        type2TextView.setText("");

        JsonObjectRequest request = new JsonObjectRequest(Request.Method.GET, url, null, new Response.Listener<JSONObject>() {
            @Override
            public void onResponse(JSONObject response) {
                try {
                    String name = response.getString("name");
                    nameTextView.setText(name);
                    numberTextView.setText(String.format("#%03d", response.getInt("id")));
                    pokemon_id = response.getInt("id");
                    String avatar_url = response.getJSONObject("sprites").getString("front_default");
                    //Picasso.get().load(avatar_url).into(pokemon_avatar);
                    new DownloadSpriteTask().execute(avatar_url);


                    JSONArray typeEntries = response.getJSONArray("types");
                    for (int i = 0; i < typeEntries.length(); i++) {
                        JSONObject typeEntry = typeEntries.getJSONObject(i);
                        int slot = typeEntry.getInt("slot");
                        String type = typeEntry.getJSONObject("type").getString("name");

                        if (slot == 1) {
                            type1TextView.setText(type);
                        }
                        else if (slot == 2) {
                            type2TextView.setText(type);
                        }
                    }

                    currentMap = loadMap(CAUGHT_POKEMONS);

                    if (currentMap.containsKey(name)) {
                        if (currentMap.get(name) == true) {
                            caughtOnOff = true;
                            catch_button = findViewById(R.id.catch_button);
                            catch_button.setText("Current status: Caught, Release Pokemon?");
                        }
                        else {
                            caughtOnOff = false;
                            catch_button = findViewById(R.id.catch_button);
                            catch_button.setText("Current status: Free to catch!");
                        }
                    }
                    else {
                        caughtOnOff = false;
                        catch_button = findViewById(R.id.catch_button);
                        catch_button.setText("Current status: Free to catch!");
                    }


                    String description_url = "https://pokeapi.co/api/v2/pokemon-species/" + response.getInt("id") + "/";

                    JsonObjectRequest request2 = new JsonObjectRequest(
                            Request.Method.GET, description_url,
                            null, new Response.Listener<JSONObject>() {
                        @Override
                        public void onResponse(JSONObject response) {
                            try {
                                JSONArray typeEntries = response.getJSONArray("flavor_text_entries");
                                for (int i = 0; i < typeEntries.length(); i++) {
                                    JSONObject typeEntry = typeEntries.getJSONObject(i);
                                    if(typeEntry.getJSONObject("language").getString("name").startsWith("en"))
                                    {
                                        pokemon_description.setText(typeEntry.getString("flavor_text"));
                                        break;
                                    }
                                }
                            } catch (JSONException e) {
                                Log.e("cs50", "Pokemon json error", e);
                            }
                        }
                    }, new Response.ErrorListener() {
                        @Override
                        public void onErrorResponse(VolleyError error) {
                            Log.e("cs50", "Pokemon description error", error);
                        }
                    });
                    requestQueue.add(request2);

                } catch (JSONException e) {
                    Log.e("cs50", "Pokemon json error", e);
                }
            }
        }, new Response.ErrorListener() {
            @Override
            public void onErrorResponse(VolleyError error) {
                Log.e("cs50", "Pokemon details error", error);
            }
        });

        requestQueue.add(request);

    }

    public void toggleCatch(View view) {
            if (caughtOnOff == false) {
                caughtOnOff = true;
                currentMap.remove(nameTextView);
                currentMap.put(nameTextView.getText().toString(), true);
                saveMap(CAUGHT_POKEMONS, currentMap);
                catch_button = findViewById(R.id.catch_button);
                catch_button.setText("Current status: Caught, Release Pokemon?");
            } else {
                caughtOnOff = false;
                currentMap.remove(nameTextView);
                currentMap.put(nameTextView.getText().toString(), false);
                saveMap(CAUGHT_POKEMONS, currentMap);
                catch_button = findViewById(R.id.catch_button);
                catch_button.setText("Current status: Free to catch!");
            }
        }

    private void saveMap (String key, Map<String,Boolean> inputMap) {
        SharedPreferences sharedPreferences = getSharedPreferences(CAUGHT_POKEMONS, MODE_PRIVATE);
        if (sharedPreferences != null) {
            JSONObject jsonObject = new JSONObject(inputMap);
            String jsonString = jsonObject.toString();
            SharedPreferences.Editor editor = sharedPreferences.edit();
            editor.remove(key).commit();
            editor.putString(key, jsonString);
            editor.commit();
        }
    }

    private Map<String,Boolean> loadMap(String key) {
        Map<String,Boolean> outputMap = new HashMap<>();
        SharedPreferences sharedPreferences = getSharedPreferences(CAUGHT_POKEMONS, MODE_PRIVATE);
        try{
            if (sharedPreferences != null){
                String jsonString = sharedPreferences.getString(key, (new JSONObject()).toString());
                JSONObject jsonObject = new JSONObject(jsonString);
                Iterator<String> keysItr = jsonObject.keys();
                while(keysItr.hasNext()) {
                    String k = keysItr.next();
                    Boolean v = (Boolean) jsonObject.get(k);
                    outputMap.put(k,v);
                }
            }
        }catch(Exception e){
            e.printStackTrace();
        }
        return outputMap;
    }

    private class DownloadSpriteTask extends AsyncTask<String, Void, Bitmap> {
        @Override
        protected Bitmap doInBackground(String... strings) {
            try {
                URL url = new URL(strings[0]);
                return BitmapFactory.decodeStream(url.openStream());
            }
            catch (IOException e) {
                Log.e("cs50", "Download sprite error", e);
                return null;
            }
        }

        @Override
        protected void onPostExecute(Bitmap bitmap) {
            pokemon_avatar.setImageBitmap(bitmap);
        }
    }

}
