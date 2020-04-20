javac -classpath /usr/lib/hadoop/client/hadoop-common-2.8.5-amzn-3.jar:/usr/lib/hadoop-mapreduce/hadoop-mapreduce-client-core-2.8.5-amzn-3.jar -d build @files.jtxt
jar -cfv hadoopmapreduce.jar -C ./build .


