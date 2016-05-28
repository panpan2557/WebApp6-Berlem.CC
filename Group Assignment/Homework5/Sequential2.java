import java.io.*;
import java.nio.file.Files;
import java.util.List;
import java.util.Map;
import java.util.ArrayList;
import java.util.HashMap;

public class Sequential {
	private static String mapKey = "", linkKey1 = "", linkKey2 = "";
	private static Map<String, List<String>> list = new HashMap<String, List<String>>();
	private static Map<String, Integer> countList = new HashMap<String, Integer>();
	private static List<String> lines = new ArrayList<>();
	
	public static void main(String[] args) throws Exception{
//		File inputPath = new File(args[0]);
//		File outputFile = new File(args[1]);
		File inputPath = new File("/Users/patawat/Documents/Webapp/Homework5/src/web-Google.txt");
		File outputFile = new File("./" + "output" + "/output.txt");
		
		File directory = outputFile.getParentFile();
		if(null != directory){
			directory.mkdirs();
		}
		
		//for(File item: inputPath.listFiles()){
			System.out.println("Reading File...");
			List<String> lines = Files.readAllLines(inputPath.toPath());
			System.out.println("Read File Done...");
			System.out.println("Mapping...");
			map(lines);
			System.out.println("Map Done...");
		//}
		System.out.println("Writing Output File...");
		saveFile(outputFile.getPath(), lines);
		System.out.println("Write File Done...");
	}
	public static void map(List<String> inputs) {
		for (String line:inputs) {
			String[] node = line.split("\\s");
			if(!list.containsKey(node[0])){
				mapKey = node[0];
				String value = node[1];
				List<String> valueList = list.get(mapKey);
				if(valueList == null) {
					valueList = new ArrayList<String>();
				}
				valueList.add(value);
				list.put(mapKey, valueList);
			} else {
				String value = node[1];
				List<String> valueList = list.get(mapKey);
				valueList.add(value);
				list.replace(mapKey, valueList);
			}
		}
		twoLinks();
	}
	public static void twoLinks(){
		for(String pkey : list.keySet()){ // 1st key
			List<String> setSkey = list.get(pkey);
			List<String> count = new ArrayList<String>();
			for(String skey: setSkey) { // 2nd key
				if(!count.contains(skey)) {
					count.add(skey);
				}
				List<String> setTkey = list.get(skey);
				if(setTkey == null) {
					continue;
				}
				for(String tkey : setTkey) { // 3rd key
					if(!count.contains(tkey)) {
						count.add(tkey);
					}
				}
				
			}
			lines.add(pkey + ": " + count + "\n");
		}
		
	}
	public static void saveFile(String path, List<String> list) throws Exception {
		FileWriter writer = new FileWriter(path);
		for(String line: lines){
			writer.write(line);
		}
		writer.close();
	}
}
