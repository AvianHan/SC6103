package SC6103_DS.src.communication;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;


public class Marshalling {

    // Marshal a Flight object to byte array
    public static byte[] marshalFlight(Message.Flight flight) {
        byte[] sourceBytes = marshalString(flight.sourcePlace);
        byte[] destBytes = marshalString(flight.destinationPlace);

        ByteBuffer buffer = ByteBuffer.allocate(
            4 + sourceBytes.length + destBytes.length + 5 * 4 + 4 + 4 + 4);
        
        buffer.put(marshalInt(flight.flightId));
        buffer.put(sourceBytes);
        buffer.put(destBytes);
        buffer.put(marshalInt(flight.year));
        buffer.put(marshalInt(flight.month));
        buffer.put(marshalInt(flight.day));
        buffer.put(marshalInt(flight.hour));
        buffer.put(marshalInt(flight.minute));
        buffer.put(marshalFloat(flight.airfare));
        buffer.put(marshalInt(flight.seatAvailability));
        buffer.put(marshalInt(flight.baggageAvailability));

        return buffer.array();
    }
}
