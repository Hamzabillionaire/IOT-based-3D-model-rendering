// import UDP library
import hypermedia.net.*;
import java.io.FileWriter;
//import java.io.IOException; 
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;

float roll, pitch,yaw;
static int i=0;
UDP udp;  // define the UDP object

/**
 * init
 */
void setup() {
  size (1366 , 768, P3D);
  // create a new datagram connection on port 4210
  // and wait for incomming message
  udp = new UDP( this, 4210 );
  //udp.log( true );     // <-- printout the connection activity
  udp.listen( true );
  loop();
  
  

}

//process events
void draw() {
  translate(width/2, height/2, 0);
  background(125);
  textSize(22);
  text("Roll: " + int(roll) + ", Pitch: " + int(pitch) + ", Yaw: " + int(yaw), -100, 265);
  // Rotate the object
  rotateX(radians(pitch));
  rotateZ(radians(roll));
  rotateY(radians(yaw));
  
  // 3D 0bject
  textSize(30);  
  fill(0, 76, 153);
  box (386, 40, 200); // Draw box
  textSize(25);
  fill(255, 255, 255);
  text("Model", -183, 10, 101);
}

void loop() {
  String ip       = "192.168.43.118";  // the remote IP address
  int port        = 4210;    // the destination port
  
  // formats the message for Pd
  String message = "This is simple Message from Computer.";
  // send the message
  udp.send( message, ip, port );
  
}

/** 
 * on key pressed event:
 * send the current key value over the network
 */
//void keyPressed() {
    
//    String message  = str( key );  // the message to send
//    String ip       = "192.168.43.118";  // the remote IP address
//    int port        = 4210;    // the destination port
    
//    // formats the message for Pd
//    message = "This is simple Message from Computer.";
//    // send the message
//    udp.send( message, ip, port );
    
//}

/**
 * To perform any action on datagram reception, you need to implement this 
 * handler in your code. This method will be automatically called by the UDP 
 * object each time he receive a nonnull message.
 * By default, this method have just one argument (the received message as 
 * byte[] array), but in addition, two arguments (representing in order the 
 * sender IP address and his port) can be set like below.
 */
// void receive( byte[] data ) {       // <-- default handler
void receive( byte[] data, String ip, int port ) {  // <-- extended handler
  
  
  // get the "real" message =
  // forget the ";\n" at the end <-- !!! only for a communication with Pd !!!
  data = subset(data, 0, data.length-2);
  String message = new String( data );
  
  String items[] = split(message, ',');
    if (items.length > 1) {
      //--- Roll,Pitch in degrees
      roll = float(items[0]);
     // println("roll: " + roll);
      pitch = float(items[1]);
      //println("pitch: " + pitch);
      yaw = float(items[2]);
      //println("Yaw: " + yaw);
    }
  
  // print the result
  println( "receive: \""+message+"\" from "+ip+" on port "+port );
  
  loop();
 /* try {
      FileWriter myWriter = new FileWriter("data_store_sensor.txt");
      myWriter.append("Roll: " + roll);
      myWriter.append("   Pitch: " + pitch);
      myWriter.append("     Yaw: " + yaw);
      //myWriter.close();
      System.out.println("Successfully wrote to the file.");
      myWriter.close();
    } catch (IOException e) {
      System.out.println("An error occurred.");
      e.printStackTrace();
    }*/
    
       try { 
             
            // Open given file in append mode. 
            BufferedWriter out = new BufferedWriter(new FileWriter("data_store_sensor.txt", true)); 
             // println(" ");     
            out.write("   No:" + i);
            i++;
            out.write("   Roll: \n" + roll);
            out.write( "  Pitch:\n " + pitch);
            out.write("   Yaw: \n" + yaw);
            out.newLine();
            
            out.close(); 
        } 
        catch (IOException e) { 
            System.out.println("exception occoured" + e); 
        } 
}
