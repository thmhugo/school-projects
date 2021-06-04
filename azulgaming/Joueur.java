import java.util.*;
import java.awt.*;

public class Joueur{
	private String name;
	private Plateau board;
	private int score;
	protected boolean firstToPlay;
	private AppControler cont;
	private AppModel mod;

	protected Joueur(String name){
		this.name = name;
		board = new Plateau();
		board.setPlayer(this);
		this.score = 0;
	}
	
	protected String giveChoiceColor () {
		/* Renvoie la couleur du tas choisi.
		   Le tas etant toujours de taille >= 1, on renvoie la premiere occurence.*/
		if (mod.choiceInterface.isEmpty())
			return null;
		else
			return mod.choiceInterface.get(0).getColor();
	}

	protected Color StringToColor(String t){
        switch(t){
            case "Rouge" : return Color.red;
            case "Bleu" : return Color.blue;
            case "Noir" : return Color.black;
            case "Blanc" : return Color.white;          
            case "Jaune" : return Color.yellow;
			case "First" : return Color.cyan;    
			case "Joker": return Color.orange;        
            default : return Color.white;
        }
    }
	
	protected void offer (ArrayList<Tuile> place, char choice) {
		if(choice == 'l')
			chooseLineInLinesToAddTile(place);
		else 
			for(Tuile t : place)
				board.addTuileToPlancher(t);
	}

	protected void offerConsole(){
		System.out.println("------------------------------------------------------------------------------");
		print();
		Jeu.print();
		ArrayList<Tuile> place = new ArrayList<Tuile>();
		
		// Choix du centre ou des fabriques..
		while (true) {
			String choice = chooseCenterOrFabriques();
			try {
				place = chosenLocation(choice);
				break;
			}
			catch (Exception e) {
				System.out.println("Veuillez recommencer avec un endroit correct.");
			}
		}
		// Choix de la couleur a garde.
		String[] color = chooseColor(place).split("::");
		if (color.length > 1)
			pickTuileColor(place, color[0], color[1]);
		else
			pickTuileColor(place, color[0], "");
		
		print(place);
		// Il faut placer la tuile PJ dans le plancher si le joueur l'a prise.
		if(firstToPlay && !board.floorContainsFirst())
			board.addTuileToPlancher(new TuilePremierJoueur());
		
		board.printBoard();
		
		char choice = lineOrFloor();
		offer(place,choice);
		board.printBoard();
	}
	
	protected void offerInterface(String color, ArrayList<Tuile> choice) {
		mod.choiceInterface = pickTuileColor(choice,color,"");
		
		if(firstToPlay && !board.floorContainsFirst()) 
			board.addTuileToPlancher(new TuilePremierJoueur());
	}

	protected void decoration(){
		board.losePoints();
		for(int i = 0; i<6; i++)
			board.addLineToWall(i);
	}

	protected static char lineOrFloor(){
		// Methode pour choisir ou on veut placer le-s tuile-s choisis.
		while(true){
			System.out.println("Ajouter les tuiles aux lignes motifs ou au plancher?(l/p)");
			Scanner sc = new Scanner(System.in);
			try{
				char lettre = sc.nextLine().charAt(0);
				if(lettre == 'p' || lettre == 'l') 
					return lettre;
			}
			catch(Exception e){
				System.out.println("Choisissez vos lignes ou le plancher");	
			}
		}
	}

	protected ArrayList <Tuile> chosenLocation (String l) {
		if(l.compareTo("c") == 0 && !Jeu.getCentre().isEmpty() && !Jeu.getCentre().containsOnlyFirst()){ 
			if(isFirst()) 
				firstToPlay = true;
			
			return Jeu.popCentre().array;
		}
		else
			return Jeu.popFab(Integer.parseInt(l)).array;
	}
	
	protected String chooseCenterOrFabriques(){
		int nbrFabrique = Jeu.fabriques.size();
		Scanner sc  = new Scanner(System.in);
		
		System.out.print("Choisissez ");
		if(nbrFabrique > 0){
			System.out.print("une fabrique ");
			if(nbrFabrique >1)
				System.out.print("(entre 0 et "+(nbrFabrique-1)+") ");
			System.out.println("ou le centre.");
		}
		else
			System.out.println("le centre.");
					
		return sc.next(); 
	}
	
	protected String chooseColor (ArrayList<Tuile> fabContent){ 
		if (Jeu.typeGame == 'e')
			return chooseColorJoker(fabContent);
		else
			return chooseColorNormal(fabContent);
	}
	
	private String chooseColorNormal (ArrayList<Tuile> fabContent){
		ArrayList<String> availableColors = colorsInChoice(fabContent);
		String color = "";
		
		if (Jeu.typeLaunch == 'c') {
			Scanner sc = new Scanner(System.in);
			while(!isColorHere(color)) {
				System.out.print("	-> Choisissez une couleur ( parmi ");
				for(String s : availableColors)
					System.out.print(s+" ");

				System.out.println(")");
				color = sc.nextLine();
			}
		}
		else {
			color = cont.mod.choosenColor;
		}
		return color;
	}
	
	private String chooseColorJoker (ArrayList<Tuile> fabContent){ 
		// Methode qui permet de choisir une couleur et un joker ou simplement une couleur/joker.
		ArrayList<String> availableColors = colorsInChoice(fabContent);
		String joker = "", color = "";
		
		if (Jeu.typeLaunch == 'c') {
			Scanner sc = new Scanner(System.in);
			while (!isColorHere(color)) {
				System.out.print("	-> Choisissez une couleur/joker  (parmi ");
				for(String s : availableColors)
					System.out.print(s+" ");
				
				System.out.println(") :");
				System.out.println("(Si vous souhaitez choisir une couleur et le joker, veuillez ecrire \"Couleur,Joker\")");
				String resTab[] = sc.nextLine().split(",");
				color = resTab[0];
				joker = "";

				// On regarde si le tableau est de taille superieur a 1 sinon on ne peut pas assigner une valeur a la variable joker.
				if (resTab.length > 1)
					joker = resTab[1];
			}
			// Non implemente pour l'interface..
		}
		return color+"::"+joker;
	}
	
	private boolean isColorHere (String color) {
		return Tuile.colorlist.contains(color);
	}
	
	protected ArrayList<Tuile> pickTuileColor (ArrayList<Tuile> fabContent, String color, String joker) {
		// Methode qui renvoie une main d'une seule couleur uniquement.
		ArrayList<Tuile> choice = fabContent;
		
		if (Jeu.typeLaunch == 'c') 
			System.out.println();

		if(Tuile.colorlist.contains(color) ){
			for(int i = 0; i<fabContent.size(); i++){
				Tuile t = fabContent.get(i);
				
				if(t.getColor().compareTo(color) !=0  && t.getColor().compareTo(joker) !=0){
					Jeu.getCentre().addTuile(t);
					choice.remove(i--);
				}
			}
		}
		return choice;
	}
	
	protected boolean chooseLineInLinesToAddTile(ArrayList<Tuile> place){
		if (Jeu.typeLaunch == 'c') {
			Scanner s = new Scanner(System.in);
			while(true){
				System.out.println("Choisissez un nombre entre 1 et 5");
				try{
					// On fait -1 car dans le tableau de LigneMotif, l'indice commence  0.
					int i = s.nextInt()-1;
					
					if(i < 0 || i > 5 ) 
						// Si le chiffre choisi est incorrecte, on refait appel  la meme fonction.
						System.out.println("Choix invalide.");
					else {
						if(board.getLines().isCompleteLine(i))
							System.out.println("Cette ligne est complete pour ce tour, choisissez en une autre !");
						else if(board.canAddTuileToLine(i,place.get(0))){
							spreadTiles(place, i);
							return true;
						}
						else
							System.out.println("Choix de ligne invalide.");
					}
				}
				catch(Exception e){
					System.out.println("Entrez un nombre !");
					chooseLineInLinesToAddTile(place);
				}
			}
		}
		else {
			if(board.getLines().isCompleteLine(mod.choosenLine))
				// On fait appel a une methode de la vue qui cree une nouvelle fenetre avec ce message.
				cont.view.printMsg("Choix invalide, la ligne est complete pour ce tour.");
			
			else if(board.canAddTuileToLine(mod.choosenLine,place.get(0))){
				spreadTiles(place, mod.choosenLine);
				return true;
			}
			else
				cont.view.printMsg("Choix de ligne invalide.");
			
		}
		return false;
	}

	protected void spreadTiles(ArrayList<Tuile> tuiles, int line){
		// Methode pour ajouter les tuiles a  la ligne ou au plancher a la coordonne y = line.
		int k = 0, free = (line+1) - board.getLines().numberTuileInLine(line);
		
		for(Tuile t : tuiles){
			if(k < (free))
				board.addTuileToLine(line,t);
			else
				board.addTuileToPlancher(t);
			k++;
		}
	}

	protected boolean isFirst(){
		// Methode qui determine le prochain premier joueur, ssi la tuile se trouve au centre.
		for(Tuile t : Jeu.getCentre().array){
			if(t.getColor().compareTo("First") == 0){
				Jeu.getCentre().array.remove(t);
				return true;
			}
		}
		return false;
	}
	
	protected void print(){
		System.out.println("------"+ name + "------"+"| Score: "+score);
	}

	protected void print(ArrayList<Tuile> place){
		print();
		System.out.print("Votre tas : ");
		for(Tuile t : place)
			t.print();
		
		System.out.println("");
		System.out.println("*********************************");
	}

	public String toString(){
		return "------"+ name + "------"+"| Score: "+score;
	}

	private ArrayList<String> colorsInChoice(ArrayList<Tuile> choice){
		ArrayList<String> colors = new ArrayList<String>();
		for(Tuile t : choice){
			if(!colors.contains(t.getColor())){
				colors.add(t.getColor());
			}
		}
		return colors;
	}
	
	//----- Getters & setters -----//

	protected int getScore() {
		return score;
	}

	protected void minusScore(int s) {
		this.score -= s;
	}
	
	protected void addScore(int s) {
		this.score += s;
	}
	
	protected boolean isFirstForNextRound() {
		return firstToPlay;
	}

	protected Plateau getBoard () {
		return board;
	}
	
	protected void setApp (AppControler app) {
		this.cont = app;
		this.mod = cont.mod;
	}
	
	protected void setFirst (boolean b) {
		firstToPlay = b;
	}
	
	protected AppControler getControler () {
		return cont;
	}
	
	protected String getName () {
		return name;
	}
}
