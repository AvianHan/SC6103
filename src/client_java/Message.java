package SC6103_DS.src.client_java;

public class Message {
    private int message_type;
    private int request_id;
    private byte[] payload;

    public Message(int message_type, int request_id, byte[] payload) {
        this.message_type = message_type;
        this.request_id = request_id;
        this.payload = payload;
    }

    public int get_message_type() {
        return message_type;
    }

    public int get_request_id() {
        return request_id;
    }

    public byte[] get_payload() {
        return payload;
    }

    // Nested class for DepartureTime
    public static class DepartureTime {
        private int year;
        private int month;
        private int day;
        private int hour;
        private int minute;

        public DepartureTime(int year, int month, int day, int hour, int minute) {
            this.year = year;
            this.month = month;
            this.day = day;
            this.hour = hour;
            this.minute = minute;
        }

        @Override
        public String toString() {
            return String.format("%d-%02d-%02d %02d:%02d", year, month, day, hour, minute);
        }
    }
}
