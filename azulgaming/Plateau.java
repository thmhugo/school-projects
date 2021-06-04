import java.util.ArrayList;
import java.util.Scanner;

public class Plateau {
	private Joueur player;
	private Plancher floor;
	private LignesMotif lines;
	private Mur wall;
	
	protected Plateau () {
		floor = new Plancher ();
		lines = new LignesMotif();
		wall = new Mur(Jeu.typeGame == 'n' || Jeu.typeGame == 'e');
	}
	
	protected void printBoard () {
		System.out.println("\n"+"Lignes motifs            Mur");
		for (int y = 0 ; y < 5 ; y ++) {
			System.out.print(y+1+ " | ");
			lines.printOneLine(y);
			System.out.print("    ");
			wall.printOneLine(y);
			System.out.println();
		}
		System.out.println("\n"+"      Plancher");
		floor.print();
		System.out.println("\n");
	}
	
	protected void winPoints (int x, int y) {
		player.addScore(wall.numberOfNeighbour(x, y)+1);
	}
	
	protected void winPointsEnd () {
		// Les chiffres ne sortent pas de nul part, cf les regles.
		if (wall.isComplete()) 
			player.addScore(2);
		
		int numberOfColumn = wall.numberOfColumnComplete();
		int numberOfColor = wall.sameColorComplete();
		
		player.addScore(numberOfColumn*7);
		player.addScore(numberOfColor*10);
		
	}
	
	protected void losePoints () {
		if (!floor.isEmpty()) {
			for (Case c : floor.array)
				if (!c.isEmpty())
					switch(c.getX()) {
					case 0: case 1: player.minusScore(1); break;
					case 2: case 3: case 4: player.minusScore(2); break;
					case 5: case 6: player.minusScore(3); break;
					}
			// On vide le plancher et on le defausse.
			Jeu.discard.fill(floor.emptyOut());
			floor.resetFloor();
		}
	}
	
	protected boolean lineInWallComplete () {
		// methode de condition de victoire si une ligne est complete dans le mur.
		return wall.isComplete();
	}
	
	protected void addTuileArrayToPlancher (ArrayList <Tuile> t) {
		for(Tuile tile : t) 
			floor.addElement(tile);
	}
	
	protected boolean addTuileToPlancher (Tuile t) {
		// Si le tableau floor est plein alors on le defausse directement.
		if (floor.isComplete()) {
			Jeu.discard.addTuile(t);
			return false;
		}
		else {
			floor.addElement(t);
			return true;
		}
	}

	protected boolean canAddTuileToLine(int y, Tuile t){
		int x = lines.emptyLocationInLine(y);
		return lines.isLegalMove(x, y) && lines.sameColor(y, t) && !lines.isCompleteLine(y) && wall.isLegalLineColorYCase(t.getColor(), y);
	}

	protected boolean addTuileToLine (int y, Tuile t) {
		// On verifie que x et y soient bien dans les Lignes, que la couleur correspond bien a la ligne,
		// qu'elle n'est pas complete et que la ligne dans le mur ne possede pas une tuile de la meme couleur.
		int x = lines.emptyLocationInLine(y);
		
		if (canAddTuileToLine(y,t)) {
			lines.addElement(x, y, t);
			return true;
		}
		else
			return false;
	}
	
	protected boolean addLineToWall (int y) {
		Tuile t = lines.getTuileInY(y);
		
		if (t == null) 
			return false;
		
		if (lines.isCompleteLine(y)) 
			switch (Jeu.typeGame) {
			case 'e': return addLineToWallJoker(y,t);
			case 'n': return addLineToWallNormal(y,t);
			default: return addLineToWallVar(y,t);
			}
		else 
			return false;
	}
	
	private void endTurn (int x, int y, Tuile t) {
		// Calcul des gains de point et on defausse le reste de la ligne.
		wall.addElement(x, y, t);
		winPoints(x, y);
		Jeu.discard.fill(lines.removeSameY(y));
		Jeu.discard.remove(t);
	}
	
	private boolean addLineToWallNormal (int y, Tuile t) {
		int x = wall.coordXColor(t.getColor(),y);
			
		if (x == -1)
			return false;
		endTurn(x, y, t);
		return true;
	}
	
	private int giveXJoker (int y) {
		if (Jeu.typeLaunch == 'c') {
			Scanner sc  = new Scanner(System.in);
			System.out.println("Veuillez selectionner une couleur du mur ou mettre la tuile Joker (sur la meme ligne que la ligneMotif completee).");
			while (true) {
				String s = sc.next(); 
				if (Tuile.colorlist.contains(s) && s.compareTo("Joker") != 0) {
					// On regarde si la position est occupee ou non
					if (wall.locationIsOccupied(s, y) != -1)
						return wall.locationIsOccupied(s, y);
				}
				System.out.println("Veuillez reessayer.");
			}
		}
		else 
			return player.getControler().askCoordinateX(player,"Joker");
	}
	
	private boolean addLineToWallJoker (int y, Tuile t) {
		int x = -1;
		int nbrJoker = lines.numberOfJokerInLine(y);
		// x vaut -1 car c'est la valeur par defaut.
		// On regarde si la ligne est pleine de joker.
		if (nbrJoker == y) {
			// Si c'est le cas, il faut choisir une couleur (emplacement libre) ou mettre la tuile joker.
			x = giveXJoker(y);
		}
		else 
			x = wall.coordXColor(t.getColor(),y);
			
		if (x == -1)
			return false;
			
		else {
			if (nbrJoker != -1) 
				// Si il y a un Joker dans la ligne, il faut placer la tuile dans le mur. (cf regle)
				t = lines.getJoker(y);
			
			endTurn(x, y, t);
			return true;
		}
	}
	
	private boolean conditionToAddLine (int x, int y,String color) {
		return wall.isLegalDiagColor(color, x, y) && wall.isLegalLineColorX(color, x) && wall.isCorrectLocation(x, y, color);
	}
	
	private int giveXVar (int y, String color) {
		int x = -1;
		if (Jeu.typeLaunch == 'c') {
			Scanner sc  = new Scanner(System.in);
			boolean condition = false, condition1 = false;
			
			System.out.println("Veuillez selectionner une coordonnee x (entre 1 et 5) ou mettre la tuile de couleur: "+color);
			while (true) {
				try{
					x = sc.nextInt()-1; 
					condition = x >= 0 && x <= 4 && wall.locationIsOccupied("Incolore", y) != -1;
					condition1 = conditionToAddLine(x, y, color) && wall.isAlmostFullOrOneColor(color) && wall.isLegalCorner(x, y, color);

					if (condition &&  condition1
							|| condition && conditionToAddLine(x, y, color) && !wall.isLegalCorner(x, y, color))  
							return x;
					else 
						System.out.println("Veuillez reessayer, valeur incorrecte.");
				}
				catch(Exception e){
					System.out.println("Veuillez reessayer, valeur incorrecte.");
				}
			}
		}
		else 
			return player.getControler().askCoordinateX(player,color);
	}

	private boolean addLineToWallVar (int y, Tuile t) {
		int x = -1;
		String color = t.getColor();
		
		// Cas ou il n'y a pas de diagonale de meme couleur a completer dans le mur.
		if (wall.nbrColorRemaining(color) > 0 && wall.hasADiagToComplete(color,y) == -1)
			x = giveXVar(y, color);
		else
			/* Methode qui complete directement la coordonnee x par une coordonnee d'une case qui a besoin
			d'etre rempli en diagonale par la meme couleur. */
			x = wall.hasADiagToComplete(color,y);
		
		if (wall.isLegalDiagColor(color, x, y)) {
			endTurn(x, y, t);
			return true;
		}
		else 
			return false;
	}

	protected boolean floorContainsFirst(){
		// Methode qui determine si le plancher contient un jeton premier joueur.
		for(Case c : floor.array){
			if(c.getTuile() != null && c.getTuile().getColor().compareTo("First") == 0)
				return true;
		}
		return false;
	}
	
	//----- Getters & setters -----//

	protected LignesMotif getLines(){
		return lines;
	}
	
	protected Mur getWall(){
		return wall;
	}
	
	protected void setPlayer (Joueur j) {
		player = j;
	}

	protected Plancher getFloor(){
		return this.floor;
	}
}
