import java.util.ArrayList;

public class Tuile {
	static ArrayList<String> colorlist = new ArrayList<String>() {{add("Bleu");add("Jaune");add("Rouge");add("Noir");add("Blanc");}};
	private String color;

	protected Tuile(String c){
		this.color = c;
	}

	protected String getColor(){
		return this.color;
	}

	protected void print(){
		System.out.print(this.color+" ");
	}

	protected String getInitialColor () {
		switch(color) {
		case "First": return "+f";
		case "Bleu": return "+b";
		case "Rouge": return "+r";
		case "Noir": return "+n";
		case "Blanc": return "+B";
		case "Jaune": return "+j";
		case "Joker": return "+J";
		default: return "";
		}
	}
}
