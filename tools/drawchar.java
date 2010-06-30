import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.*;
import org.eclipse.swt.widgets.*;

public class drawchar {


	public static int horizontal = 6;
	public static int vertical = 8;
	
	static boolean[][] bit = new boolean [horizontal] [vertical];

	
	public static void main(String[] args) {

		Display display = new Display();
		Shell shell = new Shell(display);
		shell.setText("CharPainter");
		shell.setLayout(new GridLayout(2, false));

		Group group1 = new Group(shell, SWT.SHADOW_IN);
		group1.setText("Paint the char");
		group1.setLayout(new GridLayout(6, true));

		Group group2 = new Group(shell, SWT.SHADOW_IN);
		group2.setText("Result");

		Button[] but = new Button[horizontal * vertical];

		Text result = new Text(group2, SWT.MULTI | SWT.BORDER);
		result.setBounds(5, 30, 300, 30);


		for (int i = 0; i < horizontal * vertical; i++) {
			but[i] = new Button(group1, SWT.CHECK);
			but[i].addListener(SWT.Selection, new SelectionListener(i, result, bit));
		}

		shell.pack();
		shell.open();
		while (!shell.isDisposed()) {
			if (!display.readAndDispatch()) {
				display.sleep();
			}
		}
		display.dispose();
	}
}
