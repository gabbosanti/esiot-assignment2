package esiot.dru;

/**
 * 
 * An example of Dashboard for the DRU subsystem
 * 
 *  Entry point --> Runs the application
 * 
 */
class DashboardLauncher   {

	static DashboardView view = null;
	static LogView log = null;
    static String portName = "COM3";

	public static void main(String[] args) throws Exception {	
		/*
		if (args.length != 1){
			System.err.println("Args: <serial port>");
			System.exit(1);
		}*/

		view = new DashboardView();
		log = new LogView();
		
		DashboardController contr = new DashboardController(portName,view,log);
		view.registerController(contr);
		
		view.display();
		log.display();
	}
}