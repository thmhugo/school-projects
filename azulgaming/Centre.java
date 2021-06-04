import java.util.ArrayList;

public class Centre extends Distribution{
	protected Centre () {
		array = new ArrayList <Tuile> ();
	}
	
	protected boolean containsOnlyFirst () {
		for (Tuile t : array) {
			if (t.getColor().compareTo("First") != 0)
				return false;
		}
		return true;
	}
}
