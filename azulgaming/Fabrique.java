import java.util.ArrayList;

public class Fabrique extends Distribution{
	protected Fabrique () {
		array = new ArrayList <Tuile> ();
	}	
	protected ArrayList<Tuile> giveOutSameColor (String color) {
		ArrayList <Tuile> res = new ArrayList<Tuile>();
		for (Tuile t : array) {
			if (t.getColor().compareTo(color) == 0) 
				res.add(t);
		}
		return res;
	}
	
	protected ArrayList<Tuile> giveOutExcept (String color) {
		ArrayList <Tuile> res = new ArrayList<Tuile>();
		for (Tuile t : array) {
			if (t.getColor().compareTo(color) != 0)
				res.add(t);
		}
		return res;
	}
}
