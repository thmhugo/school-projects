import java.util.ArrayList;

public class Mur extends TabJoueur{
	protected Mur (boolean isNormal) {
		array = new ArrayList<Case>();
		if (isNormal)
			fillWallInit();
		else {
			for (int y = 0 ; y < 5 ; y++) {
				for (int x = 0 ; x < 5 ; x++) 
					array.add(new Case (x, y, this, null, "Incolore")) ;
			}
		}
	}
	
	private void fillWallInit () {
		int k = 0;
        for(int i = 0 ; i < 5 ; i++){
            for(int j = 0 ; j < 5 ; j++){
                int n = (k - i) %5;
                String c = Tuile.colorlist.get(n);
                array.add(new Case(j,i,this,null,c));
                k++;
            }
        }
	}
	
	protected void printOneLine (int y) {
		for (int x = 0 ; x < 5 ; x ++ ) {
			for (Case c : array) {
				if (c.getX() == x && c.getY() == y) {
					if (c.isEmpty()) 
						System.out.print(c.getInitialColor()+" ");
					else
						System.out.print(c.getTuile().getInitialColor()+" ");
				}
			}
		}
	}

	private boolean isCompleteLine (int y) {
		int cpt = 0;
		
		for (Case c : array) 
			if (c.getY() == y && !c.isEmpty())
				cpt++;
		
		return cpt == 5;
	}

	protected boolean isComplete() {
		for (int i = 0 ; i < 5 ; i++) {
			if (isCompleteLine(i))
				return true;
		}
		return false;
	}
	
	protected void addElement (int x, int y, Tuile element) {
		// Overwrite car on ne veut pas creer de nouvelle case dans Mur.
		for (Case c : array) {
			if (c.getX() == x && c.getY() == y)
				c.setElement(element);
		}
	}

	protected boolean isLegalMove(int x, int y) {
		if (x > 5 || y > 5 || x < 0 || y < 0) 
			return false;
		else {
			for (Case c : array) {
				if (c.getX() == x && c.getY() == y) 
					return false;
			}
			return true;
		}
	}
	
	protected boolean isLegalLineColorYCase(String color, int y) {
		if (color.compareTo("Joker") == 0)
			return true;
		
		for (Case c : array) {
			if (c.getY() == y && !c.isEmpty() &&c.getColor().compareTo(color) == 0) 
				return false;
		}
		return true;
	}
	

	protected int coordXColor (String color,int y) {
		// On retourne la coordonnee x de la couleur et de la ligne associee dans le mur.
		for (Case c : array) {
			if (c.getY() == y && c.getColor().compareTo(color) == 0) 
				return c.getX();
		}
		return -1;
	}
	
	protected int numberOfNeighbour(int x, int y) {
		/* Retourne le nombre de voisins horizontalement et verticalement.
		   Total vaut 8 car la case qui a ete place reduit le nombre de colonne et de ligne possible
		   car on repasse 2 fois par la meme case de facon vertical et horizontal. */
		int cptEmpty = 0, total = 8;	
		
		for (Case c : array) {
			if (c.getY() == y && c.getX() == x);
			else if (c.getY() == y && c.isEmpty()) 
				cptEmpty ++;
			else if (c.getX() == x && c.isEmpty())
				cptEmpty ++;
		}
		return (total - cptEmpty);
	}
	
	protected int numberOfColumnComplete () {
		// Retourne le nombre de colonne complte.
		int cpt = 0, nbrColumn = 0;
		
		for (int i = 0 ; i < 5 ; i++) {
			for (Case c : array) {
				if (c.getX() == i)
					cpt++;
			}
			
			if (cpt == 5)
				nbrColumn ++;
			
			cpt = 0;
		}
		return nbrColumn;
	}
	
	protected int sameColorComplete () {
		// Retourne le nombre de case de couleur complete.
		int cpt = 0, nbrColor = 0;
		ArrayList <String> color = Tuile.colorlist;
		
		for (String s : color) {
			for (Case c : array) {
				if (!c.isEmpty() && c.getTuile().getColor().compareTo(s) == 0)
					cpt++;
			}
			if (cpt == 5)
				nbrColor ++;
			
			cpt = 0;
		}
		return nbrColor;
	}

	protected int locationIsOccupied (String color, int y) {
		/* Methode permettant de savoir si a la position y et a la couleur correspondante
		   est occupe ou non. 
		   Elle renvoie -1 si elle est occupee sinon elle renvoie la coordonnee x de la place vacante. */
		for (Case c : array) {
			if (c.getY() == y && c.isEmpty() && c.getColor().compareTo(color) == 0)
				return c.getX();
		}
		return -1;
	}
	
	protected Tuile locationIsOccupied (int x, int y) {
		/* Methode permettant de savoir si a la position y et a la couleur correspondante
		   est occupe ou non. 
		   Elle renvoie -1 si elle est occupee sinon elle renvoie la coordonnee x de la place vacante. */
		for (Case c : array) {
			if (c.getY() == y && c.getX() == x && !c.isEmpty())
				return c.getTuile();
		}
		return null;
	}
	
	//----- Fonctions pour le mur incolore -----//
	
	protected boolean isAlmostFullOrOneColor (String color) {
		return nbrColorRemaining(color) == 1 || nbrColorRemaining(color) == 5;
	}
	
	private Case returnBeginningDiag (int x, int y) {
		// Methode qui permet de remonter dans la diagonale le plus en haut a gauche possible.
		int i = x, j = y;
		Case ca = null;
		
		while (i >= 0 && j >= 0) {
			for (Case c : array)
				if (c.getX() == i && c.getY() == j) 
					ca = c;
			i--;
			j--;
		}
		return ca;
	}
	
	protected boolean isLegalLineColorX (String color, int x) {
		// Methode qui regarde a la coordonne x si il y a deja une tuile de meme couleur.
		for (Case c : array)
			if (c.getX() == x && !c.isEmpty() &&c.getTuile().getColor().compareTo(color) == 0) 
				return false;
		return true;
	}
	
	private boolean isLegalLineColorY (String color,int x, int y) {
		// Methode qui regarde a la coordonnee y si la couleur peut etre placer a la coordonnee x.
		for (Case c: array)
			if (c.getY() == y && c.getX() != x && !c.isEmpty() && c.getTuile().getColor().compareTo(color) == 0)
				return false;
		return true;
	}
	
	protected boolean isLegalDiagColor (String color, int x, int y) {
		/* Diagonale descendente vers la droite.
		   On regarde si sur la diagonale il y a une couleur differente,
		   si c'est le cas on renvoie faux. */
		Case ca = returnBeginningDiag(x, y);
		int i = ca.getX(), j = ca.getY();
		
		while (i < 5 && j < 5) {
			for (Case c : array) {
				boolean b = c.getX() == i && c.getY() == j;
				if (b && !isLegalLineColorY(color, i, j)
						|| b && !c.isEmpty() && c.getTuile().getColor().compareTo(color) != 0) 
					return false;
			}
			i++;
			j++;
		}
		return true;
	}
	
	protected int nbrCaseDiag (int x, int y) {
		// Calcule le nombre de case en diagonale selon la coordonnee x et y de la case d'origine.
		Case ca = returnBeginningDiag(x, y);
		int i = ca.getX(), j = ca.getY(), cpt = 0;
		while (i < 5 && j < 5) {
			for (Case c : array)
				if (c.getX() == i && c.getY() == j) 
					cpt++;
			i++;
			j++;
		}
		return cpt;
	}

	protected boolean isCorrectLocation (int x, int y , String color) {
		/* Methode qui verifie si le placement de la tuile est correct selon:
	  	   Si la tuile va dans un coin mais qu'il reste qu'une seule tuile ou toutes les tuiles de la meme couleur a mettre.
		   Ou dans le cas il reste 5 tuiles a mettre mais que la diagonale est plus petite */
		if (isLegalCorner(x, y, color) && isAlmostFullOrOneColor(color))
			return true;
		else if (nbrCaseDiag(x, y) - nbrColorRemaining(color) != 0 && nbrColorRemaining(color) == 5)
			return true;
		else
			return nbrCaseDiag(x, y) - nbrColorRemaining(color) == 0;
	}
	
	protected boolean isLegalCorner (int x, int y, String color) {
		/* Methode qui verifie que la coordonnee x,y est un coin special et qui verifie
		dans les deux coins si il y a deja une tuile de la meme couleur ou non */
		switch (x) {
		case 0: if (y == 4) break;
		case 4: if (y == 0) break;
		default: return false;
		}
		
		for (Case c : array)
			switch (c.getX()) {
			case 0: if (c.getY() == 0 && !c.isEmpty() && c.getTuile().getColor().compareTo(color) == 0) return false;
			case 4: if (c.getY() == 4 && !c.isEmpty() && c.getTuile().getColor().compareTo(color) == 0) return false;
			}
		return true;
	}
	
	protected int hasADiagToComplete (String color, int y) {
		// Renvoie la coordonne x d'une case en diagonale de la meme couleur a completer.
		for (Case c : array) {
			if (!c.isEmpty() && c.getTuile().getColor().compareTo(color) == 0) {
				Case ca = hasADiagToCompleteAux (c.getX(),c.getY(),y,color);
				if (ca != null) 
					return ca.getX();
			}
		}
		return -1;
	}
	
	private Case hasADiagToCompleteAux (int x,int y,int yy, String color) {
		// L'attribut yy correspond a la coordonne y de la ligne qui va etre mise dans le mur.
		Case ca = returnBeginningDiag(x, y);
		int i = ca.getX(), j = ca.getY();
		
		while (i < 5 && j < 5)
			for (Case c : array) {
				if (c.getX() == i && c.getY() == j && c.getY() == yy && c.isEmpty() && isLegalLineColorX(color, c.getX()))
					return c;
			i++;
			j++;
		}
		return null;
	}
	
	protected int nbrColorRemaining (String color) {
		// Renvoie le nombre de tuile de la couleur restante a placer sur le mur.
		int cpt = 0;
		for (Case c : array) {
			if (!c.isEmpty() && c.getTuile().getColor().compareTo(color) == 0)
				cpt  ++;
		}
		return 5-cpt;
	}
}