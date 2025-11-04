package com.grp3project;

import javafx.animation.KeyFrame;
import javafx.animation.Timeline;
import javafx.beans.property.SimpleIntegerProperty;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.scene.control.Alert;
import javafx.scene.control.Label;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.TextField;
import javafx.scene.control.cell.TextFieldTableCell;
import javafx.scene.layout.Background;
import javafx.scene.layout.BackgroundFill;
import javafx.scene.layout.Border;
import javafx.scene.layout.BorderStroke;
import javafx.scene.layout.BorderStrokeStyle;
import javafx.scene.layout.BorderWidths;
import javafx.scene.layout.CornerRadii;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Pane;
import javafx.scene.paint.Color;
import javafx.util.Duration;
import javafx.util.converter.IntegerStringConverter;

public class SimulatorController {

    @FXML private TextField memorySize;
    @FXML private TextField coalesceInterval;
    @FXML private TextField compactionInterval;

    @FXML private TableView<Process> processTable;
    @FXML private TableColumn<Process, Integer> pidCol;
    @FXML private TableColumn<Process, Integer> sizeCol;
    @FXML private TableColumn<Process, Integer> burstCol;
    @FXML private TableColumn<Process, Integer> arrivalCol;

    @FXML private HBox memoryDisplay;
    @FXML private TableView<FreeBlock> freeListTable;
    @FXML private TableColumn<FreeBlock, Integer> startAddressCol;
    @FXML private TableColumn<FreeBlock, Integer> endAddressCol;
    @FXML private TableColumn<FreeBlock, Integer> freeSizeCol;

    @FXML private Label timeLabel;

    private Simulator simulator;
    private Timeline uiUpdater;
    private ObservableList<Process> processList = FXCollections.observableArrayList();

    @FXML
    private void initialize() {
        // Make process table editable
        processTable.setEditable(true);

        pidCol.setCellValueFactory(cell -> new SimpleIntegerProperty(cell.getValue().getPid()).asObject());
        sizeCol.setCellValueFactory(cell -> new SimpleIntegerProperty(cell.getValue().getSize()).asObject());
        burstCol.setCellValueFactory(cell -> new SimpleIntegerProperty(cell.getValue().getBurstTime()).asObject());
        arrivalCol.setCellValueFactory(cell -> new SimpleIntegerProperty(cell.getValue().getArrivalTime()).asObject());

        // Enable inline editing for size, burst, arrival time
        sizeCol.setCellFactory(TextFieldTableCell.forTableColumn(new IntegerStringConverter()));
        sizeCol.setOnEditCommit(e -> e.getRowValue().setSize(e.getNewValue()));

        burstCol.setCellFactory(TextFieldTableCell.forTableColumn(new IntegerStringConverter()));
        burstCol.setOnEditCommit(e -> e.getRowValue().setBurstTime(e.getNewValue()));

        arrivalCol.setCellFactory(TextFieldTableCell.forTableColumn(new IntegerStringConverter()));
        arrivalCol.setOnEditCommit(e -> e.getRowValue().setArrivalTime(e.getNewValue()));

        processTable.setItems(processList);

        // free list table setup
        startAddressCol.setCellValueFactory(cell -> new SimpleIntegerProperty(cell.getValue().getStartAddress()).asObject());
        endAddressCol.setCellValueFactory(cell -> new SimpleIntegerProperty(cell.getValue().getEndAddress()).asObject());
        freeSizeCol.setCellValueFactory(cell -> new SimpleIntegerProperty(
                cell.getValue().getEndAddress() - cell.getValue().getStartAddress() + 1).asObject());
    }

    @FXML
    private void addProcess() {
        int nextPid = processList.size() + 1;
        Process newProc = new Process(nextPid, 100, 5, processList.size() * 2); // default example
        processList.add(newProc);
    }

    @FXML
    private void removeSelectedProcess() {
        Process selected = processTable.getSelectionModel().getSelectedItem();
        if (selected != null) processList.remove(selected);
    }

    @FXML
    private void startSimulator() {
        if (processList.isEmpty()) {
            new Alert(Alert.AlertType.WARNING, "Please add at least one process!").showAndWait();
            return;
        }

        try {
            int memSize = Integer.parseInt(memorySize.getText());
            int coalesceInt = Integer.parseInt(coalesceInterval.getText());
            int compactInt = Integer.parseInt(compactionInterval.getText());

            simulator = new Simulator(memSize, coalesceInt, compactInt);
            simulator.getProcessList().addAll(processList);
            simulator.run();

            uiUpdater = new Timeline(new KeyFrame(Duration.seconds(1), e -> refreshUI()));
            uiUpdater.setCycleCount(Timeline.INDEFINITE);
            uiUpdater.play();

        } catch (NumberFormatException e) {
            new Alert(Alert.AlertType.ERROR, "Please fill all settings with valid numbers.").showAndWait();
        }
    }

    private void refreshUI() {
        // Memory visualization
        memoryDisplay.getChildren().clear();
        int totalSize = simulator.getMemory().getSize();
        double visualWidth = 800; // total width of the memory bar

        // Draw blocks: both processes and free spaces
        int lastAddr = 0;

        for (FreeBlock free : simulator.getMemory().getFreeList().getFreeList()) {
            if (free.getStartAddress() > lastAddr) {
                // allocated region before this hole
                Pane used = makeBlock(free.getStartAddress() - lastAddr, totalSize, visualWidth, Color.LIGHTBLUE, "Used");
                memoryDisplay.getChildren().add(used);
            }
            // add hole
            Pane hole = makeBlock(free.getSize(), totalSize, visualWidth, Color.LIGHTGRAY, "Free");
            memoryDisplay.getChildren().add(hole);
            lastAddr = free.getEndAddress() + 1;
        }

        if (lastAddr < totalSize) {
            // memory after last free block
            Pane used = makeBlock(totalSize - lastAddr, totalSize, visualWidth, Color.LIGHTBLUE, "Used");
            memoryDisplay.getChildren().add(used);
        }

        // update free list table
        ObservableList<FreeBlock> freeBlocks = FXCollections.observableArrayList(simulator.getMemory().getFreeList().getFreeList());
        freeListTable.setItems(freeBlocks);

        timeLabel.setText("Time: " + simulator.getCurrentTime() + "s");
    }

    private Pane makeBlock(int blockSize, int totalSize, double totalWidth, Color color, String label) {
        double widthRatio = (double) blockSize / totalSize * totalWidth;
        Pane block = new Pane();
        block.setPrefWidth(widthRatio);
        block.setPrefHeight(30);
        block.setBackground(new Background(new BackgroundFill(color, new CornerRadii(3), null)));
        block.setBorder(new Border(new BorderStroke(Color.BLACK, BorderStrokeStyle.SOLID, CornerRadii.EMPTY, BorderWidths.DEFAULT)));

        Label lbl = new Label(label);
        lbl.setTextFill(Color.BLACK);
        lbl.setLayoutX(5);
        lbl.setLayoutY(5);
        block.getChildren().add(lbl);

        return block;
    }
}
