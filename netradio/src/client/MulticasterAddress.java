package src.client;

/**
 * Represents a multicaster address used to store coordinates of a multicaster
 * received from a manager after a LIST command.
 */
public class MulticasterAddress {

	/** Is the string representation of the casting address of the multicaster. */
	private String castAddress;
	/** Is the casting port of the multicaster. */
	private Integer castPort;
	/** Is the string representation of the communication address of the multicaster. */
	private String comAddress;
	/** Is the communication port of the multicaster. */
	private Integer comPort;

	/**
	 * Builds a MulticasterAddress.
	 *
	 * @param castAddress is the string representation of the casting address of the multicaster.
	 * @param castPort is the multicasting port of the multicaster.
	 * @param comAddress is the string representation of the communication address of the multicaster.
	 * @param comPort is the communication port of the multicaster.
	 */
	public MulticasterAddress(String castAddress, Integer castPort, String comAddress, Integer comPort) {
		this.castAddress = Client.lightenAddress(castAddress);
		this.castPort = castPort;
		this.comAddress = (null == comAddress ? null : Client.lightenAddress(comAddress));
		this.comPort = comPort;
	}

	/**
	 * Builds a MulticasterAddress with only castAddress and castPort.
	 *
	 * @param castAddress is the string representation of the casting address of the multicaster.
	 * @param castPort is the multicasting port of the multicaster.
	 */
	public MulticasterAddress(String castAddress, Integer castPort) {
		this(castAddress, castPort, null, null);
	}

	public String getCastAddress() {
		return castAddress;
	}

	public Integer getCastPort() {
		return castPort;
	}

	public String getComAddress() {
		return comAddress;
	}

	public Integer getComPort() {
		return comPort;
	}

	@Override
	public boolean equals(Object o) {
		if (this == o) {
			return true;
		}

		if (!(o instanceof MulticasterAddress)) {
			return false;
		}

		MulticasterAddress multicaster = (MulticasterAddress) o;

		return castAddress.equals(multicaster.castAddress) &&
		castPort == multicaster.castPort &&
		comAddress.equals(multicaster.comAddress) &&
		comPort == multicaster.comPort;
	}
}
