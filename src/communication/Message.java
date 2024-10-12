package SC6103_DS.src.communication;

public class Message {

    // Message Type Constants
    public static final byte REGISTER_REQUEST = 0x00;
    public static final byte QUERY_FLIGHT_ID_REQUEST = 0x01;
    public static final byte QUERY_FLIGHT_INFO_REQUEST = 0x02;
    public static final byte MAKE_SEAT_RESERVATION_REQUEST = 0x03;
    public static final byte QUERY_BAGGAGE_AVAILABILITY_REQUEST = 0x04;
    public static final byte ADD_BAGGAGE_REQUEST = 0x05;

    // Nested DepartureTime structure within Flight
    public static class DepartureTime {
        public int year;
        public int month;
        public int day;
        public int hour;
        public int minute;

        public DepartureTime(int year, int month, int day, int hour, int minute) {
            this.year = year;
            this.month = month;
            this.day = day;
            this.hour = hour;
            this.minute = minute;
        }
    }

    // Flight structure
    public static class Flight {
        public int flightId;
        public String sourcePlace;
        public String destinationPlace;
        public DepartureTime departureTime;
        public float airfare;
        public int seatAvailability;
        public int baggageAvailability;

        public Flight(int flightId, String sourcePlace, String destinationPlace, 
                      DepartureTime departureTime, float airfare, 
                      int seatAvailability, int baggageAvailability) {
            this.flightId = flightId;
            this.sourcePlace = sourcePlace;
            this.destinationPlace = destinationPlace;
            this.departureTime = departureTime;
            this.airfare = airfare;
            this.seatAvailability = seatAvailability;
            this.baggageAvailability = baggageAvailability;
        }
    }

    // Message fields
    public byte messageType;
    public int requestId;
    public int dataLength;
    public byte[] data;

    // Constructor for Message class with direct byte array data
    public Message(byte messageType, int requestId, int dataLength, byte[] data) {
        this.messageType = messageType;
        this.requestId = requestId;
        this.dataLength = dataLength;
        this.data = data;
    }

    // Constructor for Message class with Flight object data
    public Message(byte messageType, int requestId, Flight flightData) {
        this.messageType = messageType;
        this.requestId = requestId;
        this.data = Marshalling.marshalFlight(flightData);
        this.dataLength = data.length;
    }

    // Method to print Message information
    public void printMessageInfo() {
        System.out.println("Message Type: " + messageType);
        System.out.println("Request ID: " + requestId);
        System.out.println("Data Length: " + dataLength);
    }
}
