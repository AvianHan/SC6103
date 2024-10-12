import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;

public class Marshalling {

    // Marshals an integer to network byte order
    public static byte[] marshalInt(int value) {
        return ByteBuffer.allocate(4).putInt(Integer.reverseBytes(value)).array();
    }

    // Unmarshals an integer from network byte order
    public static int unmarshalInt(byte[] data) {
        return Integer.reverseBytes(ByteBuffer.wrap(data).getInt());
    }

    // Marshals a float to network byte order
    public static byte[] marshalFloat(float value) {
        int intBits = Float.floatToIntBits(value);
        int netIntBits = Integer.reverseBytes(intBits);
        return ByteBuffer.allocate(4).putInt(netIntBits).array();
    }

    // Unmarshals a float from network byte order
    public static float unmarshalFloat(byte[] data) {
        int intBits = Integer.reverseBytes(ByteBuffer.wrap(data).getInt());
        return Float.intBitsToFloat(intBits);
    }

    // Marshals a string with a 4-byte length prefix
    public static byte[] marshalString(String str) {
        byte[] strBytes = str.getBytes(StandardCharsets.UTF_8);
        byte[] lengthBytes = marshalInt(strBytes.length);
        ByteBuffer buffer = ByteBuffer.allocate(4 + strBytes.length);
        buffer.put(lengthBytes);
        buffer.put(strBytes);
        return buffer.array();
    }

    // Unmarshals a string with a 4-byte length prefix
    public static String unmarshalString(byte[] data) {
        ByteBuffer buffer = ByteBuffer.wrap(data);
        int length = unmarshalInt(new byte[] { buffer.get(), buffer.get(), buffer.get(), buffer.get() });
        byte[] strBytes = new byte[length];
        buffer.get(strBytes);
        return new String(strBytes, StandardCharsets.UTF_8);
    }

    // Marshals an entire message
    public static byte[] marshalMessage(MessageFormat.Message message) {
        ByteBuffer buffer = ByteBuffer.allocate(1 + 4 + 4 + message.dataLength);
        buffer.put(message.messageType);
        buffer.put(marshalInt(message.requestId));
        buffer.put(marshalInt(message.dataLength));
        buffer.put(message.data);
        return buffer.array();
    }

    // Unmarshals an entire message
    public static MessageFormat.Message unmarshalMessage(byte[] byteData) {
        ByteBuffer buffer = ByteBuffer.wrap(byteData);
        
        byte messageType = buffer.get();
        int requestId = unmarshalInt(new byte[] { buffer.get(), buffer.get(), buffer.get(), buffer.get() });
        int dataLength = unmarshalInt(new byte[] { buffer.get(), buffer.get(), buffer.get(), buffer.get() });

        byte[] data = new byte[dataLength];
        buffer.get(data);

        return new MessageFormat.Message(messageType, requestId, dataLength, data);
    }
}
