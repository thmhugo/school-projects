package src.client;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.MulticastSocket;

/**
 * A Receiver is in charge of the multicasting message reception.
 */
public class Receiver extends Thread {

	/** Set to true on [ctrl-c] (@see Client.startListening). */
	private volatile boolean cancel = false;
	/** The multicaster to listen to. */
	private MulticastSocket mso;
	private Printer printer;

	/**
	 * Builds a Receiver given a multicaster socket.
	 *
	 * @param mso is the socket of the multicaster.
	 */
	Receiver(MulticastSocket mso, boolean colorizedPrinting) {
		this.mso = mso;
		this.printer = new Printer(colorizedPrinting);
	}

	@Override
	public void run() {
		byte[] data = new byte[Client.CONTENT_MESS_SIZE];
		DatagramPacket packet = new DatagramPacket(data, data.length);

		while (!cancel) {
			try {
				mso.receive(packet);
				String receivedMessage =
					new String(packet.getData(), 0, packet.getLength());

				if (Client.messageHasRightFormat(receivedMessage)) {
					printer.printMessage(receivedMessage);
				}
				else {
					printer.printError("Unexpected message " + receivedMessage + ".");
				}
			}
			catch (IOException e) {
				if (!cancel) {
					printer.printError("Unexpected failure on packet reception.");
				}
			}
		}
	}

	public void cancel() {
		this.cancel = true;
		this.mso.close();
	}
}
