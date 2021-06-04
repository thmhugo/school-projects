import java.util.ArrayList;

public abstract class TabJoueur {
	protected ArrayList <Case> array;
	
	protected int length () {
		int cpt = 0;
		for (Case c : array) {
			if (c.getTuile() != null) 
				cpt++;
		}
		return cpt;
	}
	
	protected boolean isEmpty () {
		return array.isEmpty();
	}
	
	protected abstract boolean isLegalMove (int x, int y);
	
	protected void addElement (int x, int y, Tuile element) {
		for (Case c : array) {
			if (c.getX() == x && c.getY() == y)
				c.setElement(element);
		}
	}
}
