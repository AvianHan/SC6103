package src;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class UserInterface extends JFrame {

    private Client client;  // Reference to the Client class, used to communicate with the server
    private String selectedOperation;  // Stores the current selected operation

    // GUI components
    private JTextArea outputArea;  // Area to display output
    private JTextField inputField1, inputField2, inputField3;  // Input fields for user inputs
    private JLabel inputPromptLabel1, inputPromptLabel2, inputPromptLabel3;  // Labels for input fields
    private JButton executeButton, testAddressButton, quitButton, queryFlightIdButton, queryFlightInfoButton, 
                    makeReservationButton, queryBaggageButton, addBaggageButton, followFlightIdButton;  // Buttons for different actions

    // Constructor for initializing the GUI and setting the client reference
    public UserInterface(Client client) {
        this.client = client;
        this.client.setUserInterface(this);  // Set this UI as the callback for the client

        setTitle("Flight Information System");  // Set the title of the window
        setSize(600, 500);  // Set the size of the window
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);  // Set the default close operation
        initGUI();  // Initialize the GUI components
    }

    // Initialize the GUI components
    private void initGUI() {
        outputArea = new JTextArea();  // Text area to display the output
        outputArea.setEditable(false);  // Make the text area non-editable

        inputField1 = new JTextField(10);  // Input field 1 with a width of 10 characters
        inputField2 = new JTextField(10);  // Input field 2 with a width of 10 characters
        inputField3 = new JTextField(10);  // Input field 3 with a width of 10 characters

        inputPromptLabel1 = new JLabel("Input 1: ");  // Label for the first input field
        inputPromptLabel2 = new JLabel("Input 2: ");  // Label for the second input field
        inputPromptLabel3 = new JLabel("Input 3: ");  // Label for the third input field

        executeButton = new JButton("Execute");  // Button to execute the selected operation
        quitButton = new JButton("Quit");  // Button to quit the application

        // Initialize buttons for different actions (query flight, baggage, etc.)
        queryFlightIdButton = new JButton("Query Flight ID");
        queryFlightInfoButton = new JButton("Query Flight Info");
        makeReservationButton = new JButton("Make Seat Reservation");
        queryBaggageButton = new JButton("Query Baggage");
        addBaggageButton = new JButton("Add Baggage");
        followFlightIdButton = new JButton("Follow Flight Id");

        // Input panel to hold the input fields and their corresponding labels
        JPanel inputPanel = new JPanel();
        inputPanel.setLayout(new GridLayout(3, 2));  // Set a 3x2 grid layout for the input fields and labels
        inputPanel.add(inputPromptLabel1);
        inputPanel.add(inputField1);
        inputPanel.add(inputPromptLabel2);
        inputPanel.add(inputField2);
        inputPanel.add(inputPromptLabel3);
        inputPanel.add(inputField3);

        // Button panel to hold action buttons
        JPanel buttonPanel = new JPanel();
        buttonPanel.setLayout(new GridLayout(3, 2, 10, 10));  // Set a 3x2 grid layout with 10px spacing
        buttonPanel.add(queryFlightIdButton);  // Add Query Flight ID button
        buttonPanel.add(queryFlightInfoButton);  // Add Query Flight Info button
        buttonPanel.add(makeReservationButton);  // Add Make Seat Reservation button
        buttonPanel.add(queryBaggageButton);  // Add Query Baggage button
        buttonPanel.add(addBaggageButton);  // Add Add Baggage button
        buttonPanel.add(quitButton);  // Add Quit button
        buttonPanel.add(followFlightIdButton);  // Add Follow Flight ID button

        // Set up the main container and layout
        Container container = getContentPane();
        container.setLayout(new BorderLayout());  // Use BorderLayout for the main layout
        container.add(new JScrollPane(outputArea), BorderLayout.CENTER);  // Add output area in the center with scroll support
        container.add(inputPanel, BorderLayout.NORTH);  // Add input fields panel at the top
        container.add(buttonPanel, BorderLayout.SOUTH);  // Add button panel at the bottom
        container.add(executeButton, BorderLayout.EAST); // Add the execute button on the right

        // Add button to test server address
        testAddressButton = new JButton("Test Server Address");
        testAddressButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                String serverAddress = client.getServerAddress();  // Fetch server address
                outputArea.append("Server Address: " + serverAddress + "\n");  // Display server address in the output area
            }
        });
        buttonPanel.add(testAddressButton);  // Add the Test Server Address button to the panel

        // Add a button to test the connection to the server
        JButton testConnectionButton = new JButton("Test Connection");

        testConnectionButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                String serverAddress = client.getServerAddress();  // Fetch server address
                client.testConnection();  // Test connection with the server
                outputArea.append("testing connection with server: " + serverAddress + "\n");  // Display test connection result
            }
        });

        // Add Test Connection button to the panel
        buttonPanel.add(testConnectionButton);

        // Action listener for Query Flight ID button
        queryFlightIdButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                selectedOperation = "query_flight_id";  // Set selected operation as query flight ID
                // Make the appropriate input fields and labels visible
                inputPromptLabel1.setVisible(true);
                inputPromptLabel2.setVisible(true);
                inputPromptLabel3.setVisible(false);  // Only need two inputs
                inputField1.setVisible(true);
                inputField2.setVisible(true);
                inputField3.setVisible(false);

                inputPromptLabel1.setText("Source Place:");  // Update label text
                inputPromptLabel2.setText("Destination Place:");
            }
        });

        // Action listener for Query Flight Info button
        queryFlightInfoButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                selectedOperation = "query_flight_info";  // Set selected operation as query flight info
                // Make only the first input field visible (no need for more than one input)
                inputPromptLabel1.setVisible(true);
                inputPromptLabel2.setVisible(false);  
                inputPromptLabel3.setVisible(false);
                inputField1.setVisible(true);
                inputField2.setVisible(false);
                inputField3.setVisible(false);

                inputPromptLabel1.setText("Flight ID:");  // Update label text for flight ID
            }
        });

        // Action listener for Follow Flight ID button
        followFlightIdButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                selectedOperation = "follow_flight_id";  // Set selected operation as follow flight info
                // Make only the first input field visible (no need for more than one input)
                inputPromptLabel1.setVisible(true);
                inputPromptLabel2.setVisible(false);  
                inputPromptLabel3.setVisible(false);
                inputField1.setVisible(true);
                inputField2.setVisible(false);
                inputField3.setVisible(false);

                inputPromptLabel1.setText("Flight ID:");  // Update label text for flight ID
            }
        });

        // Action listener for Make Seat Reservation button
        makeReservationButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                selectedOperation = "make_seat_reservation";  // Set selected operation as seat reservation
                // Make two input fields visible
                inputPromptLabel1.setVisible(true);
                inputPromptLabel2.setVisible(true);
                inputPromptLabel3.setVisible(false);  // Only two inputs are needed
                inputField1.setVisible(true);
                inputField2.setVisible(true);
                inputField3.setVisible(false);

                inputPromptLabel1.setText("Flight ID:");  // Update label text for flight ID
                inputPromptLabel2.setText("Number of Seats:");  // Update label text for number of seats
            }
        });

        // Action listener for Query Baggage button
        queryBaggageButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                selectedOperation = "query_baggage_availability";  // Set selected operation as query baggage
                // Make only the first input field visible
                inputPromptLabel1.setVisible(true);
                inputPromptLabel2.setVisible(false);  
                inputPromptLabel3.setVisible(false);
                inputField1.setVisible(true);
                inputField2.setVisible(false);
                inputField3.setVisible(false
