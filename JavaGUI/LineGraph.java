// LineGraph.java   Written by Brandon O'Bryant   Last Update: 3/10/01
// See LineGraph.txt for an explanation of LineGraph.java

import java.awt.event.*;
import javax.swing.*;
import java.awt.*;

public class LineGraph extends JPanel
{
    // JNI functions used to connect to the RPS system
    public native void callLoadBufferClient(double[] dArray, int num, java.lang.String route);
    public native void callPredBufferClient(double[] dArray1,double[] dArray2, int num, java.lang.String route);
    public native void callMeasureBufferClient(double[] dArray, int num, java.lang.String route); 

    static
    {
        System.loadLibrary("LineGraph");
    }

    // Variables

    static int MEASURE=0;
    static int LOAD=1;
    static int PRED=2;
    static int MEASURE_PRED=3;
    static int LOAD_PRED=4;

    int config;  
    double rate;
    String measurepath, predpath; //Routes to the standard and prediction targets
    int valsCount; // Number of datapoints in standardLineVals
    int lengthToDispLeft; // The maximum to display,
    int lengthToDispRight; // The maximum to display,
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
    JTextField numPointsLeft, numPointsRight, maxY, minY, Rate; // Used in the graph control panel that is displayed below the graph


    // Connects to acquire load measurements as well as prediction values  
    public LineGraph(int conf, double r, String route1, int num1, String route2, int num2, boolean ci) {
	config=conf;
	rate=r;
	init(route1, num1, route2, num2);

	if (config==MEASURE || config==LOAD || config==MEASURE_PRED || config==LOAD_PRED) { 
	    standardLine=true;
	} else {
	    standardLine=false;
	}
	if (config==PRED || config==MEASURE_PRED || config==LOAD_PRED) { 
	    predictionLine=true;
	    errorBars=ci;
	} else {
	    predictionLine=false;
	    errorBars=false;
	}	    
	
	
	JFrame theWindow = new JFrame("RPS Java GUI ("+route1+" / "+route2+")");
	theWindow.setSize(600,500);
	theWindow.addWindowListener(new ExitListener());
	Container content = theWindow.getContentPane();
	content.add(this, BorderLayout.CENTER);
	    
	// The control panel below the graph
	JPanel graphInfo = new JPanel(new GridLayout(2,4));
	numPointsLeft.setText(lengthToDispLeft + "");
	numPointsRight.setText(lengthToDispRight + "");
	graphInfo.add(new JLabel("Left points:"));
	graphInfo.add(numPointsLeft);
	graphInfo.add(new JLabel("Right points:"));
	graphInfo.add(numPointsRight);
	graphInfo.add(new JLabel()); graphInfo.add(new JLabel());
	graphInfo.add(new JLabel("Max"));
	graphInfo.add(maxY);
	graphInfo.add(new JLabel("Min"));
	graphInfo.add(minY);
	graphInfo.add(new JLabel("Rate"));
	graphInfo.add(Rate);
	content.add(graphInfo, BorderLayout.SOUTH);
	
	if ((standardLine && !predictionLine) || (!standardLine && predictionLine)) {
	    setSize(windowWidth, windowHeight);
	} else if (standardLine && predictionLine) {
	    windowWidth = windowWidth*2 - boundaryX*2;
	    setSize(windowWidth, windowHeight);
	} else {
	    System.out.println("No graph to display.");
	}
	theWindow.setVisible(true);
	beginTimer();
	
    }
    
    // Initialize all values
    private void init(String route1, int num1, String route2, int num2) {
	measurepath = new String(route1);
	predpath = new String(route2);
	numPointsLeft = new JTextField(3);
	numPointsRight = new JTextField(3);
	maxY = new JTextField(3);
	minY = new JTextField(3);
	Rate = new JTextField(3);
	Rate.setText(rate+"");
	valsCount = 0;
	lengthToDispLeft = num1;
	lengthToDispRight = num2;
	stdLineDispVals = new double[lengthToDispLeft];
	prdLineDispVals = new double[lengthToDispRight];
	uprPrdLineDispVals = new double[lengthToDispRight];
	lwrPrdLineDispVals = new double[lengthToDispRight];
	curMinY = 0; curMaxY = 1;
	graphWidth = 480; graphHeight = 400;
	windowWidth = 600; windowHeight = 500;
	boundaryX = 60;
	boundaryY = 50;
	standardLineVals = new double[lengthToDispLeft];
	predictionLineVals = new double[lengthToDispRight];
	predictionLineErrors = new double[lengthToDispRight];
	uprPredictionLineVals = new double[lengthToDispRight];
	lwrPredictionLineVals = new double[lengthToDispRight];
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
	timer = new javax.swing.Timer(new java.lang.Double(1000/rate).intValue(), new ActionListener() {
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
	    if (config==MEASURE || config==MEASURE_PRED) { 
		callMeasureBufferClient(standardLineVals, lengthToDispLeft, measurepath);
	    } else {
		callLoadBufferClient(standardLineVals,lengthToDispLeft,measurepath);
	    }
	}
	if (predictionLine) {
	    callPredBufferClient(predictionLineVals, predictionLineErrors, lengthToDispRight, predpath);
	}

	valsCount=standardLineVals.length;

	updateStandardDisp(); // Transfer data to display array
	updatePredictionDisp();  // Transfer data to display arrays
	setRange();
	repaint();
	
    }
    
    // Transfers values that will be displayed from standardLineVals to stdLineDispVals
    private void updateStandardDisp() {
	if (valsCount < lengthToDispLeft){
	    for (int i=0 ; i < valsCount ; i++) {
		stdLineDispVals[i] = standardLineVals[i];
	    }
	} else {
	    for (int i=0 ; i < lengthToDispLeft ; i++) {
		stdLineDispVals[i] = standardLineVals[valsCount-lengthToDispLeft+i];
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
	for (int i=0 ; i < lengthToDispRight ; i++) {
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
	for (int i = 0; i < lengthToDispLeft ; i++) {
	    if (stdLineDispVals[i] > max) {
		max = stdLineDispVals[i];
	    }
	}
	for (int i = 0; i < lengthToDispRight ; i++) {
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
	boundaryY = 0; //windowHeight;
	if (standardLine && predictionLine) {
	    graphWidth = (int)((float)windowWidth * .5);
	    boundaryX = 0; // windowWidth / 20;
	} else { 
	    graphWidth = (int)((float)windowWidth * 1.0);
	    boundaryX = 0 ; //windowWidth / 10;
	}
	graphHeight = (int)((float)windowHeight * 0.99);
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
	    if (valsCount < lengthToDispLeft) {
		count = valsCount;
	    } else {
		count = lengthToDispLeft;
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
	    int offx;
	    if (config==PRED) { 
		offx=0;
	    } else {
		offx=graphWidth;
	    }
	    g.setColor(Color.lightGray); // Prepare graph
	    g.fillRect(boundaryX+offx, boundaryY, graphWidth, graphHeight); 
	    g.setColor(Color.black); // Border
	    g.drawRect(boundaryX+offx, boundaryY, graphWidth, graphHeight); 
	    
	    int count = lengthToDispRight; // The number that will be displayed
	    
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
		int point1X = boundaryX + 1 + (offx); 
		int point1Y = (int)((float)boundaryY + (float)graphHeight - (float)i * increment);
		int point2X = boundaryX - 1 + graphWidth + (offx);
		int point2Y = (int)((float)boundaryY + (float)graphHeight - (float)i * increment);
		g.drawLine(point1X, point1Y, point2X, point2Y);
	    }
	    
	    // Draw the PredictionLine
	    g.setColor(Color.red);
	    for(int i = 1 ; i < count ; i++) {
		int point1X = calcXpos(i-1, count) + (offx);
		int point1Y = calcYpos(prdLineDispVals[i-1]);
		int point2X = calcXpos(i, count) + (offx);
		int point2Y = calcYpos(prdLineDispVals[i]);
		g.fillOval(point1X-1, point1Y-1, 3, 3); 
		g.drawLine(point1X, point1Y, point2X, point2Y);
	    }
	    
	    // Draw errorbars
	    if (errorBars && predictionLine) {
		g.setColor(Color.blue);
		// Top error bar
		for(int i = 1 ; i < count ; i++) {
		    int point1X = calcXpos(i-1, count) + (offx);
		    int point1Y = calcYpos(uprPrdLineDispVals[i-1]);
		    int point2X = calcXpos(i, count) + (offx);
		    int point2Y = calcYpos(uprPrdLineDispVals[i]);
		    g.fillOval(point1X-1, point1Y-1, 3, 3); 
		    g.drawLine(point1X, point1Y, point2X, point2Y);
		}
		// Bottom error bar
		for(int i = 1 ; i < count ; i++) {
		    int point1X = calcXpos(i-1, count) + (offx);
		    int point1Y = calcYpos(lwrPrdLineDispVals[i-1]);
		    int point2X = calcXpos(i, count) + (offx);
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
	int conf;
	double rate;
	String ep1, ep2;
	int num1,num2;

	if ((args.length< 4) || (args.length > 7)) {
	    System.out.println("Usage: LineGraph config rate [endpoint1 num1] [endpoint2 num2 [conf]]");
	    System.exit(-1);
	}
	if (args[0].equalsIgnoreCase("l")) {
	    conf=LOAD;
	    if (args.length>4) {
		System.out.println("Incorrect config");
		System.exit(-1);
	    }
	} else if (args[0].equalsIgnoreCase("m")) {
	    conf=MEASURE;
	    if (args.length>4) {
		System.out.println("Incorrect config");
		System.exit(-1);
	    }
	} else if (args[0].equalsIgnoreCase("p")) {
	    conf=PRED;
	    if (args.length>5) {
		System.out.println("Incorrect config");
		System.exit(-1);
	    }
	} else if (args[0].equalsIgnoreCase("lp")) {
	    conf=LOAD_PRED;
	} else if (args[0].equalsIgnoreCase("mp")) {
	    conf=MEASURE_PRED;
	} else {
	    conf=LOAD;
	    System.out.println("Incorrect config");
	    System.exit(-1);
	}

	rate= new java.lang.Double(args[1]).doubleValue();
	ep1=args[2];
	num1= new java.lang.Integer(args[3]).intValue();

	if (conf==LOAD || conf==MEASURE || conf==PRED) {
	    ep2="";
	    num2=0;
	} else {
	    ep2=args[4];
	    num2= new java.lang.Integer(args[5]).intValue();
	}

	if (conf==PRED) { 
	    ep2=ep1;
	    ep1="";
	    num2=num1;
	    num1=0;
	}

	LineGraph lg = new LineGraph(conf,rate,ep1,num1,ep2,num2,args.length==7 || args.length==5);

    }
};


  
