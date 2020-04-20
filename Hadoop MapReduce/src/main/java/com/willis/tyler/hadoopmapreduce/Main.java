package com.willis.tyler.hadoopmapreduce;

import java.io.IOException;
import java.util.Date;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.FileInputFormat;
import org.apache.hadoop.mapred.FileOutputFormat;
import org.apache.hadoop.mapred.JobClient;
import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.mapred.TextInputFormat;
import org.apache.hadoop.mapred.TextOutputFormat;

public class Main {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        try {
            long start = new Date().getTime();

            JobConf conf = new JobConf(Main.class);

            conf.setJobName("Conf");
            conf.setOutputKeyClass(Text.class);
            conf.setOutputValueClass(IntWritable.class);
            conf.setMapperClass(Map1.class);
            conf.setReducerClass(Reduce1.class);
            conf.setInputFormat(TextInputFormat.class);
            conf.setOutputFormat(TextOutputFormat.class);
            FileInputFormat.setInputPaths(conf, new Path(args[0]));
            FileOutputFormat.setOutputPath(conf, new Path(args[1]));
            JobClient.runJob(conf);

            long endStepOne = new Date().getTime();

            System.out.println("Job Step One took " + (endStepOne - start) + " milliseconds");

//            JobConf confTwo = new JobConf(Conf.class);
//
//            confTwo.setJobName("Conf step two");
//            confTwo.setOutputKeyClass(Text.class);
//            confTwo.setOutputValueClass(Text.class);
//            confTwo.setMapperClass(Map2.class);
//            confTwo.setReducerClass(Reduce2.class);
//            confTwo.setInputFormat(TextInputFormat.class);
//            confTwo.setOutputFormat(TextOutputFormat.class);
//            FileInputFormat.setInputPaths(confTwo, new Path(args[2]));
//            FileOutputFormat.setOutputPath(confTwo, new Path(args[3]));
//            JobClient.runJob(confTwo);
//
//            long end = new Date().getTime();
//            System.out.println("Job took " + (end - start) + " milliseconds");
        } catch (IOException ex) {
            Logger.getLogger(Main.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

}
