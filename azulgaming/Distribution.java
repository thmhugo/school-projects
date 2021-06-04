import java.util.ArrayList;
import java.util.Random;

public abstract class Distribution {
	protected ArrayList <Tuile> array;
	
	protected boolean isEmpty () {
		return array.isEmpty();
	}
	
	protected void addTuile (Tuile t) {
		array.add(t);
	}
	
	protected Tuile drawElement () {
		Random rand = new Random();
		int randomNum = rand.nextInt(array.size());
		return array.remove(randomNum);
	}

	protected void fill (ArrayList <Tuile> tab) {
		for (Tuile t : tab) {
			array.add(t);
		}
	}
	
	protected ArrayList <Tuile> giveContents () {
		return array;
	}

	protected void print(){
		for(Tuile t : array){
			t.print();
		}
	}
 }
