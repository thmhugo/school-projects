public class Test {

	public static void testPlateau () {
		Jeu.typeGame = 'n';
		Jeu.typeLaunch = 'c';
		Joueur player = new Joueur ("Pierre");
		
		Jeu.players.add(player);
		Plateau board = player.getBoard();
		
		Tuile t00 = new Tuile("Bleu");
		Tuile t03 = new Tuile("Jaune");
		Tuile t04 = new Tuile ("Blanc");
		Tuile t05 = new Tuile ("Rouge");

		board.addTuileToLine(0, t00);
		board.addLineToWall(0);
		
		board.addTuileToLine(3, t00);
		board.addTuileToLine(3, t00);
		board.addTuileToLine(3, t00);
		board.addTuileToLine(3, t00);
		board.addLineToWall(3);
		
		board.addTuileToLine(2, t03);
		board.addTuileToLine(2, t03);
		board.addTuileToLine(2, t03);
		board.addLineToWall(2);
		
		board.addTuileToLine(3, t03);
		board.addTuileToLine(3, t03);
		board.addTuileToLine(3, t03);
		board.addTuileToLine(3, t03);
		board.addLineToWall(3);
		
		board.printBoard();
		System.out.println("Score: "+player.getScore());
	}

	
	public static void phasePrepPropTest () {
		Jeu.fillCenterTableFirstRound();
		Jeu.typeLaunch = 'c';
		while(!Jeu.fabriques.isEmpty() && !Jeu.centre.isEmpty()){
			Joueur first = Jeu.getNextFirst();
			first.offerConsole();
			for(Joueur j : Jeu.players){
				if(Jeu.fabriques.isEmpty() && Jeu.centre.isEmpty()) 
					break;
					
				if(!j.equals(first)){
					j.offerConsole();
					System.out.println("");
				}
			}
		}

		for(Joueur j: Jeu.players){
			j.decoration();
			j.print();
			j.getBoard().printBoard();
		}
	}
	
	public static void decorationTest () {
		Joueur player = new Joueur ("Pierre");
		Joueur player2 = new Joueur ("Jacques");
		Jeu.typeLaunch = 'c';
		Jeu.players.add(player);
		Jeu.players.add(player2);
		Plateau board = player.getBoard();
		
		Tuile t00 = new Tuile("Bleu");
		Tuile t01 = new Tuile("Noir");
		Tuile t02 = new Tuile("Jaune");
		Tuile t03 = new Tuile("Rouge");
		Tuile t04 = new Tuile("Blanc");
		
		board.addTuileToLine(0, t00);
		player.decoration();
		board.addTuileToLine(0, t01);
		player.decoration();
		board.addTuileToLine(0, t02);
		player.decoration();
		board.addTuileToLine(0, t03);
		player.decoration();
		board.addTuileToLine(0, t04);
		player.decoration();
		
		board.printBoard();
		System.out.println(Jeu.whoWon());
	}
	
	public static void variantesTest3 () {
		// Test de la variante.
		Jeu.typeGame = 'v';
		Jeu.typeLaunch = 'c';
		Joueur player = new Joueur ("Pierre");
		Joueur player2 = new Joueur ("Jacques");
		
		Jeu.players.add(player);
		Jeu.players.add(player2);
		
		Plateau board = player.getBoard();
		
		Tuile t00 = new Tuile("Bleu");
		Tuile t01 = new Tuile("Jaune");
		Tuile t02 = new Tuile("Blanc");
		Tuile t03 = new Tuile("Noir");
		Tuile t04 = new Tuile("Rouge");
		
//		board.addTuileToLine(0, t00);
		board.printBoard();
//		player.decoration();
//		board.printBoard();
//		
//		board.addTuileToLine(4, t00);
//		board.addTuileToLine(4, t00);
//		board.addTuileToLine(4, t00);
//		board.addTuileToLine(4, t00);
//		board.addTuileToLine(4, t00);
//		board.printBoard();
//		player.decoration();
//		board.printBoard();
		
		for (int i = 0 ; i < 5 ; i ++) {
			board.addTuileToLine(i, t00);
			for (int j = 0 ; j < i ; j++) {
				board.addTuileToLine(i, t00);
			}
			board.printBoard();
			player.decoration();
			board.printBoard();
		}
//		
//		for (int i = 0 ; i < 5 ; i ++) {
//			board.addTuileToLine(i, t01);
//			for (int j = 0 ; j < i ; j++) {
//				board.addTuileToLine(i, t01);
//			}
//			board.printBoard();
//			player.decoration();
//			board.printBoard();
//		}
//		
//		for (int i = 0 ; i < 5 ; i ++) {
//			board.addTuileToLine(i, t02);
//			for (int j = 0 ; j < i ; j++) {
//				board.addTuileToLine(i, t02);
//			}
//			board.printBoard();
//			player.decoration();
//			board.printBoard();
//		}
//		
//		for (int i = 0 ; i < 5 ; i ++) {
//			board.addTuileToLine(i, t03);
//			for (int j = 0 ; j < i ; j++) {
//				board.addTuileToLine(i, t03);
//			}
//			board.printBoard();
//			player.decoration();
//			board.printBoard();
//		}
//		
//		for (int i = 0 ; i < 5 ; i ++) {
//			board.addTuileToLine(i, t04);
//			for (int j = 0 ; j < i ; j++) {
//				board.addTuileToLine(i, t04);
//			}
//			board.printBoard();
//			player.decoration();
//			board.printBoard();
//		}
	}
	
	public static void testGame () {
		// Test si fillCenterTable fonctionne bien.
		Jeu.typeGame = 'n';

		Jeu.players.add(new Joueur ("Pierre"));
		Jeu.players.add(new Joueur ("Jacques"));
		Jeu.fillCenterTableFirstRound();
		
		Jeu.print();
		while (!Jeu.bag.isEmpty()) {
			for (int i = 0 ; i < 5 ; i ++) {
				Fabrique f = (Jeu.popFab(0));
				Jeu.discard.addTuile(f.array.get(0));
				Jeu.discard.addTuile(f.array.get(1));
			}
			
			Jeu.print();
			
			Jeu.fillCenterTable();
			
			Jeu.print();
		}
	}
	
	public static void testJoker () {
		// Mettre Jaune en reponse.
		Jeu.typeGame = 'e';
		Jeu.typeLaunch = 'c';
		Joueur player = new Joueur ("Pierre");
		
		Jeu.players.add(player);
		Jeu.isJokerGame();
		
		Plateau board = player.getBoard();
		
		Tuile t00 = new Tuile("Bleu");
		Tuile t02 = new Tuile("Joker");
		Tuile t03 = new Tuile("Rouge");
		Tuile t04 = new Tuile("Noir");
		Tuile t05 = new Tuile("Blanc");
		
		board.addTuileToLine(0, t00);
		board.addLineToWall(0);
		
		board.addTuileToLine(0, t02);
		board.addLineToWall(0);
		
		board.addTuileToLine(0, t03);
		board.addLineToWall(0);
		
		board.addTuileToLine(0, t04);
		board.addLineToWall(0);
		
		board.addTuileToLine(0, t05);
		board.addLineToWall(0);
		
		board.printBoard();
		
		System.out.println(board.lineInWallComplete());
	}
	
	

	public static void main(String[] args) {
		testPlateau();
//		phasePrepPropTest();
//		decorationTest();
//		variantesTest2();
//		variantesTest3();
//		testGame();
//		testJoker();
	}
}
