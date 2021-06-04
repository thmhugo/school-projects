package codelab4.vizualisation;

import java.util.ArrayList;
import java.util.HashMap;

import codelab4.App;
import codelab4.commons.Tools;
import codelab4.datamodel.MapState;
import codelab4.datamodel.MapState.Item;
import codelab4.datamodel.MapState.Position;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.Background;
import javafx.scene.layout.BackgroundImage;
import javafx.scene.layout.BackgroundPosition;
import javafx.scene.layout.BackgroundRepeat;
import javafx.scene.layout.BackgroundSize;
import javafx.scene.layout.Pane;

public class MapGenerator {

	// Must be a multiple of 16 make the positioning of objects easier.
	private static int MAP_SIDE;
	private static int CELL_WITDH;

	private static final String PNG_PATH = "/src/resources/png/";

	private Pane mapDisplayPane;
	private MapState mapState;

	public MapGenerator(MapState mapState, Pane mapDisplayPane) {
		MapGenerator.MAP_SIDE = App.LOW_RES ? 336 : 496;
		MapGenerator.CELL_WITDH = MAP_SIDE / 16;

		this.mapState = mapState;
		initMapDisplayPane(mapDisplayPane);
	}

	private void initMapDisplayPane(Pane mapDisplayPane) {
		final BackgroundImage bgImg = createBackgroundImage();

		this.mapDisplayPane = mapDisplayPane;
		this.mapDisplayPane.setPrefSize(MAP_SIDE, MAP_SIDE);
		this.mapDisplayPane.setBackground(new Background(bgImg));
	}

	public void generateMap() {
		final HashMap<Item, ArrayList<Position>> states = mapState.getState();

		for (final Item entry : states.keySet()) {
			if ( entry == Item.CHARACTER )
				generateCharacter();
			else if ( entry.isDisplayable() )
				generateDisplayableItems(states, entry);
		}
	}

	private void generateDisplayableItems(
		final HashMap<Item, ArrayList<Position>> states, final Item entry
	) {
		final String name = entry.name().toLowerCase();

		for (final Position p : states.get(entry))
			mapDisplayPane.getChildren().add(generateImageview(p, name));
	}

	private void generateCharacter() {
		String pngName = "character-";
		final Position direction = mapState.getDirectionPos();

		if (direction.equals(MapState.NORTH))
			pngName += "north";
		else if (direction.equals(MapState.SOUTH))
			pngName += "south";
		else if (direction.equals(MapState.EAST))
			pngName += "east";
		else if (direction.equals(MapState.WEST))
			pngName += "west";

		if ( pngName != "character-" ) {
			mapDisplayPane.getChildren().add(
				generateImageview(mapState.getCharacterPos(), pngName)
			);
		}
	}

	private BackgroundImage createBackgroundImage() {
		final Image img = Tools.loadImageFromResources(PNG_PATH + "tilemap.png", MAP_SIDE);
		final BackgroundImage bgImg = new BackgroundImage(
										img,
										BackgroundRepeat.REPEAT,
										BackgroundRepeat.NO_REPEAT,
										BackgroundPosition.DEFAULT,
										BackgroundSize.DEFAULT
									);
		return bgImg;
	}

	private ImageView generateImageview(final Position p, final String name) {
		final Image img = Tools.loadImageFromResources(
								PNG_PATH + name + ".png",
								CELL_WITDH
							);
		final ImageView imageView = new ImageView(img);

		imageView.relocate((p.getX() + 1) * CELL_WITDH, (p.getY() + 1) * CELL_WITDH);

		return imageView;
	}

	public MapState getMapState() {
		return mapState;
	}

	public void setMapState(MapState mapState) {
		this.mapState = mapState;
	}

	public Pane getGeneratedPane() {
		return mapDisplayPane;
	}
}
