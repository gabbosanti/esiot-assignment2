package esiot.dru;

import javax.swing.*;

//This class contains all the LOG from the serial line

class LogView extends JFrame  {

	private JTextArea log;

	public LogView(){
		super("Log ");
		setSize(600,600);
		this.setResizable(false);
		JPanel mainPanel = new JPanel();
		log = new JTextArea(30,40);
		log.setEditable(false);
		JScrollPane scrol = new JScrollPane(log);
		mainPanel.add(scrol);
		this.getContentPane().add(mainPanel);
	}
	
	public void log(String msg){
		SwingUtilities.invokeLater(() -> {
			log.append( msg +"\n");
		});
	}
	
	public void display() {
		SwingUtilities.invokeLater(() -> {
			this.setVisible(true);
		});
	}
}
