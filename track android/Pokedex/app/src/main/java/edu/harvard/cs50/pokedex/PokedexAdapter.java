package edu.harvard.cs50.pokedex;

import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Filter;
import android.widget.Filterable;
import android.widget.LinearLayout;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.JsonObjectRequest;
import com.android.volley.toolbox.Volley;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.List;

public class PokedexAdapter extends RecyclerView.Adapter<PokedexAdapter.PokedexViewHolder> implements Filterable {

    // PokedexAdapter has a subclass PokedexViewHolder
    // Among other things it is responsible for changing activity to details of a pokemon
    public static class PokedexViewHolder extends RecyclerView.ViewHolder {
        public LinearLayout containerView;
        public TextView textView;

        PokedexViewHolder(View view) {
            super(view);

            containerView = view.findViewById(R.id.pokedex_row);
            textView = view.findViewById(R.id.pokedex_row_text_view);

            containerView.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    Pokemon current = (Pokemon) containerView.getTag();
                    Intent intent = new Intent(v.getContext(), PokemonActivity.class);
                    intent.putExtra("url", current.getUrl());

                    v.getContext().startActivity(intent);
                }
            });
        }
    }

    // variables that we will use inside the class
    private List<Pokemon> pokemon = new ArrayList<>();
    private List<Pokemon> filtered = new ArrayList<>(pokemon);
    private RequestQueue requestQueue;

    // ok this is the constructor of the class PokedexAdapter
    // It's executed or available when an object is created
    // It triggers the loadPokemon method which will populate a list of pokemon
    PokedexAdapter(Context context) {
        requestQueue = Volley.newRequestQueue(context);
        loadPokemon();
    }

    // this method queries the API and loads the list of pokemons
    public void loadPokemon() {
        String url = "https://pokeapi.co/api/v2/pokemon?limit=151";
        JsonObjectRequest request = new JsonObjectRequest(Request.Method.GET, url, null, new Response.Listener<JSONObject>() {
            @Override
            public void onResponse(JSONObject response) {
                try {
                    JSONArray results = response.getJSONArray("results");
                    for (int i = 0; i < results.length(); i++) {
                        JSONObject result = results.getJSONObject(i);
                        String name = result.getString("name");
                        pokemon.add(new Pokemon(
                            name.substring(0, 1).toUpperCase() + name.substring(1),
                            result.getString("url")
                        ));
                    }
                    notifyDataSetChanged();
                } catch (JSONException e) {
                    Log.e("cs50", "Json error", e);
                }
            }
        }, new Response.ErrorListener() {
            @Override
            public void onErrorResponse(VolleyError error) {
                Log.e("cs50", "Pokemon list error", error);
            }
        });

        requestQueue.add(request);
    }

    // Then we’ll override onCreateViewHolder, after our view holder is created, to create our view that represents a row.
    // Apparently the big idea is to create an object that will populate itself with some value on the way instead of everything in RAM.
    @NonNull
    @Override
    public PokedexViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.pokedex_row, parent, false);

        return new PokedexViewHolder(view);
    }

    // We’ll also need onBindViewHolder to set the values of each row, given a position of the row.
    // It does this by assigning the view holder to a position, and calling the adapter's onBindViewHolder() method.
    // That method uses the view holder's position to determine what the contents should be, based on its list position.
    @Override
    public void onBindViewHolder(@NonNull PokedexViewHolder holder, int position) {
        Pokemon current;

        if(filtered.size() == 0) {
            current = pokemon.get(position);
        }
        else {
            current = filtered.get(position);
        }
        holder.textView.setText(current.getName());
        holder.containerView.setTag(current);
    }

    // This is a method of RecyclerViews
    // we Override the getItemCount method to return the size of the ArrayList of filtered pokemons instead of the full item count ?
    @Override
    public int getItemCount() {
        if (filtered.size() == 0) {
            return pokemon.size();
        }
        else {
            return filtered.size();
        }
    }

    // we override a method getFilter() and inside we create an object that we return
    // this object is of class PokemonFilter
    // the class PokemonFilter has methods PokemonFilter.performFiltering(text of the SearchView)
    // the method above returns the results (basically it returns a FilterResults object containing the pokemon names ?)
    // why would we need to get an object back?
    // why not just perform the update on the RecyclerView and be done?

    @Override
    public Filter getFilter() {
        return new PokemonFilter();
    }

    // this is an object which takes the constraint text and has a method which returns a filtered ArrayList
    // logically we should call PokemonFilter.performFiltering on an object when we need to update the filtered ArrayList
    // The results returned apparently get used in publishResults to insert them inside the ArrayList filtered
    private class PokemonFilter extends Filter {

        // this method proceeds to filtering and returns an ArrayList with only the pokemons that were filtered
        // it does performFiltering with the CharSequence constraint as parameter.
        // constraint should be what was typed in the SearchView.
        @Override
        protected FilterResults performFiltering(CharSequence constraint) {
            FilterResults results = new FilterResults();
            List<Pokemon> filteredPokemon = new ArrayList<>();
            if (constraint == null || constraint.length() == 0) {
                filteredPokemon.addAll(pokemon);
            }
            else {
                for (Pokemon current_pokemon : pokemon) {
                    // Big O notation is n because we do one pass over the whole set of items pokemon
                    if (current_pokemon.getName().toLowerCase().startsWith(constraint.toString().toLowerCase())) {
                        filteredPokemon.add(current_pokemon);
                    }
                }
            }
            results.values = filteredPokemon;
            return results;
        }

        // This method "publishes" the results or rather copies them inside of the List filtered
        @Override
        protected void publishResults(CharSequence constraint, FilterResults results) {
            filtered.clear();
            filtered.addAll((List<Pokemon>) results.values);
            notifyDataSetChanged();
        }
    }
}
