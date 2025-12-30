package esiot.dru;

import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.Color;

import javax.swing.*;

class DashboardView extends JFrame implements ActionListener  {

	private JButton btnTakeOff;
	private JButton btnLanding;
	private JLabel lblDroneState;
	private JLabel lblHangarState;
	private JLabel lblDistance;
	
	private DashboardController controller;	
	
	public DashboardView(){
		super("Drone Remote Unit (DRU)");
		setSize(400,300);
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		JPanel mainPanel = new JPanel();
		mainPanel.setLayout(new BoxLayout(mainPanel, BoxLayout.Y_AXIS));
		
		//Monitor Panel
		JPanel monitorPanel = new JPanel(new GridLayout(3,2 ));
		monitorPanel.setBorder(BorderFactory.createTitledBorder("Status Monitor"));
		
		lblDroneState = new JLabel("IDLE");
		lblHangarState = new JLabel("NORMAL");
		lblDistance= new JLabel("---");

		monitorPanel.add(new JLabel("Drone State:"));
		monitorPanel.add(lblDroneState);
		monitorPanel.add(new JLabel("Hangar State:"));
		monitorPanel.add(lblHangarState);
		monitorPanel.add(new JLabel("Distance:"));
		monitorPanel.add(lblDistance);

		//command Panel
		JPanel controlPanel = new JPanel();
		btnTakeOff = new JButton("Take-off");
		btnLanding = new JButton("Landing");

		btnTakeOff.addActionListener(this);
		btnLanding.addActionListener(this);
		controlPanel.add(btnTakeOff);
		controlPanel.add(btnLanding);

		mainPanel.add(monitorPanel);
		mainPanel.add(controlPanel);
		this.getContentPane().add(mainPanel);
	}	
	
	public void display() {
		SwingUtilities.invokeLater(() -> {
			this.setVisible(true);
		});
	}

	public void registerController(DashboardController contr){
		this.controller = contr;
	}

	public void setDroneState( String state){
		SwingUtilities.invokeLater(() -> lblDroneState.setText(state));
	}

	public void setHangarState(String state, Color color){
		SwingUtilities.invokeLater(() -> {
			lblHangarState.setText(state);
			lblHangarState.setForeground(color);
		});
	}

	public void setDistance(String distance){
		SwingUtilities.invokeLater(() -> lblDistance.setText(distance + " m"));
	}

		@Override
		public void actionPerformed(ActionEvent ev){
			if (ev.getSource() == btnTakeOff) {
            controller.notifyTakeOff();
        } else if (ev.getSource() == btnLanding) {
            controller.notifyLanding();
        }
		}
}

	