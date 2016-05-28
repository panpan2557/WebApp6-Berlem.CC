import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.StringTokenizer;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

public class TwoLinksNode {

	public static class NodeMapper extends Mapper<Object, Text, Text, Text> {

		private final static Text mvalue = new Text();
        private Text mkey = new Text();

		@Override
		public void map(Object key, Text value, Context context) throws IOException, InterruptedException {
			StringTokenizer itr = new StringTokenizer(value.toString());

			String node = itr.nextToken();
			String first = itr.nextToken();

			mkey.set(node);
			mvalue.set("> " + first);
			context.write(mkey, mvalue);

			mkey.set(first);
			mvalue.set("< " + node);
			context.write(mkey, mvalue);
		}
	}

	public static class NodeCombiner extends Reducer<Text, Text, Text, Text> {

		private Text cvalue = new Text();
		private Text ckey = new Text();

		@Override
		public void reduce(Text key, Iterable<Text> values, Context context) throws IOException, InterruptedException {
			List<String> nodes = new ArrayList<String>();
			List<String> seconds = new ArrayList<String>();

			for (Text val : values) {
				String[] first = val.toString().split(" ");
				if (first[0].equals("<")) {
					nodes.add(first[1]);
				} else if (first[0].equals(">")) {
					seconds.add(first[1]);
				}
			}

			for (String node : nodes) {
				for (String sec: seconds) {
					if (!sec.equals(node)) {
						cvalue.set(sec);
						ckey.set(node);
						context.write(ckey, cvalue);											
					}
				}
			}
		}
	}

	public static class NodeReducer extends Reducer<Text, Text, Text, Text> {

		private Text result = new Text();

		@Override
		public void reduce(Text key, Iterable<Text> values, Context context) throws IOException, InterruptedException {

			String seconds = "";
			for (Text val : values) {
				seconds += (val.toString());
			}

			result.set(seconds);
			context.write(key, result);
		}
	}

	public static void main(String[] args) throws Exception {
		Configuration conf = new Configuration();
		Job job = Job.getInstance(conf, "two-links node count");
		System.out.println("TwoLinksNode count");
		job.setJarByClass(TwoLinksNode.class);
		job.setMapperClass(NodeMapper.class);
		job.setCombinerClass(NodeCombiner.class);
		job.setReducerClass(NodeReducer.class);
		job.setOutputKeyClass(Text.class);
		job.setOutputValueClass(Text.class);
		FileInputFormat.addInputPath(job, new Path(args[0]));
		FileOutputFormat.setOutputPath(job, new Path(args[1]));
		System.out.println("Max split " + FileInputFormat.getMaxSplitSize(job));
		System.out.println("Min split " + FileInputFormat.getMinSplitSize(job));
		System.exit(job.waitForCompletion(true) ? 0 : 1);
	}
}