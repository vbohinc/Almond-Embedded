import org.eclipse.swt.widgets.*;

public class SelectionListener implements Listener {
	int number;
	Text result;
	boolean status;
	boolean[][] bit;

	public SelectionListener(int number, Text result, boolean[][] bit) {
		this.number = number;
		this.result = result;
		this.bit = bit;
	}

	@Override
	public void handleEvent(Event e) {

		this.bit[number % 6][number / 6] = !this.bit[number % 6][number / 6];
		drawResult(this.bit, this.result);
	}

	public void drawResult(boolean[][] bit, Text result) {
		Integer res = 0;

		String compound = "";

		compound += "{ ";


		for (int i1 = 0; i1 < bit.length; i1++) {
			
			compound += "0x";

			for (int i2 = 0; i2 < bit[i1].length; i2++) {
				
				if (bit[i1][i2]) {
					res = res + (int) Math.pow(2, (bit[i1].length-1)-i2);
				}
				
			}

			compound += Integer.toHexString(res);
			if(i1!=bit.length-1) {
			compound += ", ";
			} else { compound += " ";}
			res = 0;

		}

		compound += "}\n";

		result.setText(compound);
	}

}
