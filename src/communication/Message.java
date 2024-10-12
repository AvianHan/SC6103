public class Message {

    // Message Type Constants
    public static final byte REGISTER_REQUEST = 0x00;
    public static final byte QUERY_FLIGHT_ID_REQUEST = 0x01;
    public static final byte QUERY_DEPARTURE_TIME_REQUEST = 0x02;
    public static final byte MAKE_SEAT_RESERVATION_REQUEST = 0x03;
    public static final byte SELECT_MEAL_REQUEST = 0x04;
    public static final byte ADD_EXTRA_BAGGAGE_REQUEST = 0x05;

    // Message structure
    public static class MessageStructure {
        public byte messageType;
        public int requestId;
        public int dataLength;
        public byte[] data;

        public MessageStructure(byte messageType, int requestId, int dataLength, byte[] data) {
            this.messageType = messageType;
            this.requestId = requestId;
            this.dataLength = dataLength;
            this.data = data;
        }
    }
}
