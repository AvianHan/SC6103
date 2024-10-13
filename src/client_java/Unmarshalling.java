package SC6103_DS.src.client_java;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;

public class Unmarshalling {

    // Decode an int from 4 bytes
    public static int decode_int(byte[] data, int offset) {
        return ByteBuffer.wrap(data, offset, 4).getInt();
    }

    // Decode a float from 4 bytes
    public static float decode_float(byte[] data, int offset) {
        return ByteBuffer.wrap(data, offset, 4).getFloat();
    }

    // Decode a string with a 4-byte length prefix
    public static String decode_string(byte[] data, int offset) {
        int length = decode_int(data, offset); // String length
        byte[] strBytes = new byte[length];
        System.arraycopy(data, offset + 4, strBytes, 0, length);
        return new String(strBytes, StandardCharsets.UTF_8);
    }

    // Decode departure time structure
    public static Message.DepartureTime decode_departure_time(byte[] data, int offset) {
        int year = decode_int(data, offset);
        int month = decode_int(data, offset + 4);
        int day = decode_int(data, offset + 8);
        int hour = decode_int(data, offset + 12);
        int minute = decode_int(data, offset + 16);
        return new Message.DepartureTime(year, month, day, hour, minute);
    }

    // Decode a full message header and payload
    public static Message decode_message(byte[] data) {
        ByteBuffer buffer = ByteBuffer.wrap(data);
        int message_type = buffer.get(); // 1-byte message type
        int request_id = buffer.getInt(); // 4-byte request ID
        int payload_len = buffer.getInt(); // 4-byte payload length
        byte[] payload = new byte[payload_len];
        buffer.get(payload); // Variable-length payload

        return new Message(message_type, request_id, payload);
    }

}
