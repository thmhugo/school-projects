import java.awt.*;
import java.awt.event.*;
import java.util.ArrayList;
import javax.swing.*;

public class App extends JFrame {
	private AppModel mod;
    private AppControler cont;
    
	private JPanel board = new JPanel();
	private JPanel common = new JPanel();
	static ArrayList<JButton> buttons = new ArrayList<JButton>();
	static ArrayList<JButton> buttonsFabrique = new ArrayList<JButton>();
    protected JFrame choiceFrame;
	protected JButton nextButton;
	private JLabel pseudo;
	private JPanel lineAndWalll;

	// Les CellPanel continnent des coordonnees x et y et un JPanel.
	private ArrayList<CellPanel> linePan = new ArrayList<CellPanel>();
	private ArrayList<CellPanel> floorPan = new ArrayList<CellPanel>(); 
	protected ArrayList<CellPanel> fabriquePan = new ArrayList<CellPanel>(); 
	private ArrayList<CellPanel> wallPan = new ArrayList<CellPanel>(); 

	protected App(AppModel mod, AppControler cont) {
		this.mod = mod;
		this.cont = cont;
		this.setTitle("AZUL");
		this.setSize(840, 1200);
		this.setLayout(new GridLayout(2, 1));
		commonInit();
		boardInit();
		this.setVisible(true);
		this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
	}

	protected void commonInit() {
		// Initialisation du Centre et des Fabriques.
		common.setBackground(Color.gray);
		common.setLayout(new GridLayout(2, 1));
		common.add(fabriquesInit());
		common.add(centerInit());
		this.add(common);
	}

	protected void boardInit() {
		// Initialisation du plateau.
		board.setBackground(Color.white);
		board.setLayout(new GridLayout(3, 1));
		board.add(pseudoPanelInit());
		board.add(lineAndWallInit());
		board.add(floorInit());
		this.add(board);
	}

	protected JPanel pseudoPanelInit(){
		JPanel pan = new JPanel();
		JLabel playerpseudo = new JLabel(mod.player.toString());
		playerpseudo.setForeground(Color.black);
		playerpseudo.setHorizontalAlignment(JLabel.CENTER);
		playerpseudo.setFont(new Font("Arial", Font.BOLD, 20));
		
		pseudo = playerpseudo;
		JButton nextButton = new JButton("Fin du tour");
		nextButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				cont.endTurnOffer();
			}
		});
		nextButton.setVisible(false);
		nextButton.setEnabled(false);
		this.nextButton= nextButton;
		
		pan.add(playerpseudo);
		pan.add(spacingColumn());
		pan.add(nextButton);
		return pan;
	}
	
	protected JPanel tileInFabriquesInit(Fabrique f) {
		// Initialisation des tuiles qui seront dans les fabriques.
		JPanel pan = new JPanel(new GridLayout(2, 2, 5, 5));
		if(!f.array.isEmpty()){
			for (Tuile t : f.array) { // init des 4 cases dans les fabriques
				JPanel tilePan = new JPanel();
				Color c = StringToColor(t.getColor());

				tilePan.setBackground(c);
				tilePan.setBorder(BorderFactory.createCompoundBorder(BorderFactory.createLineBorder(Color.black, 2),
				BorderFactory.createLineBorder(c, 20)));

				pan.add(tilePan);
			}
		
		}	return pan;	
	}

	protected JPanel fabriquesInit() {
		// Initialisation des fabriques.
		JPanel fabPan = new JPanel();
		int i = 0;
		
        for(Fabrique f : Jeu.fabriques){
           JButton fbut = new JButton();
            
            fbut.setBackground(Color.white);
            fbut.setBorderPainted(false);
           
            JPanel pan = tileInFabriquesInit(f);
            fabriquePan.add(new CellPanel(pan, i, 0));
            fbut.addActionListener(new ActionListener(){
                public void actionPerformed(ActionEvent event){    
                   cont.selectFabrique(f);
                   disableAll();
                   fbut.setVisible(false);
                }
            });           
            fbut.setEnabled(true); 
            fbut.add(pan);
            buttonsFabrique.add(fbut);
            fabPan.add(fbut);
            i++;
        }
        return fabPan;
	}
	
	protected JPanel tilesInCenterInit(ArrayList<Tuile> array) {
		// Initialisation des tuiles dans le centre.
		JPanel tilesPan = new JPanel();
		for (Tuile t : array) {
			JPanel tilePan = new JPanel();
			Color c = StringToColor(t.getColor());
				
			tilePan.setBackground(c);
			tilePan.setBorder(BorderFactory.createCompoundBorder(BorderFactory.createLineBorder(Color.black, 2),
					BorderFactory.createLineBorder(c, 20)));
			tilesPan.add(tilePan, BorderLayout.CENTER);
		}
		return tilesPan;

	}

	protected JPanel centerInit() {
		// Initialisation du centre
		JPanel mainPan = new JPanel();
        JButton jbut = new JButton();
        jbut.add(tilesInCenterInit(Jeu.centre.array));
        jbut.setBackground(Color.white);
        jbut.setBorderPainted(false);
        jbut.addActionListener(new ActionListener(){
			public void actionPerformed(ActionEvent e) {
				boolean b = cont.selectCenter();
				/* Si la methode renvoie true, sa veut dire que le joueur a joue et donc que les autres fabriques
				   ne doivent plus etre disponible. */
				if (b)
					disableAll();
			}

        });
        mainPan.add(jbut);
        return mainPan;
	}

	protected JPanel floorInit(){
		// Initialisation du plancher.
        JPanel pan = new JPanel();
        
        pan.add(spacingColumn());
        JPanel casePan = new JPanel();
        int cpt = 0;
        pan.add(spacingColumn());
        pan.add(floorAddButtonInit());

        for(Case c : mod.player.getBoard().getFloor().array){
        	// Chaque case du plancher ne retire pas le meme nombre de point.
            JLabel minus = new JLabel(minusScore(cpt));
            JPanel tuile = new JPanel();
            
            if(!c.isEmpty())
                tuile.setBackground(StringToColor(c.getTuile().getColor()));
            else
            	// Couleur de base de la case du plancher afin de pouvoir l'exploiter dans d'autres methodes.
                tuile.setBackground(Color.gray);

            tuile.setBorder((BorderFactory.createCompoundBorder(
                BorderFactory.createLineBorder(Color.black, 2)  ,
                BorderFactory.createLineBorder(tuile.getBackground(),20)
                )));

            casePan.add(minus);
            casePan.add(tuile);
            pan.add(casePan);
            floorPan.add(new CellPanel(tuile,cpt,0));
            cpt ++;
        }

        pan.add(spacingColumn());
        return pan;
    }

    protected String minusScore(int cpt){
        String pt = "-1";
        if (cpt > 1 && cpt <= 4)
        		pt = "-2";
        if 	(cpt > 4)
                pt = "-3";
    
        return pt;
    }
    
    protected JPanel floorAddButtonInit(){
    	// Methode qui ajoute le bouton pour ajouter des tuiles au plancher.
        JButton b = new JButton("+");
        b.setVisible(false);
        b.setEnabled(false);
        b.addActionListener(new ActionListener(){
			public void actionPerformed(ActionEvent e) {
                choiceFrame.dispose();
				cont.selectFloor(-1);
                for(JButton bt : buttons){
                	bt.setVisible(false);
                	bt.setEnabled(false);
				}
				
				nextButton.setVisible(true);
				nextButton.setEnabled(true);
            }
        });
        buttons.add(b);
        JPanel panBut = new JPanel();
        panBut.add(b);
        return panBut;
    }
	
	protected Color StringToColor(String t) {
		// Converti un String en Color.
		switch (t) {
		case "Rouge": return Color.red;
		case "Bleu":  return Color.blue;
		case "Noir":  return Color.black;
		case "Blanc": return Color.white;
		case "Jaune": return Color.yellow;
		case "First": return Color.cyan;
        case "Joker": return Color.orange;
		default:      return Color.white;
		}
	}

	protected JPanel lineAndWallInit() {
		// Initialisation des lignes et du mur.
		JPanel lineAndWall = new JPanel();
		lineAndWall.setLayout(new GridLayout(1, 3));
		lineAndWall.add(linePanel());
		lineAndWall.add(spacingColumn());
		lineAndWall.add(wallPanel());
		
		this.lineAndWalll = lineAndWall;
		return lineAndWall;
	}
	
	protected JPanel wallPanel() {
		// Represente le mur.
		JPanel lmp = new JPanel();
        
        if(Jeu.typeGame == 'n' || Jeu.typeGame == 'e')
            lmp = coloredWall();
        else 
            lmp = colorlessWall();

        return lmp;
    }

    private JPanel coloredWall(){
        JPanel pan = new JPanel();
        pan.setLayout(new GridLayout(5,5,5,5)); 
        Color[] colors = {Color.blue, Color.yellow, Color.red, Color.black, Color.white};
        int k = 0;
        for(int i = 0 ; i < 5 ; i++){
            for(int j = 0 ; j < 5 ; j++){
                int n = (k - i) %5;
                JPanel l = new JPanel();
                Color c = colors[n].darker();
                JLabel txt = new JLabel("-");
                
                l.add(txt);
                l.setBackground(c);
                l.setBorder(BorderFactory.createCompoundBorder(
                    BorderFactory.createLineBorder(c, 1)  ,
                    BorderFactory.createLineBorder(Color.black, 1))
                    );
                pan.add(l);
                k++;
                wallPan.add(new CellPanel(l,j,i));
            }
        }
        return pan;
    }

    private JPanel colorlessWall(){
    	// Initialise le mur sans couleur pour l'extension.
        JPanel pan = new JPanel();
        pan.setLayout(new GridLayout(5,5,5,5)); 
        for(int i = 0; i<5; i++){
            for(int j = 0; j<5; j++){
                JPanel l = new JPanel();
                l.setBackground(Color.gray);
                l.setBorder(BorderFactory.createCompoundBorder(
                    BorderFactory.createLineBorder(Color.black, 1)  ,
                    BorderFactory.createLineBorder(Color.black, 1))
                    );
                pan.add(l);
                wallPan.add(new CellPanel(l,j,i));
            }
        }
        return pan;
    }

	protected JPanel linePanel() {
		// Represente les lignes motif.
		JPanel lmp = new JPanel();
		lmp.setLayout(new GridLayout(5, 5, 5, 5));
		int k = 4;

		for (int i = 0; i < 5; i++) {
			final int index = i;
			JButton b = new JButton("+");
			b.setVisible(false);
			b.setEnabled(false);

			b.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
                    choiceFrame.dispose();
					final int n = index;
					// il faut tout d'abord regarder si il est possible de choisir la ligne avant de l'exploiter.
					if (cont.selectLine(n)) {
						for (JButton bt : buttons) {
							bt.setVisible(false);
							bt.setEnabled(false);
						}
						nextButton.setVisible(true);
						nextButton.setEnabled(true);
					}
				}
			});
			
			int cpt = 0;
			for (int j = 0; j < 5; j++) {
				JPanel l = new JPanel();
				// On ne met en evidence que certaines cases car la ligne motif ne fait pas 5x5

				if (j >= k) {
					l.setBorder(BorderFactory.createCompoundBorder(BorderFactory.createLineBorder(Color.black, 1),
							BorderFactory.createLineBorder(Color.black, 1)));
					linePan.add(new CellPanel(l, cpt, i));
					l.setBackground(Color.gray);
					cpt ++;
				}
				
				lmp.add(l);
				buttons.add(b);
				lmp.add(b);
			}
			k--;
		}
		return lmp;
	}

	protected JPanel spacingColumn() {
		return new JPanel();
	}

	protected void colorLine(int y, int n, Color color) {
		// Fonction qui sert a remplir une ligne a la coordonne y.
		for (CellPanel c : linePan) {
			if (c.getY() == y && c.getPanel().getBackground().equals(Color.gray) && n != 0) {
				c.getPanel().setBackground(color);
				n--;
			}
		}
	}
	
	private void resetColorLine () {
		for (CellPanel c : linePan) {
			c.getPanel().setBackground(Color.gray);
		}
	}
	
	protected void refreshFloor () {
		int cpt = 0;
		resetColorFloor();
		for (Case c : mod.player.getBoard().getFloor().array) {
			if (!c.isEmpty()) {
				floorPan.get(cpt).getPanel().setBackground(StringToColor(c.getTuile().getColor()));
				floorPan.get(cpt).getPanel().setBorder(BorderFactory.createCompoundBorder(BorderFactory.createLineBorder(Color.black, 2),
						BorderFactory.createLineBorder(floorPan.get(cpt).getPanel().getBackground(), 20)));
			}
			cpt ++;
		}
	}
	
	private void resetColorFloor () {
		for (CellPanel c : floorPan) {
			c.getPanel().setBackground(Color.gray);
			c.getPanel().setBorder(BorderFactory.createCompoundBorder(BorderFactory.createLineBorder(Color.black, 2),
	                BorderFactory.createLineBorder(c.getPanel().getBackground(), 20)));
		}
	}

	protected void printMsg(String s) {
		// Sert a afficher un message dans une autre fenetre.
		JOptionPane.showMessageDialog(board, s);
	}
	
	protected void showOffer (String msg) {
		JFrame f = new JFrame();
		// On empeche la fermeture de la fenetre de selection.
		f.setDefaultCloseOperation(WindowConstants.DO_NOTHING_ON_CLOSE);
		f.setTitle("Choix de la couleur "+msg);
		f.setSize(600, 100);
		JPanel pan = new JPanel();
		for (Tuile t : mod.choiceInterface) {
			JButton tileBut = new JButton();
			Color c = StringToColor(t.getColor());
			
			tileBut.setBackground(c);
			tileBut.setBorder(BorderFactory.createCompoundBorder(BorderFactory.createLineBorder(Color.black, 2),
					BorderFactory.createLineBorder(c, 20)));

			tileBut.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					cont.selectColor(c);
					for (JButton b : buttons) {
						b.setVisible(true);
						b.setEnabled(true);
					}
				f.dispose();
				}
			});

			pan.add(tileBut);
		}
		f.add(pan);
		f.setVisible(true);
	}
	
	protected JPanel tileChoice(ArrayList<Tuile> choix){
        JPanel pan = new JPanel();
        for(Tuile t : choix){
            JPanel l = new JPanel();
            Color c = StringToColor(t.getColor());
            l.setBackground(c);
            l.setBorder(BorderFactory.createCompoundBorder(
                BorderFactory.createLineBorder(c, 20)  ,
                BorderFactory.createLineBorder(Color.black, 1))
                );
            pan.add(l);
        }
        return pan;
	}
	
	protected void heapDisplay(ArrayList<Tuile> choix, String msg){
		// Cree une nouvelle fenetre pour afficher les choix possibles ou sa main.
		choiceFrame = new JFrame();
		JLabel l = new JLabel("Votre "+msg+" :");
		JPanel pan = new JPanel();
		
		pan.setLayout(new GridLayout(2,1));
		l.setHorizontalAlignment(JLabel.CENTER);
		pan.add(l);
		pan.add(tileChoice(choix));
		choiceFrame.add(pan);
		choiceFrame.pack();
		choiceFrame.setVisible(true);
	}
	

	protected void refreshCenter() {
		// Actualise le centre et les indices des fabriques..
		common.remove(1);
		common.add(centerInit(),1);
	}
	
	protected void refresh () {
		// Methode utilisee a chaque changement de joueur.
		pseudo.setText(mod.player.toString());
		for(JButton b : buttonsFabrique)
			b.setEnabled(true);
		refreshLines();
		refreshFloor();
		refreshWall();
		board.invalidate();
		board.validate();
		board.repaint();
	}

	protected void refreshLines () {	
		// Tout d'abord on "nettoie" les lignes puis on le rempli.
		resetColorLine();
		for (int i = 0 ; i < linePan.size() ; i ++) {
			CellPanel cell = linePan.get(i);
			Tuile t = mod.player.getBoard().getLines().getTuile(cell.getX(), cell.getY());
			if (t != null) {
				Color c = StringToColor(t.getColor());
				linePan.get(i).getPanel().setBackground(c);
			}
		}
	}
	
	protected void refreshWall () {
		lineAndWalll.remove(2);
		lineAndWalll.add(wallPanel());

		for (CellPanel c : wallPan) {
			Tuile t = mod.player.getBoard().getWall().locationIsOccupied(c.getX(), c.getY());
			if(t != null) {
				if (Jeu.typeGame != 'v') {
					c.getPanel().remove(0);
					// On colorie les cases qui sont occupees.
					JLabel txt = new JLabel("+");
					c.getPanel().add(txt);
				}
				c.getPanel().setBackground(StringToColor(t.getColor()));
			}
		}
	}
	
	protected void startNextRound () {
		// On supprime le premier panel de common qui est l'ensemble des fabriques puis on la reinitialise.
		common.remove(0);
		common.add(fabriquesInit(), 0);
	}
	
	protected String giveXToPlace (String name, String color) {
		// Affiche une fenetre externe demandant a l'utilisateur une coordonnee x.
		return JOptionPane.showInputDialog(name+
				": Veuillez donner la coordonnee x ou vous voulez mettre la tuile dans le mur. (Entre 1 et 5)"
				+"[Tuile de couleur "+color+"]");
	}
	
	protected void disableAll () {
		// Desactive les boutons lorsqu'une fabrique est choisi.
		for (JButton b : buttonsFabrique) {
			b.setEnabled(false); 
		}
	}
}