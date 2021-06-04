public class Case {
	private Plancher floor;
	private LignesMotif lines;
	private Mur wall;
	private Tuile element;
	private int x, y;
	private String color;
	
	protected Case(int x, int y){
		this.x = x; this.y = y;
	}

	protected Case(int x, int y,Tuile element){
		this(x,y);
		this.element = element;
	}

	protected Case(int x, int y,Plancher p,Tuile element){
		this(x,y,element);
		floor = p;
	}

	protected Case(int x, int y,LignesMotif l,Tuile element){
		this(x,y,element);
		lines = l;
	}

	protected Case(int x, int y,Mur m,Tuile element,String c){
		this(x,y,element);
		wall = m;
		color = c;
	}
	
	protected String getInitialColor () {
		switch(color) {
		case "Bleu": return "-b";
		case "Rouge": return "-r";
		case "Noir": return "-n";
		case "Blanc": return "-B";
		case "Jaune": return "-j";
		default: return ". ";
		}
	}

	protected String getColor () {
		return color;
	}

	protected Tuile getTuile(){
		return element;
	}
	
	protected void setElement(Tuile t) {
		element = t;
	}

	protected boolean isEmpty(){
		return this.element == null;
	}

	protected int getX() {
		return x;
	}

	protected int getY() {
		return y;
	}
}
