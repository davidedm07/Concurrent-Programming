package source.HWJ.es3;

import java.util.concurrent.RecursiveTask;

import source.HWJ.base.Node;
import source.HWJ.base.Processor;
/**
 * This class implements the java fork/join framework
 * For the correct functioning the method needs to know the
 * depth of the tree passed as input
 * @author davide
 *
 */
public class ForkJoinTask extends RecursiveTask<Integer> {
	private Node root;
	private int depth;
	private long numberOfNodes; 
	private static final long serialVersionUID = 1L;
	//threshold that gives the best speedUp
	// checked by sperimental tests
	public static final long THRESHOLD = 7;


	public ForkJoinTask(Node root) {
		// TODO Auto-generated constructor stub
		this.root = root;
	}

	public ForkJoinTask(Node root, int depth) {
		// TODO Auto-generated constructor stub
		this.root = root;
		this.depth = depth;
		this.numberOfNodes =(long)( Math.pow(2, depth)-1);
	}

	@Override
	protected Integer compute() {
		if (this.depth <= THRESHOLD ) {
			return recursiveComputation(this.root);
		}
		else {
			ForkJoinTask sx  = new ForkJoinTask(this.root.getSx(),this.depth-1);
			ForkJoinTask dx = new ForkJoinTask(this.root.getDx(),this.depth-1);
			sx.fork();
			int dxAns = dx.compute();
			int sxAns = sx.join();
			return dxAns + sxAns + new Processor(1000).onerousFunction(root.getValue());
		}
	}
		
	private int recursiveComputation(Node node) {
		if (node == null)
			return 0;
		else
			return recursiveComputation(node.getSx()) +
					recursiveComputation(node.getDx()) +
					new Processor(1000).onerousFunction(node.getValue());
	}
	
	
	public int getDepth() {
		return depth;
	}

	public void setDepth(int depth) {
		this.depth = depth;
	}

	public long getNumberOfNodes() {
		return numberOfNodes;
	}

	public void setNumberOfNodes(long numberOfNodes) {
		this.numberOfNodes = numberOfNodes;
	}


}
