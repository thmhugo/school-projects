import javax.swing.JButton;
import java.awt.*;

public class AppControler {
	protected AppModel mod;
	protected App view;
	
	protected AppControler (Joueur player) {
		this.mod = new AppModel(player);
		view = new App (this.mod,this);
	}

	protected boolean selectCenter(){
		mod.choosenLocation = "c";
		
		// Il faut tout d'abord vérifier que le centre est jouable ou non.
		if (!Jeu.centre.isEmpty() && !Jeu.centre.containsOnlyFirst() && !mod.hasAlreadyPlayed) {
			if(mod.player.isFirst()) 
				mod.player.firstToPlay = true;
			
			mod.choiceInterface = mod.player.chosenLocation(mod.choosenLocation);
			// On affiche le contenu du centre.
			view.showOffer("du centre.");
			view.refreshCenter();
			mod.hasAlreadyPlayed = true;
			return true;
		}
		else if (mod.hasAlreadyPlayed) 
			view.printMsg("Vous avez deja joue, veuillez passer votre tour.");
		else 
			view.printMsg("Impossible de prendre seulement la tuile premier joueur ou le centre est vide.");
			
		return false;
	}
	
	protected void selectFabrique (Fabrique f) {
		int n = 0;
		for (Fabrique fa : Jeu.fabriques) {
			if (fa.equals(f))
				break;
			n++;
		}
		mod.choosenLocation = Integer.toString(n);
		mod.choiceInterface = mod.player.chosenLocation(mod.choosenLocation);
		view.showOffer("de la fabrique.");
		mod.hasAlreadyPlayed = true;
	}

	protected void selectColor (Color c) {
		mod.choosenColor = colorToString(c);
		mod.player.offerInterface(mod.choosenColor, mod.choiceInterface);

		view.heapDisplay(mod.choiceInterface, "tas");
		view.refreshCenter();
		view.refreshFloor();
	}
	
	protected boolean selectLine (int index) {
		mod.choosenLine = index;
		// La condition ici permet de rejouer si on s'est trompe de ligne.
		if (mod.player.chooseLineInLinesToAddTile(mod.choiceInterface)) {
			view.colorLine(index, mod.choiceInterface.size(), view.StringToColor(mod.player.giveChoiceColor()));
			view.refreshFloor();
			// On desactive les boutons apres avoir joue.
			for(JButton b : App.buttons)
				b.setEnabled(false);
			return true;
		}
		else
			return false;
	}
	
	protected void selectFloor (int index) {
	/* On ajoute directement les tuiles dans le plancher sans utiliser de condition
	   car l'excedent part directement dans la defausse. */
		mod.choosenLine = index;
		mod.player.getBoard().addTuileArrayToPlancher(mod.choiceInterface);
		view.refreshFloor();
	}
	
	protected void endTurnOffer () {
		/* Methode qui sert a changer de joueur dans la phase d'offre et aussi
		   passer a la decoration */
		mod.nextPlayer();
		mod.player.setApp(this);
		
		if (Jeu.fabriques.isEmpty() && Jeu.centre.isEmpty()) {
			for (Joueur j : Jeu.players) 
				j.decoration();
			
			if (Jeu.whoWon() != null) {
				for (Joueur j : Jeu.players) 
					j.getBoard().winPointsEnd();
				view.printMsg(Jeu.whoWon().toString()+" a gagne!");
				System.exit(0);
			}
			else {
				mod.nextPlayerFirst();
				mod.player.setApp(this);
				mod.player.firstToPlay = false;
				
				Jeu.centre.addTuile(new TuilePremierJoueur());
				boolean b = Jeu.fillCenterTable();
				
				if (!b) {
					view.printMsg("La phase de preparation a ete interrompu car le sac et la defausse sont vides..");
					System.exit(0);
				}
				else {
					view.startNextRound();
					view.refreshCenter();
				}
			}
		}
			
		view.nextButton.setVisible(false);
		view.nextButton.setEnabled(false);
		view.refresh();
	}

	private String colorToString(Color c){
		//Switch impossible avec un objet Color
		if(c.equals(Color.RED))
			return "Rouge";
		else if(c.equals(Color.WHITE))
			return "Blanc";
		else if(c.equals(Color.BLACK))
			return "Noir";
		else if(c.equals(Color.YELLOW))
			return "Jaune";
		else if(c.equals(Color.BLUE))
			return "Bleu";
		else if(c.equals(Color.orange))
			return "Joker";
		else if(c.equals(Color.cyan))
			return "First";
		else
			return "";
	}
	
	protected int askCoordinateX (Joueur j,String color) {
		// Methode qui demande une coordonnee x ou mettre la tuile pour la variante et l'extension.
		while (true) {
			String s = view.giveXToPlace(j.toString(), color);
			try {
				int x = Integer.parseInt(s);
				if (x > 0 || x < 6)
					return x - 1;
			}
			catch (Exception e) {
				s = view.giveXToPlace(j.toString(), color);
			}
		}
	}
}
