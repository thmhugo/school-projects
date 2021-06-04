import java.util.ArrayList;

public class AppModel {
	protected Joueur player;
	protected boolean hasAlreadyPlayed;
	private int numPlayer;
	protected String choosenLocation;
	protected String choosenColor;
	protected int choosenLine;
	protected ArrayList<Tuile> choiceInterface = new ArrayList<Tuile>();
	
	protected AppModel (Joueur player) {
		this.player = player;
	}
	
	protected void nextPlayer () {
		numPlayer = (numPlayer+1)%(Jeu.players.size());
		player = Jeu.players.get(numPlayer);
		choosenLocation = " ";
		choosenColor = " ";
		choosenLine = -2;
		choiceInterface = new ArrayList<Tuile>();
		hasAlreadyPlayed = false;
	}
	
	protected void nextPlayerFirst () {
		Joueur j = Jeu.getNextFirst();
		int cpt = 0;
		
		for (Joueur p : Jeu.players) {
			if (p.equals(j)) {
				player = p;
				numPlayer = cpt;
				choosenLocation = " ";
				choosenColor = " ";
				choosenLine = -2;
				choiceInterface = new ArrayList<Tuile>();
				hasAlreadyPlayed = false;
			}
			cpt ++;
		}
	}
}