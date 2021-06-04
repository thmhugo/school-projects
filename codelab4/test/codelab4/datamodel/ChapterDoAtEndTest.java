package codelab4.datamodel;

import static org.junit.Assert.*;

import org.json.JSONException;
import org.json.JSONObject;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import codelab4.datamodel.Chapter.DoAtEnd;
import codelab4.datamodel.MapState.Item;
import codelab4.datamodel.MapState.Position;

public class ChapterDoAtEndTest {

	@Test
	public void testCreateValid() throws JSONException, InstantiationException {
		String testJSONString = "{" +
			"\"item_to_create\" : \"AXE\"," +
			"\"recipe\" : {" +
				"\"BRANCH\" : 3," +
				"\"STONE\" : 1 }," +
			"\"message\" : \"Tu as tout ce qu'il te faut pour fabriquer ta hache !\"}";

		JSONObject validJSON = new JSONObject(testJSONString);
		DoAtEnd doAtEndValid = new Chapter.DoAtEnd(validJSON);

		assertEquals( doAtEndValid.getItemToCreate(), Item.AXE );
		assertEquals( (int)doAtEndValid.getRecipe().get(Item.BRANCH), 3 );
		assertEquals( (int)doAtEndValid.getRecipe().get(Item.STONE), 1 );
		assertEquals(
			"Tu as tout ce qu'il te faut pour fabriquer ta hache !",
			doAtEndValid.getMessage()
		);
		assertEquals( null, doAtEndValid.getPositionItemCreated() );
	}

	@Test
	public void testCreateValid2() throws JSONException, InstantiationException {
		String testJSONString = "{" +
			"\"item_to_create\" : \"SHELTER\"," +
			"\"recipe\" : {" +
				"\"WOOD\" : 3," +
				"\"LEAF\" : 3 }," +
			"\"message\" : \"Super ! Tu as un abri qui va te tenir au sec !\","+
			"\"position\" : [6,12]}";

		JSONObject validJSON = new JSONObject(testJSONString);
		DoAtEnd doAtEndValid = new Chapter.DoAtEnd(validJSON);

		assertEquals( doAtEndValid.getItemToCreate(), Item.SHELTER );
		assertEquals( (int)doAtEndValid.getRecipe().get(Item.WOOD), 3 );
		assertEquals( (int)doAtEndValid.getRecipe().get(Item.LEAF), 3 );
		assertEquals(
			"Super ! Tu as un abri qui va te tenir au sec !",
			doAtEndValid.getMessage()
		);
		assertEquals(new Position(6,12), doAtEndValid.getPositionItemCreated() );
	}

	@Test(expected=InstantiationException.class)
	public void testCreateUnvalidItemToCreate()
	throws JSONException, InstantiationException {
		String testJSONString = "{" +
			"\"item_to_create\" : \"AXEs\"," +
			"\"recipe\" : {" +
				"\"BRANCH\" : 3," +
				"\"STONE\" : 1 }," +
			"\"message\" : \"Tu as tout ce qu'il te faut pour fabriquer ta hache !\"}";
		JSONObject validJSON = new JSONObject(testJSONString);
		new Chapter.DoAtEnd(validJSON);
	}

	@Test(expected=JSONException.class)
	public void testCreateDuplicateItemInRecipe()
	throws JSONException, InstantiationException {
		String testJSONString = "{" +
			"\"item_to_create\" : \"AXE\"," +
			"\"recipe\" : {" +
				"\"BRANCH\" : 3," +
				"\"BRANCH\" : 1 }," +
			"\"message\" : \"Tu as tout ce qu'il te faut pour fabriquer ta hache !\"}";
		JSONObject validJSON = new JSONObject(testJSONString);
		new Chapter.DoAtEnd(validJSON);
	}

	@Test(expected=InstantiationException.class)
	public void testCreateEmptyMessageAttribute()
	throws JSONException, InstantiationException {
		String testJSONString = "{" +
			"\"item_to_create\" : \"AXE\"," +
			"\"recipe\" : {" +
				"\"BRANCH\" : 3," +
				"\"STONE\" : 1 }," +
			"\"message\" : \"\"}";
		JSONObject validJSON = new JSONObject(testJSONString);
		new Chapter.DoAtEnd(validJSON);
	}
}
