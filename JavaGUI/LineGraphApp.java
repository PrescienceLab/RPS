// LineGraphApp.java   Written by Brandon O'Bryant   Last Update: 3/10/01
// See LineGraphApp.txt for an explanation of LineGraphApp.java

import java.awt.event.*;
import javax.swing.*;
import java.awt.*;
import LineGraph;

public class LineGraphApp extends JFrame implements ActionListener
{
  JTextField hostField, portField, connField, protField;
  
  public LineGraphApp() {
    super("The Application Window - Test");
    setSize(800,600);
    addWindowListener(new ExitListener());
    
    // BEGIN MENU CODE
    JMenuBar menuBar;
    JMenu fileMenu, createSubmenu;
    JMenuItem connectionMenuItem, exitMenuItem;
    
    // Create the menu bar
    menuBar = new JMenuBar();
    setJMenuBar(menuBar);
    
    //Build the file menu
    fileMenu = new JMenu("File");
    fileMenu.setMnemonic(KeyEvent.VK_A);
    fileMenu.getAccessibleContext().setAccessibleDescription("The Main Menu");
    menuBar.add(fileMenu);
        
    //Add submenus
    fileMenu.addSeparator();
    createSubmenu = new JMenu("Create");
    createSubmenu.setMnemonic(KeyEvent.VK_S);
    fileMenu.add(createSubmenu);
    
    // Add exit option
    exitMenuItem = new JMenuItem("Exit",KeyEvent.VK_T);
    exitMenuItem.getAccessibleContext().setAccessibleDescription("exits the application");
    exitMenuItem.addActionListener(this);
    exitMenuItem.setName("exit application");
    fileMenu.add(exitMenuItem);

    //Add menu items to create submenu
    connectionMenuItem = new JMenuItem("Create new connection",KeyEvent.VK_T);
    connectionMenuItem.getAccessibleContext().setAccessibleDescription("Create a new connection");
    connectionMenuItem.addActionListener(this);
    connectionMenuItem.setName("create new connection");
    createSubmenu.add(connectionMenuItem);
    // END MENU BAR CODE

    Container content = getContentPane();
    content.setBackground(Color.lightGray);
    
    // This panel that the buttons it contains do not do anything.
    JPanel panel1 = new JPanel(new GridLayout(1,5));
    panel1.add(new JButton("..."));
    panel1.add(new JButton("..."));
    panel1.add(new JButton("..."));
    panel1.add(new JButton("..."));
    panel1.add(new JButton("..."));
    content.add(panel1, BorderLayout.NORTH);
    
    setVisible(true);
  }
  
    // When a menu or button is selected/pressed one of the following commands occurs and the appropriate function is called
  public void actionPerformed(ActionEvent e) {
    JComponent source = (JComponent)(e.getSource());
    System.out.println("Action event detected.  Event source: " + source.getName());
    String command = source.getName();
    if (command == "create new connection") {
      newConnection();
    } else if (command == "launch new connection") {
      createNewGraph();
    } else if (command == "exit application") {
	System.exit(0);
    }
  }

  // Constructs New Connection window 
  public void newConnection() {
    // Create new window
    JFrame connectionWindow = new JFrame("New Connection");
    connectionWindow.setSize(400,300);
    connectionWindow.setDefaultCloseOperation(DISPOSE_ON_CLOSE);
    
    // Define varaibles and items
    hostField = new JTextField(30);
    portField = new JTextField(5);
    protField = new JTextField("tcp",4);
    connField = new JTextField("client",8);
    JLabel title, hostLabel,portLabel, connLabel, protLabel, eLabel;
    title = new JLabel("Create New Connection");
    hostLabel = new JLabel("Host:");
    portLabel = new JLabel("Port:");
    connLabel = new JLabel("Connection Type:");
    protLabel = new JLabel("Protocol:");
    eLabel = new JLabel(); // Empty label for spacing
    
    JButton createButton = new JButton("Create");
    createButton.addActionListener(this);
    createButton.setName("launch new connection");
    
    // Put everything on the content pane
    Container content = connectionWindow.getContentPane();
    content.setBackground(Color.white);
    
    JPanel topPanel = new JPanel(new GridLayout(1,1));
    topPanel.setPreferredSize(new Dimension(400,75));
    JPanel middlePanel = new JPanel(new GridLayout(4,2));
    middlePanel.setPreferredSize(new Dimension(400,200));
    JPanel bottomPanel = new JPanel(new GridLayout(1,3));
    bottomPanel.setPreferredSize(new Dimension(400,25));
    topPanel.add(title);
    middlePanel.add(hostLabel);
    middlePanel.add(hostField);
    middlePanel.add(portLabel);
    middlePanel.add(portField);
    middlePanel.add(protLabel);
    middlePanel.add(protField);
    middlePanel.add(connLabel);
    middlePanel.add(connField);
    bottomPanel.add(eLabel); bottomPanel.add(eLabel);
    bottomPanel.add(createButton);
    content.add(topPanel, BorderLayout.NORTH);
    content.add(middlePanel, BorderLayout.CENTER);
    content.add(bottomPanel, BorderLayout.SOUTH);
    
    connectionWindow.setVisible(true);
  }
  
  public void createNewGraph() {
    System.out.println("Opening new window and putting graph in it");
    String fullPath = connField.getText() + ":" + protField.getText() + ":" + hostField.getText() + ":" + portField.getText(); // construct the path
    LineGraph testRun = new LineGraph(fullPath);
  }

  public class ExitListener extends WindowAdapter {
    public void windowClosing(WindowEvent event) {
      System.exit(0);
    }
  }
  
  public static void main(String args[])
  {
      LineGraphApp L1 = new LineGraphApp(); 
  }

};
    
