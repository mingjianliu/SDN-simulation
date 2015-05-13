import java.io.*;
import java.util.*;
import java.text.*;
import java.math.*;
import java.util.regex.*;

public class Solution {

    public static void insertIntoSorted(int[] ar) {
        // Fill up this function  
    }


/* Tail starts here */
     public static void main(String[] args) {
        Scanner in = new Scanner(System.in);
        int s = in.nextInt();
        int[] ar = new int[s];
         for(int i=0;i<s;i++){
            ar[i]=in.nextInt(); 
         }
         printArray(ar);
    }


    private static void printArray(int[] ar) {
      System.out.println("Here is the output");
      for(int n: ar){
         System.out.print(n+" ");
      }
        System.out.println("");
   }


}
