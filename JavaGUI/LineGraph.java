// LineGraph.java   Written by Brandon O'Bryant   Last Update: 3/10/01
// See LineGraph.txt for an explanation of LineGraph.java

import java.awt.event.*;
import javax.swing.*;
import java.awt.*;

public class LineGraph extends JPanel
{
    // JNI functions used to connect to the RPS system
    public native void callLoadBufferClient(double[] dArray, java.lang.String route);
    public native void callPredBufferClient(double[] dArray1,double[] dArray2, java.lang.String route);
    public native void callMeasureBufferClient(double[] dArray, java.lang.String route); // untested
    static
    {
        System.loadLibrary("LineGraph");
    }

    // Variables
    String path1, path2; //Routes to the standard and prediction targets
    int valsCount; // Number of datapoints in standardLineVals
    int lengthToDisp; // The maximum to display,
    private double curMinY, curMaxY; // The lowest and highest Y values
    private int graphHeight, graphWidth; // Actual display region sizes
    private int boundaryX; // Space around the display region
    private int boundaryY;
    private int windowHeight, windowWidth; // Defaults
    private javax.swing.Timer timer;
    double[] standardLineVals; // This and next 4 hold all incoming data values. 
    double[] predictionLineVals;
    double[] predictionLineErrors;
    double[] uprPredictionLineVals;
    double[] lwrPredictionLineVals;
    double[] stdLineDispVals; // buffer to be displayed
    double[] prdLineDispVals; // buffer to be displayed
    double[] uprPrdLineDispVals; // buffer to be displayed
    double[] lwrPrdLineDispVals; //buffer to be displayed
    private boolean errorBars, predictionLine, standardLine; // boolean values that determine what will be displayed or called
    JTextField numPoints, maxY, minY; // Used in the graph control panel that is displayed below the graph

    // Connect only to acquire load measurements
    public LineGraph(String route) {
	init(route, "");

	JFrame graph = new JFrame(route);
	graph.setSize(600,500);
	graph.addWindowListener(new ExitListener());
	Container content = graph.getContentPane();
	content.setBackground(Color.lightGray);

	// The control panel below the graph
	JPanel graphInfo = new JPanel(new GridLayout(2,4));
	numPoints.setText(lengthToDisp + "");
	graphInfo.add(new JLabel("Max"));
	graphInfo.add(maxY);
	graphInfo.add(new JLabel("Num. Points:"));
	graphInfo.add(numPoints);
	graphInfo.add(new JLabel("Min"));
	graphInfo.add(minY);
	graphInfo.add(new JLabel()); graphInfo.add(new JLabel());
	content.add(graphInfo, BorderLayout.SOUTH);

	content.add(this, BorderLayout.CENTER);
	
	if (standardLine && !predictionLine) {   // tell JFrame to size itself
	    setSize(windowWidth, windowHeight);
	} else if (standardLine) {
	    windowWidth = windowWidth*2 - boundaryX*2;
	    setSize(windowWidth, windowHeight);
	} else {
	    System.out.println("No graph to display.");
	}
	graph.setVisible(true);
	beginTimer();
    }

    // Connects to acquire load measurements as well as prediction values  
    public LineGraph(String route1, String route2) {
	init(route1, route2);
	standardLine = true;
	predictionLine = true;
	errorBars = true;
	
	JFrame theWindow = new JFrame(route1);
	theWindow.setSize(600,500);
	theWindow.addWindowListener(new ExitListener());
	Container content = theWindow.getContentPane();
	content.add(this, BorderLayout.CENTER);

	// The control panel below the graph
	JPanel graphInfo = new JPanel(new GridLayout(2,4));
	numPoints.setText(lengthToDisp + "");
	graphInfo.add(new JLabel("Max"));
	graphInfo.add(maxY);
	graphInfo.add(new JLabel("Num. Points:"));
	graphInfo.add(numPoints);
	graphInfo.add(new JLabel("Min"));
	graphInfo.add(minY);
	graphInfo.add(new JLabel()); graphInfo.add(new JLabel());
	content.add(graphInfo, BorderLayout.SOUTH);

	if (standardLine && !predictionLine) {
	    setSize(windowWidth, windowHeight);
	} else if (standardLine) {
	    windowWidth = windowWidth*2 - boundaryX*2;
	    setSize(windowWidth, windowHeight);
	} else {
	    System.out.println("No graph to display.");
	}
	theWindow.setVisible(true);
	beginTimer();
    }
    
    // Initialize all values
    private void init(String route1, String route2) {
	path1 = new String(route1);
	path2 = new String(route2);
	numPoints = new JTextField(3);
	maxY = new JTextField(3);
	minY = new JTextField(3);
	valsCount = 0;
	lengthToDisp = 15;
	stdLineDispVals = new double[lengthToDisp];
	prdLineDispVals = new double[lengthToDisp];
	uprPrdLineDispVals = new double[lengthToDisp];
	lwrPrdLineDispVals = new double[lengthToDisp];
	curMinY = 0; curMaxY = 1;
	graphWidth = 480; graphHeight = 400;
	windowWidth = 600; windowHeight = 500;
	boundaryX = 60;
	boundaryY = 50;
	standardLineVals = new double[1024];
	predictionLineVals = new double[180];
	predictionLineErrors = new double[180];
	uprPredictionLineVals = new double[180];
	lwrPredictionLineVals = new double[180];
	standardLine = true;
	predictionLine = false;
	errorBars = true;
    }
    
    // This function will be used to display and update the axes on the graph.  It is not called in LineGraph
    public void displayAxes(Container content) {
	JPanel xAxis = new JPanel(new FlowLayout());
	JPanel yAxis = new JPanel(new FlowLayout());
	xAxis.add(new JLabel("xAxis"));
	yAxis.add(new JLabel("yAxis"));
	content.add(xAxis, BorderLayout.WEST);
	content.add(yAxis, BorderLayout.SOUTH);
    }
    
    // Every 1 second calls getNewData function
    private void beginTimer() {
	timer = new javax.swing.Timer(1000, new ActionListener() {
		public void actionPerformed(ActionEvent evt) {
		    getNewData();
		}
	    });
	timer.start();
    }
    
    // Calls all the functions that will acquire data, recalculate, then update everything
    private void getNewData() {

	double dArr[] = new double[3];
	if (standardLine) {
	    callLoadBufferClient(dArr, path1);
	}
	if (predictionLine) {
	    callPredBufferClient(predictionLineVals, predictionLineErrors, path2);
	}

	//System.out.println("Java data -1-: " + dArr[0] + " -2-: " + dArr[1] + " -3-: " + dArr[2]);  // Displays the measurement data acquired

	System.out.println("valsCount="+valsCount);
	standardLineVals[valsCount]=(double)dArr[0];
	
	valsCount = valsCount + 1; 
	updateStandardDisp(); // Transfer data to display array
	updatePredictionDisp();  // Transfer data to display arrays
	setRange();
	repaint();
	
    }
    
    // Transfers values that will be displayed from standardLineVals to stdLineDispVals
    private void updateStandardDisp() {
	if (valsCount < lengthToDisp){
	    for (int i=0 ; i < valsCount ; i++) {
		stdLineDispVals[i] = standardLineVals[i];
	    }
	} else {
	    for (int i=0 ; i < lengthToDisp ; i++) {
		stdLineDispVals[i] = standardLineVals[valsCount-lengthToDisp+i];
	    }
	}
    }
    
    // Uses newly acquired prediction data to update the display arrays
    private void updatePredictionDisp() {
	
	// calculate error data
	for (int i = 0 ; i < predictionLineVals.length ; i++) {
	    uprPredictionLineVals[i] = predictionLineVals[i] + predictionLineErrors[i];
	    lwrPredictionLineVals[i] = predictionLineVals[i] - predictionLineErrors[i];
	    if (lwrPredictionLineVals[i] < 0) {
		lwrPredictionLineVals[i] = 0;
	    }
	}
	
	// move the data to the display buffer
	for (int i=0 ; i < lengthToDisp ; i++) {
	    prdLineDispVals[i] = predictionLineVals[i];
	    uprPrdLineDispVals[i] = uprPredictionLineVals[i];
	    lwrPrdLineDispVals[i] = lwrPredictionLineVals[i];
	}
    }
    
    // Returns the X coordinate where the point should be drawn
    private int calcXpos(int pos, int numToDisp) {
	float increment = 0;
	if (numToDisp != 1) {
	    increment = (float)graphWidth / (float)(numToDisp - 1);
	} else {
	    increment = 0;
	}
	int xPosFromWindowSide = (int)((float)pos * increment + (float)boundaryX);
	return xPosFromWindowSide;
    }
    
    // Return the Y coordinate where the point should be drawn
    private int calcYpos(double val) {
	double range = curMaxY - curMinY;
	double distFromGraphTop = curMaxY - val;
	float increment = (float)graphHeight / (float)range;
	int yPosFromWindowTop = (int)((float)distFromGraphTop * increment + (float)boundaryY);  
	return yPosFromWindowTop;
    }
    
    // If necessary, based on new data, calcs a new range for actual display
    private void setRange() {
	// Find Max from Prediction data and Standard Line data, set curMaxY
	double max = 0;
	for (int i = 0; i < lengthToDisp ; i++) {
	    if (stdLineDispVals[i] > max) {
		max = stdLineDispVals[i];
	    }
	    if (predictionLine && (prdLineDispVals[i] > max)) {
		max = prdLineDispVals[i];
	    }
	    if (errorBars && (uprPrdLineDispVals[i] > max)) {
		max = uprPrdLineDispVals[i];
	    }
	}
	curMaxY = max + (5 - (max % 5));
	maxY.setText(curMaxY + ""); // Updates the control panel
	minY.setText(curMinY + "");
    }
    
    // If the user changes the size of the window, this will alter internal variables so that the graph display will scale accordingly
    private void recalculateDims() {
	windowWidth = this.getWidth();
	windowHeight = this.getHeight();
	boundaryY = windowHeight / 10;
	if (predictionLine) {
	    graphWidth = (int)((float)windowWidth * .45);
	    boundaryX = windowWidth / 20;
	} else { 
	    graphWidth = (int)((float)windowWidth * .8);
	    boundaryX = windowWidth / 10;
	}
	graphHeight = (int)((float)windowHeight * .8);
    }
    
    public void paintComponent(Graphics g) {
	if (this.getWidth() != windowWidth || this.getHeight() != windowHeight){
	    recalculateDims();
	}
	
	g.setColor(Color.white); // Clear image
	g.fillRect(0, 0, windowWidth, windowHeight);
	if (standardLine) {
	    g.setColor(Color.lightGray); // Prepare graph
	    g.fillRect(boundaryX, boundaryY, graphWidth, graphHeight); 
	    g.setColor(Color.black); // Draw border
	    g.drawRect(boundaryX, boundaryY, graphWidth, graphHeight);
	    
	    int count; // The number that will be displayed
	    if (valsCount < lengthToDisp) {
		count = valsCount;
	    } else {
		count = lengthToDisp;
	    }
	    
	    // Draw horizontal lines: gray lines that make graph easier to read
	    g.setColor(Color.gray);
	    double range = curMaxY - curMinY; // data value range
	    double graphRange = 0;
	    if (range % 10 == 0) {
		graphRange = range;
	    } else {
		graphRange = (10 * (range/10 + 1));
	    }
	    int iter = (int)(graphRange / 10); // Number of lines to draw
	    float increment = 0;
	    if (iter != 0) {
		increment = (float)graphHeight / (float)iter;
	    }
	    
	    for (int i=1 ; i <= iter ; i++) {
		int point1X = boundaryX + 1;
		int point1Y = (int)((float)boundaryY + (float)graphHeight - (float)i * increment);
		int point2X = boundaryX - 1 + graphWidth;
		int point2Y = (int)((float)boundaryY + (float)graphHeight - (float)i * increment);
		g.drawLine(point1X, point1Y, point2X, point2Y);
	    }
	    
	    // Draw the points and the standard line
	    g.setColor(Color.black);
	    for(int i = 1 ; i < count ; i++) {
		int point1X = calcXpos(i-1, count);
		int point1Y = calcYpos(stdLineDispVals[i-1]);
		int point2X = calcXpos(i, count);
		int point2Y = calcYpos(stdLineDispVals[i]);
		g.fillOval(point1X-1, point1Y-1, 3, 3); 
		g.drawLine(point1X, point1Y, point2X, point2Y);
	    }
	}
	
	// Draw predictionLine if active
	if (predictionLine) {
	    g.setColor(Color.lightGray); // Prepare graph
	    g.fillRect(boundaryX+graphWidth, boundaryY, graphWidth, graphHeight); 
	    g.setColor(Color.black); // Border
	    g.drawRect(boundaryX+graphWidth, boundaryY, graphWidth, graphHeight); 
	    
	    int count = lengthToDisp; // The number that will be displayed
	    
	    // Draw horizontal lines
	    g.setColor(Color.gray);
	    double range = curMaxY - curMinY; // data value range
	    double graphRange = 0;
	    if (range % 10 == 0) {
		graphRange = range;
	    } else {
		graphRange = (10 * (range/10 + 1));
	    }
	    int iter = (int)(graphRange / 10); // Number of lines to draw
	    float increment = 0;
	    if (iter != 0) {
		increment = (float)graphHeight / (float)iter;
	    }

	    for (int i=1 ; i <= iter ; i++) {
		int point1X = boundaryX + 1 + (graphWidth); 
		int point1Y = (int)((float)boundaryY + (float)graphHeight - (float)i * increment);
		int point2X = boundaryX - 1 + graphWidth + (graphWidth);
		int point2Y = (int)((float)boundaryY + (float)graphHeight - (float)i * increment);
		g.drawLine(point1X, point1Y, point2X, point2Y);
	    }
	    
	    // Draw the PredictionLine
	    g.setColor(Color.blue);
	    for(int i = 1 ; i < count ; i++) {
		int point1X = calcXpos(i-1, count) + (graphWidth);
		int point1Y = calcYpos(prdLineDispVals[i-1]);
		int point2X = calcXpos(i, count) + (graphWidth);
		int point2Y = calcYpos(prdLineDispVals[i]);
		g.fillOval(point1X-1, point1Y-1, 3, 3); 
		g.drawLine(point1X, point1Y, point2X, point2Y);
	    }
	    
	    // Draw errorbars
	    if (errorBars && predictionLine) {
		g.setColor(Color.green);
		// Top error bar
		for(int i = 1 ; i < count ; i++) {
		    int point1X = calcXpos(i-1, count) + (graphWidth);
		    int point1Y = calcYpos(uprPrdLineDispVals[i-1]);
		    int point2X = calcXpos(i, count) + (graphWidth);
		    int point2Y = calcYpos(uprPrdLineDispVals[i]);
		    g.fillOval(point1X-1, point1Y-1, 3, 3); 
		    g.drawLine(point1X, point1Y, point2X, point2Y);
		}
		// Bottom error bar
		for(int i = 1 ; i < count ; i++) {
		    int point1X = calcXpos(i-1, count) + (graphWidth);
		    int point1Y = calcYpos(lwrPrdLineDispVals[i-1]);
		    int point2X = calcXpos(i, count) + (graphWidth);
		    int point2Y = calcYpos(lwrPrdLineDispVals[i]);
		    g.fillOval(point1X-1, point1Y-1, 3, 3); 
		    g.drawLine(point1X, point1Y, point2X, point2Y);
		}
	    }
	}
    }

    // Terminates program is window is closed
    public class ExitListener extends WindowAdapter {
	public void windowClosing(WindowEvent event) {
	    System.exit(0);
	}
    }

    // Main is only used for the command line version
    public static void main(String[] args) {

	if ((args.length == 0) || (args.length > 4)) {
	    System.out.println("Usage: LineGraph loadbuffer predictionbuffer [callrateinms]");
	} else if (args.length == 1) {
	    //System.out.println("Standard"); 
	    LineGraph lg = new LineGraph(args[0]);
	} else if (args.length == 2) {
	    //System.out.println("Prediction");
	    LineGraph lg = new LineGraph(args[0], args[1]);
	} else {
	    System.out.println("Unknown Error");
	}
    }
};


  
