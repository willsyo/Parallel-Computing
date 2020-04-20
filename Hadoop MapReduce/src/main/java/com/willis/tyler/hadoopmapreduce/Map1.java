package com.willis.tyler.hadoopmapreduce;

import java.io.IOException;
import java.util.HashSet;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.Mapper;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reporter;

/**
 *
 * @author Trevor Flynn {@literal <trevorflynn@liquidcrystalstudios.com>}
 */
public class Map1 extends MapReduceBase implements Mapper<LongWritable, Text, Text, IntWritable> {
    
    @Override
    public void map(LongWritable key, Text value, OutputCollector<Text, IntWritable> output, Reporter reporter) throws IOException {
        //Parse input
        String[] rawTokens = value.toString().split(" ");
        //Get unique CIDs
        HashSet<String> tokens = new HashSet<>();
        for (String t : rawTokens) {
            tokens.add(t);
        }
        
        //Convert to Text objects
        for (String token : tokens) {
            //Stores as <documentID> = 
            output.collect(new Text(token), new IntWritable(1));
        }
    }

}
