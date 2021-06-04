import java.util.ArrayList;

public class Plancher extends TabJoueur{
	protected Plancher () {
		floorTab();
	}
	
	// Initialisation du tableau. (methode utilisee dans le constructeur et dans emptyOut)
	private void floorTab () {
		array = new ArrayList<Case>();
		for (int x = 0 ; x < 7 ; x++) 
			array.add(new Case(x, 0, this, null));
	}
	
	protected void resetFloor () {
		floorTab();
	}
	
	protected void print() {
		for (int i = 0 ; i < 7 ; i ++) {
			if (i == 0 || i == 1) 
				System.out.print("-1 ");
			else if (i == 5 || i == 6)
				System.out.print("-3 ");
			else
				System.out.print("-2 ");
		}
		
		System.out.println();
		
		for (Case c: array) {
			if (c.getTuile() != null)
				System.out.print(c.getTuile().getInitialColor()+"  ");
			else
				System.out.print(".  ");
		}
	}
	
	protected ArrayList <Tuile> emptyOut () {
		ArrayList <Tuile> tab = new ArrayList<Tuile>();
		for (Case c : array) 
			if (!c.isEmpty() && c.getTuile().getColor().compareTo("First") != 0)
				tab.add(c.getTuile());
		floorTab();
		return tab;
	}

	protected boolean isComplete() {
		return length() == 7;
	}

	protected boolean isLegalMove(int x, int y) {
		if (x > 7 || y > 1 || y < 0 || x < 0 ) 
			// Le plancher a une taille maximale de 7 et une hauteur de 1.
			return false;
		else {
			for (Case c : array) {
				if (c.getX() == x && c.getY() == y && c.getTuile() != null) 
					return false;
			}
			return true;
		}
	}
	
	protected void addElement (Tuile element) {
		for (Case c : this.array) {
			if (c.isEmpty()) {
				c.setElement(element);
				return;
			}
		}
	}
}