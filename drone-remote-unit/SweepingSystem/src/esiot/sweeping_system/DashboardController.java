package esiot.sweeping_system;

public class DashboardController  {

	// static final String MSG_MAINTENANCE_DONE 	= "ok";
	// static final String MSG_DISCHARGE 			= "di";
	static final String CMD_ACTIVATE = "ACTIVATE";
	static final String CMD_OPEN = "OPEN";

	SerialCommChannel channel;
	DashboardView view;
	
	public DashboardController(String port, DashboardView view, LogView logger) throws Exception {
		this.view = view;
		
		channel = new SerialCommChannel(port, 9600);	

		new MonitoringAgent(channel,view,logger).start();
		
		System.out.println("Waiting Arduino...");		
		Thread.sleep(2000);		
	
	}

	public void notifyTakeOff(){
		channel.sendMsg(CMD_ACTIVATE);
	}
	public void notifyLanding(){
		channel.sendMsg(CMD_OPEN);
	}
	
	/*
	public void notifyMaintenanceDone() {
		  channel.sendMsg(MSG_MAINTENANCE_DONE);
	}

	public void notifyDischarge() {
		  channel.sendMsg(MSG_DISCHARGE);
	}*/

}
