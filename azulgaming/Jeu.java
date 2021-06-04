import java.util.*;

public class Jeu{
	static protected ArrayList <Fabrique> fabriques = new ArrayList<>();
	static protected Sac bag = new Sac ();
	static protected Defausse discard = new Defausse();
	static protected ArrayList <Joueur> players = new ArrayList<>();
	static protected Centre centre = new Centre();
	static protected char typeGame = 'n';
	static protected char typeLaunch;
	
	public static void main(String[] args){
		Scanner sc = new Scanner(System.in);
		String choix = "";
		System.out.println("A quel type du jeu voulez vous jouer? (Console ou Interface)");
		while (true) {
			try{
				choix= sc.nextLine();
				choix = choix.toLowerCase();
				if (choix.compareTo("console") == 0 || choix.compareTo("interface") == 0) 
					break;
				else
					System.out.println("Reessayez..");	
			}
			catch(Exception e){
				System.out.println("Reessayez..");	
			}
		}
		if (choix.compareTo("interface") == 0) 
			Launcher.launchGame();
		else
			launchGameConsole();
	}
	
	protected static void launchGameInterface () {
		typeLaunch = 'i';
		AppControler app = new AppControler(players.get(0));
		players.get(0).setApp(app);
	}
	
	private static void launchGameConsole () {
		typeLaunch = 'c';
		setTypeGame();
		initPlayers();
		isJokerGame();
		fillCenterTableFirstRound();
		// Tant qu'il n'y a pas de joueur gagnant, on continue a boucler.
		while(whoWon() == null)
			playRound();
		for (Joueur j : Jeu.players) 
			j.getBoard().winPointsEnd();
		System.out.println(whoWon().toString()+" a gagne!");
	}
	
	private static Joueur mostPoint () {
		Joueur player = players.get(0);
		for(Joueur j : players){
			if(j.getScore() > player.getScore())
				player = j;
		}
		return player;
	}

	protected static Joueur whoWon () {
		for(Joueur j : players){
			if(j.getBoard().getWall().isComplete())
				return mostPoint();
		}
		return null;
	}

	protected static void playRound(){
		Joueur first = getNextFirst();
		// Apres avoir recupere le premier joueur, il ne sera plus le premier a jouer au prochain tour.
		first.firstToPlay = false;

		System.out.println("-----------OFFRE-----------");
		do{
			first.offerConsole();
			for(Joueur j : players){
				if(!j.equals(first)){
					j.offerConsole();
					System.out.println("");
				}
			}
		}
		while(!fabriques.isEmpty() || !centre.isEmpty());
		System.out.println("-----------DECORATION-----------");
		for(Joueur j : players){
			j.decoration();
			j.print();
			j.getBoard().printBoard();
		}
		boolean b = fillCenterTable();
		if (!b) {
			System.out.println("La phase de preparation a ete interrompu car le sac et la defausse sont vides..");
			return;
		}
		Jeu.centre.addTuile(new TuilePremierJoueur());
	}
	
	protected static void initPlayers(){
		while(true){
			int nbPlayer = selectNbPlayers();
			
			createPlayers(nbPlayer);
			for(Joueur j : players)
				j.print();
			System.out.println("Commencer a jouer ?(o/n)");
			Scanner sc = new Scanner(System.in);
			char choix = sc.nextLine().charAt(0);
			
			if(choix == 'o') 
				break;
			else
				sc = new Scanner(System.in);
		}
	}
	
	protected static void createPlayers(int nbPlayer){
		Scanner sc = new Scanner(System.in);
		for(int i =0; i<nbPlayer; i++){
			System.out.println("Entrez le nom du joueur "+ (i+1));
			players.add(new Joueur(sc.nextLine()));
		}
	}

	protected static int selectNbPlayers(){
		Scanner sc = new Scanner(System.in);
		int n;
		
		while(true){
			System.out.println("Entrez le nombre de joueur");
			try{
				n = sc.nextInt();
				if(n == 2 || n == 3 || n == 4) 
					break;
				else
					System.out.println("Il faut 2, 3 ou 4 joueurs !");
			}
			catch(Exception e){
				System.out.println("Un chiffre est attendu.");
				sc = new Scanner(System.in);
			}
		}
		return n;
	}
	
	protected static Fabrique popFab(int i){
		Fabrique fab = fabriques.get(i);
		fabriques.remove(i);
		return fab;
	}

	protected static Centre popCentre(){
		Centre c = centre;
		centre = new Centre();
		return c;
	}

	protected static void print(){
		System.out.print("Centre : ");
		centre.print();
		System.out.println("");
		System.out.print("Fabriques : ");
		int k = 0;
		for(Fabrique f : fabriques){
			System.out.print("-"+k+"- : ");
			f.print(); 
			System.out.print(" | ");
			k++;
		}
		System.out.println(""+"\n");
	}

	protected static Joueur getNextFirst(){
		for(Joueur j : players){
			if (j.isFirstForNextRound()) 
				return j;
		}
		return players.get(0);
	}
	
	//----- Fonctions pour la phase de prep -----//
	
	protected static boolean fillCenterTable () {
		// Methode qui regarde si les fabriques et le centre sont vides..
		for (int i = 0 ; i < nbrFabriques(); i ++) 
			fabriques.add(new Fabrique());
		
		for (Fabrique f : fabriques) {
			if (bag.isEmpty() || !bag.enoughTuile()) {
				bag.fill(discard.giveContents());
				discard.resetContent();
			}

			if (bag.isEmpty() && discard.isEmpty()) 
				return false;
			
			f.fill(bag.giveToFabrique());
		}
		return true;
	}
	
	protected static void fillCenterTableFirstRound () {
		// Methode a utilise uniquement au premier tour.
		Tuile t = bag.getTuilePJ();
		if (t == null) {
			System.out.println("Erreur, la tuile PJ n'est pas presente dans sac..");
			return;
		}
		centre.addTuile(t);
		
		if (nbrFabriques() == -1) {
			// -1 correspond a la valeur par defaut et donc a une erreur.
			System.out.println("Erreur, le nombre de joueur n'est pas correct.");
			return;
		}
	
		fillCenterTable();
	}
	
	protected static int nbrFabriques () {
		// Methode qui renvoie le nombre de fabrique selon le nombre de joueur, cf regle du jeu.
		switch (players.size()) {
		case 2: return 5;
		case 3: return 7;
		case 4: return 9;
		default: return -1;
		}
	}
	
	//----- Fonctions pour les variantes du jeu -----//
	
	private static char askTypeGame () {
		while(true){
			System.out.println("Veuillez choisir entre le jeu normal (n) ou la variante [mur incolore] (v) ou l'extension [avec joker] (e) .");
			Scanner sc = new Scanner(System.in);
			try{
				char lettre = sc.nextLine().charAt(0);
				// On met en miniscule le caractere recupere afin d'avoir moins de conditions possible (plus besoin de v�rifier en majuscule).
				lettre = Character.toLowerCase(lettre);
				
				if(lettre == 'n' || lettre == 'v' || lettre == 'e')  
					return lettre;
			}
			catch(Exception e){
				System.out.println("Reessayez..");	
			}
		}
	}
	
	protected static void setTypeGame () {
		typeGame = askTypeGame();
	}
	
    protected static void isJokerGame () {
		if (typeGame == 'e')
			setJokerGame();
	}
	
	private static void setJokerGame () {
		Tuile.colorlist.add("Joker");
		if (players.size() == 2) {
			for (int i = 0 ; i < 100 ; i ++) {
				if (i%20 == 0)
					bag.array.set(i, new Tuile("Joker"));
			}
		}
		else {
			for (int i = 0 ; i < 100 ; i ++) {
				if (i%20 == 0 || i%19 == 0)
					bag.array.set(i, new Tuile("Joker"));
			}
		}
	}

	//----- Getters & setters -----//
	protected static Defausse getDiscard() {
		return discard;
	}

	protected static void setPlayers(ArrayList<Joueur> players) {
		Jeu.players = players;
	}

	protected static Centre getCentre() {
		return centre;
	}
}
