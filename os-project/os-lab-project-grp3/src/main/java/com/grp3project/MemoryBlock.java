
package com.grp3project;
import javafx.scene.paint.Color;

// helper class for the visualization
public class MemoryBlock {
    final int startAddress;
    final int size;
    final String label;
    final Color color;

    MemoryBlock(int startAddress, int size, String label, Color color) {
        this.startAddress = startAddress;
        this.size = size;
        this.label = label;
        this.color = color;
    }
    
    int startAddress() { return startAddress; }
    int size() { return size; }
    String label() { return label; }
    Color color() { return color; }
}