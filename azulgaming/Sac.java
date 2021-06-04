import java.util.ArrayList;

public class Sac extends Distribution{
	protected Sac () {
		array = new ArrayList <Tuile> ();
		
		for (String color: Tuile.colorlist) {
			for (int i = 0 ; i < 20 ; i++) 
				array.add(new Tuile(color));
		}

		array.add(new TuilePremierJoueur());
	}
	
	// Overwrite car on ne veut pas que le Sac puisse se vider.
	protected ArrayList <Tuile> emptyOut () {
		return null;
	}
	
	// Cette methode va servir a remplir une Fabrique de 4 tuiles.
	protected ArrayList <Tuile> giveToFabrique () {
		ArrayList <Tuile> tab = new ArrayList <Tuile> ();
		
		for (int i = 0 ; i < 4 ; i ++) {
			tab.add(drawElement());
		}
		return tab;
	}
	
	protected Tuile getTuilePJ () {
		for (Tuile t : array) {
			if (t.getColor().equals("First")) {
				array.remove(t);
				return t;
			}
		}
		return null;
	}
	
	protected boolean enoughTuile () {
		return array.size() > 4;
	}
}
