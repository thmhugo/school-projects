package codelab4.commons;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map.Entry;

import org.json.JSONException;
import org.json.JSONObject;

import codelab4.datamodel.MapState.Item;
import codelab4.datamodel.MapState.Position;
import javafx.scene.image.Image;

/**
 * Tools contains all the static functions used by mutliple classes.
 */
public class Tools {

	public static <T> ArrayList<T> createArrayListWith(final T...elements) {
		final ArrayList<T> res = new ArrayList<T>();

		for (final T e : elements)
			res.add(e);

		return res;
	}

	public static HashMap<Item, ArrayList<Position>>
	hashMapWithArrayDeepCopy(final HashMap<Item, ArrayList<Position>> original) {
   		HashMap<Item, ArrayList<Position>> copy = new HashMap<Item, ArrayList<Position>>();

		for (final Entry<Item, ArrayList<Position>> entry : original.entrySet())
			   copy.put(entry.getKey(), arrayListDeepCopy(entry.getValue()));

		return copy;
	}

	public static ArrayList<Position> arrayListDeepCopy(final ArrayList<Position> original) {
		ArrayList<Position> copy = new ArrayList<Position>();

		for (final Position p: original)
			copy.add(p.clone());

		return copy;
	}

	@FunctionalInterface
	public static interface BiConsumerWithException<T1, T2> {
		public void accept(T1 arg1, T2 arg2) throws JSONException, InstantiationException;
	}

	public static <T>
	void applyToJSONKeys(JSONObject json, BiConsumerWithException<String,T> biConsumer)
	throws JSONException, NullPointerException, InstantiationException {
		if ( json != null ) {
			Iterator<String> keys = json.keys();

			while ( keys.hasNext() ) {
				String key = keys.next().toString();

				biConsumer.accept(key, (T) json.get(key));
			}
		}
		else
			throw new NullPointerException();
	}

	public static boolean isAnInteger(final String str) {
		Integer integer;

		if ( str.length() >= 1 ) {
			try {
				integer = Integer.valueOf(str);
				return integer >= 1 && integer < Integer.MAX_VALUE;
			}
			catch (NumberFormatException nbe) {
				return false;
			}
		}
		return false;
	}

	public static Image loadImageFromResources(final String path, final int size) {
		return new Image(
					new Tools().getClass().getResource(path).toString(),
					size, size,
					false, false
				);
	}
}
