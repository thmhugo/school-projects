import java.util.ArrayList;
import java.util.Arrays;

public class LignesMotif extends TabJoueur{
	protected LignesMotif () {
		array = new ArrayList<Case>();
		int x = 0;
		for (int y = 0 ; y < 5 ; y++) {
			for (int i = 0; i <= x; i++) {
				array.add(new Case(i, y, this, null));
			}
			x++;
		}
	}
	
	private void resetLineTab (int y) {
		for (Case c : array) {
			if (c.getY() == y) {
				c.setElement(null);
			}
		}
	}
	
	private String spacing (int k) {
		// Fonction qui sert juste pour l'impression console afin d'espacer.
		char[] tab = new char[k];
		Arrays.fill(tab, ' ');
		return new String(tab);
	}
	
	protected void printOneLine (int y) {
		System.out.print(spacing(4*3-y*3));
		/* Les multiplications par 3 viennent du fait que le Sysout juste en bas affiche 3 charactres dont 2 espaces
		   Il faut donc 3 fois plus d'espaces pour obtenir un affichage conforme.
		   Le 4 est la valeur max du tableau  */
		for (int x = 0 ; x < y+1 ; x ++ ) {
			for (Case c : array) {
				if (c.getX() == x && c.getY() == y) {
					if (c.isEmpty()) 
						System.out.print(".  ");
					else
						System.out.print(c.getTuile().getInitialColor()+" ");
				}
			}
		}
	}
	
	protected boolean isCompleteLine (int y) {
		// Par convention -1 correspond  la valeur par defaut.
		int cpt = -1;
		for (Case c : array) {
			if (c.getY() == y && !c.isEmpty())
				cpt++;
		}
		return cpt == y;
	}

	protected boolean isLegalMove(int x, int y) {
		// On verifie que les coordonnes ne soient pas hors du tableau.
		if (y < 0 || y > 4) return false;

		switch (y) {
		case 0: if (x != 0) return false;
		case 1: if (x < 0 || x > 1) return false;
		case 2: if (x < 0 || x > 2) return false;
		case 3: if (x < 0 || x > 3) return false;
		case 4: if (x < 0 || x > 4) return false;
		}
		
		for (Case c : array) {
			if (c.getX() == x && c.getY() == y && c.getTuile() != null) 
				return false;
		}
		return true;
	}
	
	protected boolean sameColor (int y, Tuile t) {
		String color = t.getColor();
		
		for (Case c : array) {
			boolean b = !c.isEmpty() && c.getTuile().getColor().compareTo(color) != 0 && c.getTuile().getColor().compareTo("Joker") != 0;
			
			if (y == c.getY() && !c.isEmpty() && t.getColor().compareTo("Joker") == 0);
			else if (y == c.getY() && b)
				return false;
		}
		return true;
	}
	
	protected ArrayList <Tuile> removeSameY (int y) {
		ArrayList <Tuile> t = new ArrayList<Tuile>();
		for (Case c : array) {
			if (c.getY() == y && !c.isEmpty()) {
				t.add(c.getTuile());
			}
		}
		/* On ne met pas toutes les tuiles dans la defausse car
		   l'une des tuiles sera place sur le mur. */
		t.remove(0);
		resetLineTab(y);
		
		return t;
	}
	
	protected Tuile getTuileInY (int y) {
		// Cette methode retourne une tuile a la coordonnee x selon y
		int nbrJoker = numberOfJokerInLine(y);
		
		// On verifie qu'il y a un au moins un joker dans la ligne et qu'elle n'est pas full de joker
		if (nbrJoker != -1 && nbrJoker != y) {
			for (Case c : array) {
				/* Il faut faire attention au cas ou il y a un joker dans la ligne.
				   En effet il faut retourner une tuile de couleur pour qu'on puisse
				   par la suite la placer correctement sur la case de couleur dans le mur. */
				if (c.getY() == y && !c.isEmpty() && c.getTuile().getColor().compareTo("Joker") != 0)
					return c.getTuile();
			}
		}
		else {
			for (Case c : array) {
				if (c.getY() == y && !c.isEmpty())
					return c.getTuile();
			}
		}
		return null;
	}
	
	protected int emptyLocationInLine (int y) {
		// Methode qui renvoie une coordonnee x d'un emplacement libre dans une ligne
		for (Case c : array) {
			if (c.getY() == y && c.isEmpty()) 
				return c.getX();
		}
		return -1;
	}
	
	protected int numberTuileInLine (int y) {
		int cpt = 0;
		for (Case c : array) {
			if (c.getY() == y && !c.isEmpty()) 
				cpt ++;
		}
		return cpt;
	}
	
	protected int numberOfJokerInLine (int y) {
		int cpt = -1;
		for (Case c : array) {
			if (c.getY() == y && !c.isEmpty() && c.getTuile().getColor().compareTo("Joker") == 0) 
				cpt ++;
		}
		return cpt;
	}
	
	protected Tuile getJoker (int y) {
		for (Case c : array) {
			if (c.getY() == y && !c.isEmpty() && c.getTuile().getColor().compareTo("Joker") == 0) 
				return c.getTuile();
		}
		return null;
	}
	
	protected Tuile getTuile (int x, int y) {
		for (Case c : array) {
			if (c.getX() == x && c.getY() == y && !c.isEmpty())
				return c.getTuile();
		}
		return null;
	}
}