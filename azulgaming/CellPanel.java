import javax.swing.JPanel;

public class CellPanel  {
	private JPanel label;
	private int x,y;
		
	protected CellPanel  (JPanel label, int x, int y) {
		this.label = label;
		this.x = x;
		this.y = y;
	}

	protected JPanel getPanel() {
		return label;
	}

	protected int getX() {
		return x;
	}

	protected int getY() {
		return y;
	}
}
