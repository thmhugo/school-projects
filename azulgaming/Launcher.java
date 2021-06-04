import java.awt.*;

public class Launcher{
	public static void launchGame () {
		EventQueue.invokeLater(() -> {
            new Initializer();
        });
	}
}