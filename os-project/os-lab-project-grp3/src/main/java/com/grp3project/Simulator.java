package com.grp3project;

import java.util.ArrayList;
import java.util.Iterator;

import javafx.animation.KeyFrame;
import javafx.animation.Timeline;
import javafx.beans.property.BooleanProperty;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.util.Duration;

public class Simulator {
    private Memory memory;
    private ArrayList<Process> processList;
    private ArrayList<Process> finishedProcesses;
    private ArrayList<Process> readyQueue;
    private int time;
    private int numOfFinishedProcesses;
    private Timeline timeline;
    private int coalesceInterval;
    private int compactionInterval;

    private final BooleanProperty running = new SimpleBooleanProperty(false);

    public Simulator(int size, int coalesceInterval, int compactionInterval) {
        this.memory = new Memory(size);
        this.processList = new ArrayList<Process>();
        this.finishedProcesses = new ArrayList<Process>();
        this.readyQueue = new ArrayList<Process>();
        this.coalesceInterval = coalesceInterval;
        this.compactionInterval = compactionInterval;
        this.numOfFinishedProcesses = 0;
        this.time = 0;
    }

    public void run() {

        // we tick every second for the timer while there is still an unfinished process
        timeline = new Timeline(new KeyFrame(Duration.seconds(1), event -> {
            if (numOfFinishedProcesses >= processList.size()) {
                stopSimulator();
                return;
            }
            
            updateRunningProcesses();
            checkFinishedProcesses(time);
            if (checkCoalesce(time)) {
                System.out.println("Coalesced the Free List");
            }
            if (checkCompaction(time)) {
                System.out.println("Compacted the Memory Array");
            }
            checkArrival(time);
            checkReadyQueue();
            System.out.println("Time: " + time);
            printRunningProcesses();
            printReadyQueue();
            printFreeList();
            time++;
        }));
        timeline.setCycleCount(Timeline.INDEFINITE);
        timeline.play();
        running.set(true);
    }

    // we check for arriving processes
    public void checkArrival(int time) {
        for (Process process : processList) {
            if (process.getArrivalTime() <= time && !readyQueue.contains(process) && !memory.getMemArray().contains(process) && !process.isFinished()) {
                readyQueue.add(process);
            }
        }
    }


    // we check for finished processes and remove them
    public void checkFinishedProcesses(int time) {
        ArrayList<Process> toRemove = new ArrayList<>();
        for (Process process : memory.getMemArray()) {
            if (process.getRemainingTime() <= 0 && !process.isFinished()) {
                process.setFinished(true);

                // compute for process stats
                process.setTurnaroundTime(time - process.getArrivalTime());
                process.setResponseTime(process.getTurnaroundTime() - process.getBurstTime());
                process.setWaitingTime(process.getTurnaroundTime() - process.getBurstTime());
                finishedProcesses.add(process);
                toRemove.add(process);

                numOfFinishedProcesses++;
            }
        }

        for (Process p : toRemove) {
            memory.removeProcess(p);
        }
    }


    // we check for processes in the ready queue and add them to the memory array
    // since there might be processes that are now able to be inserted into the memory array 
    // after coalescing or compacting
    public void checkReadyQueue() {
        Iterator<Process> iterator = readyQueue.iterator();
        while (iterator.hasNext()) {
            Process process = iterator.next();
            if (memory.addProcess(process)) {
                iterator.remove();
            }
        }
    }

    // we tick the remaining time of the processes
    public void updateRunningProcesses() {
        for (Process process : memory.getMemArray()) {
            if (!process.isFinished()) {
                process.setRemainingTime(process.getRemainingTime() - 1);
            }
        }
    }


    // we check if it is time to coalesce
    public boolean checkCoalesce(double time) {
        if (time > 0 && time % coalesceInterval == 0) {
            memory.coalesce();
            return true;
        }
        return false;
    }

    // we check if it is time to compact
    public boolean checkCompaction(double time) {
        if (time > 0 && time % compactionInterval == 0) {
            memory.compaction();
            return true;
        }
        return false;
    }

    // pausing the simulator
    public void pauseSimulator() {
        if (timeline != null) {
            timeline.pause();
            running.set(false);
        }
    }

    // resuming the simulator
    public void resumeSimulator() {
        if (timeline != null) {
            timeline.play();
            running.set(true);
        }
    }

    // stopping the simulator
    public void stopSimulator() {
        if (timeline != null) {
            timeline.stop();
        }
        running.set(false);
    }

    public boolean isRunning() {
        return running.get();
    }

    public BooleanProperty runningProperty() {
        return running;
    }

    // print functions just for debugging
    private void printRunningProcesses() {
        System.out.println("Running processes:");
        for (Process process : memory.getMemArray()) {
            System.out.println(process.getPid());
        }
    }

    private void printReadyQueue() {
        System.out.println("Ready queue:");
        for (Process process : readyQueue) {
            System.out.println(process.getPid());
        }
    }

    private void printFreeList() {
        System.out.println("Free list:");
        for (FreeBlock block : memory.getFreeList().getFreeList()) {
            System.out.println(block.getStartAddress() + " - " + block.getEndAddress());
        }
    }

    // getter and setter functions
    public Memory getMemory() {
        return memory;
    }

    public ArrayList<Process> getProcessList() {
        return processList;
    }

    public int getCurrentTime() {
        return time;
    }

    public ArrayList<Process> getReadyQueue() {
        return readyQueue;
    }

    public ArrayList<Process> getFinishedProcesses() {
        return finishedProcesses;
    }
}
