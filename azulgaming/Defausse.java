import java.util.ArrayList;

public class Defausse extends Distribution {
	protected Defausse () {
		array = new ArrayList <Tuile> ();
	}
	
	protected void remove (Tuile t) {
		array.remove(t);
	}
	
	protected void resetContent () {
		array = new ArrayList<Tuile>();
	}
}
