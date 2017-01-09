package source.HWJ.base;

public class ImplementedNode implements Node {

	private Node sx; // left child of the current node
	private Node dx; // right child of the current node
	private int value; // value of the current node

	public ImplementedNode () {
		//this.value = (int)(Math.random()*10);
		this.value = 1;
		this.sx = null;
		this.dx = null;
	}
	
	public ImplementedNode(int value) {
		this.value = value;
		this.sx = null;
		this.dx = null;
	}
	
	@Override
	public Node getSx() {
		return this.sx;
	}

	@Override
	public Node getDx() {
		return this.dx;
	}

	@Override
	public int getValue() {
		return this.value;
	}

	public void setSx(Node x) {
		this.sx = x;
	}

	public void setDx(Node x) {
		this.dx = x;
	}

	@Override
	public String toString() {
		String result = "Node Value = " + this.getValue();
		if (this.getDx()!= null)
			result += " right son = " + this.getDx().getValue();
		if (this.getSx()!= null)
			result += " left son = " + this.getSx().getValue();
		return result;
	}
}
