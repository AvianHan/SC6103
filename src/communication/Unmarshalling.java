package SC6103_DS.src.communication;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;

public class Unmarshalling {

    // Demarshal an entire Message object from byte array
    public static Message demarshalMessage(byte[] byteArray) {
        ByteBuffer buffer = ByteBuffer.wrap(byteArray);

        byte messageType = buffer.get();
        int requestId = demarshalInt(new byte[] { buffer.get(), buffer.get(), buffer.get(), buffer.get() });
        int dataLength = demarshalInt(new byte[] { buffer.get(), buffer.get(), buffer.get(), buffer.get() });

        byte[] data = new byte[dataLength];
        buffer.get(data);

        return new Message(messageType, requestId, dataLength, data);
    }

    // Demarshal a Flight object from byte array
    public static Message.Flight demarshalFlight(byte[] byteArray) {
        ByteBuffer buffer = ByteBuffer.wrap(byteArray);

        int flightId = demarshalInt(new byte[] { buffer.get(), buffer.get(), buffer.get(), buffer.get() });
        String sourcePlace = demarshalString(buffer);
        String destinationPlace = demarshalString(buffer);
        Message.DepartureTime departureTime = demarshalDepartureTime(buffer);
        float airfare = demarshalFloat(new byte[] { buffer.get(), buffer.get(), buffer.get(), buffer.get() });
        int seatAvailability = demarshalInt(new byte[] { buffer.get(), buffer.get(), buffer.get(), buffer.get() });
        int baggageAvailability = demarshalInt(new byte[] { buffer.get(), buffer.get(), buffer.get(), buffer.get() });

        return new Message.Flight(flightId, sourcePlace, destinationPlace, departureTime, airfare, seatAvailability, baggageAvailability);
    }

    // Demarshal a DepartureTime object from ByteBuffer
    public static Message.DepartureTime demarshalDepartureTime(ByteBuffer buffer) {
        int year = demarshalInt(new byte[] { buffer.get(), buffer.get(), buffer.get(), buffer.get() });
        int month = demarshalInt(new byte[] { buffer.get(), buffer.get(), buffer.get(), buffer.get() });
        int day = demarshalInt(new byte[] { buffer.get(), buffer.get(), buffer.get(), buffer.get() });
        int hour = demarshalInt(new byte[] { buffer.get(), buffer.get(), buffer.get(), buffer.get() });
        int minute = demarshalInt(new byte[] { buffer.get(), buffer.get(), buffer.get(), buffer.get() });
        return new Message.DepartureTime(year, month, day, hour, minute);
    }

    // Method to demarshal a String from ByteBuffer
    public static String demarshalString(ByteBuffer buffer) {
        int length = buffer.getInt();
        byte[] stringBytes = new byte[length];
        buffer.get(stringBytes);
        return new String(stringBytes, StandardCharsets.UTF_8);
    }

    // Method to demarshal an int from byte array
    public static int demarshalInt(byte[] data) {
        return ByteBuffer.wrap(data).getInt();
    }

    // Method to demarshal a float from byte array
    public static float demarshalFloat(byte[] data) {
        return ByteBuffer.wrap(data).getFloat();
    }
}
