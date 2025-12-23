package esiot.sweeping_system;
import java.awt.Color;

public class MonitoringAgent extends Thread {

	SerialCommChannel channel;
	DashboardView view;
	LogView logger;
	
	//static final String APP_PREFIX 	=  "sw:";
	//static final String LOG_PREFIX 	=  "lo:";
	
	// static final String MSG_STATE 		= "st:";
	// static final String[] stateNames = {"Available", "Full", "Maintenance"}; 
	// static final int AVAILABLE = 0;
	// static final int FULL = 1;
	// static final int IN_MAINTENANCE = 2;
	
	
	public MonitoringAgent(SerialCommChannel channel, DashboardView view, LogView log) throws Exception {
		this.view = view;
		this.logger = log;
		this.channel = channel;
	}
	
	public void run(){
		// boolean inMaintenance = false;
		// boolean isFull = false;
		while (true){
			try {
				String msg = channel.receiveMsg();
				// logger.log("new msg: "+msg);				
				if (logger != null) logger.log("Receiving: " + msg);
				
				if (msg.contains("[CHT] IDLE")){
					view.setDroneState("RESTING (INSIDE)");
					view.setHangarState("NORMAL", Color.BLACK);
				} else if (msg.contains("[CHT] TAKEOFF")){
					view.setDroneState("TAKING OFF...");
				} else if (msg.contains("[CHT] DRONE_OUT")){
					view.setDroneState("OPERATING (OUTSIDE)");
				}else if (msg.contains("[CHT] LANDING")){
					view.setDroneState("LANDING...");
				}else if ( msg.contains("[CHT] ALARM")){
					view.setHangarState("ALARM!!", Color.RED);
				}else if (msg.contains("DISTANZA: ")){
					String[] parts = msg.split(":");
					if (parts.length > 1){
						view.setDistance(parts[1].trim());
					}
				}
			} catch (Exception ex){
				ex.printStackTrace();
			}
		}
	}
}
					