To make changes to the code, first, install Netbeans 8.0. This will be the IDE that will be used to make changes.  
Open the project in netbeans, it will show a lot of errors.  
Open the `pom.xml` file, and set the `sys.path` property to the location of the proejct folder.  
After that, run a clean and build.  

Now you will be able to make changes to the project.  

After you are done making changes, pull your changes onto the hadoop cluster master, and run the `./build.sh` script. 
This will create the jar file that will be used to run the project. See the `README.md` file for instructions on how to run. 