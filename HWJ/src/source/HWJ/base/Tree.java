package source.HWJ.base;

public class Tree {
	public static Node buildTree(int numNodes) {
		ImplementedNode root;
		int depth = (int) (Math.log(numNodes+1)/Math.log(2));
		root = (ImplementedNode) generateTree(depth-1);
		return root;
	}
	
	public static Node buildTreeDepth(int depth) {
		return generateTree(depth-1);
	}

	private static Node generateTree (int depth) {
		if (depth < 0) 
			return null;
		ImplementedNode currentNode = new ImplementedNode();
		if (depth > 0) {
			currentNode.setSx(generateTree(depth-1));
			currentNode.setDx(generateTree(depth-1));
		}
		return currentNode;
	}
	
	/**
	 * this method creates a tree assigning as value for each node its depth
	 * @param depth
	 * @param value
	 * @return the root node 
	 */
	private static Node generateTree (int depth,int value) {
		if (depth < 0) 
			return null;
		ImplementedNode currentNode = new ImplementedNode(value);
		if (depth >= 0) {
			value++;
			currentNode.setSx(generateTree(depth-1,value));
			currentNode.setDx(generateTree(depth-1,value));
		}
		return currentNode;
	}
	
	public static int numberOfNodes(Node node) {
		int cont;
		if (node!= null)
			cont= 1;
		else 
			return 0;
			
		Node dx = node.getDx();
		Node sx = node.getSx();
		cont += numberOfNodes(dx);
		cont += numberOfNodes(sx);
		return cont;	
	}
	
	public static int sumOfNodes(Node node) {
		int sum;
		if (node!= null)
			sum= node.getValue();
		else 
			return 0;
			
		Node dx = node.getDx();
		Node sx = node.getSx();
		sum += sumOfNodes(dx);
		sum += sumOfNodes(sx);
		return sum;	
	}
	
	public static Node generateTreeNumberOfNodes(int depth) {
		Node root = null;
		root =  generateTree(depth-1,1);
		return root;
	}


}
