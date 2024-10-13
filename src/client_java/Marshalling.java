package SC6103_DS.src.client_java;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;

public class Marshalling {
    
    // Encode an int as 4 bytes
    public static byte[] encode_int(int value) {
        return ByteBuffer.allocate(4).putInt(value).array();
    }

    // Encode a float as 4 bytes
    public static byte[] encode_float(float value) {
        return ByteBuffer.allocate(4).putFloat(value).array();
    }

    // Encode a string with a 4-byte length prefix
    public static byte[] encode_string(String value) {
        byte[] strBytes = value.getBytes(StandardCharsets.UTF_8);
        ByteBuffer buffer = ByteBuffer.allocate(4 + strBytes.length);
        buffer.putInt(strBytes.length);
        buffer.put(strBytes);
        return buffer.array();
    }

    // Encode departure time with year, month, day, hour, and minute (20 bytes total)
    public static byte[] encode_departure_time(int year, int month, int day, int hour, int minute) {
        ByteBuffer buffer = ByteBuffer.allocate(20);
        buffer.putInt(year);
        buffer.putInt(month);
        buffer.putInt(day);
        buffer.putInt(hour);
        buffer.putInt(minute);
        return buffer.array();
    }

    // Build a complete message according to the protocol
    public static byte[] build_message(int message_type, int request_id, byte[] payload) {
        int payload_len = payload.length;
        ByteBuffer buffer = ByteBuffer.allocate(1 + 4 + 4 + payload_len);
        buffer.put((byte) message_type); // 1-byte message type
        buffer.putInt(request_id); // 4-byte request ID
        buffer.putInt(payload_len); // 4-byte payload length
        buffer.put(payload); // Variable-length payload
        return buffer.array();
    }

}
