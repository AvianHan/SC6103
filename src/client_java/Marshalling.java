package SC6103_DS.src.client_java;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;

public class Marshalling {

    // Marshal an entire Message object to byte array
    public static byte[] marshalMessage(Message message) {
        byte[] data = message.data;

        ByteBuffer buffer = ByteBuffer.allocate(1 + 4 + 4 + data.length);
        buffer.put(marshalInt(message.messageType));
        buffer.put(marshalInt(message.requestId));
        buffer.put(marshalInt(message.dataLength));
        buffer.put(data);

        return buffer.array();
    }

    // Marshal a Flight object to byte array
    public static byte[] marshalFlight(Message.Flight flight) {
        byte[] sourceBytes = marshalString(flight.sourcePlace);
        byte[] destBytes = marshalString(flight.destinationPlace);
        byte[] departureBytes = marshalDepartureTime(flight.departureTime);

        ByteBuffer buffer = ByteBuffer.allocate(
            4 + sourceBytes.length + destBytes.length + departureBytes.length + 4 + 4 + 4);

        buffer.put(marshalInt(flight.flightId));
        buffer.put(sourceBytes);
        buffer.put(destBytes);
        buffer.put(departureBytes);
        buffer.put(marshalFloat(flight.airfare));
        buffer.put(marshalInt(flight.seatAvailability));
        buffer.put(marshalInt(flight.baggageAvailability));

        return buffer.array();
    }

    // Marshal a DepartureTime object to byte array
    public static byte[] marshalDepartureTime(Message.DepartureTime departureTime) {
        ByteBuffer buffer = ByteBuffer.allocate(5 * 4);
        buffer.put(marshalInt(departureTime.year));
        buffer.put(marshalInt(departureTime.month));
        buffer.put(marshalInt(departureTime.day));
        buffer.put(marshalInt(departureTime.hour));
        buffer.put(marshalInt(departureTime.minute));
        return buffer.array();
    }

    // Method to marshal a String with length prefix
    public static byte[] marshalString(String value) {
        byte[] stringBytes = value.getBytes(StandardCharsets.UTF_8);
        ByteBuffer buffer = ByteBuffer.allocate(4 + stringBytes.length);
        buffer.putInt(stringBytes.length);
        buffer.put(stringBytes);
        return buffer.array();
    }

    // Method to marshal an int into a byte array
    public static byte[] marshalInt(int value) {
        ByteBuffer buffer = ByteBuffer.allocate(4);
        buffer.putInt(value);
        return buffer.array();
    }

    // Method to marshal a float into a byte array
    public static byte[] marshalFloat(float value) {
        ByteBuffer buffer = ByteBuffer.allocate(4);
        buffer.putFloat(value);
        return buffer.array();
    }
}
