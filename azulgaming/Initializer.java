import java.awt.*;
import java.awt.event.*;
import java.util.ArrayList;
import javax.swing.*;

public class Initializer extends JFrame{
    private JPanel panel;
    private JPanel panelText = new JPanel();
    private JComboBox boxesNb;
    private JComboBox boxesMode;
    private ArrayList<JTextField> texts;
    protected int nbplayers = 2;
    protected char gamemode = 'n';

    protected Initializer(){
        this.setTitle("Initialisation des joueurs");
        this.setSize(600,150);
        this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        panel = new JPanel();
               
        // Box pour le nombre de joueurs et le mode de jeu
        comboboxNbInit();
        comboboxModeInit();
        
        panel.add(panelText);
        
        boutonJouerInit();
        this.setContentPane(panel);
        this.setVisible(true);
        createTextFields();
    }

    protected void createPlayers(){
    	Jeu.typeGame = gamemode;
        for(JTextField j : texts)
            Jeu.players.add(new Joueur(j.getText()));

        Jeu.isJokerGame();
        this.dispose();
        Jeu.fillCenterTableFirstRound(); 
        Jeu.launchGameInterface();
    }

    protected void createTextFields(){
        panelText.removeAll();
        JLabel pseudoLabel = new JLabel("Pseudo des joueurs : ");
        JPanel textsfields = new JPanel();
        
        panelText.add(pseudoLabel);
        textsfields.setLayout(new GridLayout(nbplayers,1));
        texts = new ArrayList<>();
        
        for(int i = 0; i<nbplayers; i++){
            JTextField j = new JTextField(10);
            texts.add(j);
            textsfields.add(j);
            panelText.add(textsfields);
            this.pack();
            this.validate();
        }
    }

    protected void comboboxModeInit(){
        JPanel panelBoxesMode = new JPanel();
        JLabel modeLabel = new JLabel("Choix du mode de jeu") ;
        String [] str = {"Normal","Extension","Variante"};
        
        panelBoxesMode.add(modeLabel);
        boxesMode = new JComboBox<>(str);
        boxesMode.addActionListener(new ChangeGameMode());
        panelBoxesMode.add(boxesMode);
        panel.add(panelBoxesMode);
    }

    protected void comboboxNbInit(){
        JPanel panelBoxesNb = new JPanel();
        JLabel nbLabel = new JLabel("Nombre de joueurs ") ;
        Integer[] nb = {2,3,4}; 
        
        panelBoxesNb.add(nbLabel);
        boxesNb = new JComboBox(nb);
        boxesNb.addActionListener(new ChangeNumberOfPlayers());
        panelBoxesNb.add(boxesNb);
        panel.add(panelBoxesNb);
    }
    
    protected void boutonJouerInit(){
        JButton button = new JButton("Jouer !");
        button.addActionListener(e -> {createPlayers();});
        panel.add(button);
    }
    
    private class ChangeNumberOfPlayers implements ActionListener{
        public void actionPerformed(ActionEvent e) {
            nbplayers = (int)boxesNb.getSelectedItem();
            createTextFields();
        }
    }

    private class ChangeGameMode implements ActionListener{
        public void actionPerformed(ActionEvent e){
            String choice = (String)boxesMode.getSelectedItem();
            switch(choice){
                case "Normal" : gamemode = 'n'; break;
                case "Extension" : gamemode = 'e'; break;
                case "Variante" : gamemode = 'v'; break;
                default : gamemode = 'n'; break;
            }
        }
    }
}